import os.path
from utils import build_and_expect_default, build_unittests_and_expect_success, build_reports_and_expect_success


class Test_CustA__Disco:
    @classmethod
    def setup_class(cls):
        cls.variant = "CustA/Disco"

    def test_unit_tests(self):
        build_unittests_and_expect_success(self.variant)

    def test_build(self):
        build_and_expect_default(self.variant)

    def test_reports(self):
        build_reports_and_expect_success(self.variant)

        """SWE.4 reports shall be created"""
        assert os.path.isfile(f"build/{self.variant}/test/src/keyboard_interface/reports/html/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/keyboard_interface/reports/coverage/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/light_controller/reports/html/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/light_controller/reports/coverage/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/main_control_knob/reports/html/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/main_control_knob/reports/coverage/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/power_signal_processing/reports/html/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/power_signal_processing/reports/coverage/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/spled/reports/html/index.html")
