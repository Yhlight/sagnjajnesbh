#!/bin/bash

# Build ANTLR parsers as WebAssembly modules
# This allows running on any platform including Windows

echo "Building ANTLR parsers as WebAssembly..."

# Install emscripten if not present
if ! command -v emcc &> /dev/null; then
    echo "Installing Emscripten..."
    git clone https://github.com/emscripten-core/emsdk.git
    cd emsdk
    ./emsdk install latest
    ./emsdk activate latest
    source ./emsdk_env.sh
    cd ..
fi

# Build ANTLR runtime for WASM
cd third_party/antlr4-runtime
mkdir -p build_wasm
cd build_wasm

emcmake cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=OFF \
    -DWITH_DEMO=OFF \
    -DBUILD_TESTING=OFF

emmake make -j$(nproc)

cd ../../..

# Compile parsers to WASM
mkdir -p build/wasm

emcc -std=c++17 -O3 \
    -I third_party/antlr4-runtime/runtime/src \
    -I third_party/generated \
    -I src \
    third_party/generated/css3/*.cpp \
    src/chtl/css/CSSCompiler.cpp \
    third_party/antlr4-runtime/build_wasm/runtime/libantlr4-runtime.a \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="CSSParser" \
    -s EXPORTED_FUNCTIONS="['_compile_css','_get_output','_malloc','_free']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap','stringToUTF8','UTF8ToString']" \
    -o build/wasm/css_parser.js

emcc -std=c++17 -O3 \
    -I third_party/antlr4-runtime/runtime/src \
    -I third_party/generated \
    -I src \
    third_party/generated/javascript/*.cpp \
    src/chtl/javascript/JavaScriptCompiler.cpp \
    third_party/antlr4-runtime/build_wasm/runtime/libantlr4-runtime.a \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="JSParser" \
    -s EXPORTED_FUNCTIONS="['_compile_js','_get_output','_malloc','_free']" \
    -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap','stringToUTF8','UTF8ToString']" \
    -o build/wasm/js_parser.js

echo "WebAssembly modules created!"