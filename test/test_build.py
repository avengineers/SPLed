#!/usr/bin/env python3

import unittest
import os.path
from pathlib import Path
from utils import run_process


class TestBuild(unittest.TestCase):

    def test_build_spl__alpha(self):
        variant = 'spl/alpha'
        self.build_and_expect_default(variant)
        
    def test_build_spl__beta(self):
        variant = 'spl/beta'
        self.build_and_expect_default(variant)

    def test_build_spl__gamma(self):
        variant = 'spl/gamma'
        self.build_and_expect_default(variant)

    def build_and_expect_default(self, variant, target='all'):
        """build wrapper shall build the default target and related outputs."""

        exit_code = run_process([
            'build.bat',
            '-build',
            '-variants', variant,
            '-target', target,
            '-reconfigure'
        ])

        self.assertEqual(0, exit_code)
        self.expect_binary(variant, file_name="my_main.exe")

    def expect_binary(self, variant, bin_type='elf', suffix='', file_name=''):
        """Hex file of given configuration shall exist."""
        self.assert_expected_file_exists(self.get_artifact_name(variant, bin_type, suffix, file_name))
    
    def assert_expected_file_exists(self, expected_file):
        self.assertTrue(os.path.isfile(expected_file), f"File {expected_file} shall exist.")
        
    @staticmethod
    def get_artifact_name(variant, bin_type='elf', suffix='', file_name='') -> Path:
        """Hex file of given configuration shall exist."""
        file = file_name if file_name else f"{variant.replace('/', '_')}{suffix}.{bin_type}"
        return Path(f"build/{variant}/prod/{file}")
