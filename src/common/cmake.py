import logging
import subprocess
from subprocess import CompletedProcess

from src.creator.variant import Variant
from src.creator.workspace_artifacts import WorkspaceArtifacts


class CMake:
    executable = "cmake"

    def __init__(self, workspace_artifacts: WorkspaceArtifacts):
        self.logger = logging.getLogger(__name__)
        self.workspace_artifacts = workspace_artifacts

    def run(self, variant: Variant, build_kit: str = "prod", target: str = "all"):
        ret_status = self.configure(variant, build_kit)
        if ret_status.returncode == 0:
            ret_status = self.build(variant, build_kit, target)
        return ret_status

    def configure(self, variant: Variant, build_kit: str = "prod"):
        arguments = (f" -DBUILD_KIT:STRING={build_kit}"
                     f" -DFLAVOR:STRING={variant.flavor}"
                     f" -DSUBSYSTEM:STRING={variant.subsystem}"
                     f" -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE"
                     f" -DCMAKE_BUILD_TYPE:STRING={variant.to_string('_')}"
                     f" -S{self.workspace_artifacts.root_dir}"
                     f" -B{self.workspace_artifacts.get_build_dir(variant, build_kit)}"
                     f" -G Ninja ")
        return self.run_cmake(arguments)

    def build(self, variant: Variant, build_kit: str = "prod", target: str = "all") -> CompletedProcess:
        arguments = (f" --build {self.workspace_artifacts.get_build_dir(variant, build_kit)}"
                     f" --config {variant.to_string('_')}"
                     f" --target {target} -- ")
        return self.run_cmake(arguments)

    def run_cmake(self, arguments: str) -> CompletedProcess:
        command = self.executable + " " + arguments
        print(f"Running {command}")
        return subprocess.run(command.split())
