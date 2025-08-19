# VSCode + CMake 工作流指南

## 概述

CHTL项目采用标准的VSCode + CMake开发工作流，支持所有主流编译器（GCC、Clang、MSVC）。

## 环境要求

### 必需工具
- **VSCode** - 主要开发环境
- **CMake 3.16+** - 构建系统
- **C++17编译器** - 以下任一即可：
  - GCC 9+ (推荐MinGW64 on Windows)
  - Clang 10+
  - MSVC 2019+ (可选)

### VSCode扩展
推荐安装以下扩展：
- **CMake Tools** - CMake集成
- **C/C++** - IntelliSense和调试
- **CMake** - CMake语法支持

## 快速开始

### 1. 打开项目
```bash
# 克隆项目
git clone <repository-url>
cd chtl

# 在VSCode中打开
code .
```

### 2. 配置CMake
在VSCode中：
1. 按 `Ctrl+Shift+P` 打开命令面板
2. 输入 "CMake: Configure"
3. 选择编译器工具包（如果有多个）

### 3. 构建项目
在VSCode中：
1. 按 `Ctrl+Shift+P`
2. 输入 "CMake: Build"
或者按 `F7`

### 4. 运行和调试
1. 按 `Ctrl+Shift+P`
2. 输入 "CMake: Debug"
或者按 `F5`

## 命令行构建

### Windows (推荐MinGW64)
```batch
# 调试构建
scripts\build\build_debug.bat

# 发布构建
scripts\build\build_release.bat

# 完整构建（编译器 + VSCode插件 + 模块）
scripts\integration\build_complete.bat
```

### Linux/macOS
```bash
# 调试构建
./scripts/build/build_debug.sh

# 发布构建
./scripts/build/build_release.sh

# 完整构建（编译器 + VSCode插件 + 模块）
./scripts/integration/build_complete.sh
```

## 编译器支持

### MinGW64 (Windows推荐)
```bash
# 安装MinGW64
# 方法1: 从 https://www.mingw-w64.org/ 下载
# 方法2: 使用MSYS2
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-cmake

# 确保在PATH中
echo $PATH | grep mingw64
```

### GCC (Linux)
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake3

# Arch Linux
sudo pacman -S base-devel cmake
```

### Clang (跨平台)
```bash
# Ubuntu
sudo apt install clang cmake

# macOS
brew install llvm cmake

# Windows
# 下载LLVM from https://llvm.org/
```

## 构建配置

### CMake预设
项目支持以下构建类型：
- **Debug** - 调试版本，包含调试符号
- **Release** - 发布版本，优化编译
- **RelWithDebInfo** - 带调试信息的发布版本
- **MinSizeRel** - 最小尺寸发布版本

### 构建选项
在CMake配置时可以设置：
```bash
# 启用详细输出
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..

# 指定编译器
cmake -DCMAKE_CXX_COMPILER=g++ ..
cmake -DCMAKE_CXX_COMPILER=clang++ ..

# 自定义安装路径
cmake -DCMAKE_INSTALL_PREFIX=/opt/chtl ..
```

## VSCode配置

### 工作区设置 (.vscode/settings.json)
```json
{
    "cmake.buildDirectory": "${workspaceFolder}/build-debug",
    "cmake.generator": "Ninja",
    "cmake.configureOnOpen": true,
    "cmake.buildBeforeRun": true,
    "C_Cpp.default.configurationProvider": "ms-vscode.cmake-tools",
    "C_Cpp.default.compileCommands": "${workspaceFolder}/compile_commands.json"
}
```

### 调试配置 (.vscode/launch.json)
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug CHTL Compiler",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build-debug/bin/chtl_compiler",
            "args": ["test.chtl", "-o", "test.html"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

### 任务配置 (.vscode/tasks.json)
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "CMake: Build Debug",
            "type": "shell",
            "command": "cmake",
            "args": ["--build", "build-debug", "--parallel"],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": ["$gcc"]
        },
        {
            "label": "CMake: Build Release",
            "type": "shell",
            "command": "cmake",
            "args": ["--build", "build-release", "--parallel"],
            "group": "build",
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": ["$gcc"]
        }
    ]
}
```

## 故障排除

### 常见问题

#### 1. CMake找不到编译器
```bash
# 检查编译器是否在PATH中
which g++
which clang++
which cl

# 手动指定编译器
cmake -DCMAKE_CXX_COMPILER=g++ ..
```

#### 2. 找不到头文件
```bash
# 检查include路径
cmake -DCMAKE_VERBOSE_MAKEFILE=ON ..
make VERBOSE=1
```

#### 3. 链接错误
```bash
# 检查库依赖
ldd build-debug/bin/chtl_compiler  # Linux
otool -L build-debug/bin/chtl_compiler  # macOS
```

#### 4. Windows路径问题
使用正斜杠或转义反斜杠：
```batch
cmake -DCMAKE_INSTALL_PREFIX=C:/chtl ..
```

### MinGW64特殊说明

如果使用MinGW64，确保：
1. MinGW64的bin目录在PATH中
2. 使用MinGW64的cmake（如果有的话）
3. 在MinGW64终端中运行构建脚本

```bash
# 检查MinGW64环境
gcc --version
g++ --version
cmake --version

# 构建
./scripts/build/build_debug.sh
```

## 开发工作流

### 日常开发
1. 在VSCode中打开项目
2. 修改代码
3. 按 `F7` 构建
4. 按 `F5` 调试

### 测试流程
```bash
# 构建并运行测试
cmake --build build-debug
cd build-debug
ctest

# 或使用脚本
./scripts/build/build_debug.sh
cd build-debug && ctest
```

### 发布流程
```bash
# 发布构建
./scripts/build/build_release.sh

# 打包分发
cmake --build build-release --target package
```

## 性能优化

### 并行构建
```bash
# 使用所有CPU核心
cmake --build . --parallel

# 指定核心数
cmake --build . --parallel 8
```

### 使用Ninja (更快的构建)
```bash
# 安装Ninja
# Ubuntu: sudo apt install ninja-build
# Windows: choco install ninja

# 使用Ninja生成器
cmake -G Ninja ..
ninja
```

### 使用ccache (加速重编译)
```bash
# 安装ccache
# Ubuntu: sudo apt install ccache
# macOS: brew install ccache

# 配置CMake使用ccache
cmake -DCMAKE_CXX_COMPILER_LAUNCHER=ccache ..
```

## 集成开发

### 与Git集成
VSCode自动集成Git，支持：
- 版本控制可视化
- 差异比较
- 提交和推送

### 与调试器集成
支持以下调试器：
- **GDB** (Linux/MinGW)
- **LLDB** (macOS/Clang)
- **MSVC Debugger** (Visual Studio)

### 代码分析
推荐安装：
- **Clang-Tidy** - 静态分析
- **Cppcheck** - 代码检查
- **Include What You Use** - 头文件优化

## 总结

CHTL项目采用现代C++开发最佳实践：
- 标准CMake构建系统
- VSCode现代IDE支持
- 跨平台编译器兼容
- 自动化构建和测试

这种方式确保了：
- 开发环境的一致性
- 构建过程的可重复性
- 跨平台的兼容性
- 社区贡献的便利性