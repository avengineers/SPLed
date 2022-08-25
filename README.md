# Software Product Line SPLDemo

SPLDemo is a tiny but fully fledged [SPL](https://en.wikipedia.org/wiki/Software_product_line) for automotive SW development. It is capable of holding a set of software modules that are differently used and configured among the variants. Each variant creates its own binaries.

## CI (Continuous Integration)

* [![selftests](https://github.com/avengineers/spldemo/actions/workflows/test.yml/badge.svg)](https://github.com/avengineers/spldemo/actions/workflows/test.yml)
* artifacts are archived under Github Actions (or locally inside the `build/<variant>/(prod|test)` directory)

## Preparation

This repository contains [Git submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) with the unit test framework _Unity_ and mocking framework _CMock_ in it. You don't need to take care for them: they will get checked out automatically by CMake during configuration.

You can install required and optional tools and install some VS Code extensions. The installer uses Scoop/PowerShell, so it will only run on Windows. The full list of external dependencies is written to `dependencies.json`.
* Install mandatory tools by executing `install-mandatory.bat` in the root directory of the repository.
* Visual Studio Code can be installed by executing `install-optional.bat` in the root directory of the repository. VS Code is the perfect IDE for this SPL solution.
  * In VS Code you need to install the following extensions in order to fully utilize our SPL. Hit `Ctrl+Shift+x` to search and install them.
    * CMake Tools
    * C/C++ Extension Pack

## Usage

Please read the SPL readme at: https://github.com/avengineers/SPL. The avengineers/SPL functionality is the core of this repository. SPLDemo is only demonstrating its usage in a tiny project.