# CHTL局部样式块系统实现总结

## 概述

本文档总结了CHTL编译器第二部分todo的实现：局部样式块系统。严格遵循CHTL语法文档.md中的局部样式块语法规范，实现了内联样式、自动化类名/ID、上下文推导(&)、伪类伪元素选择器等核心功能。

## 已实现的局部样式块功能

### 1. 内联样式处理

#### 1.1 语法规范
```chtl
div
{
    style
    {
        width: 100px;
        height: 200px;
        background-color: lightblue;
    }
}
```

#### 1.2 实现机制
- **解析器**: `parseStyleContent()` 识别直接CSS属性
- **AST节点**: `CSSPropertyNode` 存储属性名和值
- **生成器**: `processCSSProperty()` 处理内联样式
- **输出**: 直接添加到元素的`style`属性中

#### 1.3 生成结果
```html
<div style="width: 100px; height: 200px; background-color: lightblue;">
```

### 2. 自动化类名生成

#### 2.1 语法规范
```chtl
div
{
    style
    {
        .box  // 自动添加类名box给元素
        {
            width: 300px;
            background-color: red;
        }
    }
}
```

#### 2.2 实现机制
- **解析器**: `parseClassSelector()` 解析`.className`语法
- **AST节点**: `ClassSelectorNode` 存储类名
- **生成器**: `processClassSelector()` 自动添加类名到元素
- **全局CSS**: 类选择器规则自动添加到全局样式块

#### 2.3 生成结果
```html
<div class="box">
```
```css
.box {
    width: 300px;
    background-color: red;
}
```

### 3. 自动化ID生成

#### 3.1 语法规范
```chtl
div
{
    style
    {
        #main-box  // 自动添加ID main-box给元素
        {
            width: 400px;
            text-align: center;
        }
    }
}
```

#### 3.2 实现机制
- **解析器**: `parseIdSelector()` 解析`#idName`语法
- **AST节点**: `IdSelectorNode` 存储ID名
- **生成器**: `processIdSelector()` 自动添加ID到元素
- **全局CSS**: ID选择器规则自动添加到全局样式块

#### 3.3 生成结果
```html
<div id="main-box">
```
```css
#main-box {
    width: 400px;
    text-align: center;
}
```

### 4. 上下文推导(&)

#### 4.1 语法规范
```chtl
div
{
    style
    {
        .hover-box  // 先定义类名
        {
            background-color: blue;
        }
        
        &:hover  // 使用&表示.hover-box
        {
            background-color: darkblue;
        }
        
        &::before  // 伪元素选择器
        {
            content: "→";
        }
    }
}
```

#### 4.2 实现机制
- **解析器**: `parsePseudoSelector()` 解析`&`开头的选择器
- **AST节点**: `PseudoSelectorNode` 存储伪选择器
- **生成器**: `resolveContextSelector()` 解析上下文推导
- **上下文推导规则**:
  - `&` → 当前类名或ID
  - `&:hover` → `.className:hover` 或 `#idName:hover`
  - `&::before` → `.className::before` 或 `#idName::before`

#### 4.3 生成结果
```css
.hover-box {
    background-color: blue;
}

.hover-box:hover {
    background-color: darkblue;
}

.hover-box::before {
    content: "→";
}
```

### 5. 伪类选择器支持

#### 5.1 支持的伪类
- `:hover` - 悬停状态
- `:active` - 激活状态
- `:focus` - 焦点状态
- `:visited` - 已访问状态
- `:first-child`, `:last-child` - 结构伪类
- 等等（完整支持CSS伪类）

#### 5.2 实现机制
- **语法解析**: 自动识别`:`后的伪类名
- **上下文推导**: 与`&`结合使用
- **CSS生成**: 正确生成伪类选择器规则

### 6. 伪元素选择器支持

#### 6.1 支持的伪元素
- `::before` - 元素前插入内容
- `::after` - 元素后插入内容
- `::first-line` - 首行样式
- `::first-letter` - 首字母样式
- 等等（完整支持CSS伪元素）

#### 6.2 实现机制
- **语法解析**: 自动识别`::`后的伪元素名
- **上下文推导**: 与`&`结合使用
- **CSS生成**: 正确生成伪元素选择器规则

### 7. 混合样式处理

#### 7.1 内联样式 + 类选择器
```chtl
div
{
    style
    {
        // 内联样式
        padding: 20px;
        margin: 10px;
        
        // 类选择器
        .content
        {
            background-color: white;
            border-radius: 8px;
        }
    }
}
```

#### 7.2 生成结果
```html
<div class="content" style="padding: 20px; margin: 10px;">
```
```css
.content {
    background-color: white;
    border-radius: 8px;
}
```

## 核心实现方法

### 1. 解析器方法

#### parseStyleContent()
- 识别样式块内容类型
- 路由到相应的解析方法
- 支持内联样式、选择器、模板使用

#### parseClassSelector()
- 解析`.className { ... }`语法
- 创建`ClassSelectorNode`
- 处理类选择器内的CSS属性

#### parseIdSelector()
- 解析`#idName { ... }`语法
- 创建`IdSelectorNode`
- 处理ID选择器内的CSS属性

#### parsePseudoSelector()
- 解析`&:hover`, `&::before`等语法
- 创建`PseudoSelectorNode`
- 支持复杂伪类和伪元素

#### parseElementSelector()
- 解析元素选择器语法
- 创建`CSSRuleNode`
- 处理元素选择器内的CSS属性

### 2. 生成器方法

#### processStyleBlock()
- 处理整个样式块
- 区分局部和全局样式
- 路由到相应的处理方法

#### processCSSRule()
- 处理CSS规则
- 自动化类名/ID添加
- 上下文推导解析

#### processClassSelector()
- 处理类选择器节点
- 自动添加类名到当前元素
- 生成全局CSS规则

#### processIdSelector()
- 处理ID选择器节点
- 自动添加ID到当前元素
- 生成全局CSS规则

#### processPseudoSelector()
- 处理伪选择器节点
- 上下文推导解析
- 生成伪类/伪元素CSS规则

#### resolveContextSelector()
- 解析上下文推导语法
- 将`&`替换为实际选择器
- 支持伪类和伪元素

### 3. 样式上下文管理

#### StyleContext结构
```cpp
struct StyleContext {
    StringMap inlineStyles;     // 内联样式
    StringMap classStyles;      // 类样式
    StringMap idStyles;         // ID样式
    StringMap pseudoStyles;     // 伪类样式
    StringSet autoClasses;      // 自动生成的类名
    StringSet autoIds;          // 自动生成的ID
    String currentSelector;     // 当前选择器
    bool inLocalStyle = false;  // 是否在局部样式块中
};
```

## 严格语法遵循

### 1. 内联样式语法
- **规范**: 直接在`style{}`内添加CSS属性
- **实现**: 完全符合，属性直接作为内联样式

### 2. 自动化类名语法
- **规范**: 使用`.className`自动添加类名
- **实现**: 完全符合，自动添加类名并生成全局CSS

### 3. 自动化ID语法
- **规范**: 使用`#idName`自动添加ID
- **实现**: 完全符合，自动添加ID并生成全局CSS

### 4. 上下文推导语法
- **规范**: 使用`&`表示当前类名/ID
- **实现**: 完全符合，正确解析并替换`&`

### 5. 伪类伪元素语法
- **规范**: `&:hover`, `&::before`等
- **实现**: 完全符合，支持所有CSS伪类和伪元素

## 测试验证

### 1. 测试文件
- **完整测试**: `test_local_style_blocks.chtl` - 覆盖所有功能
- **简化测试**: `test_simple_local_style.chtl` - 基础功能验证

### 2. 测试程序
- **完整测试**: `test_local_style_parser.cpp`
- **简化测试**: `test_simple_local_style.cpp`

### 3. 测试覆盖
- ✅ 内联样式处理
- ✅ 自动化类名生成
- ✅ 自动化ID生成
- ✅ 上下文推导(&)支持
- ✅ 伪类选择器(:hover, :active等)
- ✅ 伪元素选择器(::before, ::after等)
- ✅ 混合样式处理
- ✅ 全局CSS生成

## 输出示例

### 输入CHTL代码
```chtl
div
{
    style
    {
        width: 200px;
        
        .test-box
        {
            background-color: blue;
        }
        
        &:hover
        {
            background-color: darkblue;
        }
    }
    
    text
    {
        "测试"
    }
}
```

### 输出HTML
```html
<div class="test-box" style="width: 200px;">测试</div>
```

### 输出CSS
```css
.test-box {
    background-color: blue;
}

.test-box:hover {
    background-color: darkblue;
}
```

## 架构特点

### 1. 复用现有架构
- **基于现有**: 完全基于现有的解析器和生成器架构
- **扩展增强**: 在现有基础上增强功能
- **保持兼容**: 不破坏现有功能

### 2. 状态管理
- **样式上下文**: 维护当前样式状态
- **选择器栈**: 管理嵌套选择器
- **自动计数**: 生成唯一的自动类名和ID

### 3. 双重输出
- **内联样式**: 直接添加到元素的style属性
- **全局CSS**: 选择器规则添加到全局样式块

## 总结

第二部分todo已成功完成，实现了CHTL局部样式块系统的完整功能：

### ✅ 已完成功能
1. **内联样式**: 直接CSS属性作为内联样式
2. **自动化类名**: `.className`语法自动添加类名
3. **自动化ID**: `#idName`语法自动添加ID
4. **上下文推导**: `&`符号的智能解析
5. **伪类支持**: `:hover`, `:active`等伪类选择器
6. **伪元素支持**: `::before`, `::after`等伪元素选择器
7. **全局CSS生成**: 选择器规则自动添加到全局样式块

### 🎯 实现亮点
- **严格遵循**: 完全按照CHTL语法文档实现
- **复用架构**: 基于现有代码架构扩展
- **功能完整**: 覆盖所有局部样式块特性
- **智能推导**: 上下文感知的选择器解析

### 📈 为后续开发准备
- **样式基础**: 为模板和自定义系统提供样式处理基础
- **选择器支持**: 为复杂选择器提供解析框架
- **CSS生成**: 为高级CSS功能提供生成基础

这个实现确保了CHTL局部样式块功能的完整性和正确性，为后续的模板系统和自定义系统奠定了坚实的样式处理基础。