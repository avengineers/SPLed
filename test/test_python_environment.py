"""
Guards against the compromised emSign CA (GHSA-6q48-6c7w-9gw2).

Eight intermediate CAs under "emSign ECC Root CA - G3" have compromised keys. A tool that
trusts the root accepts certificates issued with those keys. The versions below are the first
that no longer trust it.
"""

import sys
from importlib.metadata import version

import pytest
from packaging.version import Version

pytestmark = [
    pytest.mark.gate_develop_pr,
    pytest.mark.gate_develop_push,
    pytest.mark.gate_develop_nightly,
    pytest.mark.gate_release_pr,
    pytest.mark.gate_release,
]

# Python 3.13.17 bundles pip 26.2.2 for ensurepip (set in bootstrap.json and bootstrap_python.sh).
MIN_PYTHON_VERSION = (3, 13, 17)
# pip and certifi are pinned in poetry.lock.
MIN_PIP_VERSION = Version("26.2.2")
MIN_CERTIFI_VERSION = Version("2026.10.19")


def test_python_version():
    assert sys.version_info[:3] >= MIN_PYTHON_VERSION, f"Python {sys.version} is older than {MIN_PYTHON_VERSION}"


def test_pip_version():
    assert Version(version("pip")) >= MIN_PIP_VERSION, f"pip {version('pip')} is older than {MIN_PIP_VERSION}"


def test_certifi_version():
    assert Version(version("certifi")) >= MIN_CERTIFI_VERSION, f"certifi {version('certifi')} is older than {MIN_CERTIFI_VERSION}"
