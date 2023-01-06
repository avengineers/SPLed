import argparse
import logging
import shutil
from pathlib import Path
from typing import Dict, List

from cookiecutter.main import cookiecutter

from src.common.common import existing_path
from src.creator.variant import Variant
from src.creator.workspace_artifacts import WorkspaceArtifacts


class Creator:
    def __init__(self, project_name: str, out_dir: Path):
        self.logger = logging.getLogger(__name__)
        self.project_name = project_name
        self.out_dir = out_dir.absolute()
        self.project_root_dir = self.out_dir.joinpath(self.project_name)
        self.project_artifacts = WorkspaceArtifacts(self.project_root_dir)

    @classmethod
    def from_folder(cls, project_dir: Path):
        return cls(project_dir.name, project_dir.parent)

    @staticmethod
    def create_project_description(name: str, variants: List[Variant], touch_only_variants_data: bool = False) -> Dict:
        project_description = {
            'name': name,
            'variants': {},
            'touch_components': not touch_only_variants_data,
            'touch_tools': not touch_only_variants_data
        }
        variants.sort()
        for index, variant in enumerate(variants):
            project_description['variants'][f"{index}"] = vars(variant)
        return project_description

    @property
    def project_template_path(self) -> Path:
        return Path(__file__).parent.joinpath('templates/project')

    @property
    def variant_template_path(self) -> Path:
        return Path(__file__).parent.joinpath('templates/variant')

    def materialize(self, variants: List[Variant]) -> Path:
        result_path = self.materialize_workspace(variants)
        self.materialize_variants(variants)
        self.logger.info(f"Project created under: {result_path}")
        return Path(result_path)

    def materialize_workspace(self, variants, touch_only_variants_data: bool = False):
        project_description = self.create_project_description(self.project_name, variants, touch_only_variants_data)
        result_path = cookiecutter(str(self.project_template_path),
                                   output_dir=f"{self.out_dir}",
                                   no_input=True,
                                   extra_context=project_description,
                                   overwrite_if_exists=True)
        return result_path

    def materialize_variants(self, variants: List[Variant]):
        for variant in variants:
            self.materialize_variant(vars(variant), self.project_artifacts.variants_dir)

    def materialize_variant(self, variant_description: Dict, out_dir: Path):
        result_path = cookiecutter(str(self.variant_template_path),
                                   output_dir=f"{out_dir}",
                                   no_input=True,
                                   extra_context=variant_description,
                                   overwrite_if_exists=True)
        self.logger.info(f"Variant created under: {result_path}")
        return Path(result_path)

    def add_variants(self, variants: List[Variant]):
        existing_variants = self.collect_project_variants()
        new_variants = [variant for variant in variants if variant not in existing_variants]
        if len(new_variants):
            self.materialize_workspace(new_variants + existing_variants, touch_only_variants_data=True)
            self.materialize_variants(variants)
            if len(new_variants) != len(variants):
                self.logger.warning(f"Skip adding variants"
                                    f" ({', '.join([str(v) for v in variants if v not in new_variants])})"
                                    f" because they already exist in {self.project_root_dir}.")
        else:
            self.logger.warning(f"Nothing to add. All the provided variants"
                                f" ({', '.join([str(v) for v in variants])}) already exist in {self.project_root_dir}.")

    def delete_variants(self, variants: List[Variant]):
        existing_variants = self.collect_project_variants()
        variants_to_be_deleted = [variant for variant in variants if variant in existing_variants]
        if len(variants_to_be_deleted):
            remaining_variants = list(set(existing_variants) - set(variants_to_be_deleted))
            self.materialize_workspace(remaining_variants, touch_only_variants_data=True)
            for variant in variants_to_be_deleted:
                self.delete_variant_dir(variant)
            if len(variants_to_be_deleted) != len(variants):
                self.logger.warning(f"Skip deleting variants"
                                    f" ({', '.join([str(v) for v in variants if v not in variants_to_be_deleted])})"
                                    f" because they do not exist in {self.project_root_dir}.")
        else:
            self.logger.warning(f"Nothing to delete. None of the provided variants"
                                f" ({', '.join([str(v) for v in variants])}) exists in {self.project_root_dir}.")

    def delete_variant_dir(self, variant: Variant):
        variant_dir = self.project_artifacts.variants_dir.joinpath(f"{variant}")
        if variant_dir.exists():
            shutil.rmtree(variant_dir)

    def collect_project_variants(self) -> List[Variant]:
        variants = []
        variant_dirs = self.project_artifacts.variants_dir.glob("*/*/")
        for variant_dir in variant_dirs:
            variants.append(Variant(variant_dir.parent.name, variant_dir.name))
        return variants


def main(command_arguments=None):
    arguments = parse_arguments(command_arguments)
    if arguments.command_scope == 'workspace':
        Creator(arguments.name, arguments.out_dir).materialize(arguments.variant)
    else:  # scope is variant
        creator = Creator.from_folder(arguments.workspace_dir)
        if arguments.add:
            creator.add_variants(arguments.add)
        else:
            creator.delete_variants(arguments.delete)


def parse_arguments(command_arguments=None):
    parser = argparse.ArgumentParser(description='Workspace creator')
    subparsers = parser.add_subparsers(dest='command_scope')

    parser_workspace = subparsers.add_parser('workspace', help='Create a workspace')
    parser_workspace.add_argument('--name', required=True, type=str,
                                  help="Workspace name. A directory with this name will be created in the <out_dir>.")
    parser_workspace.add_argument('--variant', required=True, action='append', type=Variant.from_string,
                                  help="Variant name as <flavor>/<subsystem>. E.g. FLV1/SYS1. "
                                       "This option can be used multiple times.")
    parser_workspace.add_argument('--out_dir', required=True, type=existing_path,
                                  help="Target directory where the workspace folder will be created.")

    parser_variant = subparsers.add_parser('variant', help='Add/delete variant to existing workspace')
    command_group = parser_variant.add_mutually_exclusive_group(required=True)
    command_group.add_argument('--add', action='append', type=Variant.from_string, metavar='VARIANT',
                               help="Add a variant to a workspace. Variant name as <flavor>/<subsystem>."
                                    " E.g. FLV1/SYS1. This option can be used multiple times.")
    command_group.add_argument('--delete', action='append', type=Variant.from_string, metavar='VARIANT',
                               help="Delete a variant from a workspace. Variant name as <flavor>/<subsystem>."
                                    " E.g. FLV1/SYS1. This option can be used multiple times.")
    parser_variant.add_argument('--workspace_dir', required=True, type=existing_path,
                                help="Workspace root directory.")
    return parser.parse_args(command_arguments)


if __name__ == '__main__':
    main()
