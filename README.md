# SPLED: Software Product Line Engineering Demo

![Alt text](doc/_figures/SPLED_logo.png)

This demo repository contains a tiny but fully fledged [SPL](https://en.wikipedia.org/wiki/Software_product_line) for SW development. It is capable of holding a set of software modules that are differently used and configured among the variants. Each variant creates its own binaries.

* [![selftests](https://github.com/avengineers/spldemo/actions/workflows/test.yml/badge.svg)](https://github.com/avengineers/spldemo/actions/workflows/test.yml)
* artifacts are archived under Github Actions (or locally inside the `build/<variant>/(prod|test)` directory)

## Start developing

For installing the dependencies run:

```powershell
.\build.ps1 -install
```

It is recommended to run the install command every time you switch between different branches,
because dependencies and their versions might have been updated.

To build a variant from the command line run:

```powershell
.\build.ps1 -build
```

It is recommended to start VS Code using the `build.ps1` script to make sure all dependencies and environment variables are properly configured.

```powershell
.\build.ps1 -startVSCode
```
