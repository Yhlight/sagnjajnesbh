#!/bin/bash
# ========================================
# 模块打包脚本 - 严格按照CHTL语法文档
# 自动化CMOD/CJMOD打包
# ========================================

set -e

echo "📦 CHTL模块自动打包脚本"
echo "======================"

# 编译打包工具
echo "🔧 编译打包工具..."
cd "$(dirname "$0")/.."
g++ -std=c++17 -I src tools/pack_cmod.cpp src/Common/SimpleZip/SimpleZip.cpp -o tools/pack_cmod

if [ ! -f "tools/pack_cmod" ]; then
    echo "❌ 打包工具编译失败"
    exit 1
fi

echo "✅ 打包工具编译成功"

# 创建输出目录
mkdir -p dist/modules

# 打包珂朵莉CMOD模块
echo ""
echo "📦 打包珂朵莉CMOD模块..."
if [ -d "src/Module/Chtholly/CMOD/Chtholly" ]; then
    ./tools/pack_cmod cmod src/Module/Chtholly/CMOD/Chtholly dist/modules/Chtholly.cmod
    echo "✅ 珂朵莉CMOD打包完成"
else
    echo "⚠️ 珂朵莉CMOD模块不存在"
fi

# 打包珂朵莉CJMOD模块
echo ""
echo "📦 打包珂朵莉CJMOD模块..."
if [ -d "src/Module/Chtholly/CJMOD/Chtholly" ]; then
    ./tools/pack_cmod cjmod src/Module/Chtholly/CJMOD/Chtholly dist/modules/Chtholly.cjmod
    echo "✅ 珂朵莉CJMOD打包完成"
else
    echo "⚠️ 珂朵莉CJMOD模块不存在"
fi

# 打包由比滨结衣CMOD模块
echo ""
echo "📦 打包由比滨结衣CMOD模块..."
if [ -d "src/Module/Yuigahama/CMOD/Yuigahama" ]; then
    ./tools/pack_cmod cmod src/Module/Yuigahama/CMOD/Yuigahama dist/modules/Yuigahama.cmod
    echo "✅ 由比滨结衣CMOD打包完成"
else
    echo "⚠️ 由比滨结衣CMOD模块不存在"
fi

echo ""
echo "🎉 所有模块打包完成！"
echo "===================="
echo "📁 输出目录: dist/modules/"

# 显示打包结果
if [ -d "dist/modules" ]; then
    echo "📦 已打包的模块:"
    ls -la dist/modules/
fi

echo ""
echo "✅ 模块打包脚本执行完成"