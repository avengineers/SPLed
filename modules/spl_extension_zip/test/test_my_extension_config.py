import json
from modules.spl_extension_zip.src.my_extension import SplExtensionsConfig


def test_config_linker_basename():
    encoded_data = json.dumps({"LINK_OUT_FILE_BASENAME": "my_main"})
    assert SplExtensionsConfig.from_json(encoded_data) == SplExtensionsConfig(
        linker_output_file_basename="my_main"
    )

    encoded_data = json.dumps({"linker_output_file_basename": "my_app"})
    assert SplExtensionsConfig.from_json(encoded_data) == SplExtensionsConfig(
        linker_output_file_basename="main"
    ), "if the alias is not found, the default value is used"

    encoded_data = json.dumps({})
    assert SplExtensionsConfig.from_json(encoded_data) == SplExtensionsConfig(
        linker_output_file_basename="main"
    )


def test_config_linker_extensions():
    assert SplExtensionsConfig.from_json(
        json.dumps({})
    ).linker_output_file_extensions == ["exe"]

    assert SplExtensionsConfig.from_json(
        json.dumps({"LINK_OUT_FILE_EXTENSIONS": "exe,dll"})
    ).linker_output_file_extensions == ["exe", "dll"]

