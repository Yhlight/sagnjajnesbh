#!/bin/bash

# CHTL Compiler V3 Build Script
# 完整实现所有CHTL语法特征

echo "Building CHTL Compiler V3..."

# 创建构建目录
mkdir -p build/v3

# 编译器源文件
SOURCES="
    src/chtl/lexer/CHTLLexerV2.cpp
    src/chtl/parser/CHTLParserV3.cpp
    src/chtl/ast/CHTLASTNodesV3.cpp
    src/chtl/generator/CHTLGeneratorV3.cpp
    src/chtl/CHTLCompilerV3.cpp
    src/chtl/main_v3.cpp
"

# 编译选项
CXX="g++"
CXXFLAGS="-std=c++17 -Wall -Wextra -O2 -I./src"
OUTPUT="build/v3/chtl"

# 执行编译
$CXX $CXXFLAGS $SOURCES -o $OUTPUT

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Binary location: $OUTPUT"
    
    # 运行测试
    echo ""
    echo "Running tests..."
    
    # 测试所有功能
    echo "Testing complete CHTL features..."
    if [ -f "tests/test_all_features.chtl" ]; then
        $OUTPUT tests/test_all_features.chtl -o build/v3/test_all_features.html
        if [ $? -eq 0 ]; then
            echo "   ✓ All features test passed"
        else
            echo "   ✗ All features test failed"
        fi
    fi
    
    echo ""
    echo "CHTL Compiler V3 ready!"
    echo ""
    echo "Usage:"
    echo "  $OUTPUT input.chtl                    # Generate input.html"
    echo "  $OUTPUT input.chtl -o output.html     # Specify output file"
    echo "  $OUTPUT -d input.chtl                 # Debug mode"
    echo "  $OUTPUT -m input.chtl                 # Minified output"
    echo "  $OUTPUT -f input.chtl                 # Fragment only"
    echo "  $OUTPUT --ast input.chtl              # Print AST"
    echo "  $OUTPUT --version                     # Show version"
else
    echo "Build failed!"
    exit 1
fi