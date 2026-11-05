import importlib.util
import ssl
from pathlib import Path

import pytest
import urllib3

pytestmark = [
    pytest.mark.gate_develop_pr,
    pytest.mark.gate_develop_push,
    pytest.mark.gate_develop_nightly,
    pytest.mark.gate_release_pr,
    pytest.mark.gate_release,
]

STEP_FILE = Path(__file__).parents[1] / "tools" / "steps" / "relax_x509_strict.py"


@pytest.fixture
def relax():
    """Load the step module and restore the ssl module after the test."""
    original_verify_flags = ssl.SSLContext.__dict__["verify_flags"]
    spec = importlib.util.spec_from_file_location("relax_x509_strict", STEP_FILE)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    yield module
    ssl.SSLContext.verify_flags = original_verify_flags


def is_strict(context: ssl.SSLContext) -> bool:
    return bool(context.verify_flags & ssl.VERIFY_X509_STRICT)


def test_not_requested_keeps_strict_checks(relax):
    assert relax.relax_if_requested({}) is False
    assert relax.relax_if_requested({relax.ENV_VARIABLE: "0"}) is False
    assert is_strict(ssl.create_default_context())
    assert is_strict(urllib3.util.ssl_.create_urllib3_context())


def test_requested_drops_only_the_strict_flag(relax):
    other_flags = ssl.create_default_context().verify_flags & ~ssl.VERIFY_X509_STRICT

    assert relax.relax_if_requested({relax.ENV_VARIABLE: "1"}) is True

    default_context = ssl.create_default_context()
    assert not is_strict(default_context)
    assert not is_strict(urllib3.util.ssl_.create_urllib3_context())
    assert default_context.verify_flags == other_flags
    assert default_context.verify_mode == ssl.CERT_REQUIRED
    assert default_context.check_hostname


def test_requested_twice_relaxes_once(relax):
    relax.relax_if_requested({relax.ENV_VARIABLE: "1"})
    relaxed_setter = ssl.SSLContext.__dict__["verify_flags"]
    relax.relax_if_requested({relax.ENV_VARIABLE: "1"})
    assert ssl.SSLContext.__dict__["verify_flags"] is relaxed_setter
