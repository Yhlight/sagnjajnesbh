#!/bin/bash

# ANTLR4构建脚本
# 用于下载和构建ANTLR4运行时库

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
THIRD_PARTY_DIR="$PROJECT_ROOT/third_party"
ANTLR_DIR="$THIRD_PARTY_DIR/antlr4"

# ANTLR版本
ANTLR_VERSION="4.13.1"
ANTLR_JAR="antlr-${ANTLR_VERSION}-complete.jar"
ANTLR_JAR_URL="https://www.antlr.org/download/${ANTLR_JAR}"

# C++运行时
ANTLR_CPP_RUNTIME_URL="https://github.com/antlr/antlr4/archive/refs/tags/${ANTLR_VERSION}.tar.gz"

echo "=== ANTLR4构建脚本 ==="
echo "版本: ${ANTLR_VERSION}"
echo "目标目录: ${ANTLR_DIR}"

# 创建目录
mkdir -p "${ANTLR_DIR}"/{bin,include,lib,runtime}

# 下载ANTLR工具JAR
if [ ! -f "${ANTLR_DIR}/bin/${ANTLR_JAR}" ]; then
    echo "下载ANTLR工具..."
    curl -L "${ANTLR_JAR_URL}" -o "${ANTLR_DIR}/bin/${ANTLR_JAR}"
else
    echo "ANTLR工具已存在"
fi

# 创建antlr4命令脚本
cat > "${ANTLR_DIR}/bin/antlr4" << 'EOF'
#!/bin/bash
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
java -Xmx500M -cp "${DIR}/antlr-4.13.1-complete.jar" org.antlr.v4.Tool "$@"
EOF
chmod +x "${ANTLR_DIR}/bin/antlr4"

# 下载C++运行时源码
if [ ! -d "${ANTLR_DIR}/runtime/cpp" ]; then
    echo "下载ANTLR C++运行时..."
    cd "${ANTLR_DIR}/runtime"
    curl -L "${ANTLR_CPP_RUNTIME_URL}" -o "antlr4-${ANTLR_VERSION}.tar.gz"
    tar -xzf "antlr4-${ANTLR_VERSION}.tar.gz"
    mv "antlr4-${ANTLR_VERSION}/runtime/Cpp" cpp
    rm -rf "antlr4-${ANTLR_VERSION}" "antlr4-${ANTLR_VERSION}.tar.gz"
else
    echo "ANTLR C++运行时已存在"
fi

# 构建C++运行时
echo "构建ANTLR C++运行时..."
cd "${ANTLR_DIR}/runtime/cpp"

# 创建构建目录
mkdir -p build-linux build-windows

# Linux构建
echo "构建Linux版本..."
cd build-linux
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DCMAKE_INSTALL_PREFIX="${ANTLR_DIR}" \
         -DWITH_DEMO=OFF \
         -DWITH_STATIC_CRT=OFF
make -j$(nproc)
make install

# Windows交叉编译（如果有MinGW）
if command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    echo "构建Windows版本..."
    cd ../build-windows
    cmake .. -DCMAKE_BUILD_TYPE=Release \
             -DCMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/mingw-toolchain.cmake" \
             -DCMAKE_INSTALL_PREFIX="${ANTLR_DIR}/windows" \
             -DWITH_DEMO=OFF \
             -DWITH_STATIC_CRT=OFF
    make -j$(nproc)
    make install
else
    echo "跳过Windows构建（未找到MinGW）"
fi

echo "=== ANTLR4构建完成 ==="
echo "ANTLR工具: ${ANTLR_DIR}/bin/antlr4"
echo "C++头文件: ${ANTLR_DIR}/include/antlr4-runtime"
echo "C++库文件: ${ANTLR_DIR}/lib"