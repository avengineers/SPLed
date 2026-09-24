#!/bin/bash
#
# Wrapper: every option prints and runs one pypeline command, listed in README.md. The Linux
# counterpart of build.ps1. The logic lives in pipeline/*.yaml and pytest.ini. CI calls pytest directly.

set -euo pipefail

usage() {
    echo "Usage: ./build.sh --install | --build | --selftests"
    echo "  --build accepts --variant <name|a,b|all>, --build-kit <prod|test>, --build-type <name>, --target <name>,"
    echo "          --reconfigure, --configure-only and --ninja-args <args>;"
    echo "  --selftests accepts --filter <expr>, --marker <expr> and --pytest-extra-args <args>."
}

run() {
    echo "Executing: $*"
    "$@"
}

# `if`, not `[ -n "$2" ] && ...`: the latter returns non-zero for an empty value, which `set -e` turns into an exit.
add_arg() { if [ -n "$2" ]; then args+=("$1" "$2"); fi; }
add_input() { if [ -n "$2" ]; then args+=(-i "$1=$2"); fi; }

INSTALL=false
BUILD=false
SELFTESTS=false
VARIANTS=""
BUILD_KIT=""
BUILD_TYPE=""
TARGET=""
RECONFIGURE=""
CONFIGURE_ONLY=""
NINJA_ARGS=""
FILTER=""
MARKER="gate_develop_push"
PYTEST_EXTRA_ARGS=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --install) INSTALL=true; shift ;;
        --build) BUILD=true; shift ;;
        --selftests) SELFTESTS=true; shift ;;
        --variant|--variants) VARIANTS="$2"; shift 2 ;;
        --build-kit) BUILD_KIT="$2"; shift 2 ;;
        --build-type) BUILD_TYPE="$2"; shift 2 ;;
        --target) TARGET="$2"; shift 2 ;;
        --reconfigure) RECONFIGURE=true; shift ;;
        --configure-only|--configureOnly) CONFIGURE_ONLY=true; shift ;;
        --ninja-args) NINJA_ARGS="$2"; shift 2 ;;
        --filter) FILTER="$2"; shift 2 ;;
        --marker) MARKER="$2"; shift 2 ;;
        --pytest-extra-args) PYTEST_EXTRA_ARGS="$2"; shift 2 ;;
        --help) usage; exit 0 ;;
        *)
            echo "Unknown argument: $1" >&2
            case $1 in -[!-]*) echo "This script's options take two dashes: --marker, not -marker." >&2 ;; esac
            usage >&2
            exit 1
            ;;
    esac
done

cd "$(dirname "$0")"

if [ "$INSTALL" = true ]; then
    # There is no pypeline before the virtual environment exists, so poetry creates it first.
    run poetry config virtualenvs.in-project true --local
    run poetry install
    run .venv/bin/pypeline run --config-file pipeline/bootstrap.yaml
fi
if [ "$SELFTESTS" = true ]; then
    # A release branch names the variant it releases, so test only that one. Empty on every other branch.
    if [ -z "$FILTER" ]; then FILTER="$(.venv/bin/python pipeline/variants.py --release-filter)"; fi
    args=(); add_arg -m "$MARKER"; add_arg -k "$FILTER"
    # shellcheck disable=SC2206 # deliberate word splitting: the value is a list of pytest arguments.
    if [ -n "$PYTEST_EXTRA_ARGS" ]; then args+=($PYTEST_EXTRA_ARGS); fi
    # The report file is the verdict, so a failing gate must not stop the wrapper.
    run .venv/bin/pytest "${args[@]}" || echo "pytest exited with code $?, continuing ..."
fi
if [ "$BUILD" = true ]; then
    args=(); add_input variant "$VARIANTS"; add_input build_kit "$BUILD_KIT"; add_input build_type "$BUILD_TYPE"; add_input target "$TARGET"
    add_input reconfigure "$RECONFIGURE"; add_input configure_only "$CONFIGURE_ONLY"; add_input ninja_args "$NINJA_ARGS"
    run .venv/bin/pypeline run --config-file pipeline/variant_build.yaml "${args[@]}"
fi

if [ "$INSTALL" = false ] && [ "$BUILD" = false ] && [ "$SELFTESTS" = false ]; then
    usage
fi
