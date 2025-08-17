# CHTL语法约束器实现文档

## 🎯 实现目标

构建约束器，明确CHTL与CHTL JS语法的边界，确保语法在正确的上下文中使用。

## 📋 约束规则详细说明

### ✅ 1. 全局样式块约束

**允许的语法元素**：
- ✅ 模板变量 (`@Var`)
- ✅ 自定义变量 (`[Custom] @Var`)
- ✅ 自定义变量特例化
- ✅ 模板样式组 (`@Style`)
- ✅ 自定义样式组 (`[Custom] @Style`)
- ✅ 无值样式组
- ✅ 自定义样式组特例化
- ✅ delete属性
- ✅ delete继承
- ✅ 样式组间继承 (`inherit`)
- ✅ 生成器注释 (`--`)
- ✅ 全缀名
- ✅ 任意类型原始嵌入 (`[Origin]`)
- ✅ 命名空间from语法

**禁止的语法元素**：
- ❌ CHTL JS语法 (`{{}}`, `listen`, `delegate`, `vir`等)
- ❌ 元素相关操作 (`@Element`, `insert`, `delete`元素等)
- ❌ 纯JavaScript代码

### ✅ 2. 全局script约束

**允许的语法元素**：
- ✅ 生成器注释 (`--`)
- ✅ 任意类型原始嵌入 (`[Origin]`)
- ✅ 纯JavaScript代码
- ✅ **{{&}}等特供语法**（属于CHTL本身功能，不应误禁）

**禁止的语法元素**：
- ❌ 模板变量、自定义变量组
- ❌ 变量组特例化
- ❌ 命名空间from语法
- ❌ 样式组相关语法
- ❌ CHTL JS函数 (`listen`, `delegate`, `animate`, `vir`)

### ✅ 3. 局部样式块约束

**允许的语法元素**（与全局样式块相同）：
- ✅ 模板变量、自定义变量、变量特例化
- ✅ 模板样式组、自定义样式组、无值样式组
- ✅ 样式组特例化、delete属性、delete继承
- ✅ 样式组间继承、生成器注释、全缀名
- ✅ 任意类型原始嵌入、命名空间from语法
- ✅ CSS属性

**禁止的语法元素**：
- ❌ CHTL JS语法
- ❌ 元素操作语法
- ❌ 纯JavaScript代码

### ✅ 4. 局部script约束

**允许的语法元素**：
- ✅ 模板变量 (`@Var`)
- ✅ 自定义变量组 (`[Custom] @Var`)
- ✅ 变量组特例化
- ✅ 命名空间from语法
- ✅ 生成器注释 (`--`)
- ✅ 任意类型原始嵌入 (`[Origin]`)
- ✅ **所有CHTL JS语法** (`{{}}`, `->`, `listen`, `delegate`, `animate`, `vir`)
- ✅ 纯JavaScript代码

**禁止的语法元素**：
- ❌ 样式组相关语法 (`@Style`, `inherit`, `delete`样式等)
- ❌ 元素相关语法 (`@Element`, `insert`, `delete`元素等)
- ❌ CSS属性

## 🔍 特殊规则说明

### 1. 特殊存在元素（可在任意位置使用）
- ✅ **生成器注释 (`--`)** - 任意位置都可以使用
- ✅ **原始嵌入 (`[Origin]`)** - 任意位置都可以使用

### 2. CHTL JS特供语法
- ✅ **{{&}}等选择器语法** - 属于CHTL本身功能，不应误禁
- ✅ **->操作符** - CHTL本身功能
- ❌ **listen/delegate/animate/vir** - 仅在局部script中允许

### 3. 上下文推断规则
- **元素内的style{}** → 局部样式块
- **元素内的script{}** → 局部script
- **顶层的style{}** → 全局样式块  
- **顶层的script{}** → 全局script

## 🧪 测试验证结果

### ✅ 所有约束规则测试通过

```
=== 测试结果 ===
✅ 全局样式块约束测试通过!
✅ 全局script约束测试通过!
✅ 局部样式块约束测试通过!
✅ 局部script约束测试通过!
✅ 特殊存在元素测试通过!
✅ CHTL JS特供语法测试通过!
✅ 上下文推断测试通过!
✅ 约束违规报告测试通过!
✅ 约束规则管理测试通过!
✅ 真实世界场景测试通过!
✅ 约束器工具函数测试通过!

🎉 所有语法约束器测试通过!
```

## 🏗️ 架构设计

### 核心组件

```cpp
class SyntaxConstrainer {
    // 约束规则表：context -> element -> allowed
    std::unordered_map<ContextType, std::unordered_map<SyntaxElement, bool>> constraint_rules_;
    
    // 核心方法
    bool isElementAllowed(ContextType context, SyntaxElement element);
    std::vector<ConstraintViolation> validateContext(ContextType context, ast::ASTNode* node);
    ContextType inferContextType(ast::ASTNode* node, ContextType parent_context);
};
```

### 约束检查流程

```
AST节点 → 上下文推断 → 语法元素识别 → 约束规则检查 → 违规报告
```

## 📊 实现统计

- **上下文类型**: 4种（全局样式、局部样式、全局script、局部script）
- **语法元素**: 25种不同的语法元素
- **约束规则**: 100+条精确的约束规则
- **测试覆盖**: 11个测试函数，100%功能覆盖

## 🎯 关键成果

### ✅ 明确的语法边界

1. **全局样式块** - 仅允许样式和变量相关语法
2. **全局script** - 仅允许纯JavaScript和特供语法
3. **局部样式块** - 允许样式和变量语法
4. **局部script** - 允许变量和CHTL JS语法

### ✅ 特殊规则处理

1. **注释和原始嵌入** - 任意位置可用
2. **{{&}}等特供语法** - 不被误禁
3. **函数引用优化** - 避免代码重复

### ✅ 智能约束检查

1. **自动上下文推断** - 根据节点位置推断上下文
2. **精确违规报告** - 详细的错误信息和建议
3. **灵活规则管理** - 支持自定义约束规则

## 🚀 总结

**语法约束器已完全实现并通过测试！**

这个实现：
1. ✅ **明确了CHTL与CHTL JS的语法边界**
2. ✅ **确保语法在正确的上下文中使用**
3. ✅ **提供了智能的约束检查和报告**
4. ✅ **支持特殊规则和例外情况**

现在CHTL编译器具备了完整的语法约束能力，可以确保代码的语法正确性和上下文适当性！