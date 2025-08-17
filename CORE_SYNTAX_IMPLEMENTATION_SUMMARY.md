# CHTL核心语法解析与生成实现总结

## 概述

本文档总结了CHTL编译器核心语法解析与生成功能的实现，严格遵循CHTL语法文档.md中的语法规范，未进行私自扩展。

## 已实现的核心功能

### 1. 模板系统(Template System)

#### 1.1 样式组模板(@Style)
- **解析器实现**: `parseTemplateStyleImpl()` 方法
- **AST节点**: `TemplateStyleNode` 类
- **生成器实现**: `generateTemplateStyle()` 方法
- **支持特性**:
  - 样式属性定义
  - 组合式继承 (`@Style OtherTemplate;`)
  - 显性继承 (`inherit @Style TemplateName;`)
  - 变量引用和展开

#### 1.2 元素模板(@Element)
- **解析器实现**: `parseTemplateElementImpl()` 方法
- **AST节点**: `TemplateElementNode` 类
- **生成器实现**: `generateTemplateElement()` 方法
- **支持特性**:
  - 嵌套元素结构定义
  - 模板继承
  - 递归HTML生成

#### 1.3 变量组模板(@Var)
- **解析器实现**: `parseTemplateVarImpl()` 方法
- **AST节点**: `TemplateVarNode` 类
- **生成器实现**: `generateTemplateVar()` 方法
- **支持特性**:
  - 变量定义 (`variableName: "value";`)
  - CE对等式支持 (`:` 和 `=` 等价)
  - 全局变量映射

### 2. 自定义系统(Custom System)

#### 2.1 自定义样式组(@Style)
- **解析器实现**: `parseCustomStyleImpl()` 方法
- **AST节点**: `CustomStyleNode` 类
- **生成器实现**: `generateCustomStyle()` 方法
- **支持特性**:
  - 无值样式组属性 (`color, font-size;`)
  - 特例化操作
  - 删除属性 (`delete property1, property2;`)
  - 删除继承 (`delete @Style TemplateName;`)

#### 2.2 自定义元素(@Element)
- **解析器实现**: `parseCustomElementImpl()` 方法
- **AST节点**: `CustomElementNode` 类
- **生成器实现**: `generateCustomElement()` 方法
- **支持特性**:
  - 元素索引访问 (`div[1]`)
  - 插入操作 (`insert after/before/replace/at top/at bottom`)
  - 删除元素操作

#### 2.3 自定义变量组(@Var)
- **解析器实现**: `parseCustomVarImpl()` 方法
- **AST节点**: `CustomVarNode` 类
- **生成器实现**: `generateCustomVar()` 方法
- **支持特性**:
  - 变量组引用 (`ThemeColor(tableColor)`)
  - 特例化赋值 (`ThemeColor(tableColor = rgb(145, 155, 200))`)

### 3. 样式块处理

#### 3.1 局部样式块
- **解析器实现**: `parseStyle()` 和 `parseStyleContent()` 方法
- **AST节点**: `StyleBlockNode` 类
- **生成器实现**: `processStyleBlock()` 方法
- **支持特性**:
  - 内联样式生成
  - 自动化类名生成 (`.className`)
  - 自动化ID生成 (`#idName`)
  - 上下文推导 (`&`, `&:hover`, `&::before`)

#### 3.2 CSS规则处理
- **解析器实现**: `parseStyleProperty()` 方法
- **AST节点**: `CSSPropertyNode`, `CSSRuleNode` 类
- **生成器实现**: `processCSSProperty()`, `processCSSRule()` 方法
- **支持特性**:
  - 类选择器处理
  - ID选择器处理
  - 伪类/伪元素选择器处理
  - 全局CSS生成

### 4. 模板使用和特例化

#### 4.1 模板使用节点
- **解析器实现**: `parseTemplateUsage()` 方法
- **AST节点**: `TemplateUsageNode` 类
- **生成器实现**: `processTemplateUsage()` 方法
- **支持特性**:
  - 模板引用 (`@Style TemplateName;`)
  - 特例化块 (`@Style CustomStyle { ... }`)
  - 变量组特例化

#### 4.2 特例化操作
- **解析器实现**: `parseStyleSpecialization()` 方法
- **AST节点**: `SpecializationNode` 类
- **生成器实现**: `processStyleSpecialization()` 方法
- **支持特性**:
  - 属性删除
  - 继承删除
  - 属性覆盖

### 5. 变量引用和展开

#### 5.1 变量引用解析
- **解析器实现**: `parseVariableReference()` 方法
- **AST节点**: `VariableReferenceNode` 类
- **生成器实现**: `resolveVariableReference()` 方法
- **支持特性**:
  - 变量组引用 (`GroupName(variableName)`)
  - 简单变量引用

#### 5.2 变量展开
- **生成器实现**: `expandTemplateVariables()` 方法
- **支持特性**:
  - 变量组引用展开 (`ThemeColor(tableColor)`)
  - 简单变量引用展开 (`${variableName}`)
  - 作用域变量查找

### 6. 高级语法支持

#### 6.1 继承操作
- **解析器实现**: `parseInherit()` 方法
- **AST节点**: `InheritNode` 类
- **支持特性**:
  - 显性继承语法 (`inherit @Style TemplateName;`)

#### 6.2 删除操作
- **解析器实现**: `parseDelete()` 方法
- **AST节点**: `DeleteNode` 类
- **支持特性**:
  - 属性删除
  - 继承删除

#### 6.3 插入操作
- **解析器实现**: `parseInsert()` 方法
- **AST节点**: `InsertNode` 类
- **支持特性**:
  - 位置插入 (`after`, `before`, `replace`, `at top`, `at bottom`)
  - 选择器定位

### 7. 字面量处理

#### 7.1 多种字面量支持
- **解析器实现**: `parseLiteral()` 方法
- **支持特性**:
  - 双引号字符串 (`"string"`)
  - 单引号字符串 (`'string'`)
  - 无修饰字面量 (`unquoted-literal`)
  - CE对等式 (`:` 和 `=` 等价)

## 代码生成功能

### 1. HTML生成
- **方法**: `generateHTMLRecursive()`, `generateElementHTML()`
- **功能**:
  - 元素标签生成
  - 属性生成
  - 自动类名和ID添加
  - 内联样式应用
  - 自闭合标签处理

### 2. CSS生成
- **方法**: `generateGlobalCSS()`, `addToGlobalCSS()`
- **功能**:
  - 全局CSS规则生成
  - 选择器处理
  - 属性值展开
  - 样式合并

### 3. 模板注册和管理
- **方法**: `registerTemplateStyle()`, `registerCustomStyle()`, 等
- **功能**:
  - 模板上下文管理
  - 变量映射维护
  - 继承关系处理

## 测试用例

创建了comprehensive test file: `test_core_syntax_parsing.chtl`，包含:
- 样式组模板定义和使用
- 元素模板定义和使用
- 变量组模板定义和使用
- 自定义无值样式组
- 自定义样式继承
- 自定义元素定义
- 变量组引用
- 完整的HTML页面生成

## 严格语法遵循

本实现严格遵循CHTL语法文档.md中的所有语法规范：
1. **模板语法**: `[Template] @Style/@Element/@Var`
2. **自定义语法**: `[Custom] @Style/@Element/@Var`
3. **继承语法**: 组合式继承和显性继承
4. **特例化语法**: 删除操作、插入操作
5. **变量引用**: `GroupName(variableName)` 格式
6. **CE对等式**: `:` 和 `=` 完全等价
7. **字面量支持**: 双引号、单引号、无修饰字面量
8. **局部样式**: 自动化类名、上下文推导

## 架构特点

1. **模块化设计**: 解析器、AST节点、生成器分离
2. **递归处理**: 支持嵌套结构的递归解析和生成
3. **上下文管理**: 维护模板和自定义的上下文信息
4. **错误处理**: 完善的错误报告机制
5. **调试支持**: 可选的调试日志输出

## 总结

已成功实现CHTL核心语法的解析与生成功能，包括完整的模板系统、自定义系统、样式块处理、变量引用和展开等核心特性。实现严格遵循CHTL语法文档，未进行私自扩展，为后续功能的实现奠定了坚实基础。