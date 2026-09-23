import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).parent.parent / "pipeline"))
from variants import discover_variants, pytest_filter, select_release_variant  # noqa: E402

VARIANTS = ["Base/Dev", "Disco", "IDEA/Sloemada", "Sleep", "Spa"]


def test_discover_variants() -> None:
    assert discover_variants() == VARIANTS


@pytest.mark.parametrize(
    ("branch", "variant"),
    [
        ("release/Sleep", "Sleep"),
        ("release/Sleep/test_v1", "Sleep"),
        ("release/IDEA/Sloemada#v1.0", "IDEA/Sloemada"),
        ("release/IDEA/Sloemada/v1.0", "IDEA/Sloemada"),
    ],
)
def test_release_branch_selects_one_variant(branch: str, variant: str) -> None:
    assert select_release_variant(branch, VARIANTS) == variant


def test_release_branch_without_variant_fails() -> None:
    with pytest.raises(ValueError, match="No variant found"):
        select_release_variant("release/Spaceship", VARIANTS)


def test_pytest_filter_spells_the_test_class_name() -> None:
    assert pytest_filter("IDEA/Sloemada") == "IDEA__Sloemada"
    assert pytest_filter("Disco") == "Disco"
