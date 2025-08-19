# 🧮 CHTL JS计算器 - 编译转换示例

这是一个完整的CHTL JS到JavaScript转换示例，展示了CHTL JS编译器的核心功能！

## 🎯 **CHTL JS → JavaScript 转换对照**

### 1. **虚对象 (vir) 转换**：
```chtl
// CHTL JS代码
vir calculator = {
    currentValue: 0,
    previousValue: 0,
    operator: null
};
```
```javascript
// 转换后的JavaScript代码
const calculator = {
    currentValue: 0,
    previousValue: 0,
    operator: null
};
```

### 2. **增强选择器转换**：
```chtl
// CHTL JS代码
const display = {{#display}};              // ID选择器
const buttons = {{.number}};               // 类选择器
const firstBtn = {{button[0]}};             // 索引选择器
```
```javascript
// 转换后的JavaScript代码
const display = document.getElementById('display');
const buttons = document.querySelectorAll('.number');
const firstBtn = document.querySelectorAll('button')[0];
```

### 3. **箭头操作符转换**：
```chtl
// CHTL JS代码
calculator->currentValue = 100;
display->value = '0';
buttons->forEach(btn => btn->click());
```
```javascript
// 转换后的JavaScript代码
calculator.currentValue = 100;
display.value = '0';
buttons.forEach(btn => btn.click());
```

### 4. **listen事件监听转换**：
```chtl
// CHTL JS代码
{{.number}}->listen({
    click: function() {
        const num = this.textContent;
        inputNumber(num);
    },
    mouseenter: function() {
        this.style.background = '#e3f2fd';
    }
});
```
```javascript
// 转换后的JavaScript代码
document.querySelectorAll('.number').forEach(element => {
    element.addEventListener('click', function() {
        const num = this.textContent;
        inputNumber(num);
    });
    element.addEventListener('mouseenter', function() {
        this.style.background = '#e3f2fd';
    });
});
```

### 5. **delegate事件委托转换**：
```chtl
// CHTL JS代码
{{.btn}}->delegate({
    click: function() {
        this.style.transform = 'scale(0.95)';
    }
});
```
```javascript
// 转换后的JavaScript代码
document.addEventListener('click', function(e) {
    if (e.target.classList.contains('btn')) {
        e.target.style.transform = 'scale(0.95)';
    }
});
```

### 6. **animate动画转换**：
```chtl
// CHTL JS代码
animate({
    target: {{.calculator}},
    duration: 800,
    begin: { opacity: 0, transform: 'scale(0.8)' },
    end: { opacity: 1, transform: 'scale(1)' },
    easing: 'ease-out'
});
```
```javascript
// 转换后的JavaScript代码
const calculatorElement = document.querySelector('.calculator');
calculatorElement.animate([
    { opacity: 0, transform: 'scale(0.8) translateY(50px)' },
    { opacity: 1, transform: 'scale(1) translateY(0px)' }
], {
    duration: 800,
    easing: 'ease-out',
    fill: 'forwards'
});
```

## 🔥 **CHTL JS的优势**

### **1. 更简洁的语法**：
- `{{#id}}` 比 `document.getElementById('id')` 简洁
- `element->property` 比 `element.property` 更直观
- `listen({click: fn})` 比 `addEventListener('click', fn)` 更清晰

### **2. 强大的选择器**：
- 支持CSS选择器：`{{.class}}`, `{{#id}}`, `{{div.class}}`
- 支持索引访问：`{{button[0]}}`, `{{li[2]}}`
- 支持复杂选择器：`{{.container > .item:first-child}}`

### **3. 智能事件处理**：
- `listen` - 单元素事件监听
- `delegate` - 事件委托（性能更好）
- 支持所有JavaScript事件

### **4. 现代动画API**：
- 简化的动画语法
- 自动使用`requestAnimationFrame`
- 支持复杂的动画序列

### **5. 虚对象优化**：
- 编译时优化函数引用
- 减少运行时开销
- 更好的代码组织

## 🎊 **这个例子展示了什么？**

1. **完整的转换流程** - CHTL JS → JavaScript
2. **实际可用的应用** - 功能完整的计算器
3. **所有CHTL JS特性** - 虚对象、增强选择器、箭头操作符、事件系统、动画
4. **优雅的代码** - CHTL JS代码更简洁、易读
5. **性能优化** - 编译后的JavaScript代码高效

## 💡 **关键理解**

CHTL JS的价值在于：
- 🎯 **开发时**：写更简洁、直观的代码
- 🚀 **编译时**：转换成标准、优化的JavaScript
- 🔧 **运行时**：执行高效的原生JavaScript代码

这就是CHTL JS的真正魅力 - 让前端开发更简单、更优雅！✨