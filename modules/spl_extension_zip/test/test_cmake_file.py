from pathlib import Path
from spl_extension_zip.src.generators.cmake_file import (
    CMakeComment,
    CMakeCustomCommand,
    CMakeCustomTarget,
    CMakeFile,
    CMakeEnvironmentVariable,
)


def test_cmake_custom_command():
    target = CMakeCustomCommand(
        "build/my_output.txt",
        "dummy_command --run --project_root_dir ${CMAKE_SOURCE_DIR} --variant ${VARIANT} --build_kit ${BUILD_KIT}",
        ["file1.txt", "file2.txt"],
    )
    assert (
        target.to_string()
        == """\
add_custom_command(
    OUTPUT build/my_output.txt
    COMMAND dummy_command --run --project_root_dir ${CMAKE_SOURCE_DIR} --variant ${VARIANT} --build_kit ${BUILD_KIT}
    DEPENDS file1.txt file2.txt
    COMMENT ""
    VERBATIM
)"""
    )


def test_cmake_custom_target():
    target = CMakeCustomTarget(
        "my_target",
        ["file1.txt", "file2.txt"],
    )
    assert (
        target.to_string()
        == """\
add_custom_target(
    my_target
    DEPENDS file1.txt file2.txt
)"""
    )


def test_cmake_custom_target_all():
    target = CMakeCustomTarget(
        "my_target",
        [],
        all=True,
    )
    assert (
        f"{target}"
        == """\
add_custom_target(
    my_target ALL
)"""
    )


def test_cmake_comment():
    comment = CMakeComment("This is a comment")
    assert f"{comment}" == "# This is a comment"


def test_cmake_file(tmp_path: Path):
    my_cmake_file = CMakeFile(tmp_path / "my_cmake_file.txt")
    my_cmake_file.add_element(CMakeComment("This is a comment"))
    my_cmake_file.add_element(
        CMakeCustomCommand("build/my_output.txt", "dummy_command")
    )
    my_cmake_file.add_element(
        CMakeCustomTarget("my_target", ["file1.txt", "file2.txt"], all=True)
    )
    assert (
        my_cmake_file.to_string()
        == """\
# This is a comment
add_custom_command(
    OUTPUT build/my_output.txt
    COMMAND dummy_command
    DEPENDS 
    COMMENT ""
    VERBATIM
)
add_custom_target(
    my_target ALL
    DEPENDS file1.txt file2.txt
)"""
    )


def test_cmake_environment_variable():
    env_var = CMakeEnvironmentVariable(
        "PATH", "${PATH};${PROJECT_SOURCE_DIR}/modules/spl_extension_zip/src"
    )
    assert (
        f"{env_var}"
        == 'set(ENV{PATH} "${PATH};${PROJECT_SOURCE_DIR}/modules/spl_extension_zip/src")'
    )
