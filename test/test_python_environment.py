import sys
from importlib.metadata import version

import pytest

# Python 3.13.17 and pip 26.2.2 are the first versions that no longer trust the
# compromised emSign CAs. The bootstrap scripts pin both; this test catches a pin that drifts.
MIN_PYTHON_VERSION = (3, 13, 17)
MIN_PIP_VERSION = (26, 2, 2)


def version_tuple(version_string: str) -> tuple[int, ...]:
    return tuple(int(part) for part in version_string.split(".")[:3])


@pytest.mark.gate_develop_pr
@pytest.mark.gate_develop_push
@pytest.mark.gate_develop_nightly
@pytest.mark.gate_release_pr
@pytest.mark.gate_release
def test_python_version():
    assert sys.version_info[:3] >= MIN_PYTHON_VERSION, f"Python {sys.version} is older than {MIN_PYTHON_VERSION}"


@pytest.mark.gate_develop_pr
@pytest.mark.gate_develop_push
@pytest.mark.gate_develop_nightly
@pytest.mark.gate_release_pr
@pytest.mark.gate_release
def test_pip_version():
    assert version_tuple(version("pip")) >= MIN_PIP_VERSION, f"pip {version('pip')} is older than {MIN_PIP_VERSION}"
