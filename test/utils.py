import os
from subprocess import Popen, PIPE, STDOUT
from pathlib import Path
import pathlib
import subprocess
import time
from typing import Dict, List, Optional
import os.path
from junitparser import JUnitXml


class CommandLineExecutor:
    def __init__(self, cmd: str | List[str], cwd: Optional[Path] = None, env: Dict[str, str] = None):
        """
        A class for executing command line commands.

        Args:
        - cmd: A string or list of strings representing the command to be executed.
        - cwd: An optional Path object representing the current working directory.
        - env: An optional dictionary of environment variables to be used in the command execution.
        """
        self.command = " ".join([cmd] if isinstance(cmd, str) else cmd)
        self.current_working_directory = cwd
        self.env = env

    def execute(self) -> subprocess.CompletedProcess:
        """
        Executes the command and returns a CompletedProcess object.

        Returns:
        - A subprocess.CompletedProcess object representing the result of the command execution.
        """
        try:
            print(f"Running command: {self.command}")
            with Popen(self.command, cwd=str(self.current_working_directory or Path.cwd()), stdout=PIPE, stderr=STDOUT, bufsize=1, text=True, env=self.env, universal_newlines=True) as process:
                for line in process.stdout:
                    print(line, end="")
        except Exception:
            raise RuntimeError(f"Command '{self.command}' failed.")
        return process


def build_unittests_and_expect_success(variant):
    delete_gcov_data_files(variant)

    """Unit tests execution shall be successful."""
    assert 0 == (
        CommandLineExecutor(
            [
                "build.bat",
                "-buildKit",
                "test",
                "-variants",
                variant,
                "-target",
                "unittests",
                "-reconfigure",
            ]
        )
        .execute()
        .returncode
    )

    """Coverage report shall be created"""
    assert os.path.isfile(f"build/{variant}/test/reports/coverage/index.html")


def build_reports_and_expect_success(variant):
    delete_gcov_data_files(variant)

    """Reports generation shall be successful."""
    assert 0 == (
        CommandLineExecutor(
            [
                "build.bat",
                "-buildKit",
                "test",
                "-variants",
                variant,
                "-target",
                "all",
                "-reconfigure",
            ]
        )
        .execute()
        .returncode
    )


def get_scoop_app_directory(app_name: str) -> Optional[Path]:
    command = f"scoop which {app_name}"
    process = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
    output, _ = process.communicate()
    app_path = output.decode().strip()
    if app_path:
        return Path(os.path.expanduser(app_path)).parent.absolute()
    return None


def find_junit_reports(variant) -> List[Path]:
    out_dir = Path(f"build/{variant}/test/src")
    junit_reports = []
    for junit_report in out_dir.glob("**/junit.xml"):
        print(f"Found junit report: {junit_report}")
        junit_reports.append(junit_report)
    return junit_reports


def delete_gcov_data_files(variant) -> None:
    """
    Deletes all .gcda files found in the build directory for the specified variant.

    Args:
        variant (str): The build variant to clean up.

    Returns:
        None
    """
    out_dir = pathlib.Path(f"build/{variant}/test/src")

    for file in out_dir.glob("**/*.gcda"):
        print(f"Found coverage file: {file}")
        file.unlink()


def create_variant_specific_junit_report(variant: str, junit_report_file: Path):
    test_name = variant.replace("\\", "/").replace("/", "_")
    xml = JUnitXml.fromfile(str(junit_report_file))
    for suite in xml:
        suite.name = f"{test_name}"
    xml.write()


def build_and_expect_default(variant: str, prepackaging_artifacts_basenames: List[str] = [], target: str = "all"):
    """build wrapper shall build target and related outputs."""

    while True:
        result = CommandLineExecutor(["build.bat", "-variants", variant, "-target", target, "-reconfigure"]).execute()

        stdout_and_stderr_content = str(result.stderr) if result.stderr else ""
        if result.stdout:
            stdout_and_stderr_content += str(result.stdout)
        if result.returncode:
            if stdout_and_stderr_content:
                if "GHS_LMHOST = N/A" in stdout_and_stderr_content or "No valid floating license" in stdout_and_stderr_content:
                    print("Probably a license issue, retrying ...")
                    time.sleep(10)
                else:
                    break
            else:
                break
        else:
            break

    assert 0 == result.returncode
    for file_type in ["exe"]:
        # TODO: excuse me what? Why is the executable name hardcoded here?
        expect_binary(variant, file_name=f"spled.{file_type}")
    for file_type in ["hex", "bin", "crc"]:
        for artifact_basename in prepackaging_artifacts_basenames:
            expect_binary(variant, file_name=f"{artifact_basename}.{file_type}")


def expect_binary(variant, bin_type="elf", suffix="", file_name=""):
    """Hex file of given configuration shall exist."""
    assert_expected_file_exists(get_artifact_name(variant, bin_type, suffix, file_name))


def assert_expected_file_exists(expected_file):
    assert os.path.isfile(expected_file)


@staticmethod
def get_artifact_name(variant, bin_type="elf", suffix="", file_name="") -> Path:
    """Hex file of given configuration shall exist."""
    file = file_name if file_name else f"{variant.replace('/', '_')}{suffix}.{bin_type}"
    return Path(f"build/{variant}/prod/{file}")
