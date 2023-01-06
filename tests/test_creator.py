import json
import subprocess
from subprocess import CompletedProcess
from unittest.mock import patch

import pytest

from src.creator.creator import Creator, main, parse_arguments
from src.creator.variant import Variant
from src.creator.workspace_artifacts import WorkspaceArtifacts
from tests.utils import TestUtils, TestWorkspace


def execute_command(command: str) -> CompletedProcess:
    return subprocess.run(command.split())


class TestProjectGenerator:

    def test_materialize(self):
        out_dir = TestUtils.create_clean_test_dir('test_materialize')
        project_name = 'MyProject'
        creator = Creator(project_name, out_dir.path)
        variants = [
            Variant('Flv1', 'Sys1'),
            Variant('Flv1', 'Sys2')
        ]
        workspace_dir = creator.materialize(variants)

        assert workspace_dir == out_dir.joinpath(project_name)
        assert workspace_dir.joinpath('CMakeLists.txt').exists()
        assert workspace_dir.joinpath('variants/Flv1/Sys1/config.cmake').exists()
        assert workspace_dir.joinpath('variants/Flv1/Sys2/config.cmake').exists()
        cmake_variants_file = workspace_dir.joinpath(
            '.vscode/cmake-variants.json')
        assert cmake_variants_file.exists()
        cmake_variants_text = cmake_variants_file.read_text().strip()
        "There shall not be any empty lines in resulting json file."
        assert '' not in cmake_variants_text.replace('\r\n', '\n').split('\n')
        cmake_variants_json = json.loads(cmake_variants_text)
        assert len(cmake_variants_json['variant']['choices']) == 2
        assert 'Flv1/Sys1' in cmake_variants_json['variant']['choices']
        assert 'Flv1/Sys2' in cmake_variants_json['variant']['choices']
        assert 'Flv1/Sys3' not in cmake_variants_json['variant']['choices']
        assert 'flavor/subsystem' not in cmake_variants_json['variant']['choices']
        assert cmake_variants_json['variant']['default'] == 'Flv1/Sys1'
        "Shall be able to find existing variants from a project"
        assert creator.collect_project_variants() == variants

    def test_create_project_description(self):
        project_description = Creator.create_project_description(
            'SomeProject',
            [Variant('Flv1', 'Sys1'), Variant('Flv1', 'Sys2')]
        )
        assert project_description == {
            "name": "SomeProject",
            "touch_components": True,
            "touch_tools": True,
            "variants": {
                "0": {
                    "flavor": "Flv1",
                    "subsystem": "Sys1"
                },
                "1": {
                    "flavor": "Flv1",
                    "subsystem": "Sys2"
                }
            }
        }

    def test_parse_arguments_project(self):
        out_dir = TestUtils.create_clean_test_dir()
        user_arguments = [
            'workspace',
            '--name', 'MyProject1',
            '--variant', 'Flv1/Sys1',
            '--out_dir', f"{out_dir.path}"
        ]
        parsed_arguments = parse_arguments(user_arguments)
        assert parsed_arguments.name == 'MyProject1'
        assert parsed_arguments.out_dir == out_dir.path
        assert parsed_arguments.variant == [Variant('Flv1', 'Sys1')]

        "The variant argument can be used multiple times"
        user_arguments = [
            'workspace',
            '--name', 'MyProject1',
            '--variant', 'Flv1/Sys1',
            '--variant', 'Flv2/Sys2',
            '--out_dir', f"{out_dir.path}"
        ]
        parsed_arguments = parse_arguments(user_arguments)
        assert parsed_arguments.name == 'MyProject1'
        assert parsed_arguments.out_dir == out_dir.path
        assert parsed_arguments.variant == [Variant('Flv1', 'Sys1'), Variant('Flv2', 'Sys2')]

    def test_parse_arguments_variant_add(self):
        out_dir = TestUtils.create_clean_test_dir()
        user_arguments = [
            'variant',
            '--add', 'Flv1/Sys1',
            '--workspace_dir', f"{out_dir.path}"
        ]
        parsed_arguments = parse_arguments(user_arguments)
        assert parsed_arguments.add == [Variant('Flv1', 'Sys1')]
        assert parsed_arguments.delete is None
        assert parsed_arguments.workspace_dir == out_dir.path

    def test_parse_arguments_variant_delete(self):
        out_dir = TestUtils.create_clean_test_dir()
        user_arguments = [
            'variant',
            '--delete', 'Flv1/Sys1',
            '--workspace_dir', f"{out_dir.path}"
        ]
        parsed_arguments = parse_arguments(user_arguments)
        assert parsed_arguments.add is None
        assert parsed_arguments.delete == [Variant('Flv1', 'Sys1')]
        assert parsed_arguments.workspace_dir == out_dir.path

    def test_parse_arguments_invalid(self):
        out_dir = TestUtils.create_clean_test_dir()
        user_arguments = [
            'variant',
            '--name', 'MyProject',
            '--workspace_dir', f"{out_dir.path}"
        ]
        with pytest.raises(SystemExit) as raised_exception:
            parse_arguments(user_arguments)
        'exit code is not zero for invalid arguments'
        assert raised_exception.value.code is 2

    @pytest.mark.parametrize(
        "user_arguments",
        [
            (['--help']),
            (['workspace', '--help']),
            (['variant', '--help'])
        ],
    )
    def test_parse_arguments_help(self, user_arguments):
        with pytest.raises(SystemExit) as raised_exception:
            parse_arguments(user_arguments)
        'exit code is zero for valid arguments'
        assert raised_exception.value.code is 0

    def test_add_variant(self):
        workspace_dir = TestWorkspace.create_my_workspace('test_add_variant')
        workspace_dir.joinpath('variants/Flv1/Sys1/config.txt').write_text('')
        creator = Creator.from_folder(workspace_dir)
        creator.add_variants([Variant('Flv2', 'NewSys')])
        'Existing flavors shall not be overwritten'
        assert workspace_dir.joinpath('variants/Flv1/Sys1/config.txt').read_text() == ''
        'New flavor files are generated'
        assert workspace_dir.joinpath('variants/Flv2/NewSys/config.cmake').exists()
        cmake_variants_file = workspace_dir.joinpath(
            '.vscode/cmake-variants.json')
        assert cmake_variants_file.exists()
        cmake_variants_json = json.loads(cmake_variants_file.read_text().strip())
        assert len(cmake_variants_json['variant']['choices']) == 3

    def test_delete_variant(self):
        workspace_dir = TestWorkspace.create_my_workspace('test_delete_variant')
        creator = Creator.from_folder(workspace_dir)
        creator.delete_variants([Variant('Flv1', 'Sys2')])
        'Existing flavors shall not be overwritten'
        assert workspace_dir.joinpath('variants/Flv1/Sys1/config.txt').exists()
        assert not workspace_dir.joinpath('variants/Flv1/Sys2/config.txt').exists()


def test_create_project_running_main():
    """
    Generate, build and run unit tests for all flavors.
    """
    out_dir = TestUtils.create_clean_test_dir('test_create_project_running_main')
    with patch('sys.argv', [
        'some_name',
        'workspace',
        '--name', 'MyProject1',
        '--variant', 'Flv1/Sys1',
        '--variant', 'Flv1/Sys2',
        '--out_dir', f"{out_dir.path}"
    ]):
        main()

    project_dir = out_dir.joinpath('MyProject1')
    TestUtils.force_spl_core_usage_to_this_repo()
    project_artifacts = WorkspaceArtifacts(project_dir)
    assert project_dir.joinpath('CMakeLists.txt').exists()
    assert project_dir.joinpath('variants/Flv1/Sys1/config.cmake').exists()
    assert project_dir.joinpath('variants/Flv1/Sys2/config.cmake').exists()

    result = execute_command(f"{project_artifacts.build_script} -build -target selftests")
    assert result.returncode == 0

    "Add new variant and build again"
    with patch('sys.argv', [
        'some_name',
        'variant',
        '--add', 'Flv2/Sys1',
        '--workspace_dir', f"{project_dir}"
    ]):
        main()

    result = execute_command(f"{project_artifacts.build_script} -build -target selftests")
    assert result.returncode == 0

    "Remove two variants and build again"
    with patch('sys.argv', [
        'some_name',
        'variant',
        '--delete', 'Flv1/Sys1',
        '--delete', 'Flv2/Sys1',
        '--workspace_dir', f"{project_dir}"
    ]):
        main()

    result = execute_command(f"{project_artifacts.build_script} -build -target selftests")
    assert result.returncode == 0
