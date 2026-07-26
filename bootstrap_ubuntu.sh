#!/bin/bash
# Root-level OS prerequisites for SPLed on Ubuntu/Debian.
# Installs the apt packages the poks toolchain and build.sh assume are present
# but do not install themselves. Requires root:
#   - the devcontainer Dockerfile invokes it via RUN (root at build time)
#   - on a bare WSL/Ubuntu host, run it with:  sudo ./bootstrap_ubuntu.sh
# Proxy / cert / apt-mirror setup is NOT handled here (Dockerfile-only concern);
# a bare host is assumed to already trust the corporate CA and have working apt.
#
# Usage: bootstrap_ubuntu.sh [--clean-apt-cache]
#   --clean-apt-cache  Drop the apt lists afterwards to keep the image layer small.
#                      Image builds only -- passing it on a bare host would just force
#                      the user's next unrelated `apt install` to re-fetch the lists.
#                      (Passed explicitly rather than auto-detected: BuildKit does not
#                      create /.dockerenv in RUN steps, and cgroup v2 shows only "0::/",
#                      so there is no reliable "am I in an image build" probe.)
set -euo pipefail

clean_apt_cache=false
for arg in "$@"; do
    case "$arg" in
        --clean-apt-cache) clean_apt_cache=true ;;
        *) echo "bootstrap_ubuntu.sh: unknown argument '$arg'" >&2; exit 1 ;;
    esac
done

if [ "$(id -u)" -ne 0 ]; then
    echo "bootstrap_ubuntu.sh installs apt packages and must run as root." >&2
    echo "On a bare host run:  sudo ./bootstrap_ubuntu.sh" >&2
    exit 1
fi

apt-get update
apt-get install -y --no-install-recommends \
    libc6-dev \
    build-essential \
    7zip \
    pipx

if [ "$clean_apt_cache" = true ]; then
    apt-get clean
    rm -rf /var/lib/apt/lists/*
fi
