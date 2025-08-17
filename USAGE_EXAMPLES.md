# CHTL 增强功能使用示例

## 1. 导入功能示例

### 1.1 HTML/CSS/JS 导入（需要as语法）

```chtl
// 导入HTML文件
[Import] @Html from header.html as HeaderTemplate;
[Import] @Html from footer.html as FooterTemplate;

// 导入CSS文件
[Import] @Style from bootstrap.css as BootstrapStyles;
[Import] @Style from custom.css as CustomStyles;

// 导入JavaScript文件
[Import] @JavaScript from jquery.js as JQueryLib;
[Import] @JavaScript from utils.js as UtilFunctions;

html
{
    head
    {
        [Origin] @Style BootstrapStyles;
        [Origin] @Style CustomStyles;
    }
    
    body
    {
        [Origin] @Html HeaderTemplate;
        
        div
        {
            text { 主要内容 }
        }
        
        [Origin] @Html FooterTemplate;
        [Origin] @JavaScript JQueryLib;
        [Origin] @JavaScript UtilFunctions;
    }
}
```

### 1.2 CHTL模块导入

```chtl
// 基本模块导入
[Import] @Chtl from Components;
[Import] @Chtl from UIKit;

// 官方模块导入
[Import] @Chtl from chtl::Chtholly;
[Import] @Chtl from chtl::Chtholly.Space;

// 具体文件导入
[Import] @Chtl from ./modules/Button.chtl;
[Import] @Chtl from ./modules/Card.cmod;

// 通配符导入
[Import] @Chtl from ./components.*;
[Import] @Chtl from ./modules/*.cmod;
[Import] @Chtl from ./templates/*.chtl;
```

### 1.3 特定项目导入

```chtl
// 导入特定的自定义元素
[Import] [Custom] @Element Button from UIKit as CustomButton;

// 导入特定的模板样式
[Import] [Template] @Style Theme from UIKit as DefaultTheme;

// 导入特定的变量组
[Import] [Custom] @Var Colors from Theme as ThemeColors;
```

### 1.4 CJmod导入

```chtl
// 导入CJmod扩展
[Import] @CJmod from Extensions;
[Import] @CJmod from AdvancedFeatures.cjmod;
[Import] @CJmod from ./plugins/CustomExtension;
```

## 2. 命名空间功能示例

### 2.1 命名空间定义

```chtl
[Namespace] UI
{
    [Template] @Style ButtonStyle
    {
        background-color: blue;
        color: white;
    }
    
    [Custom] @Element Button
    {
        button
        {
            style { @Style ButtonStyle; }
        }
    }
    
    [Namespace] Components
    {
        [Custom] @Element Card
        {
            div
            {
                style
                {
                    .card
                    {
                        border: 1px solid #ccc;
                        border-radius: 8px;
                        padding: 16px;
                    }
                }
            }
        }
    }
}
```

### 2.2 命名空间使用

```chtl
body
{
    // 使用命名空间中的元素
    @Element Button from UI;
    
    // 使用嵌套命名空间中的元素
    @Element Card from UI.Components;
    
    // 使用全限定名
    @Element Button from UI;
}
```

### 2.3 同名命名空间合并

```chtl
// 文件1.chtl
[Namespace] Common
{
    [Template] @Style TextStyle { color: black; }
}

// 文件2.chtl  
[Namespace] Common
{
    [Template] @Style ButtonStyle { background: blue; }
}

// 合并后，Common命名空间包含 TextStyle 和 ButtonStyle
```

## 3. 模块系统示例

### 3.1 Cmod模块结构

```
MyModule/
  src/
    MyModule.chtl      # 主模块文件
    Button.chtl        # 子组件
    Card.chtl          # 子组件
  info/
    MyModule.chtl      # 模块信息文件
```

### 3.2 模块信息文件

```chtl
[Info]
{
    name = "MyModule";
    version = "2.1.0";
    description = "我的自定义模块";
    author = "开发者";
    license = "MIT";
    dependencies = "chtl::Chtholly@1.0.0";
    category = "ui";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
}

[Export]
{
    @Style ModuleTheme, ButtonStyle, CardStyle;
    @Element Button, Card, Modal;
    @Var Colors, Spacing;
}
```

### 3.3 CJmod模块结构

```
MyExtension/
  src/
    extension.h        # C++头文件
    extension.cpp      # C++实现文件
    utils.h           # 工具头文件
    utils.cpp         # 工具实现文件
  info/
    MyExtension.chtl   # 模块信息文件
```

### 3.4 CJmod接口实现

```cpp
// extension.h
extern "C" {
    bool chtl_js_init();
    void chtl_js_cleanup();
    void chtl_js_register_functions();
    
    // 自定义扩展函数
    std::string myCustomFunction(const std::string& input);
}

// extension.cpp
extern "C" {
    bool chtl_js_init() {
        // 初始化扩展模块
        return true;
    }
    
    void chtl_js_register_functions() {
        // 注册函数到CHTL JS运行时
        // registerFunction("myCustomFunction", myCustomFunction);
    }
    
    std::string myCustomFunction(const std::string& input) {
        // 实现自定义功能
        return "Extended: " + input;
    }
}
```

## 4. 编译器使用示例

### 4.1 基本编译

```cpp
#include "CHTLCompiler.h"

int main() {
    // 创建编译器配置
    chtl::CompilerConfig config;
    config.officialModulePath = "./official_modules";
    config.currentDirectory = "./project";
    config.enableImportSystem = true;
    config.enableNamespaceSystem = true;
    config.enableModuleSystem = true;
    
    // 创建编译器
    chtl::CHTLCompiler compiler(config);
    
    // 编译单个文件
    auto result = compiler.compileFile("input.chtl", "output.html");
    
    if (result.success) {
        std::cout << "编译成功！" << std::endl;
        std::cout << "导入的文件: " << result.importedFiles.size() << std::endl;
        std::cout << "使用的命名空间: " << result.usedNamespaces.size() << std::endl;
    } else {
        std::cout << "编译失败：" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error << std::endl;
        }
    }
    
    return 0;
}
```

### 4.2 模块操作

```cpp
// 安装模块
compiler.installModule("MyModule.cmod", "./modules");

// 打包Cmod
compiler.packCmod("./MyModule", "MyModule.cmod");

// 解包Cmod
compiler.unpackCmod("MyModule.cmod", "./extracted");

// 打包CJmod
compiler.packCJmod("./MyExtension", "MyExtension.cjmod");

// 列出已安装模块
auto modules = compiler.listInstalledModules();
for (const auto& module : modules) {
    std::cout << "模块: " << module.name << " v" << module.version << std::endl;
}
```

### 4.3 批量编译

```cpp
// 批量编译多个文件
std::vector<std::string> inputFiles = {
    "page1.chtl",
    "page2.chtl", 
    "page3.chtl"
};

auto batchResult = compiler.compileBatch(inputFiles, "./output");

std::cout << "编译统计:" << std::endl;
auto stats = compiler.getStatistics();
std::cout << "  编译文件数: " << stats.compiledFiles << std::endl;
std::cout << "  导入文件数: " << stats.importedFiles << std::endl;
std::cout << "  加载模块数: " << stats.loadedModules << std::endl;
std::cout << "  使用命名空间数: " << stats.namespacesUsed << std::endl;
std::cout << "  总编译时间: " << stats.totalCompileTime << "秒" << std::endl;
```

## 5. 高级功能示例

### 5.1 循环依赖检测

```chtl
// A.chtl
[Import] @Chtl from B;  // A导入B

// B.chtl  
[Import] @Chtl from A;  // B导入A - 循环依赖！

// 编译器会自动检测并报错：
// "Circular dependency detected: A.chtl -> B.chtl"
```

### 5.2 重复导入优化

```chtl
// 多个文件导入同一模块
[Import] @Chtl from Common;  // 第一次导入，解析文件
[Import] @Chtl from Common;  // 第二次导入，使用缓存

// 编译器会生成警告：
// "Using cached CHTL import: Common"
```

### 5.3 命名空间冲突处理

```chtl
// 文件1.chtl
[Namespace] UI
{
    [Custom] @Element Button { /* 实现1 */ }
}

// 文件2.chtl
[Namespace] UI  
{
    [Custom] @Element Button { /* 实现2 */ }  // 冲突！
}

// 编译器会报告冲突：
// "Namespace conflict in UI: Button (multiple definitions)"
```

## 6. 便利函数使用

### 6.1 快速编译

```cpp
// 编译单个文件
auto result = chtl::compiler_utils::compileFile("input.chtl", "output.html");

// 编译字符串
std::string sourceCode = "html { body { text { Hello World } } }";
auto result = chtl::compiler_utils::compileString(sourceCode, "test.chtl");
```

### 6.2 语法验证

```cpp
// 验证CHTL语法
std::vector<std::string> errors;
bool isValid = chtl::compiler_utils::validateSyntax(sourceCode, errors);

if (!isValid) {
    for (const auto& error : errors) {
        std::cout << "语法错误: " << error << std::endl;
    }
}
```

### 6.3 代码格式化

```cpp
// 格式化CHTL代码
std::string formatted = chtl::compiler_utils::formatCode(sourceCode);
std::cout << "格式化后的代码:\n" << formatted << std::endl;
```

## 7. 错误处理示例

### 7.1 导入错误

```chtl
// 错误：文件不存在
[Import] @Html from nonexistent.html as Test;
// 错误信息: "HTML file not found: nonexistent.html"

// 错误：指向目录
[Import] @Style from ./styles/ as Styles;
// 错误信息: "Import path cannot point to directory: ./styles/"

// 警告：缺少as语法
[Import] @Html from template.html;
// 警告信息: "HTML import without 'as' syntax skipped: template.html"
```

### 7.2 命名空间错误

```chtl
// 错误：命名空间不存在
@Element Button from NonExistentSpace;
// 错误信息: "Namespace not found: NonExistentSpace"

// 错误：项目不存在
@Element NonExistentButton from UI;
// 错误信息: "Item not found: NonExistentButton"
```

### 7.3 模块错误

```chtl
// 错误：模块不存在
[Import] @Chtl from NonExistentModule;
// 错误信息: "CHTL file not found: NonExistentModule"

// 错误：CJmod不存在
[Import] @CJmod from NonExistentExtension;
// 错误信息: "CJmod file not found: NonExistentExtension"
```

## 8. 最佳实践

### 8.1 项目结构建议

```
MyProject/
  src/
    main.chtl          # 主文件
    components/        # 组件目录
      Button.chtl
      Card.chtl
    styles/           # 样式目录
      theme.css
      layout.css
    scripts/          # 脚本目录
      utils.js
      main.js
  module/             # 本地模块目录
    CustomModule.cmod
    Extension.cjmod
  templates/          # 模板目录
    header.html
    footer.html
```

### 8.2 导入组织建议

```chtl
// 在文件顶部组织所有导入
[Import] @Style from ./styles/theme.css as Theme;
[Import] @JavaScript from ./scripts/utils.js as Utils;
[Import] @Chtl from chtl::Chtholly;
[Import] @Chtl from ./components.*;

[Namespace] MyApp
{
    // 定义应用特定的组件
}

// 主要内容
html { ... }
```

### 8.3 命名空间组织建议

```chtl
[Namespace] MyApp
{
    [Namespace] UI
    {
        // UI组件
        [Custom] @Element Button { ... }
        [Custom] @Element Card { ... }
    }
    
    [Namespace] Utils
    {
        // 工具组件
        [Template] @Var Helpers { ... }
        [Template] @Style Utilities { ... }
    }
}
```

## 9. 性能优化技巧

### 9.1 导入优化

```chtl
// 好的做法：使用具体路径
[Import] @Chtl from ./components/Button.chtl;

// 避免：过度使用通配符
[Import] @Chtl from ./*;  // 会导入太多不需要的文件
```

### 9.2 缓存利用

```cpp
// 清理缓存（在需要时）
compiler.clearImportCache();
compiler.clearModuleCache();

// 查看缓存使用情况
std::cout << "导入缓存大小: " << compiler.getImportCacheSize() << std::endl;
std::cout << "模块缓存大小: " << compiler.getModuleCacheSize() << std::endl;
```

### 9.3 编译配置优化

```cpp
chtl::CompilerConfig config;
config.strictMode = true;        // 启用严格模式
config.debugMode = false;        // 生产环境关闭调试
config.generateSourceMap = true; // 生成源码映射
```

## 10. 总结

CHTL编译器的增强功能提供了：

1. **完整的导入系统** - 支持所有类型的导入和路径解析
2. **强大的命名空间管理** - 自动合并、冲突检测、递归解析
3. **标准化的模块系统** - Cmod和CJmod的完整生命周期管理
4. **智能的依赖管理** - 循环依赖检测、重复导入缓存
5. **开发者友好的API** - 简单易用的编译器接口

所有功能都严格按照CHTL语法文档实现，确保了语法的一致性和规范性。