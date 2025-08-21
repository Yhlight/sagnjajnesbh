# Windows静态库构建指南

## 快速开始

### 方法1：使用GitHub Actions（推荐）

1. Fork或创建GitHub仓库
2. 将代码推送到GitHub
3. GitHub Actions会自动构建Windows库
4. 从Actions标签页下载构建好的库

### 方法2：使用预构建的库

如果您无法自行构建，可以：

1. 从Releases页面下载预构建的库
2. 解压到项目目录
3. 在Visual Studio中配置路径

## 自动构建流程

当您推送代码到GitHub后，`.github/workflows/build_windows_complete.yml`会自动：

1. 下载ANTLR工具和运行时
2. 下载CSS3和JavaScript语法文件
3. 编译ANTLR C++运行时
4. 生成CSS和JavaScript解析器
5. 构建静态库（Release和Debug版本）
6. 创建发布包

## 使用构建好的库

### 1. 下载库文件

从GitHub Actions或Releases下载后，您会得到：

```
chtl-antlr-windows-x64/
├── Release/
│   ├── chtl_css.lib
│   ├── chtl_javascript.lib
│   ├── antlr4-runtime.lib
│   └── chtl_antlr_all.lib
└── Debug/
    ├── chtl_css_d.lib
    ├── chtl_javascript_d.lib
    ├── antlr4-runtime_d.lib
    └── chtl_antlr_all_d.lib
```

### 2. Visual Studio配置

#### 项目属性设置：

1. **C/C++ → 常规 → 附加包含目录**
   ```
   $(ProjectDir)src
   $(ProjectDir)third_party\antlr4-runtime\runtime\src
   ```

2. **链接器 → 常规 → 附加库目录**
   ```
   $(ProjectDir)prebuilt\windows\x64\$(Configuration)
   ```

3. **链接器 → 输入 → 附加依赖项**
   - Release模式：`chtl_antlr_all.lib`
   - Debug模式：`chtl_antlr_all_d.lib`

### 3. 代码示例

```cpp
#include "chtl/css/CSSCompiler.h"
#include "chtl/javascript/JavaScriptCompiler.h"

int main() {
    // CSS编译
    chtl::css::CSSCompiler css;
    if (css.Compile(cssSource)) {
        std::string output = css.GetOutput();
    }
    
    // JavaScript编译
    chtl::javascript::JavaScriptCompiler js;
    if (js.Compile(jsSource)) {
        std::string output = js.GetOutput();
    }
    
    return 0;
}
```

## 手动构建（如果需要）

如果您想在本地Windows机器上构建：

```batch
# 1. 安装依赖
# - Visual Studio 2019/2022
# - CMake 3.14+
# - Java 8+

# 2. 运行构建脚本
build_all_windows.bat

# 3. 库文件会生成在
# build\antlr\*.lib
```

## 故障排除

### 链接错误

确保：
- 使用相同的运行时库（/MD或/MDd）
- Debug配置链接Debug库，Release配置链接Release库

### 找不到头文件

检查包含目录是否正确设置，特别是：
- `src/chtl/interfaces/ICompiler.h`
- ANTLR运行时头文件

### 运行时错误

确保安装了Visual C++运行时：
- Visual C++ Redistributable 2019或更新版本

## 技术支持

如果遇到问题：
1. 检查GitHub Actions的构建日志
2. 确认下载的库文件完整
3. 验证Visual Studio配置正确