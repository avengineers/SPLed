import os.path
from utils import build_and_expect_default, build_unittests_and_expect_success, build_reports_and_expect_success


class Test_CustA__Disco:
    @classmethod
    def setup_class(cls):
        cls.variant = "CustA/Disco"

    def test_unit_tests(self):
        build_unittests_and_expect_success(self.variant)

        """Coverage report shall be created"""
        assert os.path.isfile(f"build/{self.variant}/test/coverage/index.html")

    def test_build(self):
        build_and_expect_default(self.variant)

    def test_reports(self):
        build_reports_and_expect_success(self.variant)
