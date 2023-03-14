import json
from spl_extension_zip.src.my_extension import SplExtensionsConfig


def test_config_linker_basename():
    encoded_data = json.dumps({"LINKER_OUTPUT_FILE": "my_main.exe"})
    assert (
        SplExtensionsConfig.from_json(encoded_data).linker_output_file == "my_main.exe"
    )

    encoded_data = json.dumps({"linker_output_file": "my.app"})
    assert (
        SplExtensionsConfig.from_json(encoded_data).linker_output_file == "main.exe"
    ), "if the alias is not found, the default value is used"

    encoded_data = json.dumps({})
    assert SplExtensionsConfig.from_json(encoded_data).linker_output_file == "main.exe"


def test_config_linker_extensions():
    assert (
        SplExtensionsConfig.from_json(json.dumps({})).linker_byproducts_extensions == []
    )

    assert SplExtensionsConfig.from_json(
        json.dumps({"LINKER_BYPRODUCTS_EXTENSIONS": "exe,dll"})
    ).linker_byproducts_extensions == ["exe", "dll"]
