#!/bin/bash

echo "Building CHTL Compiler v2.0..."

# 创建构建目录
mkdir -p build/v2

# 编译新版本
g++ -std=c++17 -O2 -Wall \
    -I src/simple \
    src/simple/Lexer.cpp \
    src/simple/Parser.cpp \
    src/simple/Generator.cpp \
    src/simple/CHTLCompiler.cpp \
    src/simple/main.cpp \
    -o build/v2/chtl

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo ""
    echo "Running tests..."
    
    # 测试基本功能
    echo "1. Testing basic.chtl..."
    ./build/v2/chtl examples/simple/basic.chtl build/v2/basic.html
    
    # 测试带样式的例子
    echo "2. Testing styled.chtl..."
    ./build/v2/chtl examples/simple/styled.chtl build/v2/styled.html
    
    # 测试片段模式
    echo "3. Testing fragment mode..."
    ./build/v2/chtl --fragment examples/simple/basic.chtl build/v2/fragment.html
    
    # 测试压缩模式
    echo "4. Testing minified mode..."
    ./build/v2/chtl --minify examples/simple/basic.chtl build/v2/minified.html
    
    echo ""
    echo "Done! Check the output files in build/v2/"
    echo ""
    echo "Try the new features:"
    echo "  ./build/v2/chtl --help"
else
    echo "Build failed!"
    exit 1
fi