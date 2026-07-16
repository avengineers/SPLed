# Cross-Platform Alignment Proposal: `hammocking` & `SPLed`

**Goal:** Develop and build both projects consistently under **Windows**, **Linux**, and inside a
**DevPod / DevContainer**, by aligning the GitHub workflows, the devcontainer definitions, and the
build scripts (`build.ps1` / `build.sh`).

**Non-goal (for now):** Unifying the Python/native dependency managers. `hammocking` stays on **uv**,
`SPLed` stays on **Poetry + poks**. This proposal aligns *structure and behaviour*, not the package
manager choice.

---

## 1. What the two projects are

| | **hammocking** | **SPLed** |
|---|---|---|
| Nature | Reusable Python tool (C mock generator), published to **PyPI** | SPL **demo product**: C variants built to binaries (`package-mode = false`) |
| Primary artifact | Python wheel | Variant binaries (`build/<Variant>/<Kit>/<Type>`) |
| C compilation | Only in integration tests (`tests/data/mini_c_test`) | Core of the project (variants × build kits) |
| Default branch | `develop` | `develop` |

Because their *purpose* differs, full unification is neither possible nor desirable. The value is in
aligning the **developer entry points** and **CI/devcontainer plumbing** so a contributor can move
between the repos without relearning everything.

---

## 2. Current state — side-by-side

### 2.1 Toolchain & dependency management

| Concern | hammocking | SPLed |
|---|---|---|
| Python version | `>=3.10,<3.14` (CI: **3.13**) | `>=3.11,<3.12` (**3.11**) |
| Python pkg mgr | **uv** (`uv.lock`, `uv sync`) | **Poetry** (`poetry.lock`) + `avengineers/bootstrap-installer` |
| Windows native deps | **scoop** → `scoopfile.json` (mingw from `spl-bucket`) | **poks** → `poks.json` (clang/gcc/cmake/ninja) **+** scoop mingw |
| Linux native deps | `apt` (in Dockerfile: clang/llvm/cmake/ninja) | `apt` (in Dockerfile: build-essential/clang/gcc/mingw/ninja) + CMake 3.24 tarball |
| pypeline role | **Full build**: lint → test → docs | **Env-setup only** (venv, poks, scoop, env script, PR changes) |
| Actual C build | n/a (pure-Python tool) | CMake + Ninja driven **directly by `build.ps1`/`build.sh`** |

### 2.2 Build scripts

| Aspect | hammocking `build.ps1` / `.sh` | SPLed `build.ps1` / `.sh` |
|---|---|---|
| Size / scope | Thin: install → `uv run pypeline run` | Large: bootstrap, variants, kits, CMake, pytest gates, VS Code |
| `build.sh` parity | **Good** — mirrors `.ps1`, clean logging, `-c/--clean`, `-i/--install` | **Diverged** — reimplements CMake logic, uses **poetry directly** (no bootstrap, no pypeline env-setup, no poks), has bugs (`cd ../../..`, duplicated `reconfigure` block) |
| Flag style | `-clean`, `-install` (PS) / `--clean`, `--install` (sh) | `-install`, `-build`, `-selftests`, `-variants`, `-buildKit`, `-marker`, … (many) |
| `build.bat` | Wrapper | Wrapper (`-waitForKey`) |

### 2.3 DevContainers

| Aspect | hammocking | SPLed |
|---|---|---|
| Definition style | **Modern**: `Dockerfile` + `features` + `customizations.vscode` | **Legacy**: `docker-compose.yml` + multi-stage `Dockerfile`, top-level `settings`/`extensions` |
| Base image | `mcr.microsoft.com/devcontainers/python:1-3.13-bookworm` | `ubuntu:22.04` |
| Dev features | `git`, `github-cli` (devcontainer features) | none (manual apt) |
| CA-cert handling | `initializeCommand` copies host certs + `SSL_CERT_FILE` etc. | `dos2unix`/`sed` line-ending fixes; no cert handling |
| User | default (`vscode`) | `root` |
| postCreate | `./build.sh --install` | `./build.sh --install` |
| Extensions | ruff, mypy, python, cmake-language-support, toml, PR github… | flake8, cpptools, cmake-tools, powershell, catch2… |

### 2.4 GitHub workflows (`.github/workflows/ci.yml`)

| Aspect | hammocking | SPLed |
|---|---|---|
| Triggers | push/PR on `develop`, `workflow_dispatch` | push/PR on `develop` + `release/*`, **nightly schedule**, dispatch |
| Lint job | ✅ `pre-commit` + `commitlint` | ❌ none |
| Windows build/test | ✅ `build.ps1` | ✅ `build.ps1 -install` + `-selftests -marker <gate>` |
| **Linux build/test** | ✅ `ubuntu-24.04`, setup-python 3.13, setup-uv, `build.sh` | ❌ **missing** |
| Quality gates | markers `unit`/`integration` | rich `gate_*` marker matrix by event/branch |
| Test report | `mikepenz/action-junit-report@v6` | `mikepenz/action-junit-report@v6` |
| Release | ✅ `python-semantic-release` → PyPI | ❌ (dep present, no job) |
| Artifacts | none | uploads `*.7z`, `pr_changes.json` |

### 2.5 Already-shared conventions (keep these!)

`develop` default branch · `pypeline-runner` · CMake + Ninja + GCC · pytest + JUnit XML ·
`mikepenz/action-junit-report@v6` · `actions/checkout@v6` · `USER_PATH_FIRST` scoop workaround ·
`spl-bucket` mingw · semantic-release · `AGENTS.md` + `.claude` · scoop on Windows.

---

## 3. Guiding principles for alignment

1. **Same entry points, project-specific internals.** Every contributor, on every OS, runs the same
   three commands; what happens inside may differ per project.
2. **`build.sh` must be a true peer of `build.ps1`** — same flags, same steps, same dependency path
   (this is where SPLed diverges most today).
3. **CI = thin wrapper around the build scripts.** No build logic in YAML; CI only sets up the OS and
   calls `build.*`. This guarantees "works in CI" ⇔ "works locally".
4. **All three platforms are first-class**: Windows, Linux, DevContainer. Add the missing Linux CI leg
   to SPLed.
5. **Converge devcontainers on the modern schema** (`features` + `customizations.vscode`) while
   keeping each project's toolchain layer.

---

## 4. Proposed target state

### 4.1 Aligned build-script contract

Adopt a shared flag vocabulary across **all four** scripts. PowerShell uses `-flag`, Bash uses
`--flag`; names otherwise identical.

| Intent | PowerShell | Bash | hammocking | SPLed |
|---|---|---|---|---|
| Install deps only | `-install` | `--install` | ✅ | ✅ |
| Clean artifacts | `-clean` | `--clean` | ✅ | ✅ |
| Build | `-build` | `--build` | (no-op / build wheel) | variants/kits |
| Run tests | `-selftests` | `--selftests` | maps to pytest | pytest gates |
| Test filter/marker | `-marker` / `-filter` | `--marker` / `--filter` | markers `unit`/`integration` | `gate_*` |
| Full default run | *(no args)* | *(no args)* | `pypeline run` | menu / build+test |

> **Key fixes for SPLed `build.sh`:** route install through the **same path as `build.ps1`**
> (bootstrap → pypeline env-setup → poks) instead of calling Poetry directly; remove the stray
> `cd ../../..` and the duplicated `reconfigure` block; support the same `-selftests -marker` contract
> so Linux CI can call it exactly like Windows CI calls `build.ps1`.

### 4.2 Aligned DevContainer layout

Converge **both** on the modern `devcontainer.json` schema. Keep a per-project `Dockerfile` for the
toolchain layer; drop SPLed's `docker-compose` indirection unless multi-service is actually needed.

Shared skeleton (identical in both repos except the `Dockerfile` toolchain layer and extensions):

```jsonc
{
    "name": "<project>",
    "build": { "dockerfile": "Dockerfile" },
    "initializeCommand": "mkdir -p .devcontainer/certs && cp /usr/local/share/ca-certificates/*.crt .devcontainer/certs/ 2>/dev/null; true",
    "features": {
        "ghcr.io/devcontainers/features/git:1": {},
        "ghcr.io/devcontainers/features/github-cli:1": {}
    },
    "postCreateCommand": "./build.sh --install",
    "customizations": {
        "vscode": {
            "extensions": [ /* project-specific */ ],
            "settings": {
                "python.defaultInterpreterPath": ".venv/bin/python"
            }
        }
    }
}
```

Per-project differences that **remain**:

- **Base image / toolchain:** hammocking keeps `devcontainers/python` + uv; SPLed keeps an image with
  the full C/variant toolchain (clang/gcc/mingw/ninja + pinned CMake) + Poetry/poks.
- **Extensions:** hammocking (ruff/mypy/toml); SPLed (cpptools/cmake-tools/catch2).
- **CA-cert env vars:** adopt hammocking's `SSL_CERT_FILE`/`REQUESTS_CA_BUNDLE`/`CURL_CA_BUNDLE`
  pattern in SPLed too — useful behind the corporate proxy.

Recommendation: **migrate SPLed to the Dockerfile+features model** and retire `docker-compose.yml`,
the `dos2unix`/`sed` hacks (solve line endings via `.gitattributes` instead), and `remoteUser: root`
(prefer non-root, matching hammocking).

### 4.3 Aligned GitHub workflow structure

Standardize on a **common job skeleton**; keep project-specific steps inside.

```
lint            → pre-commit + commitlint   (ADD to SPLed)
test-on-windows → build.ps1 …               (both have)
test-on-linux   → build.sh …                (ADD to SPLed)
release         → semantic-release          (hammocking has; SPLed: enable when desired)
```

Concrete changes:

- **SPLed: add a `lint` job** (`pre-commit` + `wagoid/commitlint-github-action`). This requires adding
  a **`.pre-commit-config.yaml`** to SPLed (hammocking's is a good template — commitizen, ruff, mypy,
  codespell; drop `uv-lock`, add a `poetry`/`poks`-appropriate hook if wanted).
- **SPLed: add a `test-on-linux` job** on `ubuntu-24.04` that calls `./build.sh --selftests --marker
  <gate>` — mirroring the Windows job. Prerequisite: fixing `build.sh` per §4.1.
- **Shared conventions to standardize in both files:** `actions/checkout@v6` with `fetch-depth: 0`,
  the `concurrency` block (hammocking has it; add to SPLed), `mikepenz/action-junit-report@v6` with
  identical options, and the `USER_PATH_FIRST` env on Windows.
- **Keep project-specific bits:** SPLed's `gate_*` marker computation + nightly schedule + artifact
  upload; hammocking's PyPI release + docs.

---

## 5. Concrete alignment matrix

| Item | Action | Repo(s) | Effort | Priority |
|---|---|---|---|---|
| Fix SPLed `build.sh` (bugs + go through bootstrap/pypeline, not raw Poetry) | Rewrite | SPLed | M | **High** |
| Align build-script flag vocabulary (§4.1) | Edit | both | S | High |
| Add `test-on-linux` CI job | Add | SPLed | S | **High** |
| Add `lint` CI job + `.pre-commit-config.yaml` | Add | SPLed | S | High |
| Migrate devcontainer to Dockerfile+features schema | Rewrite | SPLed | M | Medium |
| Adopt CA-cert env vars in devcontainer | Edit | SPLed | S | Medium |
| Replace `dos2unix`/`sed` with `.gitattributes` (`*.sh text eol=lf`) | Edit | SPLed | S | Medium |
| Add `concurrency` block to CI | Edit | SPLed | S | Medium |
| Non-root devcontainer user | Edit | SPLed | S | Low |
| Enable semantic-release job (if releasing) | Add | SPLed | M | Low |
| Document the shared contract in both `README`/`AGENTS.md` | Edit | both | S | Medium |

---

## 6. Suggested rollout (phased)

**Phase 1 — Make Linux real for SPLed (highest value)**
1. Fix `SPLed/build.sh`: same install path as `build.ps1`, remove bugs, honour `--selftests
   --marker`.
2. Add `test-on-linux` job to `SPLed/.github/workflows/ci.yml`.
3. Verify locally in a Linux container before pushing.

**Phase 2 — Align quality gates**
4. Add `.pre-commit-config.yaml` + `lint` job to SPLed.
5. Standardize CI conventions (checkout, concurrency, junit-report options) across both repos.

**Phase 3 — Converge DevContainers**
6. Rewrite `SPLed/.devcontainer` to Dockerfile+features; retire compose + line-ending hacks; adopt
   cert env vars; non-root user.
7. Confirm both open cleanly in DevPod and VS Code Dev Containers.

**Phase 4 — Polish**
8. Align flag vocabulary in all four build scripts.
9. Document the shared 3-command contract in both READMEs/`AGENTS.md`.
10. Optionally enable the SPLed release job.

---

## 7. The end result (developer experience)

On **any** of Windows / Linux / DevContainer, in **either** repo:

```powershell
# Windows
.\build.ps1 -install      # set up toolchain + Python env
.\build.ps1               # full default build/test
```
```bash
# Linux / DevContainer
./build.sh --install      # set up toolchain + Python env
./build.sh                # full default build/test
```

DevContainer: open the folder → `postCreateCommand` runs `./build.sh --install` → ready to build.
CI: `lint` → `test-on-windows` + `test-on-linux` (thin wrappers over the same scripts) → optional
`release`.

The package managers (uv vs Poetry+poks) and the C-build depth differ **behind** these commands —
which is exactly the boundary this proposal preserves.

---

## 8. Implementation status

### Phase 1 — done (branch `116-linux-build`)

Implemented and pushed as the working basis:

- **`spl-core` (cross-repo):** `SplBuild.execute()` was hard-wired to `build.bat`, so the pytest
  self tests could only build on Windows. It is now **platform-aware** — `build.bat` on Windows,
  `bash ./build.sh` on Linux/macOS — with unit tests covering both. Pushed on branch
  `feat/platform-aware-splbuild` (avengineers/spl-core).
- **`SPLed/build.sh`:** rewritten as a true peer of `build.ps1` (aligned flags, JUnit report at
  `test/output/test-report.xml`, executable bit set); removed the stray `cd ../../..` and the
  duplicated reconfigure block.
- **`SPLed/CMakeLists.txt`:** use `:` as the `PATH` separator on non-Windows so the venv Python
  (`kconfig.cmake`) is found on Linux.
- **`SPLed/.github/workflows/ci.yml`:** added a `test-on-linux` job (`ubuntu-24.04`) that mirrors the
  Windows quality-gate selection and runs `./build.sh --install` + `--selftests`.
- Removed `test/Disco/test_Disco_linux.py` — the standard `test_Disco.py` is now cross-platform.

> **Dependency pin:** `SPLed` pins `spl-core==8.6.0` (the final release that ships the
> platform-aware `SplBuild` plus the cross-platform `gcov_maid` fix).

> **Verification note:** the local WSL environment has no network access to PyPI/GitHub, so the full
> Linux build could not be exercised locally; the `spl-core` unit tests, `bash -n`, and argument
> smoke tests pass, and the GitHub-hosted `test-on-linux` job is the end-to-end gate.

### Phases 2–4 — pending

Lint job + `.pre-commit-config.yaml`, devcontainer convergence, and flag-vocabulary polish remain as
described in §6.
