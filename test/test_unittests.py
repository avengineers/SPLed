#!/usr/bin/env python3

import unittest
import os.path
import json
from utils import run_process


class TestUnitTests(unittest.TestCase):

    def test_unit_tests_in_alpha(self):
        variant = 'spl/alpha'
        self.build_unittests_and_expect_files(variant)

        iut = 'build/{variant}/test/src/app/component/var_a/'.format(variant=variant)

        """Test reports shall be created."""
        self.assertTrue(os.path.isfile(iut + 'junit.xml'))

        """Coverage data for each object shall be generated"""
        self.assertTrue(os.path.isfile(
            iut + 'CMakeFiles/src_app_component_var_a.dir/src/component.c.obj'))
        self.assertTrue(os.path.isfile(
            iut + 'CMakeFiles/src_app_component_var_a.dir/src/component.c.gcno'))
        self.assertTrue(os.path.isfile(
            iut + 'CMakeFiles/src_app_component_var_a.dir/src/component.c.gcda'))

        """Coverage report shall be created for each component"""
        self.assertTrue(os.path.isfile(iut + 'coverage.json'))
        f = open(iut + 'coverage.json')
        json_data = json.loads(f.read())
        self.assertEqual('src/app/component/var_a/src/component.c',
                         json_data['files'][0]['file'])
        f.close()
        self.assertTrue(os.path.isfile(iut + 'coverage/index.html'))

    def test_unit_tests_in_beta(self):
        variant = 'spl/beta'
        self.build_unittests_and_expect_files(variant)

    def build_unittests_and_expect_files(self, variant):
        exit_code = run_process([
            'build.bat',
            '--build',
            '--variants', variant,
            '--target', 'unittests',
            '--reconfigure'
        ])

        """Unit tests execution shall pass."""
        self.assertEqual(0, exit_code)

        """Coverage report shall be created"""
        self.assertTrue(os.path.isfile(
            'build/{variant}/test/coverage/index.html'.format(variant=variant)))
