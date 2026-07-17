# CLAUDE.md

Guidance for AI coding agents in this repository lives in **[AGENTS.md](AGENTS.md)** — read it first.

It covers the architecture, the `build.ps1` / `build.sh` entry points (Windows and Linux/macOS/devcontainer), the SPL-specific CMake macros, the KConfig feature system, and the CI setup. Note in particular: **a PR only merges when all CI checks are green** (branch protection enforces it), and CI is a thin wrapper over the build scripts, so "green in CI" ⇔ "works locally".
