#!/usr/bin/env python3

import unittest
import os.path
import json
from utils import run_process


class TestUnitTests(unittest.TestCase):
{% for variant in cookiecutter.variants.values() %}
    def test_unit_tests_{{ variant["flavor"] }}__{{ variant["subsystem"] }}(self):
        variant = '{{ variant["flavor"] }}/{{ variant["subsystem"] }}'
        self.build_unittests_and_expect_files(variant)
{% endfor %}
    def build_unittests_and_expect_files(self, variant):
        exit_code = run_process([
            'build.bat',
            '-build',
            '-variants', variant,
            '-target', 'unittests',
            '-reconfigure'
        ])

        """Unit tests execution shall pass."""
        self.assertEqual(0, exit_code)

        """Coverage report shall be created"""
        expected_file = f"build/{variant}/test/coverage/index.html"
        self.assertTrue(os.path.isfile(expected_file), f"File {expected_file} shall exist.")
