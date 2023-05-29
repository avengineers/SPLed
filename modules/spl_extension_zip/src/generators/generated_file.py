from abc import ABC, abstractmethod
from pathlib import Path


class GeneratedFile(ABC):
    def __init__(self, path: Path) -> None:
        self.path = path

    @abstractmethod
    def to_string(self) -> str:
        ...

    def to_file(self) -> None:
        """Only write to file if the content has changed."""
        content = self.to_string()
        if not self.path.exists() or self.path.read_text() != content:
            self.path.write_text(content)
