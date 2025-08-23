#!/bin/bash
# CHTL Compiler Release Build Script for macOS
# This script builds the CHTL compiler in Release mode

set -e

echo "Building CHTL Compiler (Release Mode) on macOS..."

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed"
    echo "Please install CMake: brew install cmake"
    exit 1
fi

# Check if we have Xcode command line tools
if ! command -v clang++ &> /dev/null; then
    echo "Error: Xcode command line tools not found"
    echo "Please install: xcode-select --install"
    exit 1
fi

# Check for Homebrew and required packages
if command -v brew &> /dev/null; then
    if ! brew list antlr4-cpp-runtime &> /dev/null; then
        echo "Warning: ANTLR4 C++ runtime not found via Homebrew"
        echo "Consider installing: brew install antlr4-cpp-runtime"
    fi
else
    echo "Warning: Homebrew not found. Make sure ANTLR4 dependencies are available"
fi

# Create build directory
mkdir -p build/release
cd build/release

# Configure with CMake (Release mode)
echo "Configuring build system..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_STANDARD=17 \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=10.15 \
      -DENABLE_TESTING=OFF \
      -DENABLE_EXAMPLES=ON \
      ../..

# Build the project
echo "Building CHTL Compiler..."
make -j$(sysctl -n hw.ncpu)

# Create distribution directory
mkdir -p ../../dist/macos

# Copy executables and dynamic libraries
echo "Copying release binaries..."
find . -name "chtl*" -type f -perm +111 -exec cp {} ../../dist/macos/ \;
find . -name "*.dylib" -type f -exec cp {} ../../dist/macos/ \;

cd ../..

echo ""
echo "Build completed successfully!"
echo "Release binaries are available in: dist/macos/"
echo ""

# Make binaries executable
chmod +x dist/macos/*

# Code signing (optional, for distribution)
if command -v codesign &> /dev/null; then
    echo "Signing binaries for distribution..."
    for binary in dist/macos/chtl*; do
        if [[ -x "$binary" ]]; then
            codesign --force --sign - "$binary" 2>/dev/null || echo "Warning: Could not sign $binary"
        fi
    done
fi

echo "Binary permissions set correctly."