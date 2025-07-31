import pytest
from spl_core.test_utils.spl_build import SplBuild


class Test_Spa:
    variant: str = "Spa"
    components = [
        "components/power_signal_processing",
        "components/light_controller",
        "components/power_button",
        "components/main_control_knob",
        "components/brightness_controller",
    ]

    @pytest.mark.build
    @pytest.mark.parametrize(
        ("build_type"),
        [
            pytest.param("Debug", marks=pytest.mark.debug),
            pytest.param("Release", marks=pytest.mark.release),
        ],
    )
    def test_build(self, build_type):
        # Arrange
        spl_build: SplBuild = SplBuild(variant=self.variant, build_kit="prod", build_type=build_type, target="all")

        # Act
        result = spl_build.execute()

        # Assert
        assert result == 0, "Building failed"
        artifacts = spl_build.get_variant_artifacts()
        artifacts.append(spl_build.build_dir / "spled.exe")
        for artifact in artifacts:
            assert artifact.exists(), f"Artifact {artifact} does not exist"

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
