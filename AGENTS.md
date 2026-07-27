# SPLED: Software Product Line Engineering Demo

## Architecture Overview

SPLED is a **Software Product Line (SPL)** demonstrating variant-based configuration management.
Each variant (Disco, Sleep, Spa, etc.) compiles into separate binaries using shared components with different feature configurations.

**Key architectural concepts:**

- **Variants**: Product configurations in `variants/<VariantName>/` containing `config.txt` (KConfig features), `parts.cmake` (component selection), and `config.cmake` (CMake settings)
- **Components**: Modular C code in `components/<name>/` with CMakeLists.txt using SPL-specific macros
- **Build Kits**: `prod` (production C code) vs `test` (C++ with GoogleTest for unit testing)
- **Feature Configuration**: KConfig system (`KConfig` file) drives conditional compilation via CMake variables

## Critical Developer Workflows

### Environment Setup

**Always** run `.\build.ps1 -install` when:

- First cloning the repository
- Switching branches (dependencies may have changed)
- After pulling updates

On **Linux/macOS or inside a devcontainer**, use the peer script `./build.sh --install`. `build.sh` performs the same steps as `build.ps1`, so every build/test workflow below has a `build.sh` equivalent. The flag *vocabulary* is close but not identical (Bash uses `--flag`, PowerShell uses `-flag`):

| `build.ps1` | `build.sh` |
| --- | --- |
| `-install`, `-build`, `-clean`, `-selftests`, `-marker`, `-filter`, `-target`, `-command`, `-reconfigure` | same names, `--` prefixed |
| `-variants <v>` | `--variant <v>` (single variant; default `Disco`) |
| `-buildKit <k>` / `-buildType <t>` | `--build-kit <k>` / `--build-type <t>` |
| `-startVSCode`, `-installVSCode`, `-installOptional`, `-configureOnly`, `-pytestExtraArgs`, `-ninjaArgs`, `-waitForKey` | *no equivalent* |

`./build.sh --help` is authoritative for the Bash side.

On a **bare Linux host (e.g. WSL Ubuntu) that is not a devcontainer**, `build.sh --install` assumes some OS-level prerequisites already exist. Provision them once per machine with the two bootstrap scripts — the devcontainer image runs the same scripts, so this is a single source of truth (see [`doc/devcontainer-and-bootstrap-design.md`](doc/devcontainer-and-bootstrap-design.md)):

```bash
sudo ./bootstrap_ubuntu.sh    # root: apt packages (libc6-dev, build-essential, 7zip, pipx)
./bootstrap_python.sh         # user: uv, CPython 3.11, Poetry (into ~/.local)
# then open a fresh shell (see note below) so ~/.local/bin is on PATH:
./build.sh --install          # user: poetry install + poks toolchain
```

If `poetry` is not found after `bootstrap_python.sh`, open a new shell (or `source ~/.bashrc`) so `~/.local/bin` is on `PATH`, then run `./build.sh --install`. Inside the devcontainer this is automatic: the Dockerfile bakes both `bootstrap_ubuntu.sh` and `bootstrap_python.sh` into the image at build time, and `onCreateCommand` runs `build.sh --install`.

**Always** start VS Code with: `.\build.ps1 -startVSCode` to ensure proper environment variables and Python virtual environment activation (`.venv` with Poetry dependencies).

### VS Code CMake Extension Configuration

VS Code users can build directly using the CMake extension via `.vscode` configuration files:

- **Build Kit selection** (`.vscode/cmake-kits.json`): Choose between `prod` (production C code) and `test` (GTest C++ unit tests)
- **Variant selection** (`.vscode/cmake-variants.json`): Select variant (Disco, Spa, Sleep, Base/Dev, IDEA/Sloemada) and build type (Debug/Release)
- **CMake settings** (`.vscode/settings.json`): Configures build directory pattern `build/${variant}/${buildKit}/${buildType}`, Ninja generator, and passes BUILD_KIT/BUILD_TYPE variables to CMake
- Use CMake extension's status bar to select kit/variant/build type, then build using CMake commands or tasks

### Building Variants

```powershell
# Interactive variant selection
.\build.ps1 -build

# Specific variant
.\build.ps1 -build -variants Disco

# Clean build
.\build.ps1 -build -variants Spa -clean

# Test build (includes unit tests)
.\build.ps1 -build -buildKit test -buildType Debug
```

Build outputs: `build/<VariantName>/<BuildKit>/<BuildType?>/`

### Testing

Tests are **Python-based** using pytest for build validation and report checks:

```powershell
# Run all tests
.\build.ps1 -selftests

# Filtered tests
.\build.ps1 -selftests -filter "Disco"

# Specific markers (see pytest.ini)
.\build.ps1 -selftests -marker "build_debug"
```

Component unit tests: GTest/GMock in `components/*/test/*.cc` files, run via `test` build kit.

View reports: Use tasks "Open variant test report" / "Open variant coverage report" from VS Code.

## Continuous Integration

CI runs on **GitHub Actions** (`.github/workflows/ci.yml`) for every push/PR to `develop` and `release/*`, plus a nightly schedule. **A PR only merges when all checks are green** — branch protection enforces this, so a red build blocks the merge by definition. Don't add manual "remember to check CI" notes to docs or PRs; the gate is automatic.

Jobs:

- `determine-gate` — computes the `gate_*` quality-gate marker once (by event/branch) and shares it with all three build jobs via `needs`.
- `test-on-windows` (`windows-2025`) — `build.ps1 -install` then `-selftests -marker <gate>`.
- `test-on-linux` (`ubuntu-24.04`) — bare-runner path: `bootstrap_ubuntu.sh` + `bootstrap_python.sh`, then `build.sh --install` and `--selftests --marker <gate>`.
- `test-devcontainer` (`ubuntu-24.04`) — builds `.devcontainer/` via `devcontainers/ci` (which runs `onCreateCommand`, i.e. `build.sh --install`) and runs `build.sh --selftests --marker <gate>` inside the container.

CI is a **thin wrapper**: it only sets up the OS and calls the build scripts, so "green in CI" ⇔ "works locally". Runners are pinned to explicit images (never `*-latest`), so an OS/toolchain bump is always a reviewable change rather than a surprise.

## SPL-Specific CMake Patterns

Component `CMakeLists.txt` files use **spl-core macros** (not standard CMake):

```cmake
# Add source files (production code)
spl_add_source(src/my_component.c)

# Add test files (GTest, only in test build kit)
spl_add_test_source(test/test_my_component.cc)

# Declare dependencies on other components
spl_add_required_interface(components/rte)

# Finalize component (must be last)
spl_create_component(LONG_NAME "My Component")
```

**Conditional dependencies** use KConfig variables from `config.txt`:

```cmake
if(AUTO_OFF STREQUAL "True")
    spl_add_required_interface(components/auto_off)
endif()
```

Variant's `parts.cmake` lists components with `spl_add_component(components/<name>)`.

## KConfig Feature System

Features defined in `KConfig` (menuconfig syntax) generate CMake variables via `config.txt`:

- `CONFIG_BLINKING=y` → CMake variable `BLINKING="True"`
- `# CONFIG_AUTO_OFF is not set` → CMake variable `AUTO_OFF="False"`

Edit feature config: `.\build.ps1 -command ".venv\Scripts\poetry run guiconfig"` (requires KCONFIG_CONFIG env var set to variant's config.txt).

Check feature values in source code via generated `autoconf.h` header.

## Project-Specific Conventions

1. **No direct CMake invocation**: Always use `build.ps1` wrapper (handles variant selection, environment, Poetry, etc.)
2. **Component isolation**: Each component has own CMakeLists.txt, must declare all dependencies explicitly
3. **Test location**: Python integration tests in `test/<VariantName>/`, C++ unit tests in `components/*/test/`
4. **Dependency management**: Python deps via Poetry (`pyproject.toml`), C/C++ external deps fetched by CMake FetchContent

## Common Integration Points

- **RTE (Runtime Environment)**: `components/rte` - shared interfaces, all components depend on it
- **Main entry**: `components/main/src/main.c` - calls OS scheduler
- **OS abstraction**: `components/os` - simple task scheduler (configurable period via KConfig)
- **Platform types**: `components/platform_types` - standard types (uint8_t, etc.)

Components communicate via RTE signals/runnable interfaces (see `rte.h` for patterns).

## Key Files for Understanding

- [build.ps1](build.ps1) - Entry point for all build/test operations (Windows)
- [build.sh](build.sh) - Peer entry point for Linux/macOS/devcontainer
- [.github/workflows/ci.yml](.github/workflows/ci.yml) - Windows + Linux CI, thin wrapper over the build scripts
- [CMakeLists.txt](CMakeLists.txt#L8) - Includes variant config and spl-core framework
- [KConfig](KConfig) - Feature model definition
- [variants/Disco/parts.cmake](variants/Disco/parts.cmake) - Example component selection
- [components/spled/CMakeLists.txt](components/spled/CMakeLists.txt) - Example conditional dependencies
- [test/Disco/test_Disco.py](test/Disco/test_Disco.py) - Example pytest structure using `SplBuild` helper
