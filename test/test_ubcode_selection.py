import subprocess
import tomllib
from pathlib import Path

import pytest
from spl_core.test_utils.spl_build import SplBuild

REPO_ROOT = Path(__file__).resolve().parent.parent
SELECTION_FILE = REPO_ROOT / "ubproject.variants.toml"
COMMITTED_CONFIG = REPO_ROOT / "ubproject.toml"

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
def test_selection_file_names_the_configured_build():
    # Arrange
    spl_build = SplBuild(variant="Disco", build_kit="test", build_type="Debug", target="reports")

    # Act
    result = spl_build.execute()

    # Assert
    assert result == 0, "Building reports failed"
    assert SELECTION_FILE.is_file(), f"{SELECTION_FILE} was not written by the configure step"
    selection = tomllib.loads(SELECTION_FILE.read_text(encoding="utf-8"))
    data_file = selection["variants"]["data_file"]
    assert Path(data_file).as_posix().endswith("build/Disco/test/Debug/reports/variant_data.json")


@with_gate_markers
def test_committed_ubproject_extends_the_selection():
    # Arrange
    assert COMMITTED_CONFIG.is_file(), f"{COMMITTED_CONFIG} is missing"

    # Act
    config = tomllib.loads(COMMITTED_CONFIG.read_text(encoding="utf-8"))

    # Assert
    assert config.get("extend") == "ubproject.variants.toml"
    # `extend` replaces arrays (parse.parsers.<md|rst>.include, [source], needs.types, ...),
    # so any other table would hide the selection's own.
    assert set(config) == {"$schema", "extend"}, f"unexpected keys in ubproject.toml: {sorted(config)}"


@with_gate_markers
def test_selection_file_is_ignored():
    # Act
    result = subprocess.run(
        ["git", "check-ignore", "ubproject.variants.toml"],
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
    )

    # Assert
    assert result.returncode == 0, "ubproject.variants.toml is not git-ignored"
