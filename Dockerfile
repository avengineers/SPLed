# Multi-stage Dockerfile for SPLed project
FROM ubuntu:22.04 as base

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV PYTHONUNBUFFERED=1
ENV PYTHON_VERSION=3.11

# Install system dependencies (without cmake - we'll install specific version)
RUN apt-get update && apt-get install -y \
    build-essential \
    clang \
    gcc \
    g++ \
    git \
    curl \
    wget \
    python3.11 \
    python3.11-dev \
    python3.11-venv \
    python3-pip \
    graphviz \
    doxygen \
    cppcheck \
    ninja-build \
    mingw-w64 \
    gcc-mingw-w64 \
    g++-mingw-w64 \
    bash \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Install CMake 3.24.0 specifically
RUN wget https://github.com/Kitware/CMake/releases/download/v3.24.0/cmake-3.24.0-linux-x86_64.tar.gz \
    && tar -xzf cmake-3.24.0-linux-x86_64.tar.gz -C /opt \
    && ln -sf /opt/cmake-3.24.0-linux-x86_64/bin/cmake /usr/local/bin/cmake \
    && ln -sf /opt/cmake-3.24.0-linux-x86_64/bin/ctest /usr/local/bin/ctest \
    && ln -sf /opt/cmake-3.24.0-linux-x86_64/bin/cpack /usr/local/bin/cpack \
    && rm cmake-3.24.0-linux-x86_64.tar.gz \
    && cmake --version

# Create symbolic links for python
RUN ln -sf /usr/bin/python3.11 /usr/bin/python3 && \
    ln -sf /usr/bin/python3.11 /usr/bin/python

# Install Poetry
ENV POETRY_VERSION=2.1.3
RUN curl -sSL https://install.python-poetry.org | python3 -
ENV PATH="/root/.local/bin:$PATH"

# Configure Poetry
RUN poetry config virtualenvs.create true && \
    poetry config virtualenvs.in-project true

# Set working directory
WORKDIR /workspace

# Copy dependency files first for better caching
COPY pyproject.toml poetry.lock ./

# Install Python dependencies
#RUN poetry install --with dev --no-root -v

#RUN poetry lock

# Install Python dependencies
RUN poetry install --no-root

# Copy the entire project
COPY . .

# Install dos2unix to handle line ending conversion
RUN apt-get update && apt-get install -y dos2unix && rm -rf /var/lib/apt/lists/*

# Make build script executable and ensure it has proper line endings
RUN dos2unix build.sh 2>/dev/null || true && \
    chmod +x build.sh

# Make build script executable and ensure it has proper line endings
RUN chmod +x build.sh && \
    # Convert Windows line endings to Unix (if needed)
    sed -i 's/\r$//' build.sh

# Install dependencies by default
#RUN ./build.sh --install

# Expose any necessary ports (if the project serves content)
# EXPOSE 8080

# Set the default command
CMD ["/bin/bash"]
