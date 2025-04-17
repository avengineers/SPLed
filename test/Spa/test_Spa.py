from pathlib import Path
import pytest
from spl_core.test_utils.base_variant_test_runner import BaseVariantTestRunner


class Test_Spa(BaseVariantTestRunner):
    @property
    def component_paths(self):
        return [
            # Path("src/spled"),
            Path("src/power_signal_processing"),
            Path("src/light_controller"),
            Path("src/keyboard_interface"),
            # Path("src/console_interface"),
            Path("src/main_control_knob"),
            Path("src/brightness_controller"),
        ]

    @property
    def expected_build_artifacts(self):
        return [Path("spled.exe"), Path("compile_commands.json")]

    @pytest.mark.build
    def test_build(self):
        super().test_build(build_type="Debug")

    @pytest.mark.unittests
    def test_unittests(self):
        pass

    @pytest.mark.reports
    def test_reports(self):
        pass
