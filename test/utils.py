import json
from subprocess import Popen, PIPE, STDOUT
from pathlib import Path
import subprocess
from typing import Dict, List, Optional
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
        Executes the given command.

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


# TODO: Determine expected artifacts out of the feature model. Currently, they are hardcoded in the test.
class ArtifactsCollection:
    def __init__(self, artifacts: list[str]):
        self.artifacts = artifacts

    def __iter__(self):
        return iter(self.artifacts)

    def assert_exists_in(self, directory: Path):
        for artifact in self.artifacts:
            assert (directory / artifact).exists()


# TODO: Determine expected artifacts out of the feature model. Currently, they are hardcoded in the test.
class ComponentReportsCollection(ArtifactsCollection):
    def __init__(
        self, modules: list[str], report_types: list[str] = ["html", "coverage"]
    ):
        artifacts = []
        for module in modules:
            for report_type in report_types:
                artifacts.append(f"src/{module}/reports/{report_type}")
        super().__init__(artifacts)

    def assert_exists_in(self, directory: Path):
        for artifact in self.artifacts:
            assert (directory / artifact / "index.html").exists()


class SplBuild:
    """Class for building, expecting and archiving artifacts in an SPL repository."""

    def __init__(
        self, variant: str, build_kit: str, expected_artifacts: ArtifactsCollection
    ):
        """
        Initialize a SplBuild instance.

        Args:
            variant (str): The build variant.
            build_kit (str): The build kit.
            expected_artifacts (ArtifactsCollection): The artifacts expected to be created by the build.

        """
        self.variant = variant
        self.build_kit = build_kit
        self.expected_artifacts = expected_artifacts

    @property
    def build_dir(self):
        """
        Get the build directory.

        Returns:
            Path: The build directory path.

        """
        return Path(f"build/{self.variant}/{self.build_kit}")

    def create_artifacts_json(self):
        """
        Create a JSON file listing the collected artifacts.

        Returns:
            Path: The path to the created JSON file.

        Raises:
            Exception: If there is an error creating the JSON file.

        """
        json_content = {
            "variant": self.variant,
            "build_kit": self.build_kit,
            "artifacts": self.expected_artifacts.artifacts,
        }
        json_path = self.build_dir / "artifacts.json"

        # Delete the file if it already exists
        if json_path.exists():
            json_path.unlink()

        try:
            with open(json_path, "w") as bom_file:
                json.dump(json_content, bom_file, indent=4)
            print(f"JSON file created at: {json_path}")
            return json_path
        except Exception as e:
            print(f"Error creating JSON file: {e}")
            raise e

    def create_artifacts_zip(self):
        """
        Create a zip file containing the collected artifacts.

        Returns:
            Path: The path to the created zip file.

        Raises:
            Exception: If there is an error creating the zip file.

        """
        zip_path = self.build_dir / "artifacts.zip"

        # Delete the file if it already exists
        if zip_path.exists():
            zip_path.unlink()

        try:
            with zipfile.ZipFile(zip_path, "w") as zip_file:
                for expected_artifact in self.expected_artifacts:
                    artifact_path: Path = self.build_dir / expected_artifact
                    if artifact_path.is_dir():
                        for artifact in artifact_path.glob("**/*"):
                            if artifact.is_file():
                                zip_file.write(
                                    artifact,
                                    arcname=artifact.relative_to(self.build_dir),
                                )
                    else:
                        zip_file.write(artifact_path, arcname=expected_artifact)
            print(f"Zip file created at: {zip_path}")
            return zip_path
        except Exception as e:
            print(f"Error creating artifacts zip file: {e}")
            raise e

    def execute(self, target: str, strict: bool, archive: bool) -> int:
        """
        Build the target and therefore the expected artifacts.

        Args:
            target (str): The build target.
            strict (bool): Whether to check for expected artifacts.
            archive (bool): Whether to create an artifacts zip file.

        Returns:
            int: 0 in case of success.

        """
        assert (
            0
            == CommandLineExecutor(
                [
                    "build.bat",
                    "-buildKit",
                    self.build_kit,
                    "-variants",
                    self.variant,
                    "-target",
                    target,
                    "-reconfigure",
                ]
            )
            .execute()
            .returncode
        )

        assert self.create_artifacts_json().exists()

        if strict:
            self.expected_artifacts.assert_exists_in(self.build_dir)

        if archive:
            assert self.create_artifacts_zip().exists()

        return 0
