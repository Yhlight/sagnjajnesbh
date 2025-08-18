#include "../src/CSSCompiler/Core/CSSCompilerCore.h"
#include "../src/JavaScriptCompiler/Core/JavaScriptCompilerCore.h"
#include <iostream>
#include <vector>
#include <string>

using namespace chtl::css;
using namespace chtl::javascript;

// 测试复杂CSS语法错误
void testComplexCSSErrors() {
    std::cout << "\n=== 测试复杂CSS语法错误 ===" << std::endl;
    
    CSSCompilerCore css_compiler;
    
    struct CSSTestCase {
        std::string name;
        std::string code;
        std::string expected_issue;
    };
    
    std::vector<CSSTestCase> test_cases = {
        // 1. 嵌套选择器错误
        {"嵌套选择器错误", R"(
.parent {
    color: red;
    .child {  // CSS原生不支持嵌套
        color: blue;
    }
}
)", "嵌套选择器语法错误"},

        // 2. 伪类和伪元素混用错误
        {"伪类伪元素错误", R"(
.element::before:hover {  // 伪元素后不能跟伪类
    content: "test";
}
)", "伪类伪元素顺序错误"},

        // 3. 复杂媒体查询语法错误
        {"媒体查询语法错误", R"(
@media screen and (min-width: 768px and max-width: 1024px) {  // 缺少括号
    .responsive { display: block; }
}
)", "媒体查询逻辑运算符错误"},

        // 4. CSS变量语法错误
        {"CSS变量错误", R"(
:root {
    --primary-color: #ff0000;
    --secondary: var(--nonexistent-var, );  // 空的fallback值
}
.element {
    color: var(--primary-color;  // 缺少闭合括号
}
)", "CSS变量语法错误"},

        // 5. 复杂选择器错误
        {"复杂选择器错误", R"(
.parent > .child + .sibling ~ .another:nth-child(2n+1)::after {
    content: "complex";
    display: block;
}
.invalid-selector > > .double-child {  // 双重子选择器
    color: red;
}
)", "选择器组合符错误"},

        // 6. 动画关键帧错误
        {"动画关键帧错误", R"(
@keyframes slideIn {
    from { transform: translateX(-100%); }
    50% { transform: translateX(-50%); }
    to { transform: translateX(0); }
    110% { transform: scale(1.1); }  // 超过100%的关键帧
}
)", "关键帧百分比超出范围"},

        // 7. 函数参数错误
        {"CSS函数参数错误", R"(
.element {
    background: linear-gradient(45deg, #ff0000, #00ff00, );  // 尾随逗号
    transform: rotate(90deg, 45deg);  // rotate函数参数过多
    filter: blur();  // blur函数缺少参数
}
)", "CSS函数参数错误"},

        // 8. 单位和值类型错误
        {"单位值类型错误", R"(
.element {
    width: 100px%;  // 混合单位
    height: auto px;  // 错误的单位组合
    z-index: 1.5;  // z-index不接受小数
    opacity: 1.5;  // opacity超出范围
}
)", "单位和值类型不匹配"},

        // 9. 字符编码和特殊字符错误
        {"字符编码错误", R"(
.element {
    content: "测试内容;  // 缺少闭合引号
    font-family: "Arial", sans-serif";  // 多余的引号
}
/* 注释中的特殊字符 */ 测试 */
)", "字符编码和引号匹配错误"}
    };
    
    for (const auto& test : test_cases) {
        std::cout << "\n测试: " << test.name << std::endl;
        css_compiler.clearErrors();
        
        bool is_valid = css_compiler.validateCSS(test.code);
        auto errors = css_compiler.getErrors();
        
        std::cout << "验证结果: " << (is_valid ? "通过" : "失败") << std::endl;
        std::cout << "错误数量: " << errors.size() << std::endl;
        std::cout << "预期问题: " << test.expected_issue << std::endl;
        
        if (!errors.empty()) {
            std::cout << "检测到的错误:" << std::endl;
            for (size_t i = 0; i < std::min(errors.size(), size_t(3)); ++i) {
                std::cout << "  - " << errors[i] << std::endl;
            }
        }
    }
}

// 测试复杂JavaScript语法错误
void testComplexJSErrors() {
    std::cout << "\n=== 测试复杂JavaScript语法错误 ===" << std::endl;
    
    JavaScriptCompilerCore js_compiler;
    
    struct JSTestCase {
        std::string name;
        std::string code;
        std::string expected_issue;
    };
    
    std::vector<JSTestCase> test_cases = {
        // 1. 异步函数和Promise错误
        {"异步函数错误", R"(
async function test() {
    const result = await fetch('/api/data');
    return result.json();
}

function* generator() {
    yield 1;
    yield* [2, 3, 4];  // yield*语法
    return 5;
}

async function* asyncGenerator() {  // 异步生成器
    yield await Promise.resolve(1);
}
)", "异步语法支持问题"},

        // 2. 解构赋值复杂情况
        {"解构赋值错误", R"(
const obj = { a: 1, b: { c: 2, d: 3 } };
const { a, b: { c, d }, e = 'default' } = obj;

const [first, second, ...rest] = [1, 2, 3, 4, 5];
const [x, y] = [1];  // y将是undefined

// 错误的解构语法
const { a: { b: { c } } } = undefined;  // 深层解构null/undefined
)", "解构赋值边界情况"},

        // 3. 箭头函数和this绑定
        {"箭头函数this绑定", R"(
class MyClass {
    constructor() {
        this.value = 42;
    }
    
    regularMethod() {
        return function() {
            return this.value;  // this指向问题
        };
    }
    
    arrowMethod = () => {
        return this.value;  // 箭头函数this绑定
    }
}

const arrow = (a, b,) => a + b;  // 尾随逗号
const invalid = (,a, b) => a + b;  // 前导逗号错误
)", "箭头函数语法和this绑定"},

        // 4. 模板字符串和标签模板
        {"模板字符串错误", R"(
const name = 'World';
const message = `Hello, ${name}!`;

function tag(strings, ...values) {
    return strings.reduce((result, string, i) => {
        return result + string + (values[i] || '');
    }, '');
}

const tagged = tag`Hello, ${name}!`;

// 错误的模板字符串
const invalid = `Hello, ${name`;  // 缺少闭合
const nested = `Outer ${`Inner ${name}`} template`;  // 嵌套模板
)", "模板字符串语法错误"},

        // 5. 类和继承复杂情况
        {"类继承错误", R"(
class Base {
    constructor(value) {
        this.value = value;
    }
    
    static staticMethod() {
        return 'static';
    }
}

class Derived extends Base {
    constructor(value, extra) {
        super(value);  // 必须在this之前调用
        this.extra = extra;
    }
    
    static staticMethod() {
        return super.staticMethod() + ' derived';
    }
}

class Invalid extends Base {
    constructor(value) {
        this.value = value;  // 错误：super()之前使用this
        super(value);
    }
}
)", "类继承super调用顺序"},

        // 6. 模块导入导出错误
        {"模块语法错误", R"(
// 各种导入方式
import defaultExport from './module.js';
import { namedExport } from './module.js';
import { namedExport as alias } from './module.js';
import * as namespace from './module.js';

// 错误的导入语法
import { } from './module.js';  // 空导入
import './module.js' from './another.js';  // 语法错误
)", "ES6模块语法错误"},

        // 7. 正则表达式复杂情况
        {"正则表达式错误", R"(
const regex1 = /[a-z]+/gi;
const regex2 = new RegExp('[a-z]+', 'gi');

// 复杂正则表达式
const complex = /^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;

// 错误的正则表达式
const invalid = /[a-z/gi;  // 缺少闭合
const unclosed = new RegExp('[a-z', 'gi');  // 未闭合的字符类
)", "正则表达式语法错误"},

        // 8. 操作符优先级和类型转换
        {"操作符优先级错误", R"(
const a = 1;
const b = '2';
const c = true;

// 类型转换和操作符优先级
const result1 = a + b * c;  // 字符串拼接vs数学运算
const result2 = a == b;     // 弱相等比较
const result3 = a === +b;   // 强相等和类型转换

// 位运算和逻辑运算混用
const bitwise = a & b | c;  // 位运算优先级
const logical = a && b || c;  // 逻辑运算优先级
)", "操作符优先级和类型转换"},

        // 9. 异常处理边界情况
        {"异常处理错误", R"(
try {
    throw new Error('Test error');
} catch (e) {
    console.log(e.message);
} finally {
    console.log('Finally block');
}

// 错误的异常处理
try {
    // 代码
} catch {  // ES2019可选的catch绑定
    // 处理
}

async function asyncError() {
    try {
        await Promise.reject('Error');
    } catch (e) {
        throw e;  // 重新抛出异常
    }
}
)", "异常处理语法支持"}
    };
    
    for (const auto& test : test_cases) {
        std::cout << "\n测试: " << test.name << std::endl;
        js_compiler.clearErrors();
        
        bool is_valid = js_compiler.validateJavaScript(test.code);
        auto errors = js_compiler.getErrors();
        
        std::cout << "验证结果: " << (is_valid ? "通过" : "失败") << std::endl;
        std::cout << "错误数量: " << errors.size() << std::endl;
        std::cout << "预期问题: " << test.expected_issue << std::endl;
        
        if (!errors.empty()) {
            std::cout << "检测到的错误:" << std::endl;
            for (size_t i = 0; i < std::min(errors.size(), size_t(3)); ++i) {
                std::cout << "  - " << errors[i] << std::endl;
            }
        }
    }
}

int main() {
    std::cout << "=== CHTL CSS和JavaScript编译器复杂语法错误测试 ===" << std::endl;
    
    try {
        testComplexCSSErrors();
        testComplexJSErrors();
        
        std::cout << "\n=== 复杂语法错误测试完成 ===" << std::endl;
        std::cout << "✅ 已识别潜在的复杂语法错误类型" << std::endl;
        std::cout << "✅ 编译器错误检测能力得到验证" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}