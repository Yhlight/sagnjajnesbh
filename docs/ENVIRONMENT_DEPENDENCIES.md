# CHTL环境依赖说明

## 概述

本文档详细说明CHTL项目的环境依赖、版本要求和安装方法，确保开发者能够正确设置CHTL开发和运行环境。

---

## 系统要求

### 操作系统

#### Linux (推荐)
- **Ubuntu**: 18.04 LTS, 20.04 LTS, 22.04 LTS
- **CentOS**: 7, 8, Stream 9
- **Debian**: 10 (Buster), 11 (Bullseye), 12 (Bookworm)
- **Fedora**: 35, 36, 37
- **Arch Linux**: 最新版本

#### macOS
- **macOS**: 10.15 (Catalina), 11 (Big Sur), 12 (Monterey), 13 (Ventura)
- **架构**: Intel x64, Apple Silicon (M1/M2)

#### Windows
- **Windows**: 10 (1909+), 11
- **环境**: WSL2 (推荐), MSYS2, Visual Studio 2019+

---

## 核心依赖

### C++编译器

#### GCC (推荐)
```bash
# 最低版本: 9.0
# 推荐版本: 11.0+
# 安装 (Ubuntu)
sudo apt install g++-11

# 验证版本
g++ --version
```

**支持的GCC版本**:
- ✅ GCC 11.x (推荐)
- ✅ GCC 10.x
- ✅ GCC 9.x
- ❌ GCC 8.x (不支持C++17特性)

#### Clang
```bash
# 最低版本: 10.0
# 推荐版本: 13.0+
# 安装 (Ubuntu)
sudo apt install clang-13

# 验证版本
clang++ --version
```

**支持的Clang版本**:
- ✅ Clang 15.x (推荐)
- ✅ Clang 14.x
- ✅ Clang 13.x
- ✅ Clang 12.x
- ✅ Clang 11.x
- ✅ Clang 10.x

#### MSVC (Windows)
```powershell
# Visual Studio 2019 或更新版本
# 需要支持C++17的MSVC版本
```

**支持的MSVC版本**:
- ✅ MSVC 19.29+ (Visual Studio 2019 16.10+)
- ✅ MSVC 19.30+ (Visual Studio 2022)

### 构建工具

#### CMake
```bash
# 最低版本: 3.16
# 推荐版本: 3.20+
# 安装 (Ubuntu)
sudo apt install cmake

# 或从官方安装最新版本
wget https://github.com/Kitware/CMake/releases/download/v3.25.0/cmake-3.25.0-linux-x86_64.sh
chmod +x cmake-3.25.0-linux-x86_64.sh
sudo ./cmake-3.25.0-linux-x86_64.sh --prefix=/usr/local --skip-license

# 验证版本
cmake --version
```

**CMake版本兼容性**:
- ✅ CMake 3.25.x (推荐)
- ✅ CMake 3.24.x
- ✅ CMake 3.23.x
- ✅ CMake 3.22.x
- ✅ CMake 3.21.x
- ✅ CMake 3.20.x
- ✅ CMake 3.19.x
- ✅ CMake 3.18.x
- ✅ CMake 3.17.x
- ✅ CMake 3.16.x
- ❌ CMake 3.15.x及以下

#### Make
```bash
# GNU Make 4.0+
# 安装 (Ubuntu)
sudo apt install make

# 验证版本
make --version
```

---

## ANTLR 4依赖

### Java运行时

ANTLR 4需要Java运行时环境：

```bash
# 最低版本: Java 8
# 推荐版本: Java 11+
# 安装 (Ubuntu)
sudo apt install openjdk-11-jdk

# 设置JAVA_HOME
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk
echo 'export JAVA_HOME=/usr/lib/jvm/java-11-openjdk' >> ~/.bashrc

# 验证安装
java --version
javac --version
```

**Java版本兼容性**:
- ✅ OpenJDK 17.x (推荐)
- ✅ OpenJDK 11.x (推荐)
- ✅ OpenJDK 8.x
- ✅ Oracle JDK 11.x+
- ❌ Java 7及以下

### ANTLR工具

```bash
# ANTLR 4.13.2 (项目包含)
ls -la external/antlr-4.13.2-complete.jar

# 验证ANTLR
java -jar external/antlr-4.13.2-complete.jar
```

### ANTLR C++运行时

```bash
# 从源码编译 (自动处理)
# 或安装预编译版本 (Ubuntu)
sudo apt install libantlr4-runtime-dev

# 验证安装
pkg-config --modversion antlr4-runtime
```

---

## 可选依赖

### 开发工具

#### Git
```bash
# 版本控制 (必需)
# 最低版本: 2.20
sudo apt install git

git --version
```

#### pkg-config
```bash
# 包配置工具
sudo apt install pkg-config

pkg-config --version
```

#### 调试工具
```bash
# GDB调试器
sudo apt install gdb

# Valgrind内存检查
sudo apt install valgrind

# 性能分析工具
sudo apt install linux-perf
```

### VSCode插件开发

#### Node.js
```bash
# 最低版本: 16.0
# 推荐版本: 18.0+
# 安装 (Ubuntu)
curl -fsSL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt install nodejs

# 验证版本
node --version
npm --version
```

#### TypeScript
```bash
# 全局安装TypeScript
npm install -g typescript

# 验证版本
tsc --version
```

#### VSCode扩展工具
```bash
# 安装vsce打包工具
npm install -g @vscode/vsce

# 验证安装
vsce --version
```

---

## 平台特定依赖

### Linux特定

#### 系统库
```bash
# 标准C++库开发文件
sudo apt install libc++-dev libc++abi-dev

# 线程库
sudo apt install libpthread-stubs0-dev

# 文件系统库 (C++17 filesystem)
# GCC 9+已包含，无需额外安装
```

#### 包管理器
```bash
# Debian/Ubuntu
sudo apt install build-essential

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake3

# Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install cmake

# Arch Linux
sudo pacman -S base-devel cmake
```

### macOS特定

#### Xcode命令行工具
```bash
# 安装Xcode命令行工具 (必需)
xcode-select --install

# 验证安装
xcode-select -p
```

#### Homebrew依赖
```bash
# 安装Homebrew (如果未安装)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装构建依赖
brew install cmake git openjdk@11 pkg-config

# 设置环境变量
echo 'export PATH="/opt/homebrew/bin:$PATH"' >> ~/.zshrc
```

### Windows特定

#### Visual Studio
```powershell
# 安装Visual Studio 2019或2022
# 需要包含C++工作负载
# 或安装Build Tools for Visual Studio
```

#### WSL2 (推荐)
```powershell
# 启用WSL2
wsl --install

# 安装Ubuntu
wsl --install -d Ubuntu

# 在WSL中按照Linux指南安装依赖
```

#### MSYS2 (替代方案)
```bash
# 安装MSYS2: https://www.msys2.org/
# 在MSYS2终端中安装依赖
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-pkg-config
```

---

## 依赖验证

### 自动检查脚本

```bash
#!/bin/bash
# scripts/check_dependencies.sh

echo "🔍 CHTL依赖检查"
echo "==============="

# 检查C++编译器
check_compiler() {
    if command -v g++ &> /dev/null; then
        version=$(g++ --version | head -n1)
        echo "✅ GCC: $version"
    elif command -v clang++ &> /dev/null; then
        version=$(clang++ --version | head -n1)
        echo "✅ Clang: $version"
    else
        echo "❌ 未找到C++编译器"
        return 1
    fi
}

# 检查CMake
check_cmake() {
    if command -v cmake &> /dev/null; then
        version=$(cmake --version | head -n1)
        echo "✅ CMake: $version"
    else
        echo "❌ 未找到CMake"
        return 1
    fi
}

# 检查Java
check_java() {
    if command -v java &> /dev/null; then
        version=$(java --version 2>&1 | head -n1)
        echo "✅ Java: $version"
    else
        echo "❌ 未找到Java (ANTLR需要)"
        return 1
    fi
}

# 检查Git
check_git() {
    if command -v git &> /dev/null; then
        version=$(git --version)
        echo "✅ Git: $version"
    else
        echo "❌ 未找到Git"
        return 1
    fi
}

# 检查ANTLR
check_antlr() {
    if [ -f "external/antlr-4.13.2-complete.jar" ]; then
        echo "✅ ANTLR JAR: external/antlr-4.13.2-complete.jar"
    else
        echo "❌ 未找到ANTLR JAR文件"
        return 1
    fi
}

# 执行所有检查
echo "🔧 核心依赖检查:"
check_compiler || exit 1
check_cmake || exit 1
check_git || exit 1

echo ""
echo "☕ ANTLR依赖检查:"
check_java || exit 1
check_antlr || exit 1

echo ""
echo "🎉 所有依赖检查通过！"
echo "✅ 环境已准备就绪，可以开始构建CHTL"
```

### 依赖版本检查

```bash
#!/bin/bash
# scripts/check_versions.sh

echo "📋 CHTL依赖版本检查"
echo "==================="

# 检查C++标准支持
check_cpp_standard() {
    cat > test_cpp17.cpp << EOF
#include <filesystem>
#include <string_view>
int main() {
    std::filesystem::path p("test");
    std::string_view sv("test");
    return 0;
}
EOF

    if g++ -std=c++17 test_cpp17.cpp -o test_cpp17 2>/dev/null; then
        echo "✅ C++17支持: 正常"
        rm -f test_cpp17.cpp test_cpp17
    else
        echo "❌ C++17支持: 失败"
        rm -f test_cpp17.cpp test_cpp17
        return 1
    fi
}

# 检查CMake功能
check_cmake_features() {
    cmake_version=$(cmake --version | grep -o '[0-9]\+\.[0-9]\+\.[0-9]\+' | head -1)
    required_version="3.16.0"
    
    if [ "$(printf '%s\n' "$required_version" "$cmake_version" | sort -V | head -n1)" = "$required_version" ]; then
        echo "✅ CMake版本: $cmake_version (满足要求)"
    else
        echo "❌ CMake版本: $cmake_version (需要 >= $required_version)"
        return 1
    fi
}

# 检查Java版本
check_java_version() {
    if command -v java &> /dev/null; then
        java_version=$(java --version 2>&1 | grep -o '[0-9]\+' | head -1)
        
        if [ "$java_version" -ge 8 ]; then
            echo "✅ Java版本: $java_version (满足要求)"
        else
            echo "❌ Java版本: $java_version (需要 >= 8)"
            return 1
        fi
    else
        echo "❌ 未安装Java"
        return 1
    fi
}

# 执行版本检查
check_cpp_standard || exit 1
check_cmake_features || exit 1
check_java_version || exit 1

echo ""
echo "🎉 所有版本检查通过！"
```

---

## 安装指南

### Ubuntu/Debian一键安装

```bash
#!/bin/bash
# scripts/install_deps_ubuntu.sh

set -e

echo "🚀 Ubuntu/Debian CHTL依赖一键安装"

# 更新包列表
sudo apt update

# 安装基础开发工具
echo "📦 安装基础开发工具..."
sudo apt install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    curl \
    wget

# 安装C++编译器
echo "🔧 安装C++编译器..."
sudo apt install -y \
    g++-11 \
    clang-13 \
    libc++-dev \
    libc++abi-dev

# 安装Java
echo "☕ 安装Java..."
sudo apt install -y openjdk-11-jdk

# 设置默认编译器
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 100
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-11 100

# 设置环境变量
echo 'export JAVA_HOME=/usr/lib/jvm/java-11-openjdk' >> ~/.bashrc
echo 'export PATH=$JAVA_HOME/bin:$PATH' >> ~/.bashrc

# 安装可选工具
echo "🛠️ 安装开发工具..."
sudo apt install -y \
    gdb \
    valgrind \
    ccache \
    ninja-build

echo "✅ Ubuntu/Debian依赖安装完成！"
echo "🔄 请重新加载shell: source ~/.bashrc"
```

### macOS一键安装

```bash
#!/bin/bash
# scripts/install_deps_macos.sh

set -e

echo "🚀 macOS CHTL依赖一键安装"

# 检查Homebrew
if ! command -v brew &> /dev/null; then
    echo "📦 安装Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
fi

# 安装Xcode命令行工具
echo "🔧 安装Xcode命令行工具..."
xcode-select --install 2>/dev/null || true

# 安装依赖
echo "📦 安装开发依赖..."
brew install \
    cmake \
    git \
    openjdk@11 \
    pkg-config \
    llvm

# 设置环境变量
echo 'export JAVA_HOME=$(/usr/libexec/java_home -v 11)' >> ~/.zshrc
echo 'export PATH="/opt/homebrew/bin:$PATH"' >> ~/.zshrc
echo 'export PATH="/opt/homebrew/opt/llvm/bin:$PATH"' >> ~/.zshrc

# 安装可选工具
echo "🛠️ 安装开发工具..."
brew install \
    gdb \
    valgrind \
    ccache \
    ninja

echo "✅ macOS依赖安装完成！"
echo "🔄 请重新加载shell: source ~/.zshrc"
```

### Windows (WSL2) 安装

```bash
#!/bin/bash
# scripts/install_deps_wsl.sh

set -e

echo "🚀 Windows WSL2 CHTL依赖安装"

# 更新WSL
echo "🔄 更新WSL包列表..."
sudo apt update && sudo apt upgrade -y

# 安装Windows特定依赖
echo "🪟 安装Windows特定工具..."
sudo apt install -y \
    wslu \
    dos2unix

# 运行Ubuntu安装脚本
echo "📦 运行Ubuntu依赖安装..."
./scripts/install_deps_ubuntu.sh

# Windows路径集成
echo "🔗 配置Windows路径集成..."
echo 'export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '"'"'{print $2}'"'"'):0' >> ~/.bashrc

echo "✅ WSL2依赖安装完成！"
echo "💡 提示: 可以在Windows中访问 \\\\wsl$\\Ubuntu\\home\\$USER\\chtl"
```

---

## 环境变量配置

### 必需的环境变量

```bash
# JAVA_HOME (ANTLR需要)
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk

# PATH更新
export PATH=$JAVA_HOME/bin:$PATH

# CHTL特定环境变量
export CHTL_ROOT=/path/to/chtl
export CHTL_MODULE_PATH=$CHTL_ROOT/module
export CHTL_COMPILER_PATH=$CHTL_ROOT/build/bin/chtl_compiler
```

### 可选的环境变量

```bash
# 编译器选择
export CC=gcc-11
export CXX=g++-11

# 构建选项
export CMAKE_BUILD_TYPE=Release
export CMAKE_GENERATOR=Ninja

# ANTLR配置
export ANTLR4_ROOT=/usr/local
export ANTLR4_JAR=$CHTL_ROOT/external/antlr-4.13.2-complete.jar

# 调试选项
export CHTL_DEBUG=1
export CHTL_VERBOSE=1

# 性能选项
export CHTL_PARALLEL_BUILD=1
export CHTL_ENABLE_CACHE=1
```

### 环境配置脚本

```bash
#!/bin/bash
# scripts/setup_environment.sh

CHTL_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cat > ~/.chtl_env << EOF
# CHTL环境配置
export CHTL_ROOT="$CHTL_ROOT"
export CHTL_MODULE_PATH="\$CHTL_ROOT/module"
export CHTL_COMPILER_PATH="\$CHTL_ROOT/build/bin/chtl_compiler"

# Java环境 (ANTLR需要)
export JAVA_HOME="/usr/lib/jvm/java-11-openjdk"
export PATH="\$JAVA_HOME/bin:\$PATH"

# 编译器配置
export CC=gcc-11
export CXX=g++-11

# 构建配置
export CMAKE_BUILD_TYPE=Release
export MAKEFLAGS="-j\$(nproc)"

# ANTLR配置
export ANTLR4_JAR="\$CHTL_ROOT/external/antlr-4.13.2-complete.jar"

# 别名
alias chtl="\$CHTL_COMPILER_PATH"
alias chtl-build="cd \$CHTL_ROOT && ./scripts/build/build_complete_with_antlr.sh"
alias chtl-test="cd \$CHTL_ROOT/build && ctest"
EOF

# 添加到shell配置
echo "source ~/.chtl_env" >> ~/.bashrc

echo "✅ CHTL环境配置完成"
echo "🔄 请重新加载shell: source ~/.bashrc"
```

---

## 依赖问题排除

### 常见问题

#### 1. C++编译器版本过旧

```bash
# 问题: C++17特性不支持
# 症状: 编译错误，filesystem不可用

# 解决: 升级编译器
sudo apt install g++-11
export CXX=g++-11

# 验证C++17支持
echo '#include <filesystem>' | g++-11 -std=c++17 -x c++ -c -
```

#### 2. CMake版本过旧

```bash
# 问题: CMake版本 < 3.16
# 症状: CMakeLists.txt语法错误

# 解决: 升级CMake
# 方法1: 从官方下载
wget https://github.com/Kitware/CMake/releases/download/v3.25.0/cmake-3.25.0-linux-x86_64.sh
sudo sh cmake-3.25.0-linux-x86_64.sh --prefix=/usr/local --skip-license

# 方法2: 使用snap
sudo snap install cmake --classic

# 验证版本
cmake --version
```

#### 3. Java环境问题

```bash
# 问题: ANTLR无法运行
# 症状: java: command not found

# 解决: 安装并配置Java
sudo apt install openjdk-11-jdk
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk

# 验证Java和ANTLR
java --version
java -jar external/antlr-4.13.2-complete.jar
```

#### 4. 权限问题

```bash
# 问题: 权限不足
# 症状: Permission denied

# 解决: 设置正确权限
chmod +x scripts/*.sh
sudo chown -R $USER:$USER .

# 或使用sudo安装
sudo make install
```

#### 5. 库链接问题

```bash
# 问题: 链接库找不到
# 症状: shared library not found

# 解决: 更新库路径
sudo ldconfig

# 或设置LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

# 验证库
ldd build/bin/chtl_compiler
```

---

## 性能优化配置

### 编译性能优化

```bash
# 使用ccache加速编译
sudo apt install ccache
export CC="ccache gcc"
export CXX="ccache g++"

# 配置ccache
ccache --max-size=5G
ccache --set-config=compression=true

# 使用Ninja构建器
sudo apt install ninja-build
cmake -G Ninja ..
ninja -j$(nproc)
```

### 内存优化

```bash
# 对于内存受限的系统
export MAKEFLAGS="-j2"  # 限制并行编译数

# 使用交换文件
sudo fallocate -l 2G /swapfile
sudo chmod 600 /swapfile
sudo mkswap /swapfile
sudo swapon /swapfile
```

---

## 容器化环境

### Docker开发环境

```dockerfile
# Dockerfile.dev
FROM ubuntu:22.04

# 安装依赖
RUN apt update && apt install -y \
    build-essential \
    cmake \
    g++-11 \
    git \
    openjdk-11-jdk \
    pkg-config \
    ccache \
    ninja-build \
    gdb \
    valgrind

# 设置环境变量
ENV JAVA_HOME=/usr/lib/jvm/java-11-openjdk
ENV CC=gcc-11
ENV CXX=g++-11
ENV PATH=$JAVA_HOME/bin:$PATH

# 创建工作目录
WORKDIR /chtl

# 设置用户
ARG USERNAME=developer
RUN useradd -m -s /bin/bash $USERNAME
USER $USERNAME

# 设置开发环境
RUN echo 'alias chtl-build="./scripts/build/build_complete_with_antlr.sh"' >> ~/.bashrc
```

```bash
# 使用Docker开发环境
docker build -f Dockerfile.dev -t chtl-dev .
docker run -it -v $(pwd):/chtl chtl-dev bash

# 在容器中构建
chtl-build
```

### 开发容器配置

```json
// .devcontainer/devcontainer.json
{
    "name": "CHTL Development",
    "dockerFile": "../Dockerfile.dev",
    "workspaceFolder": "/chtl",
    "settings": {
        "terminal.integrated.shell.linux": "/bin/bash"
    },
    "extensions": [
        "ms-vscode.cpptools",
        "ms-vscode.cmake-tools",
        "ms-vscode.makefile-tools"
    ],
    "forwardPorts": [3000, 8080],
    "postCreateCommand": "./scripts/setup_dev_container.sh",
    "remoteUser": "developer"
}
```

---

## 环境验证

### 完整环境测试

```bash
#!/bin/bash
# scripts/verify_environment.sh

echo "🧪 CHTL环境完整验证"
echo "==================="

# 创建测试项目
mkdir -p /tmp/chtl_env_test
cd /tmp/chtl_env_test

# 创建简单的CHTL文件
cat > test.chtl << 'EOF'
div {
    class: test;
    
    style {
        .test {
            color: red;
        }
    }
    
    script {
        {{.test}}->listen({
            click: function() {
                console.log('test');
            }
        });
    }
    
    text { 环境测试 }
}
EOF

# 测试编译
echo "🔧 测试CHTL编译..."
if $CHTL_COMPILER_PATH test.chtl -o test.html; then
    echo "✅ CHTL编译测试通过"
else
    echo "❌ CHTL编译测试失败"
    exit 1
fi

# 验证输出
if [ -f "test.html" ] && grep -q "test" test.html; then
    echo "✅ 输出文件验证通过"
else
    echo "❌ 输出文件验证失败"
    exit 1
fi

# 清理
cd /
rm -rf /tmp/chtl_env_test

echo "🎉 环境验证完全通过！"
echo "✨ CHTL开发环境已完全就绪"
```

---

## 故障排除

### 诊断工具

```bash
#!/bin/bash
# scripts/diagnose_environment.sh

echo "🔍 CHTL环境诊断"
echo "==============="

# 系统信息
echo "💻 系统信息:"
uname -a
lsb_release -a 2>/dev/null || cat /etc/os-release

# 编译器信息
echo ""
echo "🔧 编译器信息:"
which gcc g++ clang++ || echo "编译器未找到"
gcc --version 2>/dev/null || echo "GCC未安装"
clang++ --version 2>/dev/null || echo "Clang未安装"

# 构建工具信息
echo ""
echo "🛠️ 构建工具:"
which cmake make ninja || echo "构建工具缺失"
cmake --version 2>/dev/null || echo "CMake未安装"
make --version 2>/dev/null || echo "Make未安装"

# Java信息
echo ""
echo "☕ Java环境:"
which java javac || echo "Java未找到"
java --version 2>/dev/null || echo "Java未安装"
echo "JAVA_HOME: $JAVA_HOME"

# ANTLR检查
echo ""
echo "🔍 ANTLR检查:"
if [ -f "external/antlr-4.13.2-complete.jar" ]; then
    echo "✅ ANTLR JAR文件存在"
    java -jar external/antlr-4.13.2-complete.jar 2>&1 | head -3
else
    echo "❌ ANTLR JAR文件缺失"
fi

# 库检查
echo ""
echo "📚 系统库:"
pkg-config --list-all | grep -E "(antlr|zip)" || echo "相关库未找到"

# 磁盘空间
echo ""
echo "💾 磁盘空间:"
df -h . | tail -1

# 内存信息
echo ""
echo "🧠 内存信息:"
free -h

echo ""
echo "📋 诊断完成"
```

---

## 最佳实践

### 环境管理

1. **版本固定**: 使用固定版本的依赖
2. **环境隔离**: 使用容器或虚拟环境
3. **自动化**: 使用脚本自动化环境设置
4. **文档化**: 详细记录环境配置

### 依赖管理

1. **最小依赖**: 只安装必需的依赖
2. **版本兼容**: 确保依赖版本兼容
3. **定期更新**: 定期更新依赖版本
4. **安全检查**: 检查依赖的安全漏洞

### 构建优化

1. **并行构建**: 充分利用多核CPU
2. **缓存利用**: 使用ccache等缓存工具
3. **增量构建**: 避免不必要的重新编译
4. **资源监控**: 监控构建过程的资源使用

---

通过本指南，开发者可以正确设置CHTL的开发和运行环境，确保项目的顺利构建和开发。我们致力于为所有平台的开发者提供一致、可靠的环境支持。