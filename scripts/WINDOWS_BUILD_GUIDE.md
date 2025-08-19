# Windows构建指南

## 概述

在Windows上构建CHTL需要特殊的环境配置。本指南提供详细的Windows构建说明。

## 前置要求

### 必需工具

1. **Visual Studio 2019/2022** 或 **Visual Studio Build Tools**
   - 必须包含"使用C++的桌面开发"工作负载
   - 下载地址：https://visualstudio.microsoft.com/downloads/

2. **CMake 3.16+**
   - 下载地址：https://cmake.org/download/
   - 确保添加到PATH环境变量

3. **Git**
   - 下载地址：https://git-scm.com/download/win

### 可选工具

- **Ninja** - 更快的构建系统
- **7-Zip** - 更好的压缩支持
- **Node.js** - VSCode插件开发需要

## 环境设置

### 方法1：使用Developer Command Prompt (推荐)

1. 打开开始菜单
2. 搜索"Developer Command Prompt for VS 2022"（或2019）
3. 以管理员身份运行
4. 导航到CHTL项目目录
5. 运行构建脚本

### 方法2：使用自动环境设置

```batch
REM 运行环境设置脚本
scripts\integration\setup_windows_env.bat

REM 然后运行构建
scripts\integration\build_complete.bat
```

### 方法3：手动设置环境

```batch
REM 找到vcvarsall.bat并运行
"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM 验证环境
where cl
where msbuild
```

## 构建命令

### 完整构建

```batch
REM 在Developer Command Prompt中运行
scripts\integration\build_complete.bat

REM 或指定选项
scripts\integration\build_complete.bat --release --clean
```

### 单独构建组件

```batch
REM 只构建编译器
scripts\build\build_debug.bat

REM 只构建VSCode插件
scripts\build\build_vscode_plugin.bat production

REM 只构建ANTLR
scripts\build\build_antlr.bat
```

### 模块打包

```batch
REM 打包所有模块
scripts\packaging\pack_all.bat

REM 打包特定CMOD模块
scripts\packaging\pack_cmod.bat src\Module\CMOD\Chtholly

REM 打包特定CJMOD模块（带编译）
scripts\packaging\pack_cjmod.bat --compile src\Module\CJMOD\MyExtension
```

## 常见问题解决

### 问题1：找不到构建系统

**错误信息**：
```
[ERROR] No suitable build system found (Visual Studio, MinGW, or Make)
```

**解决方案**：
1. 确保从Developer Command Prompt运行
2. 或运行环境设置脚本：
   ```batch
   scripts\integration\setup_windows_env.bat
   ```

### 问题2：CMake配置失败

**错误信息**：
```
[ERROR] CMake configuration failed
```

**解决方案**：
1. 检查CMake版本：
   ```batch
   cmake --version
   ```
2. 确保Visual Studio环境已设置：
   ```batch
   where cl
   where msbuild
   ```

### 问题3：编译失败

**错误信息**：
```
[ERROR] Build failed
```

**解决方案**：
1. 检查是否有足够的磁盘空间
2. 尝试单线程构建：
   ```batch
   msbuild solution.sln /p:Configuration=Debug /m:1
   ```
3. 查看详细错误信息：
   ```batch
   msbuild solution.sln /p:Configuration=Debug /v:detailed
   ```

### 问题4：找不到可执行文件

**现象**：构建成功但找不到chtl_compiler.exe

**解决方案**：
1. 检查不同目录：
   ```batch
   dir /s chtl*.exe
   ```
2. 可能的位置：
   - `build-windows-Debug\Debug\`
   - `build-windows-Debug\src\Debug\`
   - `build-windows-Debug\bin\`

## 构建输出

### 目录结构

构建完成后，您将看到：

```
项目根目录/
├── build-windows-Debug/     # 调试构建
├── build-windows-Release/   # 发布构建
├── packages/                # 打包文件
│   ├── *.vsix              # VSCode插件
│   ├── *.cmod              # CMOD模块
│   └── *.cjmod             # CJMOD模块
└── dist/                   # 分发文件
    ├── bin/                # 可执行文件
    ├── docs/               # 文档
    └── module/             # 模块
```

### 验证构建

```batch
REM 检查编译器版本
build-windows-Debug\Debug\chtl_compiler.exe --version

REM 或者
dist\bin\chtl_compiler.exe --version

REM 测试编译
echo div { text { Hello CHTL } } > test.chtl
chtl_compiler test.chtl -o test.html
```

## 性能优化

### 加速构建

1. **使用Ninja**：
   ```batch
   cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
   ninja
   ```

2. **并行构建**：
   ```batch
   msbuild /m:%NUMBER_OF_PROCESSORS%
   ```

3. **增量构建**：
   ```batch
   REM 只重新构建变更的文件
   msbuild /p:Configuration=Debug
   ```

### 减少构建时间

1. 使用SSD硬盘
2. 关闭实时杀毒扫描（构建目录）
3. 增加虚拟内存
4. 使用RAM磁盘（高级用户）

## 故障排除

### 诊断脚本

```batch
REM 运行诊断
scripts\integration\setup_windows_env.bat

REM 检查环境变量
echo VSINSTALLDIR: %VSINSTALLDIR%
echo VCINSTALLDIR: %VCINSTALLDIR%
echo PATH: %PATH%
```

### 日志文件

构建过程会生成日志文件：
- `build-report-*.txt` - 构建报告
- `CMakeCache.txt` - CMake缓存
- `CMakeFiles/CMakeError.log` - CMake错误日志

### 获取帮助

如果遇到问题：

1. 查看构建报告文件
2. 运行带--verbose参数的脚本
3. 检查CMake错误日志
4. 在GitHub Issues报告问题

## 开发建议

### IDE配置

推荐使用以下IDE：

1. **Visual Studio 2022**
   - 完整的调试支持
   - IntelliSense
   - 集成构建

2. **Visual Studio Code**
   - 轻量级
   - 丰富的扩展
   - 跨平台

3. **CLion**
   - 强大的C++支持
   - CMake集成

### 调试配置

在Visual Studio中调试：

1. 打开生成的.sln文件
2. 设置启动项目为chtl_compiler
3. 配置命令行参数
4. 设置断点并开始调试

## 自动化脚本

### 一键构建和安装

```batch
REM 创建一键脚本
@echo off
echo 正在构建和安装CHTL...
scripts\integration\build_complete.bat --release --clean
if %errorlevel%==0 (
    quick-install.bat
    echo 构建和安装完成！
) else (
    echo 构建失败，请查看错误信息
)
pause
```

### 开发者工作流

```batch
REM 开发者日常构建脚本
@echo off
echo 开发者构建...
scripts\build\build_debug.bat
if %errorlevel%==0 (
    echo 构建成功，运行测试...
    cd build-windows-Debug
    ctest
)
```

---

通过本指南，Windows用户可以成功构建和使用CHTL编译器。如有问题，请参考故障排除部分或联系开发团队。