#!/bin/bash
# User-level Python toolchain for SPLed (no root / no sudo).
# Installs uv, a standalone CPython 3.11, and Poetry into the current user's
# ~/.local, then points Poetry at the uv 3.11 for the project virtualenv.
# Run AFTER bootstrap_ubuntu.sh (which provides pipx):
#   - the devcontainer bakes it into the image at build time (.devcontainer/Dockerfile)
#   - on a bare WSL/Ubuntu host:  ./bootstrap_python.sh
set -euo pipefail

# pipx (from bootstrap_ubuntu.sh) installs isolated CLI tools into ~/.local/bin.
pipx install uv
pipx ensurepath                        # ~/.local/bin on PATH for future shells
export PATH="$HOME/.local/bin:$PATH"   # ...and for the rest of this script

# Standalone CPython 3.11 (prebuilt: no from-source build, no GPG keyserver;
# matches pyproject requires-python <3.12,>=3.11).
uv python install 3.11
uv_py="$(uv python find 3.11)"

# Poetry 2.x ALWAYS probes PATH for a bare `python` when it resolves the venv
# interpreter — verified that `virtualenvs.use-poetry-python = true` does NOT suppress
# this. The base image ships no bare `python`, so `poetry install` dies with
# "[Errno 2] No such file or directory: 'python'". Give it one, pointing at the
# uv-managed 3.11 (which matches pyproject's requires-python). This is the same thing
# the pre-refactor image achieved; we just keep it in ~/.local instead of /usr/local.
#
# ONLY `python` — deliberately NOT `python3`: every host this runs on already has a
# system `python3`, and ~/.profile prepends ~/.local/bin, so a `python3` symlink here
# would silently shadow the OS `python3` for all of the user's other work on a bare
# WSL host. `python` alone satisfies Poetry's probe (verified) and shadows nothing.
ln -sf "$uv_py" "$HOME/.local/bin/python"

# Poetry, pinned to the uv 3.11, installed isolated via pipx.
pipx install --python "$uv_py" poetry

# in-project set GLOBALLY (no --local): this script also runs at image-build time, where
# the CWD is not a writable project dir. build.sh --install re-applies `--local` in the
# workspace anyway. use-poetry-python additionally pins Poetry's own uv-3.11 as the venv
# interpreter (belt-and-suspenders alongside the `python` symlink above).
poetry config virtualenvs.in-project true
poetry config virtualenvs.use-poetry-python true

echo "bootstrap_python.sh complete. If 'poetry' is not found, open a new shell"
echo "(or: source ~/.bashrc) so ~/.local/bin is on PATH, then run ./build.sh --install"
