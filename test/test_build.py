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
            '--build',
            '--variants', variant,
            '--target', target,
            '--reconfigure'
        ])

        self.assertEqual(0, exit_code)
        self.expect_binary(variant, file_name="main.exe")


    def expect_binary(self, variant, binType='elf', suffix='', file_name=''):
        """Hex file of given configuration shall exist."""
        self.assert_expected_file_exists(self.get_artifact_name(variant, binType, suffix, file_name))
    
    def assert_expected_file_exists(self, expected_file):
        self.assertTrue(os.path.isfile(expected_file), 'File {expected_file} shall exist.'.format(expected_file=expected_file))
        
    @staticmethod
    def get_artifact_name(variant, binType='elf', suffix='', file_name='') -> Path:
        """Hex file of given configuration shall exist."""
        file = file_name if file_name else '{variant_underscore}{suffix}.{bin}'.format(variant_underscore=variant.replace('/', '_'), bin=binType,suffix=suffix)
        return Path(f"build/{variant}/prod/{file}")
