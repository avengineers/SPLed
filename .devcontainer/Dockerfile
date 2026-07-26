FROM mcr.microsoft.com/devcontainers/base:ubuntu-24.04

# Corporate CA (proxy) — image-only concern. Paths are relative to the repo-root
# build context (devcontainer.json sets build.context "..").
COPY .devcontainer/certs/ /usr/local/share/ca-certificates/
RUN update-ca-certificates

# Ubuntu archive mirror swap — proxy workaround, image-only (archive.ubuntu.com
# is blocked by the corporate proxy; the uni-stuttgart mirror is allowed).
ARG UBUNTU_ARCHIVE_MIRROR=https://archive.ubuntu.com/ubuntu
RUN sed -i "s|http://archive.ubuntu.com/ubuntu|${UBUNTU_ARCHIVE_MIRROR}|g" \
        /etc/apt/sources.list.d/ubuntu.sources

# OS packages — single source of truth, shared with the bare-WSL path.
COPY bootstrap_ubuntu.sh /tmp/bootstrap_ubuntu.sh
RUN bash /tmp/bootstrap_ubuntu.sh --clean-apt-cache && rm -f /tmp/bootstrap_ubuntu.sh

# TLS trust for pipx/uv PyPI downloads in the Python toolchain install below.
ENV CURL_CA_BUNDLE=/etc/ssl/certs/ca-certificates.crt
ENV SSL_CERT_FILE=/etc/ssl/certs/ca-certificates.crt
ENV SSL_CERT_DIR=/etc/ssl/certs
ENV NODE_EXTRA_CA_CERTS=/etc/ssl/certs/ca-certificates.crt
ENV REQUESTS_CA_BUNDLE=/etc/ssl/certs/ca-certificates.crt
ENV PIP_CERT=/etc/ssl/certs/ca-certificates.crt

# Python toolchain (uv/CPython 3.11/Poetry) baked as the vscode user, so it lands
# in /home/vscode/.local (owned by the runtime user) and is on PATH the instant the
# container starts. Same script bare WSL and CI run at runtime — one source of truth.
COPY bootstrap_python.sh /tmp/bootstrap_python.sh
USER vscode
RUN bash /tmp/bootstrap_python.sh
# Source the poks toolchain env (written later by build.sh --install at onCreate) in
# every interactive shell, so userEnvProbe carries the compiler/cmake/ninja PATH into
# VS Code's extension host, terminals, and tasks.
RUN printf '\n# SPLed: source the poks toolchain env for interactive shells (picked up by userEnvProbe)\nfor f in /workspaces/*/build/env_setup.sh; do [ -f "$f" ] && . "$f"; done\n' >> /home/vscode/.bashrc
USER root
RUN rm -f /tmp/bootstrap_python.sh

# Python + Poetry on PATH image-wide (present at attach, before any lifecycle hook).
# This ENV covers docker-build RUN steps and plain `exec` (which inherit image env).
ENV PATH="/home/vscode/.local/bin:${PATH}"

# ...but DevPod runs onCreateCommand (`build.sh --install`) as the vscode user through a
# non-login PAM exec, whose PATH is read from /etc/environment — which does NOT include
# ~/.local/bin. Without this, build.sh dies with "Poetry not found" before it can install
# anything. Put the baked toolchain on the SYSTEM PATH too, so every process (login,
# non-login, lifecycle hooks, terminals) can find it. This is what the pre-refactor image
# achieved via /usr/local/bin symlinks; we keep the single ~/.local install location.
RUN sed -i 's|^PATH="|PATH="/home/vscode/.local/bin:|' /etc/environment \
    && grep -q '^PATH="/home/vscode/.local/bin:' /etc/environment \
    || { echo "ERROR: could not prepend ~/.local/bin to /etc/environment — its format changed; fix this RUN"; exit 1; }
