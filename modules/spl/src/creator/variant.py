import dataclasses


@dataclasses.dataclass
class Variant:
    flavor: str
    subsystem: str

    @classmethod
    def from_string(cls, variant: str):
        return cls(*variant.split('/'))

    def __str__(self):
        return self.to_string()

    def to_string(self, delimiter: str = '/') -> str:
        return f"{self.flavor}{delimiter}{self.subsystem}"

    def __lt__(self, other):
        return f"{self}" < f"{other}"

    def __hash__(self):
        return hash(f"{self}")
