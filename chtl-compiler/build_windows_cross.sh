#!/bin/bash

# Cross-compile Windows libraries on Linux using MinGW

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "$SCRIPT_DIR"

echo "=== Cross-compiling Windows libraries on Linux ==="

# Check for MinGW
if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "MinGW not found. Installing..."
    sudo apt-get update
    sudo apt-get install -y mingw-w64
fi

# Check for Java
if ! command -v java &> /dev/null; then
    echo "Java not found. Please install Java first."
    exit 1
fi

# Setup environment
export CC=x86_64-w64-mingw32-gcc
export CXX=x86_64-w64-mingw32-g++
export AR=x86_64-w64-mingw32-ar
export RANLIB=x86_64-w64-mingw32-ranlib

# Download and setup
echo "Setting up build environment..."
python3 download_antlr_prebuilt.py

# Build ANTLR runtime with MinGW
echo "Building ANTLR runtime for Windows..."
cd third_party/antlr4-runtime
mkdir -p build_mingw && cd build_mingw

# Create MinGW toolchain file
cat > mingw-toolchain.cmake << EOF
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
EOF

cmake .. -DCMAKE_TOOLCHAIN_FILE=mingw-toolchain.cmake \
         -DCMAKE_BUILD_TYPE=Release \
         -DBUILD_SHARED_LIBS=OFF \
         -DWITH_DEMO=OFF \
         -DBUILD_TESTING=OFF

make -j$(nproc)

cd "$SCRIPT_DIR"

# Build CHTL libraries
echo "Building CHTL libraries..."
mkdir -p build/mingw
cd build/mingw

# Compile CSS library
echo "Compiling CSS parser..."
$CXX -std=c++17 -O2 -static -c \
    ../../third_party/generated/css3/*.cpp \
    ../../src/chtl/css/CSSCompiler.cpp \
    -I../../third_party/antlr4-runtime/runtime/src \
    -I../../third_party/generated \
    -I../../src

$AR rcs libchtl_css.a *.o
rm *.o

# Compile JavaScript library
echo "Compiling JavaScript parser..."
$CXX -std=c++17 -O2 -static -c \
    ../../third_party/generated/javascript/*.cpp \
    ../../src/chtl/javascript/JavaScriptCompiler.cpp \
    -I../../third_party/antlr4-runtime/runtime/src \
    -I../../third_party/generated \
    -I../../src

$AR rcs libchtl_javascript.a *.o
rm *.o

# Copy ANTLR runtime
cp ../../third_party/antlr4-runtime/build_mingw/runtime/libantlr4-runtime.a .

# Create combined library
echo "Creating combined library..."
# Extract all object files and recombine
mkdir -p tmp
cd tmp
$AR x ../libchtl_css.a
$AR x ../libchtl_javascript.a
$AR x ../libantlr4-runtime.a
$AR rcs ../libchtl_antlr_all.a *.o
cd ..
rm -rf tmp

# Copy to prebuilt directory
echo "Copying libraries to prebuilt directory..."
cd "$SCRIPT_DIR"
mkdir -p prebuilt/windows/x64/Release
cp build/mingw/*.a prebuilt/windows/x64/Release/

# Rename .a to .lib for Windows compatibility
cd prebuilt/windows/x64/Release
for file in *.a; do
    mv "$file" "${file%.a}.lib"
done

echo "=== Build complete! ==="
echo "Libraries created in: prebuilt/windows/x64/Release/"
ls -la prebuilt/windows/x64/Release/