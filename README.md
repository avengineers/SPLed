# Software Product Line &lt;SPL&gt;²

&lt;SPL&gt;² is a tiny but fully fledged [SPL](https://en.wikipedia.org/wiki/Software_product_line) for automotive SW development. It is capable of holding a set of software modules that are differently used and configured among the variants. Each variant creates its own binaries.

## CI (Continuous Integration)

* Find the latest CI results [here](https://github.com/avengineers/SPL/actions).
* Find build binaries (built locally or remotely) [here](https://splbinaries.jfrog.io/ui/repos/tree/General/spl-generic-local)

## Preparation

This repository contains [Git submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules) with the unit test framework _Unity_ and mocking framework _CMock_ in it. You don't need to take care for them: they will get checked out automatically by CMake.

Then you can install required and optional tools and install some VS Code extensions. The installer uses Scoop/PowerShell, so it will only run on Windows. The full list of external dependencies is written to `install-mandatory.list`.
* Install mandatory tools by executing `install-mandatory.bat` in the root directory of the repository.
* Visual Studio Code can be installed by executing `install-optional.bat` in the root directory of the repository. VS Code is the perfect IDE for this SPL solution.
  * In VS Code you need to install the following extensions. Hit `Ctrl+Shift+x` to search and install them.
    * CMake Tools
    * C/C++ Extension Pack

## Import new GNU Make Project (as legacy sources)

Importing is only possible in commandline mode. By running `build.bat` you will take a Make project from local disk and you can automatically bring it into SPL structure. [see details here](doc/import.md)

## Legacy Sources to Configurable Sources

The imported sources will be part of the legacy/ folder and placed there in the same structure as they were in Dimensions. They are not fully transformed into the new `configurable sources` structure. It needs some manual effort to change the code accordingly. [see details here](doc/legacyToConfigurable.md)

## TDD (Test Driven Development) and Unit Testing

In order do develop software using TDD, you need to [write and run unit tests](doc/unitTesting.md).

## Debugging

In case your unit tests are not sufficient enough and a bug was found that is not covered by an automated test, you can also debug your software. By stepping through your software units code line-by-line, you can see its behavior on your PC. [see details here](doc/debugging.md)

## Build Binaries

In order to build the project's binaries you have two options: Visual Studio Code or command line interface, preference: Visual Studio Code. [see details here](doc/build.md)