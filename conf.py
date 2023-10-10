# -*- coding: utf-8 -*-
""" Configuration  """

import json
import os
import datetime

day = datetime.date.today()
# meta data #################################################################

project = "SPLed"
copyright = f"{day.year} Marquardt GmbH"
release = f"{day}"

# file handling #############################################################
# @see https://www.sphinx-doc.org/en/master/usage/configuration.html

templates_path = [
    "doc/_tmpl",
]

exclude_patterns = ["README.md", "build/modules", "build/deps", ".venv", ".git"]
include_patterns = ["index.rst", "doc/**"]

# configuration of built-in stuff ###########################################
# @see https://www.sphinx-doc.org/en/master/usage/configuration.html

numfig = True

# html config ###############################################################
# @see https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

# Omit "documentation" in title
html_title = f"{project} {release}"

html_theme = "sphinx_rtd_theme"

# Hide hyper link which leeds to the source of page displayed
html_show_sourcelink = True

html_theme_options = {
    "canonical_url": "",
    "analytics_id": "",  #  Provided by Google in your dashboard
    "display_version": True,
    "prev_next_buttons_location": "bottom",
    "style_external_links": True,
    "logo_only": False,
    "style_nav_header_background": "white",
    # Toc options
    "collapse_navigation": True,
    "sticky_navigation": True,
    "navigation_depth": 6,
    "includehidden": True,
    "titles_only": False,
}


html_logo = "doc/_figures/logo.jpg"

# EXTENSIONS AND THEIR CONFIGS ##############################################
extensions = ["sphinx_rtd_size"]

sphinx_rtd_size_width = "90%"

# mermaid config - @see https://pypi.org/project/sphinxcontrib-mermaid/ #####
extensions.append("sphinxcontrib.mermaid")

# sphinx_needs ###############################################################
extensions.append("sphinx_needs")

# test_reports ###############################################################
extensions.append("sphinxcontrib.test_reports")
tr_report_template = "doc/test_report_template.txt"

# todo #######################################################################
extensions.append("sphinx.ext.todo")

# Render Your Data Readable ##################################################
# Enables adding Jupyter notebooks to toctree
# @see https://sphinxcontribdatatemplates.readthedocs.io/en/latest/index.html
extensions.append("sphinxcontrib.datatemplates")

# needs_types - this option allows the setup of own need types like bugs, user_stories and more.
needs_types = [
    dict(directive="spec", title="Specification", prefix="S_", color="#FEDCD2", style="node"),
    dict(directive="impl", title="Implementation", prefix="I_", color="#DF744A", style="node"),
    dict(directive="test", title="Test Case", prefix="T_", color="#DCB239", style="node"),
]


# Define own options
needs_extra_options = ["integrity", "assignee", "version"]


# Define own link types
needs_extra_links = [
    # SWE.3 BP.5: link from Implementation (Software unit) to Specification (Software detailed design)
    {"option": "implements", "incoming": "is implemented by", "outgoing": "implements"},
    # SWE.4 BP.5: link from Test Case (Unit test specification) to Specification (Software detailed design)
    {"option": "tests", "incoming": "is tested by", "outgoing": "tests"},
    # SWE.4 BP.5: link from Test Case (Unit test specification) to Test Result (Unit test result)
    {"option": "results", "incoming": "is resulted from", "outgoing": "results"}
]

# Check if the SPHINX_BUILD_CONFIGURATION_FILE environment variable exists
# and if so, load the JSON file and set the 'html_context' variable
if "SPHINX_BUILD_CONFIGURATION_FILE" in os.environ:
    with open(os.environ["SPHINX_BUILD_CONFIGURATION_FILE"], "r") as file:
        html_context = json.load(file)
        include_patterns.extend(html_context.get("include_patterns", []))


# Check if the SPHINX_BUILD_CONFIGURATION_FILE environment variable exists
# and if so, load the JSON file and set the 'html_context' variable
if "AUTOCONF_JSON_FILE" in os.environ:
    with open(os.environ["AUTOCONF_JSON_FILE"], "r") as file:
        html_context["config"] = json.load(file)["features"]


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
