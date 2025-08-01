import pytest
from spl_core.test_utils.spl_build import SplBuild
from spl_core.test_utils.archive_artifacts_collection import ArchiveArtifactsCollection


class Test_Sleep:
    variant: str = "Sleep"
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
        spl_build: SplBuild = SplBuild(
            variant=self.variant,
            build_kit="prod",
            build_type=build_type,
            target="all",
        )

        # Act
        result = spl_build.execute()

        # Assert
        assert result == 0, "Building failed"

        artifacts = spl_build.get_variant_artifacts()
        artifacts.extend(
            [
                spl_build.build_dir / "spled.exe",
                spl_build.build_dir / "kconfig",
            ]
        )
        for artifact in artifacts:
            assert artifact.exists(), f"Variant artifact {artifact} does not exist"

        artifacts_collection = ArchiveArtifactsCollection(artifacts=artifacts, build_dir=spl_build.build_dir)
        assert artifacts_collection.create_archive(zip_filename=self.variant).exists(), "Artifacts archive creation failed"
        assert artifacts_collection.create_json(json_filename=self.variant).exists(), "Artifacts JSON creation failed"

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
    def test_reports(self):
        # Arrange
        spl_build: SplBuild = SplBuild(
            variant=self.variant,
            build_kit="test",
            build_type="Debug",
            target="reports",
        )

        # Act
        result = spl_build.execute()

        # Assert
        assert result == 0, "Building reports failed"
        artifacts = spl_build.get_components_artifacts(self.components)
        for artifact in artifacts:
            assert artifact.exists(), f"Artifact {artifact} does not exist"
