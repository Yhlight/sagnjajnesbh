#!/bin/bash

set -e

echo "==============================================="
echo "CHTL Compiler Release Build Script for macOS"
echo "==============================================="

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${SOURCE_DIR}/build/release"

# Create build directory
mkdir -p "${BUILD_DIR}"

# Change to build directory
cd "${BUILD_DIR}"

# Configure with CMake
echo "Configuring CMake for Release build..."
cmake -DCMAKE_BUILD_TYPE=Release "${SOURCE_DIR}"

# Build the project
echo "Building CHTL Compiler (Release)..."
make -j$(sysctl -n hw.ncpu)

# Create bin directory and copy executable
BIN_DIR="${SOURCE_DIR}/bin"
mkdir -p "${BIN_DIR}"

cp "chtl-compiler" "${BIN_DIR}/"
chmod +x "${BIN_DIR}/chtl-compiler"

echo ""
echo "==============================================="
echo "Build completed successfully!"
echo "Executable: ${BIN_DIR}/chtl-compiler"
echo "==============================================="