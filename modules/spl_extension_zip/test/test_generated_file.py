from spl_extension_zip.src.generators.generated_file import GeneratedFile


class _TestGeneratedFile(GeneratedFile):
    def to_string(self) -> str:
        return "this is a dummy string"


def test_generated_file_to_string(tmp_path):
    path = tmp_path / "dummy_file.txt"
    generated_file = _TestGeneratedFile(path)
    assert generated_file.to_string() == "this is a dummy string"
