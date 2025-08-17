# CHTL JS语法纠正文档

## ❌ 之前错误的理解

我之前给出的CHTL JS例子是错误的，比如：
```javascript
// ❌ 错误示例
listen({{button}}, "click", function() { ... });
delegate({{container}}, "button", "click", handler);
animate({{element}}, { opacity: [0, 1] });
```

## ✅ 正确的CHTL JS语法

根据CHTL语法文档的准确描述：

### 1. 增强选择器语法
```chtl
script {
    {{box}}.textContent();           // 查找类名/id为box的元素
    {{.box}};                        // 查找类名为box的元素
    {{#box}};                        // 查找id为box的元素
    {{button}};                      // 所有button元素
    {{.box button}};                 // 类名为box的元素的所有button后代
    {{button[0]}};                   // 第一个button元素
}
```

### 2. 明确CHTL语法（使用->）
```chtl
script {
    {{box}}->textContent();         // 使用->明确表示CHTL JS语法
    {{box}}->addEventListener();     // 链式访问
}
```

### 3. 增强监听器语法
```chtl
script {
    {{box}}->listen({
        click: () => {
            // 点击事件处理
        },
        mouseenter: mouseEnterEvent,  // 已存在的函数
        mousemove: function() {
            // 鼠标移动处理
        }
    });
}
```

### 4. 事件委托语法
```chtl
script {
    {{父元素选择器}}->delegate({
        target: {{选择器}} | [{{选择器1}}, {{选择器2}}],
        click: 函数,
        mouseenter: 函数,
        mouseleave: 函数
    });
}
```

### 5. 动画语法
```chtl
script {
    const anim = animate({
        target: {{选择器}} || [{{选择器1}}, {{选择器2}}] || DOM对象,
        duration: 100,
        easing: ease-in-out,
        
        begin: {
            // 起始状态CSS代码
        },
        
        when: [
            {
                at: 0.4;
                opacity: 0.5,
                transform: 'scale(1.1)'
            },
            {
                at: 0.8;
                // 其他CSS代码
            }
        ],
        
        end: {
            // 终止状态CSS代码
        },
        
        loop: -1,
        direction: ,
        delay: ,
        callback: function
    });
}
```

### 6. 虚对象语法
```chtl
script {
    vir test = listen({
        click: () => {
            // 点击处理
        }
    });
    
    test->click();  // 调用click键的函数
}
```

## 🔍 关键理解纠正

### CHTL JS的本质
1. **不是JS的超集** - CHTL JS有自己独特的语法
2. **由CHTL JS编译器解析** - 不是标准JS编译器
3. **与JS语法不兼容** - 两者是独立的语法系统

### 核心语法特点
1. **{{}}选择器** - 核心的DOM选择机制
2. **->操作符** - 明确标识CHTL JS语法
3. **对象字面量配置** - listen、delegate、animate都使用对象配置
4. **vir虚对象** - 编译期语法糖，运行时不存在

### 局部script的作用
- 在元素内部使用`script{}`
- 会被添加到高优先级的全局script块
- 不会造成全局污染

## ✅ 当前实现状态验证

让我检查我们的实现是否正确支持了这些语法：

1. **{{}}选择器** - ✅ Token: DOUBLE_LBRACE/RBRACE, AST: CHTL_JS_SELECTOR
2. **->操作符** - ✅ Token: ARROW
3. **listen函数** - ✅ Token: LISTEN, AST: CHTL_JS_LISTEN
4. **delegate函数** - ✅ Token: DELEGATE, AST: CHTL_JS_DELEGATE
5. **animate函数** - ✅ Token: ANIMATE, AST: CHTL_JS_ANIMATE
6. **vir虚对象** - ✅ Token: VIR, AST: CHTL_JS_VIR
7. **script块** - ✅ Token: SCRIPT, AST: SCRIPT_BLOCK

## 🎯 结论

我们的Token和AST系统已经正确支持了真正的CHTL JS语法！之前我在示例中混淆了语法，但底层实现是正确的。

感谢您的纠正！现在我们对CHTL JS语法有了准确的理解。