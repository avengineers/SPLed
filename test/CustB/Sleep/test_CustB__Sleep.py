import os.path
from utils import build_and_expect_default, build_unittests_and_expect_success, build_reports_and_expect_success


class Test_CustB__Sleep:
    @classmethod
    def setup_class(cls):
        cls.variant = "CustB/Sleep"

    def test_unit_tests(self):
        build_unittests_and_expect_success(self.variant)

        """Coverage report shall be created"""
        assert os.path.isfile(f"build/{self.variant}/test/coverage/index.html")

    def test_build(self):
        build_and_expect_default(self.variant)

    def test_reports(self):
        build_reports_and_expect_success(self.variant)
        
        """SWE.4 reports shall be created"""
        assert os.path.isfile(f"build/{self.variant}/test/src/brightness_controller/reports/html/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/keyboard_interface/reports/html/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/light_controller/reports/html/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/main_control_knob/reports/html/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/power_signal_processing/reports/html/index.html")
        assert os.path.isfile(f"build/{self.variant}/test/src/spled/reports/html/index.html")
