# Software Product Line Core

In this repository we construct the *SPL Core* functionality that can be integrated into CMake projects.

## CI (Continuous Integration)

* [![selftests](https://github.com/avengineers/spl/actions/workflows/gate.yml/badge.svg)](https://github.com/avengineers/spl/actions/workflows/gate.yml)

## Getting started

First thing to do is to install all SPL dependencies by opening a terminal and running:

```powershell
pipenv install
```

Now you can just create a new SPL workspace. We recommend using the workspace creator wrapper for it.

**Example:** create a workspace called `MyProject` with a flavor `FLV1/SYS1` under `C:\temp`

```powershell
.\spl.bat workspace --name MyProject --variant FLV1/SYS1 --out_dir C:\temp
```

Execute `.\spl.bat --help` to see all available options.

Note: one can use the `--variant` argument several times to create a project with multiple variants.
