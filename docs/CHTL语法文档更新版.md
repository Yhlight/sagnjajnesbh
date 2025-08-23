# CHTL语法文档（实现版）

## 📋 概述

CHTL（C++ HyperText Language）是基于C++实现的超文本语言，旨在提供更符合开发者编写HTML代码的方式。本文档基于实际编译器实现，详细说明CHTL的完整语法规范。

**协议：** MIT开源协议  
**版本：** 1.0.0  
**实现状态：** ✅ 完全实现

## 🎯 语法特性概览

### 核心特性
- ✅ **注释系统** - 三种注释类型（`//`、`/* */`、`--`）
- ✅ **字面量支持** - 无修饰字面量和引号字符串
- ✅ **CE对等式** - `:`与`=`完全等价
- ✅ **元素节点** - 支持所有HTML元素
- ✅ **属性系统** - 灵活的属性定义
- ✅ **局部样式块** - 内联样式和全局样式自动分离

### 高级特性
- ✅ **命名空间** - 支持嵌套和别名
- ✅ **模板系统** - `[Template]`定义可复用组件
- ✅ **自定义组件** - `[Custom]`定义用户组件
- ✅ **导入系统** - `[Import]`支持模块导入
- ✅ **原始嵌入** - `[Origin]`支持多种代码类型
- ✅ **CHTL JS集成** - JavaScript语法扩展

## 📝 基础语法

### 注释系统

CHTL支持三种注释类型：

```chtl
// 单行注释 - 不会被生成器识别
/* 
   多行注释 - 不会被生成器识别
*/

-- 生成器注释 - 会被生成器识别并处理
-- 这种注释会根据上下文生成对应语言的注释
```

**实现特性：**
- `--`注释会根据上下文生成不同编程语言的注释
- 在HTML上下文中生成`<!-- -->`
- 在CSS上下文中生成`/* */`
- 在JavaScript上下文中生成`//`

### 文本节点

```chtl
text {
    "这是一段文本"
}

// 简化写法
text {
    这是一段文本
}
```

### 字面量支持

CHTL支持多种字面量格式：

```chtl
// 无修饰字面量（推荐）
div {
    id: main-container;
    class: header navigation;
}

// 双引号字符串
div {
    id: "main-container";
    class: "header navigation";
}

// 单引号字符串
div {
    id: 'main-container';
    class: 'header navigation';
}
```

### CE对等式

在CHTL中，冒号`:`与等号`=`完全等价：

```chtl
// 这两种写法完全相同
div {
    id: container;        // 使用冒号
    class = navigation;   // 使用等号
}
```

## 🏗️ 元素和属性

### HTML元素

CHTL支持所有HTML元素：

```chtl
html {
    head {
        title {
            text { CHTL示例页面 }
        }
        
        meta {
            charset: utf-8;
        }
    }
    
    body {
        div {
            id: main;
            class: container;
            
            h1 {
                text { 欢迎使用CHTL }
            }
            
            p {
                text { 这是一个段落 }
            }
        }
    }
}
```

### 属性定义

```chtl
input {
    type: text;
    name: username;
    placeholder: 请输入用户名;
    required: true;
    data-validation: email;
}
```

## 🎨 样式系统

### 局部样式块

CHTL的局部样式块支持内联样式和全局样式：

```chtl
div {
    class: header;
    
    style {
        // 内联样式 - 直接应用到元素
        width: 100%;
        height: 60px;
        
        // 类选择器 - 自动添加到全局样式
        .header {
            background-color: #333;
            color: white;
        }
        
        // ID选择器
        #logo {
            font-size: 24px;
            font-weight: bold;
        }
        
        // 伪类选择器
        .header:hover {
            background-color: #555;
        }
        
        // 伪元素选择器
        .header::before {
            content: "▶ ";
        }
    }
}
```

## 📁 命名空间系统

### 基础命名空间

```chtl
namespace Components {
    div {
        class: component;
        
        text { 组件内容 }
    }
}

namespace UI.Controls {
    button {
        class: btn;
        text { 点击我 }
    }
}
```

### 命名空间别名

```chtl
namespace Components as Comp {
    div {
        class: widget;
    }
}

namespace VeryLongNamespaceName as Short {
    span {
        text { 简化引用 }
    }
}
```

### 嵌套命名空间

```chtl
namespace App {
    namespace Header {
        div {
            class: app-header;
        }
    }
    
    namespace Content {
        div {
            class: app-content;
        }
    }
}
```

## 🧩 模板系统

### 模板定义

```chtl
[Template] @Style ButtonStyle {
    .btn {
        padding: 10px 20px;
        border: none;
        border-radius: 4px;
        cursor: pointer;
    }
    
    .btn-primary {
        background-color: #007bff;
        color: white;
    }
}

[Template] @Element Button {
    button {
        class: "btn btn-primary";
        text: buttonText;
        onclick: buttonAction;
    }
}

[Template] @Var DefaultSpacing {
    16px
}
```

### 模板使用

```chtl
div {
    style {
        padding: DefaultSpacing();
    }
    
    Button {
        buttonText: "提交";
        buttonAction: "submitForm()";
    }
}
```

## 🛠️ 自定义组件

```chtl
[Custom] @Style CardStyle {
    .card {
        border: 1px solid #ddd;
        border-radius: 8px;
        padding: 16px;
        margin: 8px;
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
    }
}

[Custom] @Element Card {
    div {
        class: card;
        
        div {
            class: card-header;
            h3 { text: title; }
        }
        
        div {
            class: card-body;
            text: content;
        }
        
        div {
            class: card-footer;
            text: footer;
        }
    }
}
```

### 组件使用

```chtl
Card {
    title: "卡片标题";
    content: "这是卡片内容";
    footer: "卡片底部";
}
```

## 📦 导入系统

### CHTL模块导入

```chtl
[Import] @Chtl from Chtholly;           // 导入珂朵莉模块
[Import] @Chtl from Yuigahama;          // 导入由比滨结衣模块
[Import] @Chtl from Chtholly.Core;      // 导入特定子模块
```

### CJMOD导入

```chtl
[Import] @CJmod from Chtholly;          // 导入CJMOD扩展
[Import] @CJmod from CustomModule;      // 导入自定义CJMOD
```

### 媒体导入

```chtl
[Import] MEDIA_HTML from header.html as HeaderHTML;
[Import] MEDIA_STYLE from styles.css as MainStyles;
[Import] MEDIA_JAVASCRIPT from script.js as MainScript;
```

**限制：** 媒体导入只能搜索编译文件所在路径的文件（不递归）

### 原始嵌入导入

```chtl
[Import] @Html from ModuleName.EmbedName as CustomHTML;
[Import] @Style from ModuleName.StyleName as CustomCSS;
[Import] @JavaScript from ModuleName.ScriptName as CustomJS;
```

## 📄 原始嵌入系统

原始嵌入允许在任意节点中使用，提供了极高的灵活性：

### 官方支持类型

```chtl
[Origin] @Html HtmlBlock {
    <div class="custom-html">
        <h2>原始HTML内容</h2>
        <p>这里可以写任意HTML代码</p>
    </div>
}

[Origin] @Style StyleBlock {
    .custom-styles {
        background: linear-gradient(45deg, #ff6b6b, #4ecdc4);
        padding: 20px;
        border-radius: 10px;
    }
    
    .animation {
        animation: slideIn 0.5s ease-in-out;
    }
    
    @keyframes slideIn {
        from { opacity: 0; transform: translateY(-20px); }
        to { opacity: 1; transform: translateY(0); }
    }
}

[Origin] @JavaScript ScriptBlock {
    function initializeApp() {
        console.log('应用初始化完成');
        
        document.querySelectorAll('.btn').forEach(btn => {
            btn.addEventListener('click', handleClick);
        });
    }
    
    function handleClick(event) {
        console.log('按钮被点击:', event.target);
    }
    
    // 页面加载完成后初始化
    document.addEventListener('DOMContentLoaded', initializeApp);
}
```

### 自定义类型

通过`[OriginType]`配置可以支持自定义类型：

```chtl
[OriginType] @Vue {
    fileExtensions: [".vue"];
    syntax: "vue";
    compiler: "vue-template-compiler";
}

[Origin] @Vue VueComponent {
    <template>
        <div class="vue-component">
            <h1>{{ title }}</h1>
            <button @click="handleClick">{{ buttonText }}</button>
        </div>
    </template>
    
    <script>
    export default {
        data() {
            return {
                title: 'Vue组件',
                buttonText: '点击我'
            }
        },
        methods: {
            handleClick() {
                console.log('Vue按钮被点击');
            }
        }
    }
    </script>
    
    <style scoped>
    .vue-component {
        padding: 20px;
        border: 1px solid #ccc;
    }
    </style>
}
```

### 在任意位置使用原始嵌入

```chtl
div {
    class: container;
    
    // 在元素开头使用
    [Origin] @Style InlineStyles {
        .container { background: #f0f0f0; }
    }
    
    h1 {
        text: "标题";
        
        // 在元素中间使用
        [Origin] @JavaScript TitleScript {
            console.log('标题元素已创建');
        }
    }
    
    p {
        text: "段落内容";
    }
    
    // 在元素末尾使用
    [Origin] @Html CustomFooter {
        <footer class="custom-footer">
            <p>版权信息</p>
        </footer>
    }
}
```

## ⚙️ 配置系统

### 基础配置

```chtl
[Configuration] @Config MainConfig {
    output: {
        format: "html5";
        charset: "utf-8";
        indent: 2;
        minify: false;
    }
    
    compiler: {
        enableOptimization: true;
        sourceMap: true;
        strictMode: true;
    }
    
    modules: {
        searchPaths: ["./modules", "./node_modules"];
        autoImport: true;
    }
}
```

### 选择器自动化配置

```chtl
[Configuration] @SelectorAutomation {
    enableAutoId: true;
    enableAutoClass: true;
    idPrefix: "auto-";
    classPrefix: "comp-";
    
    rules: {
        // 自动为没有id的div添加id
        "div:not([id])": {
            autoId: true;
        };
        
        // 自动为按钮添加类
        "button": {
            autoClass: "btn";
        };
    }
}
```

## 🚀 CHTL JS集成

### 基础CHTL JS语法

```chtl
script {
    // CHTL JS代码块
    let message = "Hello CHTL!";
    
    function showMessage() {
        console.log(message);
    }
    
    // 调用函数
    showMessage();
}
```

### 虚对象（vir）

```chtl
script {
    // 定义虚对象
    vir Calculator = {
        add: function(a, b) {
            return a + b;
        },
        
        multiply: function(a, b) {
            return a * b;
        }
    };
    
    // 使用虚对象
    let result = Calculator.add(5, 3);
    console.log(result); // 输出: 8
}
```

### CJMOD集成

当导入CJMOD模块时，可以使用扩展的CHTL JS语法：

```chtl
[Import] @CJmod from Chtholly;

script {
    // 使用珂朵莉模块的printMylove功能
    const asciiArt = printMylove({
        url: "image.jpg",
        mode: "ASCII",
        width: 80,
        height: 40,
        scale: 1.0
    });
    
    console.log(asciiArt);
    
    // 使用iNeverAway功能
    vir EventSystem = iNeverAway({
        Void<Init>: function() {
            console.log("系统初始化");
        },
        
        Void<Cleanup>: function() {
            console.log("系统清理");
        },
        
        Handle: function(eventType, callback) {
            document.addEventListener(eventType, callback);
        }
    });
    
    EventSystem->Void<Init>();
    EventSystem->Handle("click", function(e) {
        console.log("点击事件:", e.target);
    });
}
```

## 📊 模块系统

### CMOD模块结构

```
ModuleName/
  src/
    ModuleName.chtl        # 主要源文件
    Component1.chtl        # 组件源文件
    Component2.chtl        # 更多组件
  info/
    ModuleName.chtl        # 模块信息文件
```

### CJMOD模块结构

```
ModuleName/
  src/
    ModuleName.cpp         # C++源文件
    ModuleName.h           # C++头文件
    Additional.cpp         # 其他源文件
  info/
    ModuleName.chtl        # 模块信息文件
```

### 混合模块结构（CMOD + CJMOD）

```
ModuleName/
  CMOD/
    ComponentName/
      src/
        ComponentName.chtl
      info/
        ComponentName.chtl
  CJMOD/
    ExtensionName/
      src/
        ExtensionName.cpp
        ExtensionName.h
      info/
        ExtensionName.chtl
```

### 模块信息文件

```chtl
[Info] {
    name = "ModuleName";
    version = "1.0.0";
    description = "模块描述";
    author = "作者名称";
    license = "MIT";
    dependencies = "RequiredModule";
    category = "ui-component";
    minCHTLVersion = "1.0.0";
    maxCHTLVersion = "2.0.0";
    keywords = "component,ui,library";
    features = "responsive,accessible,customizable";
}

[Export] {
    [Template] @Style ModuleStyle;
    [Template] @Element ModuleElement;
    [Template] @Var ModuleVariable;
    [Custom] @Style CustomStyle;
    [Custom] @Element CustomElement;
    [Origin] @Html HtmlBlock;
    [Origin] @Style StyleBlock;
    [Origin] @JavaScript ScriptBlock;
    [Configuration] @Config ModuleConfig;
}
```

## 🔍 扫描策略

CHTL编译器支持两种扫描策略：

### 滑动窗口策略（SLIDING_WINDOW）
- 双指针同时向前移动
- 前指针检测关键字，触发后指针收集
- 包含初始扫描避免遗漏开头的语法片段

### 前置提取策略（FRONT_EXTRACT）
- 从前面截取代码，避免发送给错误的编译器
- 确保代码片段分配给正确的处理器

## 🎯 最佳实践

### 1. 代码组织

```chtl
// 推荐的文件结构
[Import] @Chtl from RequiredModules;

[Configuration] @Config {
    // 配置选项
}

namespace App {
    // 主要内容
    html {
        head {
            // 头部内容
        }
        
        body {
            // 页面内容
        }
    }
}

[Origin] @Style {
    // 全局样式
}

[Origin] @JavaScript {
    // 全局脚本
}
```

### 2. 命名约定

```chtl
// 使用kebab-case命名ID和类
div {
    id: main-container;
    class: navigation-header;
}

// 使用PascalCase命名模板和组件
[Template] @Element NavigationMenu { }
[Custom] @Element UserProfile { }

// 使用camelCase命名JavaScript变量
script {
    let userName = "示例用户";
    let isLoggedIn = true;
}
```

### 3. 性能优化

```chtl
// 使用模板复用减少重复代码
[Template] @Style CommonButton {
    .btn {
        padding: 8px 16px;
        border: none;
        border-radius: 4px;
        cursor: pointer;
        transition: all 0.2s;
    }
}

// 合理使用原始嵌入避免编译开销
[Origin] @Style OptimizedStyles {
    /* 预编译的CSS */
}
```

## 🔧 调试和故障排除

### 编译器选项

```bash
# 启用详细输出
chtl-compiler --verbose input.chtl

# 仅扫描测试
chtl-compiler --scan-only input.chtl

# 禁用上下文检查
chtl-compiler --no-context-check input.chtl

# 禁用最小单元切割
chtl-compiler --no-minimal-unit input.chtl
```

### 常见错误

1. **语法错误**
   ```
   错误: 未闭合的大括号
   解决: 检查所有 { } 配对
   ```

2. **导入错误**
   ```
   错误: 模块未找到
   解决: 检查module文件夹和模块路径
   ```

3. **模板错误**
   ```
   错误: 模板参数缺失
   解决: 确保所有必需参数都已提供
   ```

## 📚 相关文档

- [项目构建指南](./项目构建指南.md) - 如何构建CHTL项目
- [CMOD开发指南](./CMOD模块开发指南.md) - 模块开发教程
- [CJMOD开发指南](./CJMOD模块开发指南.md) - C++扩展开发
- [编译器开发指南](./CHTL编译器开发指南.md) - 编译器架构说明

## 📄 许可证

本项目采用MIT许可证，详见 [LICENSE](../LICENSE) 文件。