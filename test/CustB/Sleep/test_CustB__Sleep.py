import json
import os.path
from pathlib import Path
from utils import ArtifactsCollection, spl_build


class Test_CustB__Sleep:
    @classmethod
    def setup_class(cls):
        cls.variant = "CustB/Sleep"

    def test_unit_tests(self):
        """Unit tests execution shall be successful."""
        assert 0 == spl_build(self.variant, "test", "unittests")

        """Coverage report shall be created"""
        assert os.path.isfile(f"build/{self.variant}/test/reports/coverage/index.html")

    def test_build(self):
        """build wrapper shall build target and related outputs."""
        assert 0 == spl_build(self.variant, "prod", "all")

        """executable shall exist and collected for the bom."""
        artifacts_collection = ArtifactsCollection(self.variant)
        artifacts_collection.collect(f"build/{self.variant}/*/*.exe")

        """executable shall exist and collected for the BOM."""
        assert len(artifacts_collection.artifacts) > 0

        """BOM shall be created"""
        bom_json_path = Path(f"build/{self.variant}/prod/bom.json")
        artifacts_collection.create_bom(bom_json_path)
        assert bom_json_path.is_file()

        """BOM shall contain the expected artifacts"""
        expected_artifacts = [
            os.path.basename(artifact) for artifact in artifacts_collection.artifacts
        ]
        with open(bom_json_path, "r") as bom_file:
            bom = json.load(bom_file)
            assert bom["variant"] == self.variant
            assert set(bom["artifacts"]) == set(expected_artifacts)

        """variant's artifacts zip file shall be created"""
        variant_path = self.variant.replace("/", "_")
        artifacts_zip_path = Path(f"build/{self.variant}/prod/{variant_path}.zip")
        artifacts_collection.create_artifacts_zip(artifacts_zip_path)
        assert artifacts_zip_path.is_file()

    def test_reports(self):
        """Reports generation shall be successful."""
        assert 0 == spl_build(self.variant, "test", "reports")

        """SWE.4 reports shall be created"""
        report_types = ["html", "coverage"]
        modules = [
            "brightness_controller",
            "keyboard_interface",
            "light_controller",
            "main_control_knob",
            "power_signal_processing",
            "spled",
        ]
        for report_type in report_types:
            for module in modules:
                assert os.path.isfile(
                    f"build/{self.variant}/test/src/{module}/reports/{report_type}/index.html"
                )
