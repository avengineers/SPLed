import os.path
from utils import spl_build


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

        assert os.path.isfile(f"build/{self.variant}/prod/spled.exe")

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
