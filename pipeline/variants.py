"""
The project's variants, as the JSON list CI fans out over.

Standard library only: the CI job that lists the variants runs before any virtual environment exists.

    python pipeline/variants.py
    ["Base/Dev", "Disco", "IDEA/Sloemada", "Sleep", "Spa"]

    python pipeline/variants.py --release-filter
    (the pytest -k expression of the release branch's variant, empty on any other branch)
"""

import json
import os
import sys
from pathlib import Path

PROJECT_DIR = Path(__file__).resolve().parent.parent


def discover_variants(project_dir: Path = PROJECT_DIR) -> list[str]:
    variants_dir = project_dir / "variants"
    return sorted(config.parent.relative_to(variants_dir).as_posix() for config in variants_dir.rglob("config.cmake"))


def release_branch() -> str | None:
    """The release branch this CI run builds or merges into, or None."""
    if os.environ.get("GITHUB_ACTIONS") == "true":
        # A pull request is judged by its target branch, a push by its own.
        branch = os.environ.get("GITHUB_BASE_REF") or os.environ.get("GITHUB_REF_NAME")
    else:
        branch = os.environ.get("CHANGE_TARGET") or os.environ.get("BRANCH_NAME")
    return branch if branch and branch.startswith("release/") else None


def select_release_variant(branch: str, variants: list[str]) -> str:
    """
    The one variant a release branch names: release/<Variant>, release/<Variant>/<tag> or release/<Variant>#<tag>.

    A release branch releases one product, so a branch that names none is a mistake, not a request to build everything.
    """
    name = branch.removeprefix("release/")
    # Longest first, so a variant is not matched by a shorter one that prefixes it.
    for variant in sorted(variants, key=len, reverse=True):
        if name == variant or name.startswith((f"{variant}/", f"{variant}#")):
            return variant
    raise ValueError(f"No variant found in release branch '{branch}'. Available: {variants}. Use a feature branch to build every variant.")


def selected_variants(project_dir: Path = PROJECT_DIR) -> list[str]:
    variants = discover_variants(project_dir)
    branch = release_branch()
    return [select_release_variant(branch, variants)] if branch else variants


def pytest_filter(variant: str) -> str:
    """The `-k` expression of a variant: test classes spell IDEA/Sloemada as Test_IDEA__Sloemada."""
    return variant.replace("/", "__")


def release_pytest_filter(project_dir: Path = PROJECT_DIR) -> str:
    """The `-k` expression the current release branch selects, or an empty string."""
    branch = release_branch()
    return pytest_filter(select_release_variant(branch, discover_variants(project_dir))) if branch else ""


if __name__ == "__main__":
    try:
        print(release_pytest_filter() if "--release-filter" in sys.argv else json.dumps(selected_variants()))
    except ValueError as error:
        print(f"ERROR: {error}", file=sys.stderr)
        sys.exit(1)
