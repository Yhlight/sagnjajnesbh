#!/bin/bash

# CHTL Compiler Release Build Script
# Cross-platform shell script for building CHTL compiler in release mode

set -e

# Color output for better visibility
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

print_status "Starting CHTL Compiler Release Build"
print_status "Project root: $PROJECT_ROOT"

cd "$PROJECT_ROOT"

# Check for required tools
print_status "Checking build dependencies..."

if ! command -v cmake &> /dev/null; then
    print_error "CMake is not installed or not in PATH"
    exit 1
fi

if ! command -v make &> /dev/null && ! command -v ninja &> /dev/null; then
    print_error "Neither Make nor Ninja build system found"
    exit 1
fi

print_success "Build dependencies verified"

# Detect build system
BUILD_SYSTEM="make"
if command -v ninja &> /dev/null; then
    BUILD_SYSTEM="ninja"
    CMAKE_GENERATOR="-G Ninja"
else
    CMAKE_GENERATOR=""
fi

print_status "Using build system: $BUILD_SYSTEM"

# Create build directory
BUILD_DIR="$PROJECT_ROOT/build-release"
print_status "Creating build directory: $BUILD_DIR"

if [ -d "$BUILD_DIR" ]; then
    print_warning "Release build directory exists, cleaning..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake for release build
print_status "Configuring CMake for release build..."

cmake $CMAKE_GENERATOR \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_TESTING=OFF \
    -DENABLE_DEBUG_SYMBOLS=OFF \
    -DENABLE_OPTIMIZATIONS=ON \
    -DENABLE_LTO=ON \
    -DSTRIP_SYMBOLS=ON \
    "$PROJECT_ROOT"

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed"
    exit 1
fi

print_success "CMake configuration completed"

# Build the project
print_status "Building CHTL compiler in release mode..."

if [ "$BUILD_SYSTEM" = "ninja" ]; then
    ninja -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
else
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

if [ $? -ne 0 ]; then
    print_error "Build failed"
    exit 1
fi

print_success "CHTL compiler release build completed successfully"

# Check if executables were created
print_status "Verifying build output..."

EXECUTABLES=("chtl_compiler")
for exe in "${EXECUTABLES[@]}"; do
    if [ -f "$BUILD_DIR/$exe" ] || [ -f "$BUILD_DIR/src/$exe" ]; then
        print_success "Found executable: $exe"
        
        # Get file size for release verification
        if [ -f "$BUILD_DIR/$exe" ]; then
            SIZE=$(ls -lh "$BUILD_DIR/$exe" | awk '{print $5}')
            print_status "Executable size: $SIZE"
        elif [ -f "$BUILD_DIR/src/$exe" ]; then
            SIZE=$(ls -lh "$BUILD_DIR/src/$exe" | awk '{print $5}')
            print_status "Executable size: $SIZE"
        fi
    else
        print_warning "Executable not found: $exe"
    fi
done

# Strip symbols if available (additional optimization)
print_status "Optimizing release binaries..."

for exe in "${EXECUTABLES[@]}"; do
    if [ -f "$BUILD_DIR/$exe" ]; then
        if command -v strip &> /dev/null; then
            strip "$BUILD_DIR/$exe" 2>/dev/null || true
            print_success "Stripped symbols from $exe"
        fi
    elif [ -f "$BUILD_DIR/src/$exe" ]; then
        if command -v strip &> /dev/null; then
            strip "$BUILD_DIR/src/$exe" 2>/dev/null || true
            print_success "Stripped symbols from src/$exe"
        fi
    fi
done

# Create distribution directory
DIST_DIR="$PROJECT_ROOT/dist"
print_status "Creating distribution directory: $DIST_DIR"

if [ -d "$DIST_DIR" ]; then
    rm -rf "$DIST_DIR"
fi

mkdir -p "$DIST_DIR/bin"
mkdir -p "$DIST_DIR/module"
mkdir -p "$DIST_DIR/docs"

# Copy binaries to distribution
for exe in "${EXECUTABLES[@]}"; do
    if [ -f "$BUILD_DIR/$exe" ]; then
        cp "$BUILD_DIR/$exe" "$DIST_DIR/bin/"
        print_success "Copied $exe to distribution"
    elif [ -f "$BUILD_DIR/src/$exe" ]; then
        cp "$BUILD_DIR/src/$exe" "$DIST_DIR/bin/"
        print_success "Copied src/$exe to distribution"
    fi
done

# Copy modules if they exist
if [ -d "$PROJECT_ROOT/module" ]; then
    cp -r "$PROJECT_ROOT/module/"* "$DIST_DIR/module/" 2>/dev/null || true
    print_success "Copied modules to distribution"
fi

# Copy documentation
if [ -d "$PROJECT_ROOT/docs" ]; then
    cp -r "$PROJECT_ROOT/docs/"* "$DIST_DIR/docs/" 2>/dev/null || true
    print_success "Copied documentation to distribution"
fi

# Copy license and readme
if [ -f "$PROJECT_ROOT/LICENSE" ]; then
    cp "$PROJECT_ROOT/LICENSE" "$DIST_DIR/"
fi

if [ -f "$PROJECT_ROOT/README.md" ]; then
    cp "$PROJECT_ROOT/README.md" "$DIST_DIR/"
fi

print_success "Release build process completed"
print_status "Build artifacts are in: $BUILD_DIR"
print_status "Distribution package is in: $DIST_DIR"

# Create archive if tar is available
if command -v tar &> /dev/null; then
    ARCHIVE_NAME="chtl-compiler-$(date +%Y%m%d-%H%M%S).tar.gz"
    cd "$PROJECT_ROOT"
    tar -czf "$ARCHIVE_NAME" -C dist .
    print_success "Created distribution archive: $ARCHIVE_NAME"
fi

exit 0