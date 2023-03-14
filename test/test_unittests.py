import os.path
import json
from utils import run_process


def test_unit_tests_in_alpha():
    variant = "spl/alpha"
    build_unittests_and_expect_files(variant)

    iut = "build/{variant}/test/src/app/component/var_a/".format(variant=variant)

    """Test reports shall be created."""
    assert os.path.isfile(iut + "junit.xml")

    """Coverage data for each object shall be generated"""
    assert os.path.isfile(iut + "CMakeFiles/src_app_component_var_a.dir/src/component.c.obj")
    assert os.path.isfile(iut + "CMakeFiles/src_app_component_var_a.dir/src/component.c.gcno")
    assert os.path.isfile(iut + "CMakeFiles/src_app_component_var_a.dir/src/component.c.gcda")

    """Coverage report shall be created for each component"""
    assert os.path.isfile(iut + "coverage.json")
    f = open(iut + "coverage.json")
    json_data = json.loads(f.read())
    assert "src/app/component/var_a/src/component.c" == json_data["files"][0]["file"]
    f.close()
    assert os.path.isfile(iut + "coverage/index.html")


def test_unit_tests_in_beta():
    variant = "spl/beta"
    build_unittests_and_expect_files(variant)


def build_unittests_and_expect_files(variant):
    exit_code = run_process(["build.bat", "-variants", variant, "-target", "unittests", "-reconfigure"])

    """Unit tests execution shall pass."""
    assert 0 == exit_code

    """Coverage report shall be created"""
    expected_file = f"build/{variant}/test/coverage/index.html"
    assert os.path.isfile(expected_file)
