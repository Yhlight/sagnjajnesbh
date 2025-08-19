# CHTL构建指南

## 概述

本指南详细介绍如何从源码构建CHTL编译器，包括环境配置、依赖安装、编译过程和常见问题解决。

---

## 环境要求

### 操作系统支持

- **Linux**: Ubuntu 18.04+, CentOS 7+, Debian 10+ (推荐)
- **macOS**: macOS 10.15+ (Catalina及以上)
- **Windows**: Windows 10+ (需要WSL或MSYS2)

### 编译器要求

- **GCC**: 9.0+ (推荐GCC 11+)
- **Clang**: 10.0+ (推荐Clang 13+)
- **MSVC**: Visual Studio 2019+ (Windows)

### 构建工具

- **CMake**: 3.16+ (推荐3.20+)
- **Make**: GNU Make 4.0+
- **Git**: 2.20+
- **Java**: OpenJDK 11+ (ANTLR需要)

---

## 依赖安装

### Linux (Ubuntu/Debian)

```bash
# 更新包管理器
sudo apt update

# 安装基础开发工具
sudo apt install build-essential cmake git

# 安装C++编译器
sudo apt install g++-11 clang-13

# 安装Java (ANTLR需要)
sudo apt install openjdk-11-jdk

# 安装其他依赖
sudo apt install pkg-config libzip-dev

# 验证安装
g++ --version
cmake --version
java --version
```

### macOS

```bash
# 安装Xcode命令行工具
xcode-select --install

# 安装Homebrew (如果未安装)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装依赖
brew install cmake git openjdk@11 pkg-config libzip

# 设置Java环境
export JAVA_HOME=$(/usr/libexec/java_home -v 11)
echo 'export JAVA_HOME=$(/usr/libexec/java_home -v 11)' >> ~/.zshrc

# 验证安装
clang++ --version
cmake --version
java --version
```

### Windows (WSL推荐)

```bash
# 在WSL中安装Ubuntu
wsl --install -d Ubuntu

# 进入WSL并按照Linux指南安装依赖
# 或使用MSYS2:

# 安装MSYS2
# 下载并安装: https://www.msys2.org/

# 在MSYS2中安装依赖
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-pkg-config git
```

---

## 获取源码

### 克隆仓库

```bash
# 克隆主仓库
git clone https://github.com/chtl-lang/chtl.git
cd chtl

# 查看分支
git branch -a

# 切换到开发分支（如果需要）
git checkout develop
```

### 初始化子模块

```bash
# 初始化和更新子模块
git submodule init
git submodule update --recursive
```

---

## 构建过程

### 快速构建

```bash
# 创建构建目录
mkdir build && cd build

# 配置CMake (Release模式)
cmake -DCMAKE_BUILD_TYPE=Release ..

# 编译 (使用所有CPU核心)
make -j$(nproc)

# 验证构建
ls -la lib/
ls -la bin/
```

### 调试构建

```bash
# 调试模式构建
mkdir build-debug && cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)

# 使用调试器
gdb ./bin/chtl_compiler
```

### 完整构建 (包含ANTLR)

```bash
# 使用完整构建脚本
./scripts/build/build_complete_with_antlr.sh

# 或手动构建
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_ANTLR=ON ..
make -j$(nproc)
```

---

## 构建选项

### CMake配置选项

```bash
# 基础选项
cmake -DCMAKE_BUILD_TYPE=Release ..          # 发布模式
cmake -DCMAKE_BUILD_TYPE=Debug ..            # 调试模式
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..   # 带调试信息的发布模式

# 功能选项
cmake -DENABLE_ANTLR=ON ..                   # 启用ANTLR集成
cmake -DENABLE_CJMOD=ON ..                   # 启用CJMOD支持
cmake -DENABLE_MODULES=ON ..                 # 启用模块系统
cmake -DENABLE_TESTING=ON ..                 # 启用测试

# 路径选项
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..   # 安装路径
cmake -DANTLR_ROOT=/path/to/antlr ..         # ANTLR路径

# 编译器选项
cmake -DCMAKE_CXX_COMPILER=g++-11 ..         # 指定编译器
cmake -DCMAKE_CXX_STANDARD=17 ..             # C++标准版本
```

### 高级构建选项

```bash
# 性能优化
cmake -DCMAKE_CXX_FLAGS="-O3 -march=native" ..

# 链接时优化
cmake -DCMAKE_CXX_FLAGS="-flto" ..

# 静态链接
cmake -DBUILD_STATIC=ON ..

# 交叉编译
cmake -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake ..
```

---

## 构建目标

### 核心库目标

```bash
# 编译共享组件库
make chtl_common

# 编译扫描器库
make chtl_scanner

# 编译CHTL编译器库
make chtl_compiler

# 编译CHTL JS编译器库
make chtljs_compiler

# 编译所有库
make all
```

### 可执行文件目标

```bash
# 编译主编译器
make chtl_main

# 编译测试程序
make test_compliance
make test_features

# 编译所有测试
make tests
```

### 安装目标

```bash
# 安装到系统
sudo make install

# 安装到指定目录
make install DESTDIR=/path/to/install

# 创建安装包
make package
```

---

## ANTLR集成构建

### ANTLR依赖

CHTL使用ANTLR 4进行CSS和JavaScript的原生解析：

```bash
# 检查ANTLR JAR文件
ls -la external/antlr-4.13.2-complete.jar

# 检查生成的解析器
ls -la src/CSSCompiler/Parser/css3*.cpp
ls -la src/JavaScriptCompiler/Parser/JavaScript*.cpp
```

### 重新生成ANTLR解析器

```bash
# 进入语法目录
cd src/CSSCompiler/Parser

# 使用ANTLR生成C++解析器
java -jar ../../../external/antlr-4.13.2-complete.jar \
    -Dlanguage=Cpp \
    -o . \
    css3Lexer.g4 css3Parser.g4

# 生成JavaScript解析器
cd ../../JavaScriptCompiler/Parser
java -jar ../../../external/antlr-4.13.2-complete.jar \
    -Dlanguage=Cpp \
    -o . \
    JavaScriptLexer.g4 JavaScriptParser.g4
```

### ANTLR构建验证

```bash
# 验证ANTLR集成
./build/test_antlr_basic

# 测试CSS解析
echo "body { color: red; }" | ./build/test_css_parser

# 测试JavaScript解析
echo "function test() { return 42; }" | ./build/test_js_parser
```

---

## 模块构建

### 构建CMOD模块

```bash
# 验证模块结构
./scripts/utils/validate_cmod.sh src/Module/CMOD/Chtholly

# 打包CMOD模块
./build/chtl_compiler pack-cmod src/Module/CMOD/Chtholly packages/Chtholly.cmod

# 或使用打包脚本
./scripts/packaging/pack_modules_enhanced.sh
```

### 构建CJMOD扩展

```bash
# 编译C++扩展
cd src/Module/CJMOD/Chtholly/src
g++ -std=c++17 -fPIC -shared *.cpp -o libChtholly.so

# 打包CJMOD扩展
cd ../../../../..
./build/chtl_compiler pack-cjmod src/Module/CJMOD/Chtholly packages/Chtholly.cjmod
```

---

## 测试构建

### 编译测试

```bash
# 编译所有测试
make tests

# 运行语法合规性测试
./build/bin/test_complete_chtl_compliance

# 运行功能测试
./build/bin/test_existing_features

# 运行模块测试
./build/test_chtholly_module
```

### 性能测试

```bash
# 编译性能测试
make performance_tests

# 运行性能基准测试
./build/benchmark_compiler

# 内存使用测试
valgrind --tool=memcheck ./build/chtl_compiler test.chtl
```

---

## 常见问题解决

### 编译错误

#### 找不到ANTLR

```bash
# 错误: ANTLR not found
# 解决: 确保Java已安装并设置JAVA_HOME
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk
export PATH=$JAVA_HOME/bin:$PATH

# 验证ANTLR JAR文件
ls -la external/antlr-4.13.2-complete.jar
```

#### C++标准版本错误

```bash
# 错误: C++17 features not available
# 解决: 更新编译器或指定C++标准
cmake -DCMAKE_CXX_STANDARD=17 ..

# 或指定编译器
cmake -DCMAKE_CXX_COMPILER=g++-11 ..
```

#### 链接错误

```bash
# 错误: undefined reference
# 解决: 检查库的链接顺序
make VERBOSE=1  # 查看详细编译信息

# 清理并重新构建
make clean
make -j$(nproc)
```

### 运行时错误

#### 找不到模块

```bash
# 错误: Module not found
# 解决: 设置模块搜索路径
export CHTL_MODULE_PATH="/usr/local/lib/chtl/module"

# 或复制模块到正确位置
cp packages/*.cmod /usr/local/bin/module/CMOD/
```

#### 权限错误

```bash
# 错误: Permission denied
# 解决: 设置正确的权限
chmod +x ./build/chtl_compiler

# 或使用sudo安装
sudo make install
```

---

## 自动化构建

### CI/CD配置

```yaml
# .github/workflows/build.yml
name: CHTL Build

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
      with:
        submodules: recursive
    
    - name: Install Dependencies
      run: |
        sudo apt update
        sudo apt install build-essential cmake g++-11 openjdk-11-jdk
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
    
    - name: Build
      run: cmake --build build --parallel
    
    - name: Test
      run: cd build && ctest
    
    - name: Package
      run: |
        cd build
        make package
    
    - name: Upload Artifacts
      uses: actions/upload-artifact@v3
      with:
        name: chtl-build
        path: build/packages/
```

### 构建脚本

```bash
#!/bin/bash
# scripts/build/build_all.sh

set -e

echo "🚀 开始CHTL完整构建"

# 检查环境
echo "📋 检查构建环境..."
command -v cmake >/dev/null 2>&1 || { echo "❌ CMake未安装"; exit 1; }
command -v g++ >/dev/null 2>&1 || { echo "❌ G++未安装"; exit 1; }
command -v java >/dev/null 2>&1 || { echo "❌ Java未安装"; exit 1; }

# 清理旧构建
echo "🧹 清理旧构建..."
rm -rf build
mkdir build && cd build

# 配置CMake
echo "⚙️ 配置CMake..."
cmake -DCMAKE_BUILD_TYPE=Release \
      -DENABLE_ANTLR=ON \
      -DENABLE_CJMOD=ON \
      -DENABLE_MODULES=ON \
      -DENABLE_TESTING=ON \
      ..

# 编译
echo "🔧 开始编译..."
make -j$(nproc)

# 运行测试
echo "🧪 运行测试..."
ctest --output-on-failure

# 打包模块
echo "📦 打包模块..."
../scripts/packaging/pack_modules_enhanced.sh

echo "✅ CHTL构建完成！"
echo "📁 输出目录: $(pwd)"
echo "🎯 可执行文件: $(ls -la bin/ | grep chtl)"
echo "📚 库文件: $(ls -la lib/ | grep libchtl)"
```

---

## 分布式构建

### 并行构建

```bash
# 使用所有CPU核心
make -j$(nproc)

# 指定核心数
make -j8

# 使用distcc分布式编译
export CC="distcc gcc"
export CXX="distcc g++"
make -j16
```

### 交叉编译

```bash
# ARM64交叉编译
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/arm64-toolchain.cmake ..

# 嵌入式系统交叉编译
cmake -DCMAKE_TOOLCHAIN_FILE=cmake/embedded-toolchain.cmake ..
```

---

## 性能优化构建

### 优化级别

```bash
# 最大优化
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -march=native -flto" ..

# 调试优化
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo \
      -DCMAKE_CXX_FLAGS="-O2 -g" ..

# 大小优化
cmake -DCMAKE_BUILD_TYPE=MinSizeRel \
      -DCMAKE_CXX_FLAGS="-Os -s" ..
```

### 链接时优化

```bash
# 启用LTO
cmake -DCMAKE_CXX_FLAGS="-flto" \
      -DCMAKE_EXE_LINKER_FLAGS="-flto" ..

# 静态链接
cmake -DBUILD_STATIC=ON ..
```

---

## 包管理构建

### 创建安装包

```bash
# 配置包信息
cmake -DCPACK_GENERATOR="DEB;RPM;TGZ" ..

# 构建包
make package

# 生成的包文件
ls -la chtl-*.deb
ls -la chtl-*.rpm
ls -la chtl-*.tar.gz
```

### Debian包构建

```bash
# 安装打包工具
sudo apt install debhelper dh-make

# 创建Debian包
cmake -DCPACK_GENERATOR=DEB ..
make package

# 安装包
sudo dpkg -i chtl-1.0.0-Linux.deb
```

### RPM包构建

```bash
# 安装打包工具
sudo yum install rpm-build

# 创建RPM包
cmake -DCPACK_GENERATOR=RPM ..
make package

# 安装包
sudo rpm -i chtl-1.0.0-Linux.rpm
```

---

## 构建验证

### 功能验证

```bash
# 验证编译器
./build/bin/chtl_compiler --version

# 验证模块系统
./build/bin/chtl_compiler list-modules

# 验证ANTLR集成
./build/bin/chtl_compiler test-antlr

# 编译测试文件
echo 'div { text { Hello, CHTL! } }' > test.chtl
./build/bin/chtl_compiler test.chtl -o test.html
cat test.html
```

### 性能验证

```bash
# 编译性能测试
time ./build/bin/chtl_compiler large_file.chtl

# 内存使用测试
/usr/bin/time -v ./build/bin/chtl_compiler test.chtl

# 并发测试
for i in {1..10}; do
    ./build/bin/chtl_compiler test$i.chtl &
done
wait
```

---

## 开发构建

### 增量构建

```bash
# 监控文件变化并自动构建
inotifywait -m -r src/ --format '%w%f %e' -e modify,create,delete | while read file event; do
    echo "文件变化: $file ($event)"
    make -j$(nproc)
done
```

### 模块化构建

```bash
# 只构建特定组件
make chtl_common
make chtl_scanner
make chtl_compiler

# 只构建测试
make tests

# 只构建文档
make docs
```

---

## 构建优化

### 编译缓存

```bash
# 使用ccache加速编译
sudo apt install ccache
export CC="ccache gcc"
export CXX="ccache g++"

# 配置ccache
ccache --max-size=5G
ccache --show-stats
```

### 预编译头文件

```cmake
# CMakeLists.txt中启用PCH
target_precompile_headers(chtl_compiler PRIVATE
    <iostream>
    <string>
    <vector>
    <memory>
)
```

---

## 调试构建

### 调试信息

```bash
# 启用详细调试信息
cmake -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_CXX_FLAGS="-g3 -O0 -DDEBUG" ..

# 启用地址检查
cmake -DCMAKE_CXX_FLAGS="-fsanitize=address -g" ..

# 启用内存检查
cmake -DCMAKE_CXX_FLAGS="-fsanitize=memory -g" ..
```

### 调试工具

```bash
# 使用GDB调试
gdb --args ./build/bin/chtl_compiler test.chtl

# 使用Valgrind检查内存
valgrind --leak-check=full ./build/bin/chtl_compiler test.chtl

# 使用strace跟踪系统调用
strace -o trace.log ./build/bin/chtl_compiler test.chtl
```

---

## 构建脚本开发

### 自定义构建脚本

```bash
#!/bin/bash
# custom_build.sh

# 设置环境变量
export CC=gcc-11
export CXX=g++-11
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk

# 构建函数
build_component() {
    local component=$1
    echo "🔧 构建组件: $component"
    make $component -j$(nproc)
    
    if [ $? -eq 0 ]; then
        echo "✅ $component 构建成功"
    else
        echo "❌ $component 构建失败"
        exit 1
    fi
}

# 主构建流程
main() {
    echo "🚀 开始自定义构建"
    
    # 配置
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cd build
    
    # 分步构建
    build_component chtl_common
    build_component chtl_scanner
    build_component chtl_compiler
    build_component chtljs_compiler
    
    echo "🎉 构建完成！"
}

main "$@"
```

---

## 持续集成

### GitHub Actions

```yaml
name: CHTL CI/CD

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-linux:
    runs-on: ubuntu-latest
    
    strategy:
      matrix:
        compiler: [gcc-11, clang-13]
        build_type: [Release, Debug]
    
    steps:
    - name: Checkout
      uses: actions/checkout@v3
      with:
        submodules: recursive
    
    - name: Install Dependencies
      run: |
        sudo apt update
        sudo apt install build-essential cmake ${{ matrix.compiler }} openjdk-11-jdk
    
    - name: Configure
      run: |
        cmake -B build \
              -DCMAKE_BUILD_TYPE=${{ matrix.build_type }} \
              -DCMAKE_CXX_COMPILER=${{ matrix.compiler }}
    
    - name: Build
      run: cmake --build build --parallel
    
    - name: Test
      run: cd build && ctest --output-on-failure
    
    - name: Package
      if: matrix.build_type == 'Release'
      run: cd build && make package
    
    - name: Upload Artifacts
      if: matrix.build_type == 'Release'
      uses: actions/upload-artifact@v3
      with:
        name: chtl-${{ matrix.compiler }}
        path: build/packages/
```

---

## 部署构建

### 生产环境构建

```bash
# 生产环境优化构建
cmake -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_FLAGS="-O3 -DNDEBUG -march=native" \
      -DENABLE_TESTING=OFF \
      -DBUILD_STATIC=ON \
      ..

make -j$(nproc)

# 创建发布包
make package

# 验证发布包
tar -tzf chtl-1.0.0-Linux.tar.gz
```

### Docker构建

```dockerfile
# Dockerfile
FROM ubuntu:22.04

# 安装依赖
RUN apt update && apt install -y \
    build-essential cmake g++-11 openjdk-11-jdk git

# 复制源码
COPY . /chtl
WORKDIR /chtl

# 构建
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

# 设置入口点
ENTRYPOINT ["./build/bin/chtl_compiler"]
```

```bash
# 构建Docker镜像
docker build -t chtl:latest .

# 运行编译器
docker run --rm -v $(pwd):/workspace chtl:latest /workspace/test.chtl
```

---

## 构建监控

### 构建时间分析

```bash
# 分析构建时间
make -j$(nproc) 2>&1 | ts '[%Y-%m-%d %H:%M:%S]' | tee build.log

# 分析最耗时的编译单元
grep "Building CXX object" build.log | sort -k3 -nr
```

### 构建大小分析

```bash
# 分析二进制文件大小
ls -lah build/lib/
ls -lah build/bin/

# 分析符号表大小
nm --size-sort build/lib/libchtl_compiler.a | tail -20

# 分析依赖关系
ldd build/bin/chtl_compiler
```

---

## 维护和更新

### 依赖更新

```bash
# 更新ANTLR版本
wget https://www.antlr.org/download/antlr-4.13.3-complete.jar
mv antlr-4.13.3-complete.jar external/

# 重新生成解析器
./scripts/integration/regenerate_antlr_parsers.sh
```

### 构建系统维护

```bash
# 清理构建缓存
rm -rf build/
rm -rf CMakeCache.txt

# 更新CMake配置
cmake --version
# 如果版本过旧，升级CMake

# 验证构建系统
cmake -P cmake/verify_build_system.cmake
```

---

CHTL的构建系统设计为简单、可靠和高效。通过本指南，开发者可以轻松地构建CHTL编译器，并参与到CHTL的开发中来。我们致力于为所有开发者提供优秀的构建体验。