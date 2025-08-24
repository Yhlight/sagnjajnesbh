#!/bin/bash
# 自动生成的CJMOD编译脚本
# 模块: Chtholly

echo "编译 Chtholly CJMOD扩展..."

CPP_FILES="CJMOD/ChthollyJS/src/ChthollyJS.cpp "

g++ -shared -fPIC -std=c++17 \
    -I../../include \
    -I../../include/CJMOD \
    $CPP_FILES \
    -o lib/Chtholly_cjmod.so

if [ $? -eq 0 ]; then
    echo "CJMOD扩展编译成功: Chtholly_cjmod.so"
else
    echo "CJMOD扩展编译失败"
    exit 1
fi
