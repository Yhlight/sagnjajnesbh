#!/bin/bash

# CHTL编译器构建脚本

echo "CHTL Compiler Build Script"
echo "========================="

# 检查CMake
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed"
    exit 1
fi

# 创建构建目录
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# 配置项目
echo "Configuring project..."
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "Configuration failed!"
    exit 1
fi

# 编译
echo "Building..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo "Build successful!"
echo ""
echo "Running tests..."
make test

echo ""
echo "Executable location: build/chtlc"
echo "Run './build/chtlc --help' for usage information"