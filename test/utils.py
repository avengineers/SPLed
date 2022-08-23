#!/usr/bin/env python
import os
from subprocess import Popen, PIPE, CalledProcessError


"""
Utility functions needed by all test scripts.
"""


def get_test_data(filename=""):
    return os.path.dirname(__file__) + "/data/" + filename


def get_output_folder():
    return os.path.dirname(__file__) + "/output"


def run_process(command_list):
    with Popen(command_list, stdout=PIPE, stderr=PIPE, bufsize=1, universal_newlines=True) as p:
        for line in p.stdout:
            print(line, end='')
        for line in p.stderr:
            print(line, end='')

    return p.returncode
