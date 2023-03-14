from datetime import time
import os.path
from pathlib import Path
from utils import run_process


def test_build_spl__alpha():
    variant = "spl/alpha"
    build_and_expect_default(variant)
    expect_binary(variant, file_name="my_main.zip")


def test_build_spl__beta():
    variant = "spl/beta"
    build_and_expect_default(variant)


def test_build_spl__gamma():
    variant = "spl/gamma"
    build_and_expect_default(variant)
    expect_binary(variant, file_name="my_main.zip")


def build_and_expect_default(variant, target="all"):
    """build wrapper shall build the default target and related outputs."""

    while True:
        exit_code = run_process(["build.bat", "-variants", variant, "-target", target, "-reconfigure"])

        if exit_code:
            file_path = "build/{}/prod/CMakeFiles/CMakeError.log".format(variant)
            if Path(file_path).exists():
                with open(file_path) as f:
                    log_file_content = f.read()
                    if "GHS_LMHOST = N/A" in log_file_content or "No valid floating license" in log_file_content:
                        print("probably a license issue, retrying ...")
                        time.sleep(10)
                    else:
                        break
            else:
                break
        else:
            break

    assert 0 == exit_code
    expect_binary(variant, file_name="my_main.exe")


def expect_binary(variant, bin_type="elf", suffix="", file_name=""):
    """Hex file of given configuration shall exist."""
    assert_expected_file_exists(get_artifact_name(variant, bin_type, suffix, file_name))


def assert_expected_file_exists(expected_file):
    "File {expected_file} shall exist."
    assert os.path.isfile(expected_file)


@staticmethod
def get_artifact_name(variant, bin_type="elf", suffix="", file_name="") -> Path:
    """Hex file of given configuration shall exist."""
    file = file_name if file_name else f"{variant.replace('/', '_')}{suffix}.{bin_type}"
    return Path(f"build/{variant}/prod/{file}")
