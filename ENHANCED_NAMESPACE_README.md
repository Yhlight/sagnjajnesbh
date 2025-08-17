# CHTL 增强命名空间功能文档

## 概述

CHTL编译器现已实现增强的命名空间功能，包括：
1. **同名命名空间自动合并机制**
2. **集成命名空间冲突检测策略**

这些功能确保了命名空间的灵活性和安全性，允许开发者在不同文件或代码段中定义同名命名空间，同时提供完善的冲突检测和报告机制。

## 功能特性

### 1. 同名命名空间自动合并机制

#### 基本原理
- 当进入一个已存在的命名空间时，系统会自动合并到现有的命名空间中
- 新添加的符号会被添加到现有命名空间的符号表中
- 支持多层嵌套命名空间的合并

#### 使用方法

```cpp
GlobalMap globalMap;

// 第一次定义命名空间
globalMap.enterNamespace("MyNamespace");
globalMap.addSymbol("symbol1", SymbolType::CUSTOM_ELEMENT, "value1");
globalMap.exitNamespace();

// 第二次进入同名命名空间 - 自动合并
globalMap.enterNamespace("MyNamespace");  // 或使用 mergeNamespace("MyNamespace")
globalMap.addSymbol("symbol2", SymbolType::CUSTOM_STYLE, "value2");
globalMap.exitNamespace();

// 现在两个符号都在同一个命名空间中
```

#### API接口

```cpp
class GlobalMap {
public:
    // 传统方式 - 自动支持合并
    void enterNamespace(const std::string& namespace_name);
    
    // 显式合并方式 - 返回是否合并了现有命名空间
    bool mergeNamespace(const std::string& namespace_name);
    
    // 获取命名空间层次结构
    std::vector<std::string> getNamespaceHierarchy() const;
};
```

### 2. 集成命名空间冲突检测策略

#### 冲突类型

1. **类型冲突**: 同名符号具有不同的类型
2. **值冲突**: 同名同类型符号具有不同的值
3. **作用域遮蔽**: 内层作用域的符号遮蔽外层作用域的同名符号
4. **命名空间与符号冲突**: 命名空间名称与现有符号冲突

#### 冲突检测API

```cpp
struct ConflictInfo {
    std::shared_ptr<Symbol> existing_symbol;
    std::shared_ptr<Symbol> new_symbol;
    std::string conflict_type;
    std::string suggestion;
};

class GlobalMap {
public:
    // 基础冲突检测
    bool hasConflict(const std::string& name, SymbolType type);
    std::vector<std::shared_ptr<Symbol>> getConflicts(const std::string& name);
    
    // 增强冲突检测
    std::vector<ConflictInfo> detectConflicts(const std::string& name, 
                                             SymbolType type, 
                                             const std::string& value = "");
    
    // 命名空间特定冲突检测
    bool hasNamespaceConflict(const std::string& namespace_path);
    std::vector<ConflictInfo> getNamespaceConflicts(const std::string& namespace_path);
};
```

#### 冲突处理策略

1. **命名空间合并**: 同名同类型的命名空间自动合并
2. **警告机制**: 其他类型冲突会产生警告但不阻止编译
3. **建议提供**: 每个冲突都包含解决建议

## 实现细节

### 作用域管理

```cpp
class Scope {
public:
    // 查找现有子作用域
    Scope* findChildScope(const std::string& scope_name);
    
    // 查找或创建子作用域（支持合并）
    Scope* findOrCreateChildScope(const std::string& scope_name);
    
    // 传统创建方式（总是创建新的）
    Scope* createChildScope(const std::string& scope_name);
};
```

### 符号管理增强

- `addSymbol` 方法集成了冲突检测
- 自动处理命名空间符号的合并
- 提供详细的冲突报告和建议

## 使用示例

### 示例1: 基本命名空间合并

```cpp
#include "common/GlobalMap.h"

GlobalMap globalMap;

// 文件1中定义
globalMap.enterNamespace("UI");
globalMap.addSymbol("Button", SymbolType::CUSTOM_ELEMENT, "button_def");
globalMap.exitNamespace();

// 文件2中扩展同一命名空间
globalMap.mergeNamespace("UI");  // 返回true，表示合并了现有命名空间
globalMap.addSymbol("Input", SymbolType::CUSTOM_ELEMENT, "input_def");
globalMap.exitNamespace();

// 现在UI命名空间包含Button和Input两个符号
```

### 示例2: 嵌套命名空间合并

```cpp
// 第一次定义嵌套命名空间
globalMap.enterNamespace("Company");
globalMap.enterNamespace("UI");
globalMap.addSymbol("Theme", SymbolType::TEMPLATE_STYLE, "theme1");
globalMap.exitNamespace();
globalMap.exitNamespace();

// 第二次合并到相同的嵌套命名空间
globalMap.mergeNamespace("Company");
globalMap.mergeNamespace("UI");
globalMap.addSymbol("Colors", SymbolType::TEMPLATE_STYLE, "colors1");
globalMap.exitNamespace();
globalMap.exitNamespace();

// Company::UI 命名空间现在包含Theme和Colors
```

### 示例3: 冲突检测和处理

```cpp
// 添加符号
globalMap.addSymbol("MySymbol", SymbolType::CUSTOM_ELEMENT, "original_value");

// 检测潜在冲突
auto conflicts = globalMap.detectConflicts("MySymbol", SymbolType::CUSTOM_STYLE, "new_value");

for (const auto& conflict : conflicts) {
    std::cout << "冲突类型: " << conflict.conflict_type << std::endl;
    std::cout << "建议: " << conflict.suggestion << std::endl;
}
```

## 测试覆盖

项目包含完整的测试套件：

- `test/test_namespace_enhanced.cpp`: 专门的命名空间功能测试
- `test/test_basic.cpp`: 集成了基本的命名空间合并测试

### 运行测试

```bash
cd build
make test_namespace_enhanced
./bin/test_namespace_enhanced

make test_basic  
./bin/test_basic
```

## 最佳实践

1. **使用显式合并**: 优先使用 `mergeNamespace()` 而不是 `enterNamespace()` 来明确表达合并意图

2. **检查合并结果**: 
   ```cpp
   if (globalMap.mergeNamespace("MyNamespace")) {
       // 合并了现有命名空间
   } else {
       // 创建了新命名空间
   }
   ```

3. **处理冲突**: 在添加符号前检查冲突
   ```cpp
   auto conflicts = globalMap.detectConflicts(name, type, value);
   if (!conflicts.empty()) {
       // 处理冲突
   }
   ```

4. **命名空间层次**: 使用 `getNamespaceHierarchy()` 了解当前命名空间结构

## 性能考虑

- 命名空间合并通过复用现有作用域减少内存分配
- 冲突检测在符号添加时进行，避免后期批量检查
- 符号查找使用递归搜索，支持作用域继承

## 兼容性

- 新功能完全向后兼容
- 现有代码无需修改即可享受自动合并功能
- 所有现有API继续正常工作

## 未来扩展

- 支持命名空间别名
- 跨文件命名空间依赖分析
- 命名空间级别的访问控制
- 自动命名空间冲突解决建议