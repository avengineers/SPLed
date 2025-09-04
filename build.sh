#!/bin/bash

# SPLed build script for Linux/Unix systems
# This script provides Linux equivalents to the Windows PowerShell build.ps1

# Exit on error, but allow some commands to fail gracefully
set -e
set -o pipefail

# Default values
INSTALL=false
BUILD=false
CLEAN=false
BUILD_KIT="prod"
BUILD_TYPE=""
TARGET="all"
VARIANT=""
COMMAND=""
SELFTESTS=false
RECONFIGURE=false

# Function to show usage
show_help() {
    echo "SPLed build script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --install              Install dependencies"
    echo "  --build                Build the project"
    echo "  --clean                Clean build artifacts"
    echo "  --selftests            Run self tests"
    echo "  --build-kit <kit>      Build kit: 'prod' or 'test' (default: prod)"
    echo "  --build-type <type>    Build type (Debug, Release, etc.)"
    echo "  --target <target>      Build target (default: all)"
    echo "  --variant <variant>    Variant to build (default: Disco)"
    echo "  --command <cmd>        Command to execute"
    echo "  --reconfigure          Delete CMake cache and reconfigure"
    echo "  --help                 Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0 --install"
    echo "  $0 --build --variant Disco"
    echo "  $0 --build --build-kit test --variant Sleep"
    echo "  $0 --clean --build --variant Spa"
    echo "  $0 --build --reconfigure --variant Disco"
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --install)
            INSTALL=true
            shift
            ;;
        --build)
            BUILD=true
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --selftests)
            SELFTESTS=true
            shift
            ;;
        --build-kit)
            BUILD_KIT="$2"
            shift 2
            ;;
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --target)
            TARGET="$2"
            shift 2
            ;;
        --variant)
            VARIANT="$2"
            shift 2
            ;;
        --command)
            COMMAND="$2"
            shift 2
            ;;
        --reconfigure)
            RECONFIGURE=true
            shift
            ;;
        --help)
            show_help
            exit 0
            ;;
        *)
            echo "Unknown option $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Default variant if not specified
if [ -z "$VARIANT" ]; then
    VARIANT="Disco"
fi

echo "SPLed build script"
echo "Working directory: $(pwd)"

# Check if we're in a poetry environment or need to activate it
if [ -f ".venv/bin/activate" ]; then
    echo "Activating Python virtual environment..."
    source .venv/bin/activate
elif [ -f "pyproject.toml" ] && command -v poetry &> /dev/null; then
    echo "Using Poetry environment..."
    # Poetry will handle the virtual environment
else
    echo "Warning: No Python virtual environment found"
fi

if [ "$INSTALL" = true ]; then
    echo "Installing dependencies..."

    if command -v poetry &> /dev/null; then
        poetry install
    else
        echo "Error: Poetry not found. Please install Poetry first."
        exit 1
    fi

    echo "Dependencies installed successfully."
fi

if [ "$CLEAN" = true ]; then
    echo "Cleaning build artifacts for variant '$VARIANT'..."
    rm -rf "build/$VARIANT"
    echo "Clean completed."
fi

if [ "$BUILD" = true ]; then
    echo "Building variant '$VARIANT' with build kit '$BUILD_KIT'..."

    # Create build directory
    BUILD_DIR="build/$VARIANT/$BUILD_KIT"
    if [ -n "$BUILD_TYPE" ]; then
        BUILD_DIR="build/$VARIANT/$BUILD_KIT/$BUILD_TYPE"
    fi

    echo "BUILD_DIR:$BUILD_DIR"

    mkdir -p "$BUILD_DIR"

    # Delete CMake cache and reconfigure
    if [ "$RECONFIGURE" = true ]; then
        echo "Deleting CMake cache for reconfiguration..."
        rm -f "$BUILD_DIR/CMakeCache.txt"
        rm -rf "$BUILD_DIR/CMakeFiles"
    fi

    # CMake configure
    CMAKE_ARGS="-DVARIANT=$VARIANT -DBUILD_KIT=$BUILD_KIT"
    if [ -n "$BUILD_TYPE" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DBUILD_TYPE=$BUILD_TYPE -DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    fi
    if [ "$BUILD_KIT" = "test" ]; then
        CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=tools/toolchains/gcc/toolchain_linux.cmake"
    fi

    if [ "$RECONFIGURE" = true ]; then
        echo "Reconfiguring: Deleting CMake cache..."
        rm -rf CMakeCache.txt CMakeFiles
    fi

    echo "Configuring with CMake..."
    if command -v ninja &> /dev/null; then
        cmake -B $BUILD_DIR -G Ninja $CMAKE_ARGS
    else
        cmake -B $BUILD_DIR $CMAKE_ARGS
    fi

    echo "BUILD"

    # Build
    echo "Building target '$TARGET'..."
    cmake --build $BUILD_DIR --target "$TARGET"

    echo "CD"

    cd ../../..
    echo "Build completed successfully."
fi

if [ "$SELFTESTS" = true ]; then
    echo "Running self tests..."

    if command -v poetry &> /dev/null; then
        poetry run pytest
    elif [ -f ".venv/bin/activate" ]; then
        source .venv/bin/activate
        pytest
    else
        pytest
    fi

    echo "Self tests completed."
fi

if [ -n "$COMMAND" ]; then
    echo "Executing command: $COMMAND"
    eval "$COMMAND"
fi

echo "Script completed successfully."
