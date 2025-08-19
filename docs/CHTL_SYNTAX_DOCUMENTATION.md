# CHTL语法文档

## 关于CHTL

CHTL (Chtholly Template Language) 是基于C++语言实现的现代超文本语言，旨在为开发者提供更符合直觉的HTML编写方式。CHTL采用MIT开源协议，致力于推动Web开发技术的进步。

## 设计理念

CHTL的设计理念是让HTML编写更加自然、直观和强大：
- **简洁性**: 减少冗余的标签和语法
- **直观性**: 更接近自然语言的表达方式
- **强大性**: 提供模板、自定义、模块化等高级功能
- **扩展性**: 支持CHTL JS和CJMOD扩展

---

## 基础语法

### 注释系统

CHTL支持三种注释类型：

```chtl
// 单行注释 - 不会出现在生成的HTML中
/* 
 * 多行注释 - 不会出现在生成的HTML中
 */
-- 生成器注释 - 会被保留在生成的HTML中
```

### 文本节点

使用 `text { }` 表示文本内容：

```chtl
text {
    这是一段文本
}
```

### 字面量系统

CHTL支持多种字面量格式：

```chtl
text {
    这是无修饰字面量  // 无需引号
    "这是双引号字符串"
    '这是单引号字符串'
}

style {
    color: red;        // 无修饰字面量
    background: "blue"; // 引号字符串
}
```

### CE对等式

在CHTL中，冒号 `:` 与等号 `=` 完全等价：

```chtl
div {
    class: container;   // 使用冒号
    id = main;         // 使用等号，完全等价
}
```

### 元素节点

CHTL支持所有HTML元素：

```chtl
html {
    head {
        title { text { 页面标题 } }
    }
    
    body {
        div {
            class: container;
            
            h1 { text { 欢迎使用CHTL } }
            p { text { 现代超文本语言 } }
        }
    }
}
```

### 属性系统

使用 `属性名: 属性值;` 语法：

```chtl
div {
    id: box;
    class: welcome;
    data-value: 123;
    
    text { Hello, CHTL! }
}
```

---

## 样式系统

### 局部样式块

CHTL允许在元素内部嵌套样式块：

```chtl
div {
    class: box;
    
    style {
        // 内联样式
        padding: 20px;
        margin: 10px;
        
        // 类选择器 - 自动添加到全局样式
        .box {
            background: #f0f0f0;
            border-radius: 8px;
        }
        
        // ID选择器
        #special {
            color: blue;
        }
        
        // 伪类选择器
        .box:hover {
            background: #e0e0e0;
        }
        
        // 伪元素选择器
        .box::before {
            content: "🎯";
        }
    }
}
```

### 自动添加规则

CHTL的智能自动添加系统：

**局部style块规则**：
- 如果元素没有明确的class或id，第一个类选择器和第一个ID选择器会被自动添加
- `&` 语法：类优先，如果类不存在则使用ID

**局部script块规则**：
- 如果元素没有明确的class或id，且style没有自动添加，`{{.xxx}}`或`{{#box}}`会触发自动添加
- `{{&}}` 语法：ID优先，如果ID不存在则使用类
- `{{xxx}}` 不会触发自动添加

**核心原则**: "缺什么，自动添加什么"

---

## 模板系统

### 样式组模板

使用 `[Template] @Style 组名` 创建样式组模板：

```chtl
[Template] @Style ButtonBase {
    padding: 12px 24px;
    border: none;
    border-radius: 6px;
    cursor: pointer;
    transition: all 0.3s ease;
}

[Template] @Style ButtonPrimary {
    background: #3b82f6;
    color: white;
}
```

### 元素模板

使用 `[Template] @Element 元素名` 创建元素模板：

```chtl
[Template] @Element Card {
    div {
        class: card;
        
        style {
            .card {
                background: white;
                border-radius: 8px;
                padding: 20px;
                box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            }
        }
        
        div {
            class: card-header;
            text { 卡片标题 }
        }
        
        div {
            class: card-content;
            text { 卡片内容 }
        }
    }
}
```

### 变量组模板

使用 `[Template] @Var 变量组名` 创建变量组：

```chtl
[Template] @Var ThemeColors {
    primaryColor: "#3b82f6";
    secondaryColor: "#64748b";
    accentColor: "#f59e0b";
}

// 使用变量组
div {
    style {
        color: ThemeColors(primaryColor);
        background: ThemeColors(secondaryColor);
    }
}
```

### 样式组继承

使用 `inherit @Style 样式组名` 继承样式：

```chtl
[Template] @Style ExtendedButton {
    inherit @Style ButtonBase;
    background: #10b981;
}
```

---

## 自定义系统

### 自定义元素

基于模板创建自定义元素：

```chtl
[Custom] @Element InteractiveCard from Card {
    // 删除原有元素
    delete div[1];  // 删除第二个div
    
    // 插入新元素
    insert after div[0] {
        button {
            class: card-action;
            text { 点击我 }
        }
    }
    
    // 替换元素
    replace div[0] {
        h3 {
            class: card-title;
            text { 交互式卡片 }
        }
    }
}
```

### 自定义样式组

创建无值样式组和特例化：

```chtl
[Custom] @Style FlexLayout {
    display,
    justify-content,
    align-items;
}

// 使用时填入值
div {
    style {
        @Style FlexLayout {
            display: flex;
            justify-content: center;
            align-items: center;
        }
    }
}
```

### 自定义变量组

```chtl
[Custom] @Var AppTheme {
    primaryColor: "#2563eb";
    textColor: "#1f2937";
}

// 变量组特例化
div {
    style {
        color: AppTheme(textColor = "#374151");
    }
}
```

### 索引访问

使用 `[index]` 访问特定元素：

```chtl
[Custom] @Element ModifiedList from List {
    // 为第一个li添加样式
    li[0] {
        style {
            font-weight: bold;
        }
    }
    
    // 删除第三个li
    delete li[2];
}
```

---

## 原始嵌入系统

### HTML嵌入

```chtl
[Origin] @Html CustomHeader {
    <header class="site-header">
        <nav class="main-nav">
            <a href="/">首页</a>
            <a href="/about">关于</a>
        </nav>
    </header>
}

// 使用
body {
    CustomHeader
}
```

### 样式嵌入

```chtl
[Origin] @Style Animations {
    @keyframes fadeIn {
        from { opacity: 0; }
        to { opacity: 1; }
    }
    
    .fade-in {
        animation: fadeIn 0.5s ease-in-out;
    }
}
```

### 脚本嵌入

```chtl
[Origin] @JavaScript Utils {
    function debounce(func, wait) {
        let timeout;
        return function executedFunction(...args) {
            const later = () => {
                clearTimeout(timeout);
                func(...args);
            };
            clearTimeout(timeout);
            timeout = setTimeout(later, wait);
        };
    }
}
```

### 自定义嵌入类型

CHTL支持自定义原始嵌入类型（如Vue、React等）：

```chtl
[Origin] @Vue TodoComponent {
    <template>
        <div class="todo-item">
            <input type="checkbox" v-model="completed">
            <span :class="{ completed }">{{ text }}</span>
        </div>
    </template>
    
    <script>
    export default {
        props: ['text', 'completed']
    }
    </script>
}
```

---

## 约束系统

### 元素约束

使用 `except` 约束特定类型（支持链式）：

```chtl
div {
    class: safe-content;
    
    // 约束多种类型（链式语法）
    except @Html, [Custom], [Template];
    
    // 只允许基础元素
    p { text { 安全内容 } }
}
```

**注意**: except只能约束以下三种类型：
- `@Html` - HTML元素
- `[Custom]` - 自定义类型
- `[Template]` - 模板类型

---

## 导入系统

### 基础导入

```chtl
// 导入HTML文件
[Import] @Html from header as HeaderTemplate

// 导入CSS文件
[Import] @Style from theme as ThemeStyle

// 导入JavaScript文件
[Import] @JavaScript from utils as Utils

// 导入CHTL模块
[Import] @Chtl from ModuleName

// 导入CJMOD扩展
[Import] @CJmod from ExtensionName
```

### 特定类型导入

```chtl
// 导入特定元素
[Import] [Custom] @Element ButtonComponent from ui/button

// 导入特定样式组
[Import] [Template] @Style BaseStyles from ui/styles

// 导入特定变量组
[Import] [Custom] @Var ThemeColors from config/theme
```

### 批量导入

```chtl
// 导入所有模板
[Import] [Template] from ui/templates

// 导入所有自定义
[Import] [Custom] from components/custom

// 导入所有原始嵌入
[Import] [Origin] from assets/origins
```

### 通配符导入

```chtl
// 通配符路径导入
[Import] @Chtl from modules/*
[Import] @Chtl from themes/*.cmod

// 子模块导入
[Import] @Chtl from Chtholly.*
[Import] @Chtl from Chtholly.Effects
```

---

## 命名空间系统

### 命名空间定义

```chtl
[Namespace] UI {
    [Namespace] Components {
        // 组件相关定义
    }
    
    [Namespace] Themes {
        // 主题相关定义
    }
}
```

### 命名空间使用

```chtl
// 使用命名空间中的元素
UI::Components::Button
UI::Themes::DarkTheme
```

### 自动合并

CHTL会自动合并同名命名空间，并检测冲突。

---

## 配置系统

### 命名配置组

```chtl
[Configuration] @Config ProjectConfig {
    INDEX_INITIAL_COUNT = 0;
    DISABLE_NAME_GROUP = false;
    DEBUG_MODE = true;
    
    [Name] {
        CUSTOM_ELEMENT = @Element;
        TEMPLATE_STYLE = @Style;
        IMPORT_CHTL = @Chtl;
    }
    
    [OriginType] {
        ORIGINTYPE_VUE = @Vue;
        ORIGINTYPE_REACT = @React;
    }
}
```

### 配置导入

```chtl
// 导入配置组
[Import] @Config ProjectConfig from config/project

// 全缀名导入
[Import] [Configuration] @Config ProjectConfig from config/project
```

---

## CHTL JS扩展语法

### 增强选择器

```chtl
script {
    {{.button}}     // 类选择器
    {{#header}}     // ID选择器
    {{div}}         // 标签选择器
    {{button[0]}}   // 索引选择器
    {{&}}           // 当前元素
    {{.nav a}}      // 后代选择器
}
```

### 箭头操作符

`->` 与 `.` 完全等价，用于明确使用CHTL JS语法：

```chtl
script {
    {{.button}}->addEventListener('click', handler);
    {{.button}}->style->display = 'none';
    
    // 链式调用
    {{.element}}->classList->add('active')->style->opacity = '1';
}
```

### 增强监听器

`listen` 函数支持所有82个JavaScript事件：

```chtl
script {
    {{.button}}->listen({
        click: function(e) {
            console.log('按钮被点击');
        },
        mouseenter: function(e) {
            {{this}}->addClass('hover');
        },
        focus: function(e) {
            {{this}}->addClass('focused');
        }
    });
}
```

### 事件委托

`delegate` 函数提供事件委托功能：

```chtl
script {
    {{.container}}->delegate({
        target: {{.item}},
        click: function(e) {
            console.log('委托点击事件');
        },
        mouseenter: function(e) {
            {{this}}->addClass('hover');
        }
    });
}
```

### 动画系统

`animate` 函数封装requestAnimationFrame：

```chtl
script {
    animate({
        target: {{.element}},
        duration: 1000,
        easing: 'ease-out',
        properties: {
            opacity: [0, 1],
            transform: ['translateY(20px)', 'translateY(0)']
        },
        onComplete: function() {
            console.log('动画完成');
        }
    });
}
```

### 虚对象

`vir` 关键字创建编译时语法糖，不产生运行时对象：

```chtl
script {
    vir UIManager = listen({
        init: function() {
            console.log('初始化UI');
        },
        destroy: function() {
            console.log('销毁UI');
        }
    });
    
    // 使用虚对象（编译为函数引用）
    UIManager->init();
    UIManager->destroy();
}
```

### 无序和可选键值对

CHTL JS支持无序键值对和可选参数：

```chtl
script {
    // 无序键值对
    const config = {
        timeout: 5000,
        retries: 3,
        url: "https://api.example.com",
        method: "POST"
    };
    
    // 可选键值对
    function createRequest({
        url,
        method = "GET",
        timeout?,
        headers?
    }) {
        // 函数实现
    }
}
```

---

## 删除操作语法

### 删除样式属性（支持链式）

```chtl
[Custom] @Style CleanStyle {
    @Style BaseStyle {
        delete line-height, border, margin;  // 链式删除多个属性
    }
}
```

### 删除元素

```chtl
[Custom] @Element ModifiedComponent from BaseComponent {
    delete span;        // 删除所有span元素
    delete div[1];      // 删除第二个div元素
    delete @Element SubComponent;  // 删除继承的元素
}
```

**重要**: delete不支持CSS选择器（如 `.class` 或 `#id`），只支持：
- 样式属性名（支持链式）
- 元素类型名
- 索引元素 `element[index]`
- 继承元素 `@Element ElementName`

---

## 链式语法支持

### 支持链式的操作

```chtl
// except约束（支持链式）
div {
    except @Html, [Custom], [Template];
}

// delete属性（支持链式）
[Custom] @Style CleanStyle {
    delete line-height, border, margin;
}
```

### 不支持链式的操作

```chtl
// inherit继承（不支持链式）
[Template] @Style ExtendedStyle {
    inherit @Style BaseStyle;  // 每次只能继承一个
}

// insert插入（不支持链式）
[Custom] @Element ModifiedComponent {
    insert after div[0] {
        // 每次只能插入一个位置
    }
}
```

---

## 模块系统

### CMOD模块

CMOD是CHTL组件模块，包含模板、自定义、样式组等：

**目录结构**：
```
ModuleName/
├── src/
│   ├── ModuleName.chtl     # 主模块文件（可选）
│   ├── Component1.chtl     # 组件文件
│   └── SubModule/          # 子模块（必须在src内部）
│       ├── src/
│       └── info/
└── info/
    └── ModuleName.chtl     # 模块信息文件
```

**三同名规则**: 模块文件夹名、主模块文件名、模块信息文件名必须相同。

**模块信息文件**：
```chtl
[Info] {
    name = "ModuleName";
    version = "1.0.0";
    description = "模块描述";
    author = "作者名";
    license = "MIT";
    category = "组件类别";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
}

[Export] {
    [Custom] @Element ComponentName;
    [Template] @Style StyleName;
    [Custom] @Var VariableName;
}
```

### CJMOD扩展

CJMOD是C++编写的CHTL JS语法扩展：

**目录结构**：
```
ExtensionName/
├── src/
│   ├── extension.cpp       # C++实现文件
│   └── extension.h         # C++头文件
└── info/
    └── ExtensionName.chtl  # 扩展信息文件
```

**二同名规则**: 扩展文件夹名和扩展信息文件名必须相同。

---

## 高级特性

### 全缀名访问

处理命名冲突时使用全缀名：

```chtl
// 访问特定命名空间的元素
UI::Components::Button
Theme::Dark::Colors

// 访问特定模块的组件
Chtholly::ChthollyButton
Yuigahama::YuigahamaCard
```

### 条件编译

使用配置组控制编译行为：

```chtl
[Configuration] @Config Development {
    DEBUG_MODE = true;
}

[Configuration] @Config Production {
    DEBUG_MODE = false;
}
```

---

## 最佳实践

### 项目组织

1. **模块化设计**: 使用模板和自定义创建可复用组件
2. **命名规范**: 使用驼峰命名法和有意义的名称
3. **样式管理**: 使用变量组统一管理主题和样式
4. **组件分层**: 模板 -> 自定义 -> 实例的清晰层次

### 性能优化

1. **选择器优化**: 优先使用类选择器而非复杂选择器
2. **模块按需加载**: 只导入需要的组件和功能
3. **样式继承**: 合理使用继承减少重复代码
4. **脚本优化**: 使用CHTL JS的高效语法

### 代码风格

1. **缩进**: 使用4个空格缩进
2. **注释**: 适当使用注释说明复杂逻辑
3. **命名**: 使用有意义的名称
4. **结构**: 保持清晰的代码结构

---

## 语法参考速查

### 段落类型
- `[Info]` - 模块信息
- `[Export]` - 导出声明
- `[Import]` - 导入声明
- `[Namespace]` - 命名空间
- `[Template]` - 模板定义
- `[Custom]` - 自定义定义
- `[Origin]` - 原始嵌入
- `[Configuration]` - 配置定义

### 类型标识
- `@Element` - 元素类型
- `@Style` - 样式组类型
- `@Var` - 变量组类型
- `@Html` - HTML嵌入类型
- `@JavaScript` - JavaScript嵌入类型
- `@Config` - 配置类型
- `@Chtl` - CHTL模块类型
- `@CJmod` - CJMOD扩展类型

### 操作关键字
- `inherit` - 继承操作
- `delete` - 删除操作
- `insert` - 插入操作
- `replace` - 替换操作
- `except` - 约束操作
- `from` - 来源指定
- `as` - 别名指定

### CHTL JS关键字
- `listen` - 事件监听
- `delegate` - 事件委托
- `animate` - 动画系统
- `vir` - 虚对象
- `->` - 箭头操作符

---

## 错误处理

### 常见错误

1. **语法错误**: 使用不存在的语法
2. **命名错误**: 违反三同名/二同名规则
3. **约束错误**: 约束不支持的类型
4. **导入错误**: 导入不存在的模块或文件

### 调试技巧

1. **使用生成器注释**: `--` 注释会保留在输出中
2. **检查编译输出**: 查看生成的HTML/CSS/JS
3. **验证模块结构**: 确保符合CMOD/CJMOD规范
4. **使用IDE支持**: VSCode插件提供实时错误检测

---

这是CHTL语言的完整语法文档。CHTL致力于为Web开发提供更直观、强大的解决方案，让开发者能够专注于创造而非语法细节。