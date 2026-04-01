# -*- coding: utf-8 -*-
"""Configuration"""
import datetime

from importlib.resources import files
from spl_core.report_generation.spl_sphinx import SplSphinx
from spl_core.report_generation.spl_html_settings import html_theme, html_show_sourcelink, html_theme_options  # noqa: F401

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

include_patterns = ["index.md", "doc/**"]

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
sphinx_rtd_size_width = extension_configs["sphinx_rtd_size_width"]
tr_report_template = extension_configs["tr_report_template"]
myst_enable_extensions = extension_configs["myst_enable_extensions"]
source_suffix = extension_configs["source_suffix"]

# Import default SPL sphinx-needs configuration
needs_from_toml = str(files("spl_core.report_generation").joinpath("ubproject.toml"))

# Additional import required because the configuration references custom functions defined in this module
needs_functions = SplSphinx.default_needs_functions
needs_global_options = SplSphinx.default_needs_global_options

# Provide all config values to jinja
html_context = SplSphinx.get_default_html_context()
include_patterns.extend(html_context["build_config"].get("include_patterns", []))


def rstjinja(app, docname, source):
    """
    Render our pages as a jinja template for fancy templating goodness.
    """
    # Make sure we're outputting HTML
    if app.builder.format != "html":
        return
    src = source[0]
    rendered = app.builder.templates.render_string(src, app.config.html_context)
    source[0] = rendered


def setup(app):
    app.connect("source-read", rstjinja)
