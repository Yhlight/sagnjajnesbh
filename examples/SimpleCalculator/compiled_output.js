// ========================================
// 🚀 CHTL JS编译器输出 - 转换后的标准JavaScript代码
// 这是CHTL JS代码经过编译器处理后的结果
// ========================================

// CHTL JS虚对象转换
// vir calculator = {...} → 普通JavaScript对象
const calculator = {
    currentValue: 0,
    previousValue: 0,
    operator: null,
    waitingForOperand: false,
    history: []
};

// CHTL JS增强选择器转换
// {{#display}} → document.getElementById('display')
const display = document.getElementById('display');
const historyList = document.getElementById('history-list');

// CHTL JS箭头操作符转换
// calculator->waitingForOperand → calculator.waitingForOperand
// display->value → display.value

// 输入数字函数
function inputNumber(num) {
    if (calculator.waitingForOperand) {
        display.value = String(num);
        calculator.waitingForOperand = false;
    } else {
        display.value = display.value === '0' ? String(num) : display.value + num;
    }
    calculator.currentValue = parseFloat(display.value);
}

// 输入操作符
function inputOperator(nextOperator) {
    const inputValue = parseFloat(display.value);
    
    if (calculator.previousValue === null) {
        calculator.previousValue = inputValue;
    } else if (calculator.operator) {
        const currentValue = calculator.currentValue || 0;
        const newValue = performCalculation();
        
        display.value = String(newValue);
        calculator.currentValue = newValue;
        calculator.previousValue = newValue;
    }
    
    calculator.waitingForOperand = true;
    calculator.operator = nextOperator;
}

// 执行计算
function performCalculation() {
    const prev = calculator.previousValue;
    const current = calculator.currentValue;
    const operator = calculator.operator;
    
    if (prev === null || current === null || !operator) {
        return current;
    }
    
    let result;
    switch (operator) {
        case '+': result = prev + current; break;
        case '-': result = prev - current; break;
        case '×': result = prev * current; break;
        case '÷': result = current !== 0 ? prev / current : 0; break;
        case '%': result = prev % current; break;
        default: return current;
    }
    
    // 添加到历史记录
    const historyEntry = `${prev} ${operator} ${current} = ${result}`;
    calculator.history.push(historyEntry);
    updateHistory();
    
    return result;
}

// 更新历史显示
function updateHistory() {
    if (calculator.history.length === 0) {
        historyList.innerHTML = '暂无计算记录';
        return;
    }
    
    let historyHTML = '';
    // 显示最近5条记录
    const recentHistory = calculator.history.slice(-5);
    recentHistory.forEach((entry, index) => {
        historyHTML += `<div style="padding: 5px; ${index === recentHistory.length - 1 ? 'font-weight: bold; color: #667eea;' : ''}">${entry}</div>`;
    });
    
    historyList.innerHTML = historyHTML;
}

// 重置计算器
function resetCalculator() {
    calculator.currentValue = 0;
    calculator.previousValue = null;
    calculator.operator = null;
    calculator.waitingForOperand = false;
    display.value = '0';
}

// 退格
function backspace() {
    const currentDisplay = display.value;
    if (currentDisplay.length > 1) {
        display.value = currentDisplay.slice(0, -1);
    } else {
        display.value = '0';
    }
    calculator.currentValue = parseFloat(display.value);
}

// CHTL JS listen转换 → addEventListener
// {{.number}}->listen({click: function() {...}}) → 
document.querySelectorAll('.number').forEach(button => {
    button.addEventListener('click', function() {
        const num = this.textContent;
        inputNumber(num);
    });
});

// 操作符按钮事件转换
document.getElementById('add').addEventListener('click', () => inputOperator('+'));
document.getElementById('subtract').addEventListener('click', () => inputOperator('-'));
document.getElementById('multiply').addEventListener('click', () => inputOperator('×'));
document.getElementById('divide').addEventListener('click', () => inputOperator('÷'));
document.getElementById('percent').addEventListener('click', () => inputOperator('%'));

// 特殊按钮事件转换
document.getElementById('equals').addEventListener('click', function() {
    const result = performCalculation();
    display.value = String(result);
    calculator.currentValue = result;
    calculator.previousValue = null;
    calculator.operator = null;
    calculator.waitingForOperand = true;
});

document.getElementById('clear').addEventListener('click', resetCalculator);
document.getElementById('backspace').addEventListener('click', backspace);

document.getElementById('decimal').addEventListener('click', function() {
    if (display.value.indexOf('.') === -1) {
        inputNumber('.');
    }
});

// CHTL JS键盘监听转换
document.addEventListener('keydown', function(e) {
    e.preventDefault();
    
    if (e.key >= '0' && e.key <= '9') {
        inputNumber(e.key);
    } else if (['+', '-', '*', '/', '%'].includes(e.key)) {
        const operatorMap = {'+': '+', '-': '-', '*': '×', '/': '÷', '%': '%'};
        inputOperator(operatorMap[e.key]);
    } else if (e.key === 'Enter' || e.key === '=') {
        document.getElementById('equals').click();
    } else if (e.key === 'Escape' || e.key === 'c' || e.key === 'C') {
        resetCalculator();
    } else if (e.key === 'Backspace') {
        backspace();
    } else if (e.key === '.') {
        document.getElementById('decimal').click();
    }
});

// CHTL JS animate转换 → 标准动画API
// animate({target: {{.calculator}}, ...}) → 
const calculatorElement = document.querySelector('.calculator');
calculatorElement.animate([
    { opacity: 0, transform: 'scale(0.8) translateY(50px)' },
    { opacity: 1, transform: 'scale(1) translateY(0px)' }
], {
    duration: 800,
    easing: 'ease-out',
    fill: 'forwards'
});

// CHTL JS delegate转换 → 事件委托
// {{.btn}}->delegate({click: function() {...}}) →
document.addEventListener('click', function(e) {
    if (e.target.classList.contains('btn')) {
        // 按钮点击反馈
        e.target.style.transform = 'scale(0.95)';
        setTimeout(() => {
            e.target.style.transform = '';
        }, 100);
        
        // 模拟按键音效
        console.log('🔊 按键音效: ' + e.target.textContent);
    }
});

console.log('🧮 CHTL JS计算器已启动！');
console.log('🎹 支持键盘输入：数字、运算符、Enter、Escape、Backspace');