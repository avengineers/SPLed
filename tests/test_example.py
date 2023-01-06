#!/usr/bin/env python3

from testfixtures import TempDirectory
from pathlib import Path
from example import *


class TestExample:
    def test_example__create_output(self):
        # inputs
        d = TempDirectory()
        outputFile = Path(d.path, "someFile")

        # call IUT
        Example().create_output(input="Hello World!", output=outputFile)

        # outputs
        assert outputFile.is_file()
        assert "Hello World!" == outputFile.read_text()

    def test_example__main(self):
        with TempDirectory() as d:
            # inputs
            outputFile = Path(d.path, "someOutput")

            # call IUT
            assert 0 == Example().main(["--input", "Hello Main!", "--output", str(outputFile)])

            # outputs
            assert outputFile.is_file()
            assert "Hello Main!" == outputFile.read_text()
