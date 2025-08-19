#!/bin/bash

# CHTL Compiler Release Build Script
# Standard CMake-based build for Unix-like systems

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
    print_status "Please install CMake from: https://cmake.org/download/"
    exit 1
fi

print_success "CMake found: $(cmake --version | head -1)"

# Check for C++ compiler
COMPILER_FOUND=false
COMPILER_TYPE=""

if command -v g++ &> /dev/null; then
    print_success "Found G++: $(g++ --version | head -1)"
    COMPILER_FOUND=true
    COMPILER_TYPE="GCC"
fi

if command -v clang++ &> /dev/null; then
    print_success "Found Clang++: $(clang++ --version | head -1)"
    COMPILER_FOUND=true
    COMPILER_TYPE="Clang"
fi

if [ "$COMPILER_FOUND" = false ]; then
    print_error "No C++ compiler found"
    print_status "Please install one of the following:"
    print_status "1. GCC (g++)"
    print_status "2. Clang (clang++)"
    print_status "3. On Ubuntu: sudo apt install build-essential"
    print_status "4. On macOS: xcode-select --install"
    exit 1
fi

print_success "Using $COMPILER_TYPE compiler"

# Create build directory
BUILD_DIR="$PROJECT_ROOT/build-release"
print_status "Creating build directory: $BUILD_DIR"

if [ -d "$BUILD_DIR" ]; then
    print_status "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake (let CMake detect the best generator)
print_status "Configuring with CMake..."

cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
      "$PROJECT_ROOT"

if [ $? -ne 0 ]; then
    print_error "CMake configuration failed"
    print_status "This might be due to missing dependencies or compiler issues"
    print_status "Check the error messages above for details"
    exit 1
fi

print_success "CMake configuration completed"

# Build the project using CMake (universal approach)
print_status "Building CHTL compiler in release mode..."

cmake --build . --parallel $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

if [ $? -ne 0 ]; then
    print_error "Build failed"
    print_status "Check the error messages above for details"
    exit 1
fi

print_success "CHTL compiler release build completed successfully"

# Find and verify executables
print_status "Verifying build output..."

EXE_FOUND=false
for exe in "chtl_compiler" "chtl"; do
    if find . -name "$exe" -type f | head -1 | grep -q .; then
        EXE_PATH=$(find . -name "$exe" -type f | head -1)
        SIZE=$(ls -lh "$EXE_PATH" | awk '{print $5}')
        print_success "Found executable: $EXE_PATH (Size: $SIZE)"
        EXE_FOUND=true
    fi
done

if [ "$EXE_FOUND" = false ]; then
    print_warning "Executables not found in expected locations"
    print_status "Searching for any CHTL executables..."
    find . -name "chtl*" -type f -executable | while read exe; do
        SIZE=$(ls -lh "$exe" | awk '{print $5}')
        print_status "Found: $exe (Size: $SIZE)"
        EXE_FOUND=true
    done
fi

# Create distribution directory
DIST_DIR="$PROJECT_ROOT/dist"
print_status "Creating distribution directory: $DIST_DIR"

if [ -d "$DIST_DIR" ]; then
    rm -rf "$DIST_DIR"
fi

mkdir -p "$DIST_DIR/bin"
mkdir -p "$DIST_DIR/lib"
mkdir -p "$DIST_DIR/docs"
mkdir -p "$DIST_DIR/module"

# Copy executables to distribution
find . -name "chtl_compiler" -o -name "chtl" -type f | while read exe; do
    cp "$exe" "$DIST_DIR/bin/"
    print_success "Copied $(basename "$exe") to distribution"
done

# Copy libraries to distribution
find . -name "*.a" -o -name "*.so" -type f | while read lib; do
    cp "$lib" "$DIST_DIR/lib/" 2>/dev/null || true
done

# Copy documentation
if [ -d "$PROJECT_ROOT/docs" ]; then
    cp -r "$PROJECT_ROOT/docs/"* "$DIST_DIR/docs/" 2>/dev/null || true
    print_success "Copied documentation to distribution"
fi

# Copy modules
if [ -d "$PROJECT_ROOT/module" ]; then
    cp -r "$PROJECT_ROOT/module/"* "$DIST_DIR/module/" 2>/dev/null || true
    print_success "Copied modules to distribution"
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
print_status "For VSCode: Open the project folder and use CMake extension"

# Create archive if tar is available
if command -v tar &> /dev/null; then
    ARCHIVE_NAME="chtl-compiler-$(date +%Y%m%d-%H%M%S).tar.gz"
    cd "$PROJECT_ROOT"
    tar -czf "$ARCHIVE_NAME" -C dist .
    print_success "Created distribution archive: $ARCHIVE_NAME"
fi

exit 0