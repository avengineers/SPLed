import os.path
from utils import build_and_expect_default, build_unittests_and_expect_success, build_reports_and_expect_success


class Test_CustB__Sleep:
    @classmethod
    def setup_class(cls):
        cls.variant = "CustB/Sleep"

    def test_unit_tests(self):
        build_unittests_and_expect_success(self.variant)

    def test_build(self):
        build_and_expect_default(self.variant)

    def test_reports(self):
        build_reports_and_expect_success(self.variant)

        """SWE.4 reports shall be created"""
        report_types = ["html", "coverage"]
        modules = ["brightness_controller", "keyboard_interface", "light_controller", "main_control_knob", "power_signal_processing", "spled"]
        for report_type in report_types:
            for module in modules:
                assert os.path.isfile(f"build/{self.variant}/test/src/{module}/reports/{report_type}/index.html")
