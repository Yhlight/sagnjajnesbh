#!/bin/bash

# CHTL Compiler Debug Build Script
# Cross-platform shell script for building CHTL compiler in debug mode

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

print_status "Starting CHTL Compiler Debug Build"
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
BUILD_DIR="$PROJECT_ROOT/build-debug"
print_status "Creating build directory: $BUILD_DIR"

if [ -d "$BUILD_DIR" ]; then
    print_warning "Debug build directory exists, cleaning..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure CMake for debug build
print_status "Configuring CMake for debug build..."

cmake $CMAKE_GENERATOR \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DBUILD_TESTING=ON \
    -DENABLE_DEBUG_SYMBOLS=ON \
    -DENABLE_SANITIZERS=OFF \
    "$PROJECT_ROOT"

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed"
    exit 1
fi

print_success "CMake configuration completed"

# Build the project
print_status "Building CHTL compiler in debug mode..."

if [ "$BUILD_SYSTEM" = "ninja" ]; then
    ninja -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
else
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
fi

if [ $? -ne 0 ]; then
    print_error "Build failed"
    exit 1
fi

print_success "CHTL compiler debug build completed successfully"

# Check if executables were created
print_status "Verifying build output..."

EXECUTABLES=("chtl_compiler" "chtl_test")
for exe in "${EXECUTABLES[@]}"; do
    if [ -f "$BUILD_DIR/$exe" ] || [ -f "$BUILD_DIR/src/$exe" ]; then
        print_success "Found executable: $exe"
    else
        print_warning "Executable not found: $exe"
    fi
done

# Copy compile_commands.json to project root for IDE support
if [ -f "$BUILD_DIR/compile_commands.json" ]; then
    cp "$BUILD_DIR/compile_commands.json" "$PROJECT_ROOT/"
    print_success "Copied compile_commands.json for IDE support"
fi

print_success "Debug build process completed"
print_status "Build artifacts are in: $BUILD_DIR"
print_status "To run tests: cd $BUILD_DIR && ctest"

exit 0