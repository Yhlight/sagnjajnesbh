#!/bin/bash

# CHTL V2 编译器构建脚本

echo "Building CHTL Compiler V2..."

# 创建构建目录
mkdir -p build/v2

# 编译命令
g++ -std=c++17 -O2 \
    src/chtl/main.cpp \
    src/chtl/CHTLCompilerV2.cpp \
    src/chtl/lexer/CHTLLexerV2.cpp \
    src/chtl/parser/CHTLParserV2.cpp \
    src/chtl/ast/CHTLASTNodes.cpp \
    src/chtl/generator/CHTLGeneratorV2.cpp \
    -o build/v2/chtl

if [ $? -eq 0 ]; then
    echo "Build successful!"
    echo "Binary location: build/v2/chtl"
    echo ""
    echo "Running tests..."
    
    # 测试基本功能
    echo "1. Testing basic CHTL..."
    cat > build/v2/test_basic.chtl << 'EOF'
div {
    id: main;
    class: container;
    
    h1 { text { "Hello CHTL!" } }
    
    p {
        style {
            color: blue;
            font-size: 16px;
        }
        text { "This is a test paragraph." }
    }
}
EOF
    
    ./build/v2/chtl build/v2/test_basic.chtl build/v2/test_basic.html
    echo "   Generated: build/v2/test_basic.html"
    
    # 测试局部样式
    echo "2. Testing local styles..."
    cat > build/v2/test_style.chtl << 'EOF'
div {
    style {
        .mybox {
            background: #f0f0f0;
            padding: 20px;
        }
        
        &:hover {
            background: #e0e0e0;
        }
    }
    
    text { "Hover over me!" }
}
EOF
    
    ./build/v2/chtl build/v2/test_style.chtl build/v2/test_style.html
    echo "   Generated: build/v2/test_style.html"
    
    # 测试完整特性
    if [ -f "tests/examples/full_features.chtl" ]; then
        echo "3. Testing full features..."
        ./build/v2/chtl tests/examples/full_features.chtl build/v2/full_features.html
        echo "   Generated: build/v2/full_features.html"
    fi
    
    # 测试AST打印
    echo "4. Testing AST printer..."
    ./build/v2/chtl --ast build/v2/test_basic.chtl > build/v2/test_ast.txt
    echo "   AST saved to: build/v2/test_ast.txt"
    
    echo ""
    echo "All tests completed!"
    echo ""
    echo "Usage examples:"
    echo "  ./build/v2/chtl input.chtl                    # Generate input.html"
    echo "  ./build/v2/chtl -d input.chtl output.html    # Debug mode"
    echo "  ./build/v2/chtl -m input.chtl                # Minified output"
    echo "  ./build/v2/chtl -f input.chtl                # Fragment only"
    echo "  ./build/v2/chtl --ast input.chtl             # Print AST"
else
    echo "Build failed!"
    exit 1
fi