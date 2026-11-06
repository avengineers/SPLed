"""
Guards against the compromised emSign CA (GHSA-6q48-6c7w-9gw2).

Eight intermediate CAs under "emSign ECC Root CA - G3" have compromised keys. A tool that
trusts the root accepts certificates issued with those keys. The versions below are the first
that no longer trust it; the trust-store test checks the bundles the tools actually read.
"""

import hashlib
import os
import re
import ssl
import sys
from importlib.metadata import version
from pathlib import Path

import certifi
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
# SHA-256 of the DER form of "emSign ECC Root CA - G3".
EMSIGN_ECC_ROOT_G3_SHA256 = "86a1ecba089c4a8d3bbe2734c612ba341d813e043cf9e8a862cd5c57a36bbe6b"
# Variables that point pip, requests, OpenSSL and curl at a CA bundle (set in the devcontainer).
CA_BUNDLE_VARIABLES = ("SSL_CERT_FILE", "REQUESTS_CA_BUNDLE", "PIP_CERT", "CURL_CA_BUNDLE")
PEM_CERTIFICATE = re.compile(r"-----BEGIN CERTIFICATE-----.+?-----END CERTIFICATE-----", re.DOTALL)


def ca_bundles() -> list[Path]:
    """Return the CA bundle files that the Python tools in this environment read."""
    candidates = [certifi.where(), ssl.get_default_verify_paths().cafile, *(os.environ.get(name) for name in CA_BUNDLE_VARIABLES)]
    return sorted({Path(candidate).resolve() for candidate in candidates if candidate and Path(candidate).is_file()})


def sha256_fingerprints(bundle: Path) -> set[str]:
    pem_certificates = PEM_CERTIFICATE.findall(bundle.read_text(encoding="utf-8", errors="replace"))
    return {hashlib.sha256(ssl.PEM_cert_to_DER_cert(pem)).hexdigest() for pem in pem_certificates}


def test_python_version():
    assert sys.version_info[:3] >= MIN_PYTHON_VERSION, f"Python {sys.version} is older than {MIN_PYTHON_VERSION}"


def test_pip_version():
    assert Version(version("pip")) >= MIN_PIP_VERSION, f"pip {version('pip')} is older than {MIN_PIP_VERSION}"


def test_certifi_version():
    assert Version(version("certifi")) >= MIN_CERTIFI_VERSION, f"certifi {version('certifi')} is older than {MIN_CERTIFI_VERSION}"


def test_ca_bundles_do_not_trust_emsign_ecc_root_g3():
    bundles = ca_bundles()
    assert bundles, "No CA bundle found"
    trusting = [str(bundle) for bundle in bundles if EMSIGN_ECC_ROOT_G3_SHA256 in sha256_fingerprints(bundle)]
    assert not trusting, f"These CA bundles still trust the compromised emSign ECC Root CA - G3: {trusting}"
