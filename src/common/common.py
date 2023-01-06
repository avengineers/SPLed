from pathlib import Path


def to_path(input_path: str, check_if_exists: bool = True) -> Path:
    return_path = Path(input_path)
    if not check_if_exists or return_path.exists():
        return return_path.absolute()
    else:
        raise FileNotFoundError(input_path)


def existing_path(input_path: str) -> Path:
    return to_path(input_path, True)


def non_existing_path(input_path: str) -> Path:
    return to_path(input_path, False)
