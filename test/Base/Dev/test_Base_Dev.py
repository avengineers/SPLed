from pathlib import Path
from typing import Generator

import pytest
from spl_core.test_utils.spl_build import SplBuild
from spl_core.test_utils.artifacts_archiver import ArtifactsArchiver


class Test_Base__Dev:
    variant: str = "Base/Dev"
    components = [
        "components/examples/hello_gmock",
        "components/examples/flight_controller",
    ]

    @pytest.fixture(scope="class")
    def archiver(self) -> Generator[ArtifactsArchiver, None, None]:
        archiver_instance = ArtifactsArchiver()
        out_dir = Path("build", self.variant)
        archiver_instance.add_archive(
            out_dir=out_dir,
            archive_filename=self.variant.replace("/", "__") + ".7z",
        )
        yield archiver_instance
        # Create archive and RT upload JSON after all tests in the class have completed
        archiver_instance.create_archive()

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
