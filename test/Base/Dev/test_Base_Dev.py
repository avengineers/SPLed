from pathlib import Path
from typing import Generator

import pytest
from spl_core.test_utils.spl_build import SplBuild
from spl_core.test_utils.artifacts_archiver import ArtifactsArchiver


class Test_Base__Dev:
    variant: str = "Base/Dev"
    components: list[str] = [
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
        # Create artifacts catalog
        archiver_instance.create_artifacts_json(self.variant, out_dir)
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
    @pytest.mark.gate_develop_pr
    @pytest.mark.gate_develop_push
    @pytest.mark.gate_develop_nightly
    @pytest.mark.gate_release_pr
    @pytest.mark.gate_release
    def test_reports(self, archiver: ArtifactsArchiver):
        # Arrange
        spl_build: SplBuild = SplBuild(
            variant=self.variant,
            build_kit="test",
            build_type="Debug",
            target="reports",
        )
        artifacts_to_be_archived = [spl_build.build_dir / "reports/html"]
        for component in self.components:
            artifacts_to_be_archived.append(spl_build.build_dir / component / "junit.xml")
            artifacts_to_be_archived.append(spl_build.build_dir / component / "coverage.json")
        # add variant-level reports
        artifacts_to_be_archived.append(spl_build.build_dir / "variant-coverage.json")
        artifacts_to_be_archived.append(spl_build.build_dir / "variant-junit.xml")
        archiver.register(artifacts=artifacts_to_be_archived)

        # Act
        result = spl_build.execute()

        # Assert
        assert result == 0, "Building reports failed"
        expected_artifacts = spl_build.get_components_artifacts(self.components) + artifacts_to_be_archived
        for artifact in expected_artifacts:
            assert artifact.exists(), f"Artifact {artifact} does not exist"
