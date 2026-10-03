"""The build as a pipeline step, so a pipeline file shows what runs."""

import json
import shlex
import sys
from pathlib import Path
from typing import Any

from py_app_dev.core.exceptions import UserNotificationException
from py_app_dev.core.logging import logger
from pypeline.domain.execution_context import ExecutionContext
from pypeline.domain.pipeline import PipelineStep

# pypeline loads this file by path, so its directory is not importable on its own.
sys.path.insert(0, str(Path(__file__).parent))
from variants import discover_variants  # noqa: E402


def ask(what: str, choices: list[str]) -> str:
    for number, choice in enumerate(choices, start=1):
        print(f"({number}) {choice}")
    answer = input(f"Please enter the {what} number: ").strip()
    if not answer.isdigit() or not 1 <= int(answer) <= len(choices):
        raise UserNotificationException(f"No {what} number '{answer}'.")
    logger.info(f"Selected {what}: {choices[int(answer) - 1]}")
    return choices[int(answer) - 1]


class _Step(PipelineStep[ExecutionContext]):
    def setting(self, name: str, default: Any = None) -> Any:
        """A pipeline input wins over the step's `config:` block."""
        value = self.execution_context.get_input(name)
        if value is None and self.config:
            value = self.config.get(name)
        return default if value in (None, "") else value

    def flag(self, name: str) -> bool:
        # `-i name=true` arrives as a string.
        return str(self.setting(name, False)).strip().lower() in ("1", "true", "yes", "on")

    def get_name(self) -> str:
        return self.__class__.__name__

    # Nothing declared: the real inputs are every source file, so pypeline always runs the step and ninja decides what to rebuild.
    def get_inputs(self) -> list[Path]:
        return []

    def get_outputs(self) -> list[Path]:
        return []

    def update_execution_context(self) -> None:
        pass


class CMakeBuild(_Step):
    """Configure and build one variant, a comma separated list, or `all`."""

    def run(self) -> int:
        # Only a person at a terminal is asked; CI and the tests pass everything they need.
        interactive = sys.stdin.isatty()
        available = discover_variants(self.project_root_dir)
        requested = self.setting("variant")
        if not requested:
            if not interactive:
                raise UserNotificationException("CMakeBuild needs a variant. Pass `-i variant=<name|a,b|all>`.")
            requested = ask("variant", ["all", *available])
        variants = available if requested == "all" else [name.strip().strip("/") for name in requested.split(",")]
        unknown = [name for name in variants if name not in available]
        if unknown:
            raise UserNotificationException(f"Unknown variant(s) {unknown}. Available: {available}")

        build_kit = self.setting("build_kit") or (ask("build kit", ["prod", "test"]) if interactive else "prod")
        build_type = self.setting("build_type")
        if not build_type and interactive:
            build_type = self.ask_build_type(build_kit)
        for variant in variants:
            self.build(variant, build_kit, build_type)
        return 0

    def ask_build_type(self, build_kit: str) -> str | None:
        # The test kit measures coverage, which needs debug information.
        if build_kit == "test":
            logger.info("Build kit 'test' always uses build type 'Debug'.")
            return "Debug"
        # The same list the VS Code CMake extension offers.
        try:
            cmake_variants = json.loads((self.project_root_dir / ".vscode/cmake-variants.json").read_text())
            build_types = list(cmake_variants["buildType"]["choices"])
        except (OSError, ValueError, KeyError):
            build_types = []
        if len(build_types) < 2:
            return build_types[0] if build_types else None
        return ask("build type", build_types)

    def build(self, variant: str, build_kit: str, build_type: str | None) -> None:
        target = self.setting("target", "all")
        build_dir = Path("build", variant, build_kit, build_type or "")

        configure = ["cmake", "-B", build_dir.as_posix(), "-G", "Ninja", f"-DVARIANT={variant}", f"-DBUILD_KIT={build_kit}"]
        if build_type:
            configure += [f"-DBUILD_TYPE={build_type}", f"-DCMAKE_BUILD_TYPE={build_type}"]
        if build_kit == "test":
            toolchain = "toolchain.cmake" if sys.platform.startswith("win") else "toolchain_linux.cmake"
            configure.append(f"-DCMAKE_TOOLCHAIN_FILE=tools/toolchains/gcc/{toolchain}")
        # --fresh drops the CMake cache, which is what -reconfigure used to delete by hand.
        if self.flag("reconfigure") or self.flag("configure_only"):
            configure.append("--fresh")

        logger.info(f"Building target '{target}' with build kit '{build_kit}' for variant '{variant}' ...")
        self.execute(configure)
        if self.flag("configure_only"):
            return
        build = ["cmake", "--build", build_dir.as_posix(), "--target", target]
        if build_type:
            build += ["--config", build_type]
        # cleandead deletes outputs a changed build no longer produces, so an incremental build leaves no stale artifacts.
        self.execute([*build, "--", "-t", "cleandead"])
        self.execute([*build, "--", *shlex.split(self.setting("ninja_args", ""))])

    def execute(self, command: list[str]) -> None:
        logger.info(f"Running: {' '.join(command)}")
        self.execution_context.create_process_executor(command, cwd=self.project_root_dir).execute()

