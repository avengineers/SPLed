import pytest
from spl_core.test_utils.spl_build import SplBuild


class Test_Base__Dev:
    variant: str = "Base/Dev"
    components = [
        "components/examples/hello_gmock",
        "components/examples/flight_controller",
    ]

    @pytest.mark.unittests
    def test_unittests(self):
        # Arrange
        spl_build: SplBuild = SplBuild(variant=self.variant, build_kit="test", build_type="Debug", target="unittests")

        # Act
        result = spl_build.execute()

        # Assert
        assert result == 0, "Building unittests failed"
        artifacts = spl_build.get_components_artifacts(self.components)
        for artifact in artifacts:
            assert artifact.exists(), f"Artifact {artifact} does not exist"

    @pytest.mark.reports
    def test_reports(self):
        # Arrange
        spl_build: SplBuild = SplBuild(variant=self.variant, build_kit="test", build_type="Debug", target="reports")

        # Act
        result = spl_build.execute()

        # Assert
        assert result == 0, "Building reports failed"
        artifacts = spl_build.get_components_artifacts(self.components)
        for artifact in artifacts:
            assert artifact.exists(), f"Artifact {artifact} does not exist"
