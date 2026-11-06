# -*- coding: utf-8 -*-
"""Configuration"""
import datetime

from importlib.resources import files
from spl_core.report_generation.spl_sphinx import SplSphinx
from spl_core.report_generation.spl_html_settings import html_theme, html_show_sourcelink, html_theme_options, html_sidebars, html_last_updated_fmt  # noqa: F401

day = datetime.date.today()
# meta data #################################################################

project = "SPLed"
copyright = f"{day.year}, RMT and Friends"
release = f"{day}"

# file handling #############################################################
# @see https://www.sphinx-doc.org/en/master/usage/configuration.html

templates_path = [
    "doc/_tmpl",
]

exclude_patterns = [
    "README.md",
    "build/modules",
    "build/deps",
    ".venv",
    ".git",
    "**/test_results.rst",  # We renamed this file, but nobody deletes it.
]

# configuration of built-in stuff ###########################################
# @see https://www.sphinx-doc.org/en/master/usage/configuration.html

numfig = True

# html config ###############################################################
# @see https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

# Omit "documentation" in title
html_title = f"{project} {release}"

html_logo = "doc/_figures/SPLED_logo.png"

# Get default SPL extensions and their configurations
extensions = SplSphinx.default_extensions
extension_configs = SplSphinx.default_extension_configs

# Apply extension-specific configurations
tr_report_template = extension_configs["tr_report_template"]
myst_enable_extensions = extension_configs["myst_enable_extensions"]
source_suffix = extension_configs["source_suffix"]

# Import default SPL sphinx-needs configuration, which also links test cases to their results
needs_from_toml = str(files("spl_core.report_generation").joinpath("ubproject.toml"))

# The document set of this run comes from CMake (SPL_PROJECT_DOCS_INCLUDE_PATTERNS and the
# variant's components, spl-core ADR 0005), the same list ubCode reads.
include_patterns = SplSphinx.get_default_html_context()["build_config"].get("include_patterns", [])
