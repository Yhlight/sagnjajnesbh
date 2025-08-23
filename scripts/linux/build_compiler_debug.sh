#!/bin/bash

set -e

echo "==============================================="
echo "CHTL Compiler Debug Build Script for Linux"
echo "==============================================="

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${SOURCE_DIR}/build/debug"

# Create build directory
mkdir -p "${BUILD_DIR}"

# Change to build directory
cd "${BUILD_DIR}"

# Configure with CMake
echo "Configuring CMake for Debug build..."
cmake -DCMAKE_BUILD_TYPE=Debug "${SOURCE_DIR}"

# Build the project
echo "Building CHTL Compiler (Debug)..."
make -j$(nproc)

# Create bin directory and copy executable
BIN_DIR="${SOURCE_DIR}/bin/debug"
mkdir -p "${BIN_DIR}"

cp "chtl-compiler" "${BIN_DIR}/"
chmod +x "${BIN_DIR}/chtl-compiler"

echo ""
echo "==============================================="
echo "Debug build completed successfully!"
echo "Executable: ${BIN_DIR}/chtl-compiler"
echo "==============================================="