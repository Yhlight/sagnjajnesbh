#!/bin/bash
# CHTL Compiler Release Build Script for Linux
# This script builds the CHTL compiler in Release mode

set -e

echo "Building CHTL Compiler (Release Mode) on Linux..."

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed"
    echo "Please install CMake: sudo apt install cmake (Ubuntu/Debian) or sudo yum install cmake (RHEL/CentOS)"
    exit 1
fi

# Check if we have a C++ compiler
if ! command -v g++ &> /dev/null && ! command -v clang++ &> /dev/null; then
    echo "Error: No C++ compiler found"
    echo "Please install g++: sudo apt install build-essential (Ubuntu/Debian)"
    exit 1
fi

# Check for required packages
if ! ldconfig -p | grep -q "libantlr4-runtime"; then
    echo "Warning: ANTLR4 runtime not found in system libraries"
    echo "The build may fail if ANTLR4 dependencies are not available"
fi

# Create build directory
mkdir -p build/release
cd build/release

# Configure with CMake (Release mode)
echo "Configuring build system..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_STANDARD=17 \
      -DENABLE_TESTING=OFF \
      -DENABLE_EXAMPLES=ON \
      ../..

# Build the project
echo "Building CHTL Compiler..."
make -j$(nproc)

# Create distribution directory
mkdir -p ../../dist/linux

# Copy executables
echo "Copying release binaries..."
find . -name "chtl*" -type f -executable -exec cp {} ../../dist/linux/ \;
find . -name "*.so" -type f -exec cp {} ../../dist/linux/ \;

cd ../..

echo ""
echo "Build completed successfully!"
echo "Release binaries are available in: dist/linux/"
echo ""

# Make binaries executable
chmod +x dist/linux/*

echo "Binary permissions set correctly."