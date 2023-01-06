#!/usr/bin/env python3

import sys
from os.path import dirname
from argparse import ArgumentParser
from pathlib import Path

sys.path.append(dirname(__file__))


class Example:
    def create_output(self, input: str, output: Path):
        output.write_text(input)

    def main(self, pargv):
        arg = ArgumentParser(fromfile_prefix_chars="@", prog="example.py")
        arg.add_argument("--output", "-o", help="Output file", required=True, type=Path)
        arg.add_argument("--input", "-i", help="Input to be written output file", required=True, type=str)
        args = arg.parse_args(args=pargv)
        self.create_output(input=args.input, output=args.output)
        return 0


if __name__ == "__main__":
    sys.exit(Example().main(sys.argv[1:]))
