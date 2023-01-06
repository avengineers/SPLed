from src.creator.variant import Variant


class TestVariant:
    def test_from_string(self):
        assert Variant.from_string('Bla/Blub') == Variant('Bla', 'Blub')

    def test_lt(self):
        assert Variant('A', 'A') < Variant('A', 'B')
        assert Variant('A', 'B') > Variant('A', 'A')
        assert Variant('1', 'B') < Variant('A', 'A')
        assert Variant('2', 'B') > Variant('1', 'A')

    def test_vars(self):
        assert vars(Variant('A', '1')) == {'flavor': 'A', 'subsystem': '1'}

    def test_hash(self):
        assert Variant('A', 'B') in [Variant('A', 'C'), Variant('A', 'B')]
        assert list({Variant('A', 'C'), Variant('A', 'B')} - {Variant('A', 'B')}) == [Variant('A', 'C')]
