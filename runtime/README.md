# CHTL 跨平台 ANTLR 运行时

这个目录包含了CHTL项目所需的跨平台ANTLR 4运行时预设。

## 目录结构

```
runtime/
├── README.md                    # 本文档
├── build/                       # 构建脚本和工具
│   ├── build-all.sh            # 全平台构建脚本
│   ├── build-linux.sh          # Linux构建脚本
│   ├── build-windows.bat       # Windows构建脚本
│   ├── build-macos.sh          # macOS构建脚本
│   ├── docker/                 # Docker构建环境
│   └── cmake/                  # CMake配置文件
├── prebuilt/                   # 预构建的运行时库
│   ├── linux-x64/             # Linux x64预构建
│   ├── linux-arm64/           # Linux ARM64预构建
│   ├── windows-x64/           # Windows x64预构建
│   ├── windows-x86/           # Windows x86预构建
│   ├── macos-x64/             # macOS Intel预构建
│   ├── macos-arm64/           # macOS Apple Silicon预构建
│   └── universal/             # 通用头文件
├── src/                       # 源代码和补丁
│   ├── antlr4-runtime/        # ANTLR运行时源码
│   ├── patches/               # 平台特定补丁
│   └── cmake/                 # 自定义CMake模块
├── test/                      # 测试和验证
│   ├── basic-test.cpp         # 基础功能测试
│   ├── platform-test.cpp     # 平台兼容性测试
│   └── performance-test.cpp   # 性能测试
└── tools/                     # 辅助工具
    ├── detect-platform.sh    # 平台检测脚本
    ├── verify-runtime.sh     # 运行时验证脚本
    └── package.sh            # 打包脚本
```

## 支持的平台

### Linux
- **x86_64**: Ubuntu 18.04+, CentOS 7+, Debian 9+
- **ARM64**: Ubuntu 20.04+, CentOS 8+
- **编译器**: GCC 7+, Clang 6+

### Windows
- **x86_64**: Windows 10+, Windows Server 2019+
- **x86**: Windows 10+ (32位)
- **编译器**: Visual Studio 2019+, MinGW-w64

### macOS
- **x86_64**: macOS 10.15+ (Intel)
- **ARM64**: macOS 11.0+ (Apple Silicon)
- **编译器**: Xcode 11+, Command Line Tools

## 快速开始

### 1. 构建所有平台
```bash
./build/build-all.sh
```

### 2. 构建特定平台
```bash
# Linux
./build/build-linux.sh

# Windows (在Windows上运行)
./build/build-windows.bat

# macOS
./build/build-macos.sh
```

### 3. 使用预构建版本
```bash
# 检测当前平台
./tools/detect-platform.sh

# 验证运行时
./tools/verify-runtime.sh
```

## CMake集成

在你的CMakeLists.txt中：

```cmake
# 包含ANTLR运行时
include(${CMAKE_SOURCE_DIR}/runtime/build/cmake/FindANTLRRuntime.cmake)

# 查找ANTLR运行时
find_package(ANTLRRuntime REQUIRED)

# 链接到你的目标
target_link_libraries(your_target ${ANTLR_RUNTIME_LIBRARIES})
target_include_directories(your_target PRIVATE ${ANTLR_RUNTIME_INCLUDE_DIRS})
```

## 特性

### ✅ 已支持
- [x] 静态库构建（.a/.lib）
- [x] 动态库构建（.so/.dll/.dylib）
- [x] 跨平台头文件
- [x] CMake集成
- [x] 自动平台检测
- [x] 基础测试套件

### 🚧 计划中
- [ ] 交叉编译支持
- [ ] Docker构建环境
- [ ] CI/CD集成
- [ ] 性能优化版本
- [ ] 调试符号版本

## 构建要求

### 通用要求
- CMake 3.16+
- Git
- 支持C++17的编译器

### Linux特定
- build-essential
- libuuid1-dev
- pkg-config

### Windows特定
- Visual Studio 2019+ 或 MinGW-w64
- Windows SDK

### macOS特定
- Xcode Command Line Tools
- Homebrew (推荐)

## 许可证

本项目遵循ANTLR 4的BSD许可证。详见LICENSE文件。

## 贡献

欢迎提交Issue和Pull Request来改进跨平台支持！

## 更新日志

### v1.0.0 (2024-01-XX)
- 初始版本
- 支持Linux x64/ARM64
- 支持Windows x64/x86
- 支持macOS x64/ARM64
- CMake集成
- 基础测试套件