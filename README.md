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

## Testing & Quality Gates

Tests are executed using pytest with a two-dimensional marker strategy combining *type markers* (WHAT to test) and *gate markers* (WHEN to test). The CI pipeline automatically selects the right quality gate based on the build context.

For the full testing strategy, marker definitions, and the gate assignment matrix, see [Testing Strategy](doc/testing_strategy.md).

```powershell
# Run all tests for a specific gate
.\build.ps1 -selftests -marker "gate_develop_pr"

# Filter by variant
.\build.ps1 -selftests -filter "Disco" -marker "gate_develop_push"
```

## Developer Guide

For more information about the architecture, workflows, and conventions, see [AGENTS.md](AGENTS.md). This guide covers:

- Critical developer workflows (environment setup, building, testing)
- SPL-specific CMake patterns and KConfig feature system
- VS Code CMake extension configuration
- Component structure and integration points
