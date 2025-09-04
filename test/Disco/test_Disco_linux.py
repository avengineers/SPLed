import time
from pathlib import Path
from typing import Generator, List, Optional
import platform

import pytest
from spl_core.test_utils.spl_build import SplBuild
from spl_core.test_utils.artifacts_archiver import ArtifactsArchiver

from py_app_dev.core.logging import time_it
from py_app_dev.core.subprocess import SubprocessExecutor


class SplBuildLinux(SplBuild):
    @time_it()
    def execute(self, target: Optional[str] = None, additional_args: Optional[List[str]] = None) -> int:
        """
        Execute an SPL build (of a given target).

        Args:
            target: The target to build, optional, defaults to value given in the constructor.
            additional_args: Additional arguments to pass to the build command.

        Returns:
            int: 0 in case of success.

        """
        if target is None:
            target = self.target if self.target else "all"
        return_code = -1
        while True:
                # Linux/Unix - use bash script
            cmd: list[str | Path] = [
                "bash",
                "./build.sh",
                "--build",
                "--build-kit",
                self.build_kit,
                "--variant",
                self.variant,
                "--target",
                target,
                "--reconfigure",
            ]
            if self.build_type:
                cmd.extend(["--build-type", self.build_type])
            if additional_args:
                cmd.extend(additional_args)
            # Cast to Union[str, List[Union[str, Path]]] to satisfy SubprocessExecutor type
            result = SubprocessExecutor(command=cmd).execute(handle_errors=False)
            if result is None:
                return_code = -1
                break
            return_code = result.returncode
            if result.returncode:
                if result.stdout:
                    if any(error in str(result.stdout) for error in ["No valid floating license", "No valid license", "GHS_LMHOST = N/A"]):
                        print("Probably a license issue, retrying ...")
                        time.sleep(10)
                    else:
                        break
                else:
                    break
            else:
                break
        return return_code


class Test_Disco:
    variant: str = "Disco"
    components = [
        "components/power_signal_processing",
        "components/light_controller",
        "components/power_button",
        "components/main_control_knob",
    ]

    @pytest.fixture(scope="class")
    def archiver(self) -> Generator[ArtifactsArchiver, None, None]:
        archiver_instance = ArtifactsArchiver()
        out_dir = Path("build", self.variant)
        archiver_instance.add_archive(
            out_dir=out_dir,
            archive_filename=self.variant + ".7z",
        )
        yield archiver_instance
        # Create archive and RT upload JSON after all tests in the class have completed
        archiver_instance.create_archive()

    @pytest.mark.parametrize(
        ("build_type"),
        [
            pytest.param("Debug", marks=pytest.mark.build_debug),
            pytest.param("Release", marks=pytest.mark.build_release),
        ],
    )
    def test_build(self, build_type, archiver: ArtifactsArchiver):
        # Arrange
        spl_build: SplBuild = SplBuildLinux(
            variant=self.variant,
            build_kit="prod",
            build_type=build_type,
            target="all",
        )
        artifacts = spl_build.get_variant_artifacts()
        artifacts.extend(
            [
                spl_build.build_dir / "spled.exe",
                spl_build.build_dir / "kconfig",
            ]
        )
        archiver.register(artifacts=artifacts)

        # Act
        result = spl_build.execute()

        # Assert
        assert result == 0, "Building failed"

        for artifact in artifacts:
            assert artifact.exists(), f"Variant artifact {artifact} does not exist"

    @pytest.mark.unittests
    def test_unittests(self):
        # Arrange
        spl_build: SplBuild = SplBuildLinux(
            variant=self.variant,
            build_kit="test",
            build_type="Debug",
            target="unittests",
        )

        # Act
        result = spl_build.execute()

        # Assert
        assert result == 0, "Building unittests failed"
        artifacts = spl_build.get_components_artifacts(self.components)
        for artifact in artifacts:
            assert artifact.exists(), f"Artifact {artifact} does not exist"

    @pytest.mark.reports
    def test_reports(self, archiver: ArtifactsArchiver):
        # Arrange
        spl_build: SplBuild = SplBuildLinux(
            variant=self.variant,
            build_kit="test",
            build_type="Debug",
            target="reports",
        )
        archiver.register(artifacts=[spl_build.build_dir / "reports/html"])

        # Act
        result = spl_build.execute()

        # Assert
        assert result == 0, "Building reports failed"
        artifacts = spl_build.get_components_artifacts(self.components)
        for artifact in artifacts:
            assert artifact.exists(), f"Artifact {artifact} does not exist"
