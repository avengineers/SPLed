import dataclasses
import os
import random
import shutil
import string
import subprocess
from contextlib import ContextDecorator
from pathlib import Path
from time import perf_counter
from typing import Dict, Collection

from src.common.cmake import CMake
from src.creator.creator import Creator
from src.creator.variant import Variant
from src.creator.workspace_artifacts import WorkspaceArtifacts


class ExecutionTime(ContextDecorator):
    def __init__(self, message: str = None):
        self.name = message

    def __enter__(self):
        self.time = perf_counter()
        print(f'[START] {self.name}')
        return self

    def __exit__(self, type, value, traceback):
        self.time = perf_counter() - self.time
        print(f'[END] {self.name} execution took {self.time:.3f}s')


@dataclasses.dataclass
class TestDir:
    path: Path

    def write_file(self, name: str, content: str = None) -> Path:
        file = self.path.joinpath(name)
        file.parent.mkdir(parents=True, exist_ok=True)
        file.write_text(content if content else self.gen_random_text(10))
        return file

    def delete_file(self, name: str):
        self.path.joinpath(name).unlink()

    @staticmethod
    def gen_random_text(size: int) -> str:
        return ''.join(random.choices(string.ascii_uppercase + string.digits, k=size))

    def joinpath(self, path: str) -> Path:
        return self.path.joinpath(path)

    def __str__(self):
        return f"{self.path}"


class TestUtils:
    DEFAULT_TEST_DIR = 'tmp_test'

    @staticmethod
    def create_clean_test_dir(name: str = None) -> TestDir:
        out_dir = TestUtils.this_repository_root_dir().joinpath('out')
        test_dir = out_dir.joinpath(name if name else TestUtils.DEFAULT_TEST_DIR).absolute()
        if test_dir.exists():
            # rmtree throws an exception if any of the files to be deleted is read-only
            if os.name == 'nt':
                rm_dir_cmd = f"cmd /c rmdir /S /Q {test_dir}"
                print(f"Execute: {rm_dir_cmd}")
                subprocess.call(rm_dir_cmd)
            else:
                shutil.rmtree(test_dir)
        test_dir.mkdir(parents=True, exist_ok=True)
        print(f"New clean test directory created: {test_dir}")
        return TestDir(test_dir)

    @staticmethod
    def this_repository_root_dir() -> Path:
        return Path(__file__).parent.parent.absolute()

    @staticmethod
    def force_spl_core_usage_to_this_repo():
        os.environ['SPLCORE_PATH'] = TestUtils.this_repository_root_dir().as_posix()
        os.environ['PIPENV_PIPFILE'] = TestUtils.this_repository_root_dir().as_posix() + '/Pipfile'


@dataclasses.dataclass
class DirectoryStatus:
    changed_files: Collection[Path]
    deleted_files: Collection[Path]
    new_files: Collection[Path]
    unchanged_files: Collection[Path]

    @property
    def changed_files_names(self) -> Collection[str]:
        return [file.name for file in self.changed_files]


class DirectoryTracker:
    def __init__(self, target_dir: Path):
        self.target_dir = target_dir
        self.start_status = self._collect_files_status()

    def reset_status(self):
        self.start_status = self._collect_files_status()

    def _collect_files_status(self) -> Dict:
        """
        Store a set with all files and their timestamps
        """
        status = {}
        for file in self.target_dir.glob('**/*'):
            if Path(file).is_file():
                status[file] = os.stat(file).st_mtime_ns
        return status

    def get_status(self) -> DirectoryStatus:
        current_status = self._collect_files_status()
        common_files = current_status.keys() & self.start_status.keys()
        deleted_files = self.start_status.keys() - current_status.keys()
        new_files = current_status.keys() - self.start_status.keys()
        changed_files = []
        unchanged_files = []
        for file in common_files:
            if current_status[file] != self.start_status[file]:
                changed_files.append(file)
            else:
                unchanged_files.append(file)
        status = DirectoryStatus(changed_files, deleted_files, new_files, unchanged_files)
        return status


class TestWorkspace:
    DEFAULT_VARIANT = Variant('Flv1', 'Sys1')

    def __init__(self, out_dir_name: str):
        self.workspace_dir = self.create_my_workspace(out_dir_name)
        self.workspace_artifacts = WorkspaceArtifacts(self.workspace_dir)
        self.directory_tracker = DirectoryTracker(self.workspace_dir)
        self.use_local_spl_core = True

    @staticmethod
    def create_my_workspace(out_dir_name: str) -> Path:
        out_dir = TestUtils.create_clean_test_dir(out_dir_name)
        project_name = 'MyProject'
        creator = Creator(project_name, out_dir.path)
        variants = [
            TestWorkspace.DEFAULT_VARIANT,
            Variant('Flv1', 'Sys2')
        ]
        return creator.materialize(variants)

    def install_mandatory(self):
        pass

    def link(self, variant: Variant = DEFAULT_VARIANT) -> subprocess.CompletedProcess:
        return self.execute_command(f"{self.workspace_artifacts.build_script}"
                                    f" -build -target link -variants {variant}")

    def run_cmake(self, target: str, variant: Variant = DEFAULT_VARIANT) -> subprocess.CompletedProcess:
        return CMake(self.workspace_artifacts).build(variant, target=target)

    def execute_command(self, command: str) -> subprocess.CompletedProcess:
        if self.use_local_spl_core:
            TestUtils.force_spl_core_usage_to_this_repo()
        return subprocess.run(command.split())

    def get_component_file(self, component_name: str, component_file: str) -> Path:
        return self.workspace_artifacts.get_component_path(component_name).joinpath(component_file)

    def take_files_snapshot(self):
        self.directory_tracker.reset_status()

    def get_workspace_files_status(self) -> DirectoryStatus:
        return self.directory_tracker.get_status()
