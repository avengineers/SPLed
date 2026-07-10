# SPLed Docker Setup

This document describes how to run the SPLed project in Docker containers.

## Quick Start

### Using Docker Compose (Recommended)

1. **Build and start the development container:**
   ```bash
   docker-compose up -d spled
   ```

2. **Enter the container for interactive development:**
   ```bash
   docker-compose exec spled bash
   ```

3. **Inside the container, build a variant:**
   ```bash
   ./build.sh --build --variant Disco
   ```

4. **Run tests:**
   ```bash
   ./build.sh --build --build-kit test --variant Disco
   ```

### Using Docker directly

1. **Build the Docker image:**
   ```bash
   docker build -t spled .
   ```

2. **Run the container interactively:**
   ```bash
   docker run -it --rm -v ${PWD}:/workspace spled
   ```

3. **Build inside the container:**
   ```bash
   ./build.sh --build --variant Disco
   ```

## Available Build Variants

The project supports multiple variants:
- `Disco` - Default variant
- `Sleep` - Sleep mode variant
- `Spa` - Spa mode variant
- `Base/Dev` - Base development variant

## Toolchains

The Docker container includes multiple toolchains:
- **Clang/LLVM** - Default for production builds (native Linux)
- **GCC** - Used for test builds (native Linux)
- **MinGW-w64** - Cross-compilation to Windows (available but not configured by default)

**Note**: The Docker container uses Linux-native toolchains by default. Windows-specific MinGW toolchain from Scoop is replaced with cross-compilation capability.

## Build Commands

The `build.sh` script supports these options:

- `--install` - Install dependencies
- `--build` - Build the project
- `--clean` - Clean build artifacts
- `--variant <name>` - Specify variant to build (default: Disco)
- `--build-kit <kit>` - Specify build kit: "prod" or "test" (default: prod)
- `--build-type <type>` - Specify build type (Debug, Release, etc.)
- `--target <target>` - Specify build target (default: all)

## Development Workflow

1. **Start the development container:**
   ```bash
   docker-compose up -d spled
   ```

2. **Attach to the container:**
   ```bash
   docker-compose exec spled bash
   ```

3. **Make changes to your code** (files are mounted from host)

4. **Build and test:**
   ```bash
   ./build.sh --clean --build --variant Disco
   ./build.sh --build --build-kit test --variant Disco
   ```

## Persistent Storage

Build artifacts are stored in a Docker volume named `spled-build` to persist between container runs.

## VS Code Integration

To use VS Code with the Docker container:

1. Install the "Dev Containers" extension
2. Open the project folder in VS Code
3. Use "Dev Containers: Reopen in Container" command

Alternatively, you can attach VS Code to the running container:
1. Start the container: `docker-compose up -d spled`
2. In VS Code, use "Dev Containers: Attach to Running Container"
3. Select the `spled-dev` container

## Troubleshooting

### Permission Issues
If you encounter permission issues with mounted volumes:
```bash
# Fix ownership (run from host)
sudo chown -R $USER:$USER build/
```

### Container Won't Start
Check Docker logs:
```bash
docker-compose logs spled
```

### Clean Rebuild
To completely rebuild the container:
```bash
docker-compose down
docker-compose build --no-cache
docker-compose up -d
```
