# SPLED: Software Product Line Engineering Demo

![SPLed logo](doc/_figures/SPLED_logo.png)

This demo repository contains a tiny but fully fledged [SPL](https://en.wikipedia.org/wiki/Software_product_line) for SW development. It is capable of holding a set of software modules that are differently used and configured among the variants. Each variant creates its own binaries.

* [![CI](https://github.com/avengineers/SPLed/actions/workflows/ci.yml/badge.svg)](https://github.com/avengineers/SPLed/actions/workflows/ci.yml)
* artifacts are archived under GitHub Actions (or locally inside the `build/<variant>/(prod|test)` directory)

## Start developing

SPLed builds on **Windows** (`build.ps1`) and **Linux** (`build.sh`). Both are thin wrappers: every
option prints and runs one `pypeline` command, and the build logic lives in `pipeline/*.yaml`.

| You type | Which runs |
| --- | --- |
| `-install` | the bootstrap creates `.venv`, then `pypeline run --config-file pipeline/bootstrap.yaml` |
| `-build` | `pypeline run --config-file pipeline/variant_build.yaml -i variant=<v> ...` |
| `-selftests` | `.venv/.../pytest -m <gate> -k <filter>` |

The pytest self tests build through `build.bat`/`build.sh` (spl-core `SplBuild`), so a test build
runs the same `variant_build.yaml` as a developer build. The JUnit report path is in `pytest.ini`,
so every caller writes `test/output/test-report.xml`. On a `release/<Variant>/...` branch
`-selftests` without `-filter` tests only that variant.

### Windows

```powershell
.\build.ps1 -install
```

It is recommended to run the install command every time you switch between different branches,
because dependencies and their versions might have been updated.

It is recommended to start VS Code using the `build.ps1` script to make sure all dependencies and
environment variables are properly configured.

```powershell
.\build.ps1 -startVSCode
```

### DevContainer / DevPod / Codespaces (recommended on Linux)

The repository ships a devcontainer (`.devcontainer/`) with the full toolchain baked into the image.
Open the folder in VS Code ("Reopen in Container"), run `devpod up .`, or start a Codespace — the
container's `onCreateCommand` runs `./build.sh --install` for you, so Python, Poetry, CMake, Ninja and
the compilers are on `PATH` in every terminal with no manual setup.

### Bare Linux host (e.g. WSL Ubuntu)

Provision the OS-level prerequisites once per machine, then install as usual:

```bash
sudo ./bootstrap_ubuntu.sh    # apt packages
./bootstrap_python.sh         # uv, CPython 3.11, Poetry (into ~/.local)
# open a fresh shell (or `source ~/.bashrc`) so ~/.local/bin is on PATH
./build.sh --install
```

The devcontainer image runs these same two scripts at build time, so container and bare host are
provisioned from one source of truth.

## Building a variant

```powershell
.\build.ps1 -build                       # asks which variant
.\build.ps1 -build -variants Disco
.\build.ps1 -build -variants Disco,Spa -buildType Debug
.\build.ps1 -build -variants all -buildKit test -target unittests
```

```bash
./build.sh --build --variant Disco
./build.sh --build --build-kit test --build-type Debug --variant Sleep
./build.sh --help                        # full flag list
```

Both scripts take the same options; PowerShell spells them `-buildKit`, Bash `--build-kit`.
`-reconfigure` and `-configureOnly` start from an empty CMake cache.

## Testing & Quality Gates

Tests are executed using pytest with a two-dimensional marker strategy combining *type markers*
(WHAT to test) and *gate markers* (WHEN to test). The CI pipeline automatically selects the right
quality gate based on the build context and runs one job per variant (`python pipeline/variants.py`)
on Windows, Linux, and inside the devcontainer. A `release/<Variant>/...` branch builds that variant only.

For the full testing strategy, marker definitions, and the gate assignment matrix, see
[Testing Strategy](doc/testing_strategy.md).

```powershell
# Run all tests for a specific gate
.\build.ps1 -selftests -marker "gate_develop_pr"

# One variant; a variant with a slash is spelled IDEA__Sloemada, as its test class
.\build.ps1 -selftests -filter Disco -marker "gate_develop_pr"
```

```bash
./build.sh --selftests --marker gate_develop_pr
./build.sh --selftests --filter IDEA__Sloemada --marker gate_develop_pr
```

Each CI job runs `pytest -k <variant> -m <gate>` for its own variant.

## Developer Guide

For more information about the architecture, workflows, and conventions, see [AGENTS.md](AGENTS.md). This guide covers:

- Critical developer workflows (environment setup, building, testing)
- SPL-specific CMake patterns and KConfig feature system
- VS Code CMake extension configuration
- Component structure and integration points
