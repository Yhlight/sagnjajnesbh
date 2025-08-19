#!/bin/bash

# ========================================
# CJMOD编译脚本 - 实际编译CJMOD的完整流程
# ========================================

echo "🔨 CJMOD编译工具"
echo "================"

if [ $# -lt 1 ]; then
    echo "用法: $0 <CJMOD项目路径>"
    echo "示例: $0 ./MyExtension"
    exit 1
fi

PROJECT_PATH="$1"
PROJECT_NAME=$(basename "$PROJECT_PATH")

echo "📁 项目路径: $PROJECT_PATH"
echo "📦 项目名称: $PROJECT_NAME"

# 检查项目结构
if [ ! -d "$PROJECT_PATH/src" ]; then
    echo "❌ 错误: 缺少src目录"
    exit 1
fi

if [ ! -d "$PROJECT_PATH/info" ]; then
    echo "❌ 错误: 缺少info目录"
    exit 1
fi

if [ ! -f "$PROJECT_PATH/info/$PROJECT_NAME.chtl" ]; then
    echo "❌ 错误: 缺少info文件 (二同名规则)"
    exit 1
fi

echo "✅ 项目结构验证通过"

# 创建构建目录
BUILD_DIR="$PROJECT_PATH/build"
mkdir -p "$BUILD_DIR"

echo "🔧 开始编译..."

# 进入构建目录
cd "$BUILD_DIR"

# 运行CMake
echo "⚙️ 配置CMake..."
if ! cmake ..; then
    echo "❌ CMake配置失败"
    exit 1
fi

# 编译项目
echo "🔨 编译C++代码..."
if ! make; then
    echo "❌ 编译失败"
    exit 1
fi

echo "✅ 编译成功！"

# 检查生成的.cjmod文件
CJMOD_FILE="$BUILD_DIR/$PROJECT_NAME.cjmod"
if [ -f "$CJMOD_FILE" ]; then
    echo "📦 CJMOD文件已生成: $CJMOD_FILE"
    
    # 复制到项目根目录
    cp "$CJMOD_FILE" "$PROJECT_PATH/"
    echo "📋 已复制到: $PROJECT_PATH/$PROJECT_NAME.cjmod"
    
    echo ""
    echo "🎉 CJMOD编译完成！"
    echo "================"
    echo "📦 生成文件: $PROJECT_NAME.cjmod"
    echo "📏 文件大小: $(du -h "$PROJECT_PATH/$PROJECT_NAME.cjmod" | cut -f1)"
    echo ""
    echo "📋 下一步使用："
    echo "1. 将 $PROJECT_NAME.cjmod 复制到你的CHTL项目的 module 目录"
    echo "2. 在CHTL文件中添加: [Import] @CJmod from $PROJECT_NAME"
    echo "3. 在script块中使用你创建的新语法"
    echo ""
    echo "💡 示例用法："
    echo "   script {"
    echo "       // 使用你的扩展语法"
    echo "       yourCustomSyntax('参数');"
    echo "   }"
    
else
    echo "❌ 未找到生成的CJMOD文件"
    exit 1
fi