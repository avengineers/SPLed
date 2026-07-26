# SPLed DevContainer, DevPod & Bare-Linux Bootstrap — Design

**Status:** implemented and validated (see § Validation).
**Context:** Phase 2 of [`cross-platform-alignment-proposal.md`](cross-platform-alignment-proposal.md)
— "Converge DevContainers & add DevPod support".

This is the single current design for how a SPLed development environment is provisioned on Linux.
It consolidates four iterations that were designed and implemented on
`feat/devcontainer-devpod-support`; the intermediate design and plan documents were folded into this
one at merge time.

## Goal

A contributor gets a working SPLed environment with no manual setup steps, via any of three entry
points:

- **DevPod / VS Code Dev Containers / GitHub Codespaces** — `devpod up` (or "Reopen in Container")
  builds an image and attaches. In the connected VS Code, all three build paths work immediately:
  a terminal running `./build.sh`, `cmake` invoked directly, and the CMake Tools extension.
- **Bare Linux host (e.g. WSL Ubuntu)** — two bootstrap scripts, then the normal `build.sh --install`.
- **CI** — the `test-on-linux` job provisions itself the same way a bare host does.

The provisioning steps are a **single source of truth**: the same two scripts serve all three paths.
No package list is duplicated between a Dockerfile and a script.

## Architecture

Prerequisites are split into two repo-root scripts **by privilege level**. That split is what removes
the `sudo` problem: root-only work runs where root is already available, user-level work runs
unprivileged and installs into `~/.local`.

| Layer | Provides | Runs as | When |
|---|---|---|---|
| `.devcontainer/Dockerfile` | base image, CA certs, apt-mirror swap, `ENV SSL_*`, PATH wiring | root | image build |
| `bootstrap_ubuntu.sh` | apt: `libc6-dev`, `build-essential`, `7zip`, `pipx` | root | image build (`RUN`) / `sudo` on a bare host |
| `bootstrap_python.sh` | `uv` → CPython 3.11 → Poetry, into `~/.local` | unprivileged | image build (as `vscode`) / directly on a bare host |
| `build.sh --install` (unchanged) | `poetry install` + the pypeline/poks toolchain | unprivileged | `onCreateCommand` / directly on a bare host |

**Flows**

- **Devcontainer / DevPod / Codespaces:** the image build runs certs → mirror swap →
  `bootstrap_ubuntu.sh` (root) → `bootstrap_python.sh` (as `vscode`); then
  `onCreateCommand: ./build.sh --install`.
- **Bare WSL Ubuntu:** `sudo ./bootstrap_ubuntu.sh` → `./bootstrap_python.sh` → *(fresh shell)* →
  `./build.sh --install`.
- **CI (`test-on-linux`):** the same two scripts, then `$HOME/.local/bin` onto `GITHUB_PATH`.

## Decisions

### D1 — Two scripts, split by privilege

`bootstrap_ubuntu.sh` contains **no self-elevation logic** — it simply requires root and prints a
`sudo ...` hint otherwise. `bootstrap_python.sh` never needs root because it installs only into
`~/.local`.

Proxy, CA-cert and apt-mirror handling stay **Dockerfile-only** ("universal parts only"). A bare host
is assumed to already trust its corporate CA and have working apt — that is the contributor's
machine, not these scripts' job.

### D2 — The Python toolchain is baked into the image, not run at a lifecycle hook

An earlier iteration moved `bootstrap_python.sh` to `postCreateCommand`. That traded away the
"VS Code just works" property: the Python extension probes for an interpreter before `postCreate`
finishes, so a fresh Codespace opened with no Python.

The Dockerfile now runs `bootstrap_python.sh` at **build time as the `vscode` user**, so it lands in
`/home/vscode/.local` (owned by the runtime user) and is on `PATH` the instant the container starts —
before any lifecycle hook or extension probe. It is still the *same script* the bare-host and CI
paths run, so the single-source-of-truth property is preserved.

### D3 — PATH is wired in two places, deliberately

Both are required; neither is redundant:

- **`ENV PATH="/home/vscode/.local/bin:${PATH}"`** covers docker-build `RUN` steps and plain
  `docker exec` (which inherit image env).
- **`/etc/environment`** covers everything else. DevPod runs `onCreateCommand` as `vscode` through a
  **non-login PAM exec**, whose PATH is read from `/etc/environment` — which does *not* include
  `~/.local/bin`. Without this, `build.sh --install` dies with "Poetry not found" before installing
  anything.

The `/etc/environment` edit is a `sed` with a **verification guard** that fails the build if the
file's format ever changes, rather than silently producing an image with a broken PATH.

### D4 — One environment mechanism for the poks toolchain: `userEnvProbe`

`build.sh --install` writes the poks toolchain paths (`clang`/`gcc`/`cmake`/`ninja`) into
`build/env_setup.sh`. A snippet appended to `~/.bashrc` sources that file for interactive shells, and
`userEnvProbe: loginInteractiveShell` captures the result and applies it to the VS Code extension
host, terminals, and tasks.

This is DRY — paths flow `poks.json` → `env_setup.sh` → shell → extension host with zero duplication
— and it is the standard devcontainer mechanism. **No `remoteEnv` fallback with hardcoded poks paths
is pre-shipped** (YAGNI); validation confirmed the probe reaches the extension host. If that ever
regresses, add `remoteEnv` with the stable `~/.poks/apps/<name>/<version>/bin` paths *at that point*.

### D5 — Poetry needs a bare `python` on PATH

Poetry 2.x **always** probes PATH for a bare `python` when resolving the venv interpreter — verified
that `virtualenvs.use-poetry-python = true` does *not* suppress this. The base image ships no bare
`python`, so `poetry install` fails with `[Errno 2] No such file or directory: 'python'`.

`bootstrap_python.sh` therefore symlinks `~/.local/bin/python` → the uv-managed 3.11. Deliberately
**only `python`, never `python3`**: every host this runs on already has a system `python3`, and
`~/.profile` prepends `~/.local/bin`, so a `python3` symlink would silently shadow the OS interpreter
for all of a bare-WSL user's unrelated work.

Poetry's `virtualenvs.in-project` / `use-poetry-python` config is set **globally** (no `--local`),
because the script also runs at image-build time where the CWD is not a writable project directory.
`build.sh --install` re-applies `--local` in the workspace anyway.

### D6 — Build context is the repo root; `.dockerignore` is lean and rule-bound

`context: ".."` is required so the Dockerfile can `COPY` the repo-root bootstrap scripts. This does
not bloat the image — layers contain only what `COPY`/`RUN` produce, and exactly three things are
copied (`.devcontainer/certs/`, the two scripts). The only cost is that Docker walks the repo as
build context on each rebuild, which a lean `.dockerignore` keeps cheap.

Two rules are recorded in `.dockerignore` itself, both learned the hard way:

1. **Never list anything the Dockerfile `COPY`s** — the `COPY` would find nothing.
2. **Never list a build tool's own scratch dir.** DevPod's feature-injection stage copies
   `.devpod-internal/` into the image; filtering it exits the build with status 125
   (`no items matching glob ".devpod-internal" copied (1 filtered out using .dockerignore)`). It
   stays in `.gitignore` (never commit build-tool scratch) but must never be in `.dockerignore`.

Cosmetic, tiny-tree entries were dropped: they save nothing measurable and each is a latent repeat of
rule 1.

### D7 — Only `github-cli` as a devcontainer feature

`git` already ships in `mcr.microsoft.com/devcontainers/base:ubuntu-24.04`, so the `git` feature was
pure duplication. `github-cli` is kept because it is actually used inside the container — which is
also why D6's `.devpod-internal/` fix (not feature removal) is the real build fix.

### D8 — Corporate-proxy accommodations degrade gracefully

The proxy accommodations are all no-ops outside the corporate network:

- **CA certs** — an `initializeCommand` copies the host's `/usr/local/share/ca-certificates/*.crt`
  into `.devcontainer/certs/`, which the Dockerfile `COPY`s before running `update-ca-certificates`.
  `.devcontainer/certs/` is gitignored except for a tracked `.gitkeep`, so the directory always
  exists after clone and both the copy and the COPY are no-ops off the corporate network.

  > **Known limitation:** the `initializeCommand` runs on the **host** and is a bash one-liner, so it
  > only works from a POSIX host. On a Windows host VS Code runs it through `cmd.exe`, where
  > `mkdir -p` creates a directory named `-p` and `cp` does not exist. Windows contributors using
  > Dev Containers locally should copy the CA `.crt` into `.devcontainer/certs/` by hand instead.

- **`UBUNTU_ARCHIVE_MIRROR` build arg** — `archive.ubuntu.com` is blocked by the corporate proxy; the
  uni-stuttgart mirror is allowed. It is a public mirror, so it works anywhere.
- **`SSL_*` / `*_CA_BUNDLE` env vars** — belt-and-suspenders TLS trust for `curl`/`pip`/`requests`/
  `node`; harmless where the system CA already suffices.

**Known limitation (not fixed):** a *cache-cold* image build behind the corporate proxy needs
`HTTP_PROXY`/`HTTPS_PROXY`/`NO_PROXY` for build-time egress (apt/pipx/uv). Rather than baking proxy
config into a portable image, pass them as build args on demand:

```jsonc
// devcontainer.json build.args — only for a cold build behind the proxy:
"HTTP_PROXY":  "${localEnv:HTTP_PROXY}",
"HTTPS_PROXY": "${localEnv:HTTPS_PROXY}",
"NO_PROXY":    "${localEnv:NO_PROXY}"
```

## Why `bootstrap_ubuntu.sh` installs those four packages

Hand-validating the Phase 1 Linux build on a bare WSL/Ubuntu host failed *after* `build.sh --install`
had successfully provisioned the poks toolchain. Three system-level packages that neither poks nor
Poetry install were absent:

1. **`libc6-dev`** — no glibc development files at all (no `Scrt1.o`/`crti.o`, no `libc.so`, no
   headers). CMake's compiler sanity check failed before any SPLed source was touched: the
   poks-provisioned Clang 20.1.8 could not link a trivial program (`ld: cannot find Scrt1.o`,
   `cannot find -lc`).
2. **`build-essential`** — after fixing (1), linking still failed on `crtbeginS.o`/`-lgcc`/`-lgcc_s`.
   The host had *no system GCC*. Clang auto-detects a system GCC's runtime objects at the standard
   multiarch path (`/usr/lib/gcc/x86_64-linux-gnu/`), which did not exist; the poks-provisioned GCC
   15.2.0 uses a non-standard triple (`x86_64-pc-linux-gnu`) that Clang's auto-detection does not
   scan.
3. **`7zip`** — the pytest `ArtifactsArchiver` shells out to the `7z` CLI to archive build artifacts,
   producing 5 errors (`FileNotFoundError: '7z'`) in `--selftests`.

`pipx` is the fourth, needed by `bootstrap_python.sh`.

CI never caught these because the `ubuntu-24.04` runner preinstalls all three — which is exactly why
`test-on-linux` now provisions via these scripts instead of `actions/setup-python`, and why the
`test-devcontainer` job exists.

## Validation

Executed for real with `devpod up . --recreate --ide=none` (podman-as-docker provider), and
subsequently in a DevPod with VS Code attached over SSH:

- `devpod up --recreate` completes with exit 0 — both build stages succeed, the container starts,
  `onCreateCommand` runs `build.sh --install` fully, all four poks tools download, `env_setup.sh` is
  generated.
- A **login-interactive shell** — exactly what `userEnvProbe: loginInteractiveShell` captures for the
  extension host, and what VS Code terminals use — resolves all six tools: `python`, `poetry`,
  `cmake`, `ninja`, `clang`, `gcc`. This positively confirms the D4 link.
- `./build.sh --build --variant Disco` inside the container builds and links `spled.exe` using the
  poks clang/cmake/ninja. Direct `cmake` uses the same on-PATH tools.

The CMake Tools extension UI and the Python extension's `.venv` selection are GUI-only and were
confirmed manually; the environment-delivery mechanism they depend on is covered by the above.

## CI

Three jobs run the selftest suite, each covering a distinct provisioning path:

- `test-on-windows` — `build.ps1` / Scoop.
- `test-on-linux` — bare-runner path: `bootstrap_ubuntu.sh` + `bootstrap_python.sh` + `build.sh`.
  This replaced `actions/setup-python` + `pipx install poetry`, so CI now exercises the same scripts a
  bare-WSL contributor runs.
- `test-devcontainer` — `devcontainers/ci@v0.3`, the only job exercising the image itself. Its
  feature injection does not use `.devpod-internal/` (that is DevPod-specific), so the D6 bug is not
  reproducible in CI — DevPod remains a manual verification.

## Out of scope

- Windows / `build.ps1` bootstrap parity (the Scoop path) — Phase 4.
- Non-Ubuntu/Debian distros — `bootstrap_ubuntu.sh` is apt-specific by name and content.
- Baking the poks toolchain (as opposed to the Python toolchain) into the image.
