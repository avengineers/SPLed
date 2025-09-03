from pathlib import Path
from typing import Generator

import pytest
from spl_core.test_utils.spl_build import SplBuild
from spl_core.test_utils.artifacts_archiver import ArtifactsArchiver


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
        spl_build: SplBuild = SplBuild(
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
        spl_build: SplBuild = SplBuild(
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
        spl_build: SplBuild = SplBuild(
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
