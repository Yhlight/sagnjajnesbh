# 增强的Import功能

## 概述

本项目实现了一个增强的Import功能，完全按照用户提供的规则处理各类导入语法，修正了同一路径多表达方式、循环依赖及重复导入问题。系统严格遵循CHTL语法规范，支持所有标准的导入类型和高级特性。

## 核心特性

### 1. 完整的导入类型支持
- **@Html导入**：HTML文件导入，支持带名原始嵌入
- **@Style导入**：CSS样式文件导入
- **@JavaScript导入**：JavaScript脚本文件导入
- **@Chtl导入**：CHTL模块文件导入，支持.cmod和.chtl文件
- **@CJmod导入**：CJmod模块文件导入
- **模板导入**：[Template] @Style/@Element/@Var 导入
- **自定义导入**：[Custom] @Style/@Element/@Var 导入

### 2. 智能路径解析
- **路径类型识别**：自动识别名称、带扩展名、具体路径、目录路径、通配符路径
- **搜索策略**：官方模块目录 → 当前目录module文件夹 → 当前目录
- **路径转换**：支持'.'替代'/'作为路径分隔符
- **通配符支持**：支持 `*`、`*.ext`、`path/*` 等通配符模式

### 3. 循环依赖检测
- **实时检测**：导入时实时检测循环依赖
- **依赖图管理**：维护完整的文件依赖关系图
- **循环链追踪**：提供详细的循环依赖链信息
- **拓扑排序**：支持依赖关系的拓扑排序

### 4. 重复导入处理
- **导入缓存**：避免重复处理相同的导入
- **规范化路径**：统一路径表示，正确识别相同文件
- **别名区分**：相同文件不同别名视为不同导入
- **导入统计**：提供详细的导入统计信息

## 导入规则详解

### @Html、@Style、@JavaScript导入规则

#### 基本语法
```chtl
[Import] @Html from path as alias
[Import] @Style from path as alias  
[Import] @JavaScript from path as alias
```

#### 处理规则
1. **无`as`语法时**：直接跳过，不进行任何处理
2. **有`as`语法时**：创建对应类型的带名原始嵌入节点
3. **路径处理**：
   - **文件名（不带后缀）**：在编译文件所在目录按类型搜索相关文件
   - **具体文件名（带后缀）**：在编译文件所在目录直接搜索该文件
   - **路径为文件夹或不包含具体文件信息时**：触发报错

#### 示例
```chtl
// 正确用法
[Import] @Html from header as HeaderContent        // 搜索 header.html
[Import] @Style from main as MainStyles           // 搜索 main.css
[Import] @JavaScript from utils as UtilsScript    // 搜索 utils.js
[Import] @Html from header.html as HeaderHtml     // 直接搜索 header.html

// 错误用法
[Import] @Html from header                         // 无as语法，被跳过
[Import] @Html from ./components/ as Components    // 目录路径，触发报错
```

### @Chtl导入规则

#### 基本语法
```chtl
[Import] @Chtl from path [as alias]
```

#### 处理规则
1. **名称（不带后缀）**：
   - 优先搜索官方模块目录
   - 其次搜索当前目录module文件夹
   - 最后搜索当前目录
   - 均优先匹配.cmod文件

2. **具体名称（带后缀）**：
   - 按官方模块目录→当前目录module文件夹→当前目录顺序搜索指定文件

3. **具体路径（含文件信息）**：
   - 直接按路径查找，未找到则报错

4. **具体路径（不含文件信息）**：
   - 触发报错

#### 通配符支持
```chtl
[Import] @Chtl from 具体路径.*           // 导入路径下所有.cmod和.chtl文件
[Import] @Chtl from 具体路径.*.cmod      // 导入路径下所有.cmod文件  
[Import] @Chtl from 具体路径.*.chtl      // 导入路径下所有.chtl文件

// 等价语法
[Import] @Chtl from 具体路径/*           // 导入路径下所有.cmod和.chtl文件
[Import] @Chtl from 具体路径/*.cmod      // 导入路径下所有.cmod文件
[Import] @Chtl from 具体路径/*.chtl      // 导入路径下所有.chtl文件
```

#### 子模块导入
```chtl
[Import] @Chtl from Chtholly.*          // 导入Chtholly模块的所有子模块
[Import] @Chtl from Chtholly.Space      // 导入Chtholly模块中指定的Space子模块
```

#### 示例
```chtl
// 名称导入
[Import] @Chtl from UIComponents         // 搜索 UIComponents.cmod，然后 UIComponents.chtl

// 具体文件导入
[Import] @Chtl from UIComponents.cmod    // 搜索指定的 .cmod 文件
[Import] @Chtl from UIComponents.chtl    // 搜索指定的 .chtl 文件

// 路径导入
[Import] @Chtl from ./components/Button.chtl  // 直接按路径查找

// 通配符导入
[Import] @Chtl from components/*         // 导入components目录下所有模块文件
[Import] @Chtl from components/*.cmod    // 仅导入.cmod文件

// 子模块导入
[Import] @Chtl from UI.Components        // 等价于 UI/Components
[Import] @Chtl from UI.Components.*      // 导入UI/Components下所有子模块
```

### @CJmod导入规则

#### 基本语法
```chtl
[Import] @CJmod from path [as alias]
```

#### 处理规则
1. **名称（不带后缀）**：
   - 优先搜索官方模块目录
   - 其次搜索当前目录module文件夹
   - 最后搜索当前目录
   - 仅匹配.cjmod文件

2. **具体名称（带后缀）**：
   - 按官方模块目录→当前目录module文件夹→当前目录顺序搜索指定文件

3. **具体路径（含文件信息）**：
   - 直接按路径查找，未找到则报错

4. **具体路径（不含文件信息）**：
   - 触发报错

#### 示例
```chtl
// 名称导入
[Import] @CJmod from ThirdPartyLib       // 搜索 ThirdPartyLib.cjmod

// 具体文件导入  
[Import] @CJmod from ThirdPartyLib.cjmod // 搜索指定的 .cjmod 文件

// 路径导入
[Import] @CJmod from ./libs/MyLib.cjmod  // 直接按路径查找
```

## 高级特性

### 1. 路径搜索策略

#### 搜索顺序
1. **官方模块目录**：源码编译后生成的module文件夹
2. **当前目录module文件夹**：当前工作目录下的module文件夹
3. **当前目录**：当前工作目录

#### 文件优先级
- 对于CHTL导入：.cmod文件优先于.chtl文件
- 对于CJmod导入：仅匹配.cjmod文件
- 找到文件后立即返回，不继续搜索

### 2. 循环依赖检测

#### 检测机制
```cpp
// 检测循环依赖
bool has_cycle = import_manager.detectCircularDependency("A.chtl", "B.chtl");

// 获取循环依赖链
auto cycle_chain = import_manager.getCircularDependencyChain("A.chtl");
```

#### 依赖图管理
```cpp
// 添加依赖关系
import_manager.addDependency("main.chtl", "components.chtl");

// 获取拓扑排序
auto topological_order = import_manager.getTopologicalOrder();
```

### 3. 重复导入处理

#### 重复检测
- 基于规范化路径识别相同文件
- 相同文件相同别名视为重复导入
- 相同文件不同别名视为不同导入

#### 导入缓存
```cpp
// 检查是否已导入
bool is_duplicate = import_manager.isDuplicateImport(import_info);

// 标记为已导入
import_manager.markAsImported(import_info);

// 获取缓存大小
size_t cache_size = import_manager.getImportCacheSize();
```

### 4. 错误处理和诊断

#### 错误类型
- 文件未找到错误
- 循环依赖错误
- 路径格式错误
- 导入语法错误

#### 警告类型
- 无as语法的HTML/Style/JavaScript导入警告
- 重复导入警告

#### 统计信息
```cpp
// 获取导入统计
std::string stats = import_manager.getImportStatistics();

// 获取错误和警告
auto errors = import_manager.getImportErrors();
auto warnings = import_manager.getImportWarnings();
```

## 使用示例

### 基础导入使用
```cpp
#include "ImportManager.h"

// 创建ImportManager
ImportManager import_manager(compiler_context);

// HTML导入
auto html_imports = import_manager.processHtmlImport("header", "HeaderContent");

// CHTL导入
auto chtl_imports = import_manager.processChtlImport("UIComponents", "");

// 检查循环依赖
bool has_cycle = import_manager.detectCircularDependency("A.chtl", "B.chtl");
```

### 完整CHTL文件示例
```chtl
// HTML资源导入
[Import] @Html from header as HeaderContent
[Import] @Html from footer.html as FooterContent

// CSS样式导入
[Import] @Style from main as MainStyles
[Import] @Style from components.css as ComponentStyles

// JavaScript脚本导入
[Import] @JavaScript from utils as UtilsScript
[Import] @JavaScript from app.js as AppScript

// CHTL模块导入
[Import] @Chtl from UIComponents
[Import] @Chtl from components/*
[Import] @Chtl from Chtholly.Space

// CJmod模块导入
[Import] @CJmod from ThirdPartyLib

// 模板和自定义导入
[Import] [Template] @Style ButtonStyles from ui.templates
[Import] [Custom] @Element AdvancedButton from ui.custom as MyButton

html
{
    head
    {
        [Origin] @Style MainStyles;
        [Origin] @Style ComponentStyles;
    }
    
    body
    {
        [Origin] @Html HeaderContent;
        
        div
        {
            class: content;
            @Element MyButton;
        }
        
        [Origin] @Html FooterContent;
        
        script
        {
            [Origin] @JavaScript UtilsScript;
            [Origin] @JavaScript AppScript;
        }
    }
}
```

## API参考

### ImportManager核心方法

```cpp
class ImportManager {
public:
    // 处理各类导入
    std::vector<ImportInfo> processHtmlImport(const std::string& path, const std::string& alias);
    std::vector<ImportInfo> processStyleImport(const std::string& path, const std::string& alias);
    std::vector<ImportInfo> processJavaScriptImport(const std::string& path, const std::string& alias);
    std::vector<ImportInfo> processChtlImport(const std::string& path, const std::string& alias);
    std::vector<ImportInfo> processCJmodImport(const std::string& path, const std::string& alias);
    
    // 循环依赖检测
    bool detectCircularDependency(const std::string& current_file, const std::string& target_file);
    std::vector<std::string> getCircularDependencyChain(const std::string& file_path) const;
    
    // 重复导入检测
    bool isDuplicateImport(const ImportInfo& import_info) const;
    void markAsImported(const ImportInfo& import_info);
    
    // 统计和诊断
    std::string getImportStatistics() const;
    std::vector<std::string> getImportErrors() const;
    std::vector<std::string> getImportWarnings() const;
};
```

### ImportInfo结构

```cpp
struct ImportInfo {
    ImportType type;                    // 导入类型
    std::string original_path;          // 原始路径
    std::string resolved_path;          // 解析后的绝对路径
    std::string alias_name;             // as 后的别名
    PathType path_type;                 // 路径类型
    bool has_alias;                     // 是否有别名
    std::vector<std::string> resolved_files; // 通配符匹配的文件列表
};
```

## 性能特性

- **高效路径解析**：智能缓存和规范化路径处理
- **循环依赖优化**：使用DFS算法，时间复杂度O(V+E)
- **内存管理**：自动清理过期缓存数据
- **并发安全**：支持多线程环境下的安全使用

## 错误处理

系统提供完善的错误处理机制：
- 详细的错误消息和位置信息
- 分类的警告和错误类型
- 错误恢复和继续处理能力
- 完整的诊断和调试信息

## 总结

增强的Import功能为CHTL编译器提供了：

1. **完整的导入支持**：支持所有标准导入类型和语法
2. **智能路径解析**：多策略搜索和通配符支持
3. **循环依赖检测**：实时检测和详细报告
4. **重复导入处理**：避免重复处理和资源浪费
5. **错误处理机制**：完善的错误报告和恢复
6. **高性能实现**：优化的算法和内存管理

该系统严格遵循用户提供的导入规则，确保了CHTL导入功能的正确性和可靠性。