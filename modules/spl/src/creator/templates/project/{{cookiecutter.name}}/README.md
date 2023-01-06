# Software Product Line {{cookiecutter.name}}

This is a tiny but fully fledged [SPL](https://en.wikipedia.org/wiki/Software_product_line) for automotive SW development. It is capable of holding a set of software modules that are differently used and configured among the variants. Each variant creates its own binaries.

## CI (Continuous Integration)

* **TODO:** update the CI status link -> [![selftest](https://github.com/avengineers/spldemo/actions/workflows/test.yml/badge.svg)](https://github.com/avengineers/spldemo/actions/workflows/test.yml)


## Preparation

You can install required and optional tools and install some VS Code extensions. The installer uses Scoop/PowerShell, so it will only run on Windows. The full list of external dependencies is written to `dependencies.json`.
* Install mandatory tools by executing `install-mandatory.bat` in the root directory of the repository.
* Visual Studio Code can be installed by executing `install-optional.bat` in the root directory of the repository. VS Code is the recommended IDE for this SPL solution.
  * In VS Code you need to install the following extensions in order to fully utilize our SPL. Hit `Ctrl+Shift+x` to search and install them.
    * CMake Tools
    * C/C++ Extension Pack

## Usage

### From the terminal


Build and execute the unit tests for all variants:
```cmd
.\build.bat -build -target selftests
```

Execute a target for a specific variant:

```cmd
.\build.bat -build -target <target_name> -variants <variant_name>

Example:
.\build.bat -build -target link -variants X1/T1
```

Available targets:

* `link` - just build the binary
* `all` - build and generate all configured artifacts
* `unittests` - execute the unit tests


### From VS Code

The SPL project is a CMake project and comes with predefined configuration files for the [CMake extension](https://github.com/microsoft/vscode-cmake-tools).

Once you have this extension installed, you should be able to select the ``variant``, ``build kit`` and ``target`` in the status bar on the bottom. 
