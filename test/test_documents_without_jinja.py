import ast
import re
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parent.parent
VARIANTS_DIR = REPO_ROOT / "variants"

gate_markers = [
    pytest.mark.gate_develop_pr,
    pytest.mark.gate_develop_push,
    pytest.mark.gate_develop_nightly,
    pytest.mark.gate_release_pr,
    pytest.mark.gate_release,
]


def with_gate_markers(func):
    for marker in gate_markers:
        func = marker(func)
    return func


def project_documents() -> list[Path]:
    documents = [REPO_ROOT / "index.md"]
    documents += sorted((REPO_ROOT / "doc").rglob("*.md"))
    documents += sorted((REPO_ROOT / "components").glob("**/doc/**/*.md"))
    documents += sorted((REPO_ROOT / "test").glob("**/doc/**/*.md"))
    return documents


@with_gate_markers
def test_documents_have_no_jinja():
    # Arrange
    documents = project_documents()

    # Act
    jinja_lines = [
        f"{document.relative_to(REPO_ROOT).as_posix()}:{number}: {line.strip()}"
        for document in documents
        for number, line in enumerate(document.read_text(encoding="utf-8").splitlines(), start=1)
        if "{%" in line or "{{" in line
    ]

    # Assert
    assert not jinja_lines, "Jinja left in the documents:\n" + "\n".join(jinja_lines)


@with_gate_markers
def test_every_variant_has_a_header_block():
    # Arrange
    variants = sorted(parts.parent.relative_to(VARIANTS_DIR).as_posix() for parts in VARIANTS_DIR.rglob("parts.cmake"))
    index_lines = (REPO_ROOT / "index.md").read_text(encoding="utf-8").splitlines()

    # Act
    header_blocks = {
        variant: [
            line
            for line in index_lines
            if re.fullmatch(r"\s*(`{3,}|:{3,})\{if\}\s+var\.build_config\.variant\s*==\s*\"" + re.escape(variant) + r"\"\s*", line)
        ]
        for variant in variants
    }

    # Assert
    assert variants, "no variants/**/parts.cmake found"
    wrong = {variant: len(lines) for variant, lines in header_blocks.items() if len(lines) != 1}
    assert not wrong, f"variants without exactly one header block in index.md (variant: count): {wrong}"


def literal_include_pattern_lines(tree: ast.AST) -> list[int]:
    """Lines where conf.py puts a string literal into include_patterns (assignment, +=, extend, append)."""

    def is_include_patterns(node: ast.AST) -> bool:
        return isinstance(node, ast.Name) and node.id == "include_patterns"

    def has_string_literal(node: ast.AST | None) -> bool:
        # Only a pattern written into conf.py counts: a string in a list/tuple/set, or the value itself.
        # Strings used to look the list up in the build configuration (``.get("include_patterns", [])``) do not.
        if node is None:
            return False
        is_string = lambda sub: isinstance(sub, ast.Constant) and isinstance(sub.value, str)  # noqa: E731
        return is_string(node) or any(is_string(element) for sub in ast.walk(node) if isinstance(sub, (ast.List, ast.Tuple, ast.Set)) for element in sub.elts)

    lines = []
    for node in ast.walk(tree):
        if isinstance(node, ast.Assign) and any(is_include_patterns(target) for target in node.targets) and has_string_literal(node.value):
            lines.append(node.lineno)
        elif isinstance(node, (ast.AugAssign, ast.AnnAssign)) and is_include_patterns(node.target) and has_string_literal(node.value):
            lines.append(node.lineno)
        elif (
            isinstance(node, ast.Call)
            and isinstance(node.func, ast.Attribute)
            and node.func.attr in ("extend", "append")
            and is_include_patterns(node.func.value)
            and any(has_string_literal(arg) for arg in node.args)
        ):
            lines.append(node.lineno)
    return lines


@with_gate_markers
def test_conf_py_has_no_include_list_and_no_jinja_hook():
    # Arrange
    # The run's include set reaches Sphinx only through the build configuration (spl-core ADR 0005),
    # so conf.py may take include_patterns from it, but must not list documents of its own.
    tree = ast.parse((REPO_ROOT / "conf.py").read_text(encoding="utf-8"))

    # Act
    literal_include_lines = literal_include_pattern_lines(tree)
    source_read_connects = [
        node.lineno
        for node in ast.walk(tree)
        if isinstance(node, ast.Call)
        and isinstance(node.func, ast.Attribute)
        and node.func.attr == "connect"
        and any(isinstance(arg, ast.Constant) and arg.value == "source-read" for arg in node.args)
    ]

    # Assert
    assert not literal_include_lines, f"conf.py lists include_patterns of its own (lines {literal_include_lines})"
    assert not source_read_connects, f"conf.py connects a source-read handler (lines {source_read_connects})"
