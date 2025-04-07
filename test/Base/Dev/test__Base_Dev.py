from pathlib import Path
import pytest
from spl_core.test_utils.base_variant_test_runner import BaseVariantTestRunner


class Test_Base__Dev(BaseVariantTestRunner):
    @property
    def component_paths(self):
        return [
            Path("src/examples/component_a"),
        ]

    @property
    def expected_build_artifacts(self):
        return []

    @pytest.mark.unittests
    def test_unittests(self):
        super().test_unittests(build_type="Debug")

    @pytest.mark.build
    def test_build(self):
        pass

    @pytest.mark.reports
    def test_reports(self):
        super().test_reports(build_type="Debug")
