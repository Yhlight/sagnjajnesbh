#!/bin/bash

# Build ANTLR-based CSS and JavaScript static libraries

set -e

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "$SCRIPT_DIR"

echo "Building ANTLR-based parsers..."

# Create build directory
mkdir -p build/antlr

# Compile CSS parser library
echo "Building CSS parser..."
cd build/antlr
c++ -std=c++17 -I../../third_party/antlr4-runtime/runtime/src \
    -I../../third_party/generated \
    -c ../../third_party/generated/css3/*.cpp \
    ../../src/chtl/css/CSSCompiler.cpp

# Create CSS static library
ar rcs libchtl_css.a *.o
rm *.o

# Compile JavaScript parser library
echo "Building JavaScript parser..."
c++ -std=c++17 -I../../third_party/antlr4-runtime/runtime/src \
    -I../../third_party/generated \
    -c ../../third_party/generated/javascript/*.cpp \
    ../../src/chtl/javascript/JavaScriptCompiler.cpp

# Create JavaScript static library
ar rcs libchtl_javascript.a *.o
rm *.o

# Copy ANTLR runtime library
cp ../../third_party/antlr4-runtime/build/runtime/libantlr4-runtime.a .

echo "Static libraries created:"
echo "  - build/antlr/libchtl_css.a"
echo "  - build/antlr/libchtl_javascript.a"
echo "  - build/antlr/libantlr4-runtime.a"

# Create combined static library
echo "Creating combined library..."
mkdir -p tmp
cd tmp
ar x ../libchtl_css.a
ar x ../libchtl_javascript.a
ar x ../libantlr4-runtime.a
ar rcs ../libchtl_antlr_all.a *.o
cd ..
rm -rf tmp

echo "Combined library created: build/antlr/libchtl_antlr_all.a"