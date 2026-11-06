"""Checks on a variant report built by the `reports` target.

The caller passes the expected values, read from the variant's `config.txt` and
`parts.cmake`. Nothing here reads the documents, `conf.py` or the build configuration:
the checks look only at the rendered HTML.
"""

import re
from pathlib import Path

# Specs that a feature guards, per component, read from components/<name>/doc/index.md.
FEATURE_GUARDED_SPECS: dict[str, dict[str, list[str]]] = {
    "BLINKING": {
        "components/light_controller": ["SWDD_LC-101", "SWDD_LC-203"],
    },
    "BRIGHTNESS_ADJUSTMENT_ENABLED": {
        "components/light_controller": ["SWDD_LC-204"],
    },
    "BRIGHTNESS_ADJUSTMENT_MANUAL": {
        "components/brightness_controller": ["SWDD_BC-101", "SWDD_BC-201"],
    },
    "BRIGHTNESS_ADJUSTMENT_AUTOMATIC": {
        "components/brightness_controller": ["SWDD_BC-102", "SWDD_BC-203"],
    },
    "AUTO_OFF": {
        "components/power_signal_processing": ["SWDD_PSP-004", "SWDD_PSP-005"],
        "components/auto_off": ["SWDD_AO-102", "SWDD_AO-205"],
    },
}

# Components whose design page holds one Mermaid diagram that depends on a feature:
# the feature, and a node that only the diagram for the feature switched on has.
FEATURE_DIAGRAMS: dict[str, tuple[str, str]] = {
    "components/power_signal_processing": ("AUTO_OFF", "AutoOffCheck"),
    "components/light_controller": ("BLINKING", "BlinkON"),
}

NEED_ID = re.compile(r'id="(SWDD_[A-Z]+-\d+)"')
VARIANT_LINE = re.compile(r"<strong>Variant:</strong>\s*([^<]*?)\s*<")
WRAPPER_LINK = re.compile(r'href="[^"#]*/reports/([^"/#]+)_index\.html')
MERMAID_BLOCK = re.compile(r'<pre\s+class="mermaid">(.*?)</pre>', re.DOTALL)
COMPONENT_PAGE_LINK = re.compile(r'href="(?:\.\./)*([^"#]+)/doc/index\.html')
RAW_TAG = re.compile(r"\{%\s*(?:end)?raw\s*%\}")
ARTICLE_BODY = re.compile(r'<article class="bd-article">(.*?)</article>', re.DOTALL)
REPO_ROOT = Path(__file__).resolve().parent.parent


def wrapper_page_name(component: str) -> str:
    """spl-core names a component's wrapper page after its path, with `/` replaced by `_`."""
    return component.replace("/", "_")


def built_component_pages(html_dir: Path) -> set[str]:
    """The components whose `<path>/doc/index.html` design page was built."""
    pages = set()
    for page in html_dir.rglob("doc/index.html"):
        relative = page.relative_to(html_dir)
        if relative.parts[0] in ("doc", "build"):
            continue
        pages.add(relative.parent.parent.as_posix())
    return pages


def linked_component_pages(page_html: str) -> set[str]:
    """The components whose `<path>/doc/index.html` design page a page links."""
    return {path for path in COMPONENT_PAGE_LINK.findall(page_html) if not path.startswith("..")}


def rendered_pages(html_dir: Path) -> dict[Path, str]:
    """Every rendered HTML page outside `_static`, with its text."""
    return {
        page.relative_to(html_dir): page.read_text(encoding="utf-8", errors="replace")
        for page in html_dir.rglob("*.html")
        if "_static" not in page.relative_to(html_dir).parts
    }


def check_variant_report(
    build_dir: Path,
    variant: str,
    features: dict[str, bool],
    documented_components: list[str],
) -> None:
    html_dir = Path(build_dir) / "reports" / "html"
    documented = set(documented_components)
    problems: list[str] = []

    # The header names the variant, and only the variant; the timestamp is gone.
    index_html = (html_dir / "index.html").read_text(encoding="utf-8")
    variant_lines = VARIANT_LINE.findall(index_html)
    if variant_lines != [variant]:
        problems.append(f"index.html: expected one 'Variant: {variant}' line, found {variant_lines}")
    if "Timestamp" in index_html:
        problems.append("index.html: still shows a Timestamp")

    # The article body links the variant's own coverage report exactly once.
    coverage_href = (Path(build_dir).resolve().relative_to(REPO_ROOT) / "reports" / "coverage.html").as_posix()
    article = ARTICLE_BODY.search(index_html)
    coverage_links = article.group(1).count(f'href="{coverage_href}"') if article else 0
    if coverage_links != 1:
        problems.append(f"index.html: expected one link to {coverage_href} in the article body, found {coverage_links}")

    # The components index links exactly one wrapper page per documented component.
    components_index = (html_dir / "doc" / "components" / "index.html").read_text(encoding="utf-8")
    linked = set(WRAPPER_LINK.findall(components_index))
    expected_wrappers = {wrapper_page_name(component) for component in documented}
    if linked != expected_wrappers:
        problems.append(
            "doc/components/index.html: wrapper pages differ: "
            f"missing {sorted(expected_wrappers - linked)}, unexpected {sorted(linked - expected_wrappers)}"
        )

    # No other component's design page is built.
    built = built_component_pages(html_dir)
    if built != documented:
        problems.append(
            f"component design pages differ: missing {sorted(documented - built)}, unexpected {sorted(built - documented)}"
        )

    # A feature-guarded spec is in the report exactly when its feature is on.
    pages = rendered_pages(html_dir)
    need_ids = {need_id for text in pages.values() for need_id in NEED_ID.findall(text)}

    # No page shows a Jinja raw tag: without a Jinja hook, spl-core must not write them.
    raw_tag_pages = sorted(page.as_posix() for page, text in pages.items() if RAW_TAG.search(text))
    if raw_tag_pages:
        problems.append(f"Jinja raw tags rendered as text in {len(raw_tag_pages)} pages, e.g. {raw_tag_pages[:3]}")
    for feature, specs_per_component in FEATURE_GUARDED_SPECS.items():
        for component, spec_ids in specs_per_component.items():
            expected_present = features[feature] and component in documented
            for spec_id in spec_ids:
                if (spec_id in need_ids) != expected_present:
                    state = "missing" if expected_present else "present"
                    problems.append(f"{spec_id} is {state}, but {feature} is {features[feature]} and {component} documented is {component in documented}")

    # A design page with a feature-dependent diagram holds exactly one, the matching one.
    for component, (feature, feature_node) in FEATURE_DIAGRAMS.items():
        if component not in documented:
            continue
        page = html_dir / component / "doc" / "index.html"
        diagrams = MERMAID_BLOCK.findall(page.read_text(encoding="utf-8")) if page.is_file() else []
        if len(diagrams) != 1:
            problems.append(f"{component}: expected exactly one Mermaid diagram, found {len(diagrams)}")
        elif (feature_node in diagrams[0]) != features[feature]:
            problems.append(f"{component}: the Mermaid diagram does not match {feature}={features[feature]}")

    assert not problems, f"Variant report of {variant}:\n" + "\n".join(problems)
