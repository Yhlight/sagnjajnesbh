#!/bin/bash

echo "Building Simple CHTL Compiler..."

# 创建构建目录
mkdir -p build/simple

# 编译
g++ -std=c++17 -O2 \
    -I include \
    -I src/simple \
    src/simple/SimpleCHTLCompiler.cpp \
    src/simple/simple_main.cpp \
    -o build/simple/chtl-simple

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    echo "Testing with examples..."
    
    # 测试 hello.chtl
    echo "1. Compiling hello.chtl..."
    ./build/simple/chtl-simple examples/simple/hello.chtl build/simple/hello.html
    
    # 测试 nested.chtl
    echo "2. Compiling nested.chtl..."
    ./build/simple/chtl-simple examples/simple/nested.chtl build/simple/nested.html
    
    echo ""
    echo "Done! Check the output files:"
    echo "  - build/simple/hello.html"
    echo "  - build/simple/nested.html"
else
    echo "Build failed!"
    exit 1
fi