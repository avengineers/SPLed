from pathlib import Path
from unittest.mock import Mock
from spl_extension_zip.src.my_extension import (
    BinaryPacker,
    SplPaths,
    Variant,
    create_parser,
)


def test_parser_project_root_argument():
    parser = create_parser()
    args = parser.parse_args(["--project_root_dir", "my_dir"])
    assert args.project_root_dir == Path("my_dir")
    args = parser.parse_args([])
    assert args.project_root_dir == Path(".")


def test_parser_variant_argument():
    parser = create_parser()
    args = parser.parse_args(["--variant", "SOME/NAME"])
    assert args.variant == Variant("SOME", "NAME")


def test_parser_build_kit_argument():
    parser = create_parser()
    args = parser.parse_args(["--build_kit", "SOME_BUILD_KIT"])
    assert args.build_kit == "SOME_BUILD_KIT"


def test_parser_has_run_argument():
    parser = create_parser()
    args = parser.parse_args(["--run"])
    assert args.run


def test_parser_has_generate_argument():
    parser = create_parser()
    args = parser.parse_args(["--generate"])
    assert args.generate


def test_generate_cmake_file(tmp_path: Path):
    my_file = tmp_path / "new.cmake"
    spl_config = Mock()
    spl_config.linker_output_file = "my_main.exe"
    BinaryPacker(Mock(), spl_config)._generate(my_file)
    assert my_file.exists()
    assert "OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/my_main.zip" in my_file.read_text()


def test_spl_paths():
    spl_paths = SplPaths(Path("."), Variant("SOME", "NAME"), "SOME_BUILD_KIT")
    assert spl_paths.build_dir == Path("build/SOME/NAME/SOME_BUILD_KIT")
