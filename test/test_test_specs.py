import re
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parent.parent

# A doc comment with a `.. test::` block, up to the end of its option list, and the test macro
# of the test case it documents.
TEST_SPEC = re.compile(
    r"\.\. test:: (?P<title>\S+)\n(?P<options>(?:\s*\*\s+:[^\n]*\n)+)(?:(?!\*/).)*\*/\s*(?P<macro>\w+)\(",
    re.DOTALL,
)

gate_markers = [
    pytest.mark.reports,
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


@with_gate_markers
def test_parametrized_test_specs_set_their_results():
    # Arrange
    # The results predicate links the test cases named like the spec's title. A parametrized
    # test's cases carry a suffix per parameter, so the spec of a TEST_P links them itself with
    # links_from_filter; without it the spec has no results. Its title names the test without the
    # suffix: the `/*` pattern of spl-core before 9.1 is gone.
    sources = list(REPO_ROOT.glob("components/**/test/*.cc"))

    # Act
    specs = [(path, match) for path in sources for match in TEST_SPEC.finditer(path.read_text(encoding="utf-8"))]
    parametrized = [(f"{path.relative_to(REPO_ROOT).as_posix()}: {match['title']}", match) for path, match in specs if match["macro"] == "TEST_P"]
    without_results = [name for name, match in parametrized if ":results:" not in match["options"]]
    with_pattern = [name for name, match in parametrized if match["title"].endswith("/*")]

    # Assert
    assert parametrized, "no parametrized test specs found"
    assert not without_results, f"parametrized test specs without :results: {without_results}"
    assert not with_pattern, f"parametrized test specs titled with the `/*` pattern: {with_pattern}"
