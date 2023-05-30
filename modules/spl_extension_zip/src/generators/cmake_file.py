from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from pathlib import Path
import textwrap
from typing import List, Type, TypeVar
from generators.generated_file import GeneratedFile

CMAKE_TAB = "    "


class CMakeElement(ABC):
    @abstractmethod
    def to_string(self) -> str:
        ...

    def __str__(self) -> str:
        return self.to_string()


@dataclass
class CMakeComment(CMakeElement):
    """
    Example:

    # This is a comment
    """

    comment: str

    def to_string(self) -> str:
        return f"# {self.comment}"


@dataclass
class CMakeEnvironmentVariable(CMakeElement):
    """
    Example:

    set(ENV{PATH} "${ENV{PATH}};${PROJECT_SOURCE_DIR}/modules/spl_extension_zip/src")
    """

    name: str
    value: str

    def to_string(self) -> str:
        return f'set(ENV{{{self.name}}} "{self.value}")'


@dataclass
class CMakeCustomCommand(CMakeElement):
    """
    Example:

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/<LINK_OUT_BASENAME>.zip
        COMMAND python ${PROJECT_SOURCE_DIR}/modules/spl_extension_zip/src/my_extension.py --run --project_root_dir ${CMAKE_SOURCE_DIR} --variant ${VARIANT} --build_kit ${BUILD_KIT}
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/<LINK_OUT_BASENAME>.exe ${PROJECT_SOURCE_DIR}/modules/spl_extension_zip/src/my_extension.py
        COMMENT "Generating <LINK_OUT_BASENAME>.zip"
        VERBATIM
    )

    https://cmake.org/cmake/help/latest/command/add_custom_command.html
    """

    output: str
    command: str
    depends: List[str] = field(default_factory=list)
    comment: str = ""

    def to_string(self) -> str:
        return textwrap.dedent(
            f"""\
        add_custom_command(
            OUTPUT {self.output}
            COMMAND {self.command}
            DEPENDS {" ".join(str(depend) for depend in self.depends)}
            COMMENT "{self.comment}"
            VERBATIM
        )"""
        )


@dataclass
class CMakeCustomTarget(CMakeElement):
    """
    Example:

    add_custom_target(
        my_target ALL
        DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/<LINK_OUT_BASENAME>.zip
    )

    https://cmake.org/cmake/help/latest/command/add_custom_target.html
    """

    name: str
    depends: List[str] = field(default_factory=list)
    # Indicate that this target should be added to the default build target so that it will be run every time
    all: bool = False

    def to_string(self) -> str:
        content = ["add_custom_target("]
        content.append(f"{CMAKE_TAB}{self.name}{' ALL' if self.all else ''}")
        if self.depends:
            content.append(
                f"{CMAKE_TAB}DEPENDS {' '.join(str(depend) for depend in self.depends)}"
            )
        content.append(")")
        return "\n".join(content)


T = TypeVar("T")


class CMakeFile(GeneratedFile):
    def __init__(self, path: Path) -> None:
        super().__init__(path)
        self.elements: List[CMakeElement] = []

    def add_element(self, element: CMakeElement) -> None:
        self.elements.append(element)

    def get_elements_of_type(self, element_type: Type[T]) -> List[T]:
        return [
            element for element in self.elements if isinstance(element, element_type)
        ]

    def to_string(self) -> str:
        return "\n".join(str(element) for element in self.elements)
