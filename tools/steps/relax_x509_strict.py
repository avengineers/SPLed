"""
Opt-in workaround for TLS-inspecting proxies whose CA fails Python 3.13's strict X.509 checks.

Python 3.13 sets ``ssl.VERIFY_X509_STRICT`` in ``ssl.create_default_context()``, and urllib3
sets it in its own contexts. A proxy CA whose Basic Constraints extension is not marked critical
then fails every HTTPS download of this pipeline (poks) with "Basic Constraints of CA cert not
marked critical". Python 3.12 and older accept such a CA.

With ``SPLED_RELAX_X509_STRICT=1`` this step drops only that flag from every SSL context created
later in the ``pypeline run`` process. Certificate chain, expiry and host name are still checked.
The step does nothing when the variable is not set; CI never sets it.

Remove this step once the proxy CA marks Basic Constraints critical.
"""

import os
import ssl
from collections.abc import Mapping
from pathlib import Path

from py_app_dev.core.logging import logger
from pypeline.domain.execution_context import ExecutionContext
from pypeline.domain.pipeline import PipelineStep

ENV_VARIABLE = "SPLED_RELAX_X509_STRICT"


def relax_if_requested(environ: Mapping[str, str]) -> bool:
    """Drop ``VERIFY_X509_STRICT`` from all SSL contexts if ``environ`` requests it; return whether it did."""
    if environ.get(ENV_VARIABLE) != "1":
        return False
    verify_flags = ssl.SSLContext.__dict__["verify_flags"]
    if not getattr(verify_flags.fset, "relaxed", False):

        def set_verify_flags(context: ssl.SSLContext, value: ssl.VerifyFlags) -> None:
            verify_flags.fset(context, value & ~ssl.VERIFY_X509_STRICT)

        set_verify_flags.relaxed = True  # type: ignore[attr-defined]
        ssl.SSLContext.verify_flags = verify_flags.setter(set_verify_flags)  # type: ignore[method-assign]
    return True


class RelaxX509Strict(PipelineStep[ExecutionContext]):
    def get_name(self) -> str:
        return self.__class__.__name__

    def run(self) -> int:
        if relax_if_requested(os.environ):
            logger.warning(f"{ENV_VARIABLE}=1: SSL contexts in this pipeline skip Python's strict X.509 checks (VERIFY_X509_STRICT).")
        return 0

    def get_inputs(self) -> list[Path]:
        return []

    def get_outputs(self) -> list[Path]:
        return []

    def update_execution_context(self) -> None:
        pass

    def get_needs_dependency_management(self) -> bool:
        # Patches the running process, so it must run every time.
        return False
