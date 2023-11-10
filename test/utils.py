import glob
import re
from subprocess import Popen, PIPE, STDOUT
from pathlib import Path
import pathlib
import subprocess
from typing import Dict, List, Optional
import os
import json
import zipfile


class CommandLineExecutor:
    def __init__(
        self,
        cmd: str | List[str],
        cwd: Optional[Path] = None,
        env: Dict[str, str] = None,
    ):
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
            with Popen(
                self.command,
                cwd=str(self.current_working_directory or Path.cwd()),
                stdout=PIPE,
                stderr=STDOUT,
                bufsize=1,
                text=True,
                env=self.env,
                universal_newlines=True,
            ) as process:
                for line in process.stdout:
                    print(line, end="")
        except Exception:
            raise RuntimeError(f"Command '{self.command}' failed.")
        return process


class ArtifactsCollection:
    def __init__(self, variant: str):
        self.variant = variant
        self.artifacts = []

    def collect(self, artifact_path: str):
        """Collect an artifact."""
        if "*" in artifact_path:
            self.artifacts.extend(glob.glob(artifact_path))
        else:
            self.artifacts.append(artifact_path)

    def create_bom(self, bom_json_path: Path):
        """Create a BOM (Bill of Materials) for the collected artifacts."""
        bom = {
            "variant": self.variant,
            "artifacts": [os.path.basename(artifact) for artifact in self.artifacts],
        }

        try:
            with open(bom_json_path, "w") as bom_file:
                json.dump(bom, bom_file, indent=4)
            print(f"BOM file created at: {bom_json_path}")
        except Exception as e:
            print(f"Error creating BOM file: {e}")

    def create_artifacts_zip(self, artifacts_zip_path: Path):
        """Create a zip file containing the collected artifacts."""
        try:
            with zipfile.ZipFile(artifacts_zip_path, "w") as zip_file:
                for artifact in self.artifacts:
                    """Adjust the arcname as needed to control the path inside the zip file"""
                    zip_file.write(
                        artifact,
                        arcname=os.path.basename(artifact))
            print(f"Artifacts zip file created at: {artifacts_zip_path}")
        except Exception as e:
            print(f"Error creating artifacts zip file: {e}")


def spl_build(variant: str, build_kit: str, target: str):
    if build_kit == "test":
        delete_gcov_data_files(variant)

    """build wrapper shall build target and related outputs."""
    result = CommandLineExecutor(
        [
            "build.bat",
            "-buildKit",
            build_kit,
            "-variants",
            variant,
            "-target",
            target,
            "-reconfigure",
        ]
    ).execute()
    return result.returncode


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
