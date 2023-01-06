import argparse
import os
import json
from contextlib import contextmanager
from pathlib import Path
from typing import Dict

import kconfiglib


def to_path(input_path: str, check_if_exists: bool = True) -> Path:
    """TODO: use function from common python module"""
    return_path = Path(input_path)
    if not check_if_exists or return_path.exists():
        return return_path.absolute()
    else:
        raise FileNotFoundError(input_path)


def existing_path(input_path: str) -> Path:
    """TODO: use function from common python module"""
    return to_path(input_path, True)


def non_existing_path(input_path: str) -> Path:
    """TODO: use function from common python module"""
    return to_path(input_path, False)


@contextmanager
def working_directory(some_directory: Path):
    current_directory = Path().absolute()
    try:
        os.chdir(some_directory)
        yield
    finally:
        os.chdir(current_directory)


class KConfig:
    def __init__(self, k_config_model_file: Path, k_config_file: Path = None,
                 k_config_root_directory: Path = None):
        """
        :param k_config_model_file: Feature model definition (KConfig format)
        :param k_config_file: User feature selection configuration file
        :param k_config_root_directory: all paths for the included configuration paths shall be relative to this folder
        """
        if not k_config_model_file.exists():
            raise FileNotFoundError(
                f"File {k_config_model_file} does not exist.")
        with working_directory(k_config_root_directory or k_config_model_file.parent):
            self.config = kconfiglib.Kconfig(k_config_model_file)
        if k_config_file:
            if not k_config_file.exists():
                raise FileNotFoundError(
                    f"File {k_config_file} does not exist.")
            self.config.load_config(k_config_file, replace=False)

    def get_json_values(self) -> Dict:
        config_dict = {}

        def write_node(node):
            sym = node.item
            if not isinstance(sym, kconfiglib.Symbol):
                return

            if sym.config_string:
                val = sym.str_value
                if sym.type in [kconfiglib.BOOL, kconfiglib.TRISTATE]:
                    val = (val != 'n')
                elif sym.type == kconfiglib.HEX:
                    val = int(val, 16)
                elif sym.type == kconfiglib.INT:
                    val = int(val)
                config_dict[sym.name] = val

        for n in self.config.node_iter(False):
            write_node(n)
        return config_dict
    

    def get_cmake_content(self) -> str:
        config_dict = self.get_json_values()
        return '\n'.join([f'set({key} "{value}")' for key, value in config_dict.items()])  
    

    def generate_header(self, output_file: Path):
        output_file.parent.mkdir(parents=True, exist_ok=True)
        self.config.write_autoconf(filename=output_file)
        
        with open(output_file, "r") as outfile:
            content = outfile.read()
            
        with open(output_file, "w") as outfile:
            outfile.write('#ifndef autoconf\n#define autoconf\n\n' + content + '\n#endif\n')
        
        
    def generate_json(self, output_file: Path):
        output_file.parent.mkdir(parents=True, exist_ok=True)
        
        # Serializing json
        feature_json = {'features': self.get_json_values()}
        json_object = json.dumps(feature_json, indent=4)
        
        # Writing to sample.json
        with open(output_file, "w") as outfile:
            outfile.write(json_object)
        
        self.get_json_values


    def generate_cmake(self, output_file: Path):
        output_file.parent.mkdir(parents=True, exist_ok=True)
            
        with open(output_file, "w") as outfile:
            outfile.write(self.get_cmake_content())   

def main():
    parser = argparse.ArgumentParser(description='KConfig generation')
    parser.add_argument('--kconfig_model_file',
                        required=True, type=existing_path)
    parser.add_argument('--kconfig_config_file',
                        required=False, type=existing_path)
    parser.add_argument('--out_header_file', required=True,
                        type=non_existing_path)
    parser.add_argument('--out_json_file', required=False, 
                        type=non_existing_path)
    parser.add_argument('--out_cmake_file', required=False, 
                        type=non_existing_path)
    arguments = parser.parse_args()
    kconfig = KConfig(
        arguments.kconfig_model_file,
        arguments.kconfig_config_file
    )
    
    kconfig.generate_header(arguments.out_header_file)
    
    if arguments.out_json_file:
        kconfig.generate_json(arguments.out_json_file)

    if arguments.out_cmake_file:
        kconfig.generate_cmake(arguments.out_cmake_file)        


if __name__ == '__main__':
    main()
