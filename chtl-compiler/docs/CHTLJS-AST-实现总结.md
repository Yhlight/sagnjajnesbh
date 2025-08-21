# CHTL JS AST节点实现总结

## 概述

本文档总结了CHTL JS AST节点系统的实现，严格按照CHTL语法文档中关于CHTL JS的部分进行设计和开发。

## 重要原则

1. **CHTL JS不是JavaScript** - CHTL JS是CHTL的扩展语法，有自己独特的语法和功能
2. **完全独立于CHTL** - CHTL JS AST系统完全独立于CHTL AST，不共享任何组件
3. **JS由专门的JS编译器处理** - CHTL JS不负责处理JavaScript内容，纯JS代码由JavaScript编译器解析

## 实现的AST节点类型

### 1. 基础节点

- **CHTLJSDocumentNode** - CHTL JS文档根节点
- **ScriptBlockNode** - script { } 块节点（CHTL JS视角）

### 2. 增强选择器节点

- **EnhancedSelectorNode** - {{selector}} 语法
  - 支持自动判断（{{box}}）
  - 类选择器（{{.box}}）
  - ID选择器（{{#box}}）
  - 标签选择器（{{button}}）
  - 后代选择器（{{.box button}}）
  - 索引访问（{{button[0]}}）
- **SelectorAccessNode** - 选择器的索引访问

### 3. CHTL JS操作符

- **ArrowOperatorNode** - -> 操作符（推荐使用，明确CHTL JS语法）
- **DotOperatorNode** - . 操作符（与->功能相同）

### 4. CHTL JS特殊函数

- **ListenCallNode** - listen({ }) 增强监听器
- **DelegateCallNode** - delegate({ }) 事件委托
- **AnimateCallNode** - animate({ }) 动画函数
- **INeverAwayCallNode** - iNeverAway({ }) 创建标记函数组
- **PrintMyLoveCallNode** - printMylove({ }) 图片转字符功能

### 5. 虚对象系统

- **VirDeclarationNode** - vir name = ... 虚对象声明
- **VirAccessNode** - virObject->method() 虚对象访问

### 6. 状态标签系统

- **StateTagNode** - <State> 状态标签
- **FunctionWithStateNode** - functionName<State> 带状态的函数

### 7. 数据结构节点

- **ObjectLiteralNode** - { key: value } 对象字面量
- **PropertyNode** - 对象属性节点
- **ArrayLiteralNode** - [item1, item2] 数组字面量

### 8. 函数相关节点

- **FunctionDeclarationNode** - function() { }
- **ArrowFunctionNode** - () => { }
- **ParameterListNode** - 参数列表
- **BlockStatementNode** - { } 语句块
- **ExpressionStatementNode** - 表达式语句

### 9. 字面量节点

- **IdentifierNode** - 标识符
- **StringLiteralNode** - 字符串字面量
- **NumberLiteralNode** - 数字字面量
- **BooleanLiteralNode** - 布尔字面量

### 10. JavaScript片段

- **JSCodeFragmentNode** - 纯JavaScript代码片段（交给JS编译器处理）

## 关键设计特点

### 1. 增强选择器的智能解析

```cpp
// 自动识别选择器类型
{{box}}      // 自动判断是标签/类/ID
{{.box}}     // 明确的类选择器
{{#box}}     // 明确的ID选择器
{{button[0]}} // 索引选择器
```

### 2. 虚对象的编译时处理

虚对象完全是编译时的语法糖：
- vir声明时记录函数键
- 使用时替换为全局函数引用
- 不创建实际的JavaScript对象

### 3. 状态标签支持函数重载

通过状态标签实现同名函数的区分：
```cpp
Void<A>: function(int, int) { }
Void<B>: function(int, int) { }
```

### 4. 完整的配置对象支持

为各种CHTL JS函数提供类型化的配置对象，便于编译器验证和优化。

## 文件结构

```
include/chtl_js/ast/
├── CHTLJSASTNode.h          # 基类和访问者接口
├── CHTLJSDocumentNode.h     # 文档节点
├── ScriptBlockNode.h        # Script块节点
├── EnhancedSelectorNode.h   # 增强选择器
├── OperatorNodes.h          # 操作符节点
├── SpecialFunctionNodes.h   # CHTL JS特殊函数
├── VirNodes.h               # 虚对象相关
├── StateTagNode.h           # 状态标签
├── ObjectLiteralNode.h      # 对象字面量
├── LiteralNodes.h           # 字面量节点
├── FunctionNodes.h          # 函数相关节点
├── JSCodeFragmentNode.h     # JS代码片段
└── CHTLJSASTBuilder.h       # AST构建器

src/chtl_js/ast/
├── CHTLJSASTNode.cpp
└── CHTLJSNodeImplementations.cpp
```

## 与CHTL AST的区别

1. **完全独立的命名空间** - `chtl::chtljs::ast` vs `chtl::ast`
2. **独立的节点类型枚举** - `CHTLJSNodeType` vs `ASTNodeType`
3. **专门的访问者接口** - `CHTLJSVisitor` vs `ASTVisitor`
4. **不同的语法特性** - 增强选择器、虚对象等CHTL JS独有

## 测试验证

`tests/TestCHTLJSAST.cpp` 包含完整的测试用例：
- 基础节点测试
- 增强选择器测试
- CHTL JS特殊函数测试
- 虚对象系统测试
- 状态标签测试
- 完整结构测试

## 锚点记忆

- **CHTL JS AST已完成** - 所有CHTL JS语法元素都有对应的AST节点
- **严格遵循语法文档** - 没有私自添加或修改功能
- **完全独立架构** - 与CHTL AST完全分离，独立的文件系统
- **CHTL JS不是JS** - CHTL JS是独特的扩展语法，不是JavaScript的超集

## 下一步工作

1. 实现CHTL解析器（TODO 8）
2. 实现CHTL JS解析器（TODO 9）
3. 将AST转换为可执行的代码