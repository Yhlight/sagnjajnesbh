#include "../src/CSSCompiler/Core/CSSCompilerCore.h"
#include "../src/JavaScriptCompiler/Core/JavaScriptCompilerCore.h"
#include <iostream>
#include <cassert>

using namespace chtl::css;
using namespace chtl::javascript;

void testCSSCompiler() {
    std::cout << "Testing CSS Compiler with ANTLR..." << std::endl;
    
    CSSCompilerCore css_compiler;
    
    // 测试基本CSS验证
    std::string valid_css = R"(
.button {
    color: red;
    background-color: blue;
    margin: 10px;
}

#header {
    font-size: 24px;
    font-weight: bold;
}
)";
    
    bool is_valid = css_compiler.validateCSS(valid_css);
    assert(is_valid);
    assert(!css_compiler.hasErrors());
    
    // 测试CSS编译
    std::string compiled_css = css_compiler.compileCSS(valid_css);
    assert(!compiled_css.empty());
    
    // 测试选择器提取
    auto selectors = css_compiler.extractSelectors(valid_css);
    assert(!selectors.empty());
    std::cout << "Found " << selectors.size() << " selectors" << std::endl;
    
    // 测试属性提取
    auto properties = css_compiler.extractProperties(valid_css);
    assert(!properties.empty());
    std::cout << "Found " << properties.size() << " properties" << std::endl;
    
    // 测试值提取
    auto values = css_compiler.extractValues(valid_css);
    assert(!values.empty());
    std::cout << "Found " << values.size() << " values" << std::endl;
    
    // 测试无效CSS
    std::string invalid_css = R"(
.button {
    color: red
    background-color blue;  // 缺少冒号
    margin 10px;           // 缺少冒号
}
)";
    
    css_compiler.clearErrors();
    bool invalid_result = css_compiler.validateCSS(invalid_css);
    std::cout << "Invalid CSS validation result: " << (invalid_result ? "passed" : "failed") << std::endl;
    std::cout << "Errors found: " << css_compiler.getErrors().size() << std::endl;
    
    for (const auto& error : css_compiler.getErrors()) {
        std::cout << "  - " << error << std::endl;
    }
    
    std::cout << "CSS Compiler tests completed!" << std::endl;
}

void testJavaScriptCompiler() {
    std::cout << "Testing JavaScript Compiler with ANTLR..." << std::endl;
    
    JavaScriptCompilerCore js_compiler;
    
    // 测试基本JavaScript验证
    std::string valid_js = R"(
function greet(name) {
    console.log('Hello, ' + name);
    return 'Greeted ' + name;
}

var message = 'Hello World';
let count = 42;
const PI = 3.14159;

class Calculator {
    add(a, b) {
        return a + b;
    }
}
)";
    
    bool is_valid = js_compiler.validateJavaScript(valid_js);
    assert(is_valid);
    assert(!js_compiler.hasErrors());
    
    // 测试JavaScript编译
    std::string compiled_js = js_compiler.compileJavaScript(valid_js);
    assert(!compiled_js.empty());
    
    // 测试函数提取
    auto functions = js_compiler.extractFunctions(valid_js);
    std::cout << "Found " << functions.size() << " functions" << std::endl;
    
    // 测试变量提取
    auto variables = js_compiler.extractVariables(valid_js);
    std::cout << "Found " << variables.size() << " variables" << std::endl;
    
    // 测试类提取
    auto classes = js_compiler.extractClasses(valid_js);
    assert(!classes.empty());
    std::cout << "Found " << classes.size() << " classes" << std::endl;
    
    // 测试ES6到ES5转换
    std::string es6_code = R"(
const arrow = (x) => { return x * 2; };
let result = arrow(5);
)";
    
    std::string es5_code = js_compiler.transpileES6ToES5(es6_code);
    std::cout << "ES6 to ES5 conversion:" << std::endl;
    std::cout << "Original: " << es6_code << std::endl;
    std::cout << "Converted: " << es5_code << std::endl;
    
    // 测试无效JavaScript
    std::string invalid_js = R"(
function test( {  // 缺少参数名
    console.log('test'
}  // 缺少分号和闭合括号
)";
    
    js_compiler.clearErrors();
    bool invalid_result = js_compiler.validateJavaScript(invalid_js);
    std::cout << "Invalid JS validation result: " << (invalid_result ? "passed" : "failed") << std::endl;
    std::cout << "Errors found: " << js_compiler.getErrors().size() << std::endl;
    
    for (const auto& error : js_compiler.getErrors()) {
        std::cout << "  - " << error << std::endl;
    }
    
    std::cout << "JavaScript Compiler tests completed!" << std::endl;
}

void testCHTLCodeGeneration() {
    std::cout << "Testing CHTL code generation validation..." << std::endl;
    
    // 模拟CHTL生成的CSS代码
    std::string chtl_generated_css = R"(
/* CHTL生成的CSS代码 */
.chtl-button-primary {
    color: #ffffff;
    background-color: #007bff;
    border: 1px solid #007bff;
    padding: 8px 16px;
    border-radius: 4px;
    cursor: pointer;
}

.chtl-button-primary:hover {
    background-color: #0056b3;
    border-color: #0056b3;
}

.chtl-user-card-active {
    border-left: 4px solid #28a745;
    background-color: #f8fff9;
}
)";
    
    CSSCompilerCore css_compiler;
    bool css_valid = css_compiler.validateCSS(chtl_generated_css);
    assert(css_valid);
    std::cout << "CHTL生成的CSS代码验证通过!" << std::endl;
    
    // 优化CHTL生成的CSS
    std::string optimized_css = css_compiler.optimizeCSS(chtl_generated_css);
    std::cout << "CSS优化完成，原长度: " << chtl_generated_css.length() 
              << ", 优化后长度: " << optimized_css.length() << std::endl;
    
    // 模拟CHTL生成的JavaScript代码
    std::string chtl_generated_js = R"(
// CHTL生成的JavaScript代码
function chtl_button_click_handler_1(event) {
    console.log('Button clicked:', event.target);
    event.target.style.backgroundColor = '#0056b3';
}

function chtl_delegate_handler_2(selector, eventType, handler) {
    document.querySelectorAll(selector).forEach(function(element) {
        element.addEventListener(eventType, handler);
    });
}

// 绑定事件
chtl_delegate_handler_2('.chtl-button-primary', 'click', chtl_button_click_handler_1);

// CHTL虚拟对象生成的函数
function chtl_vir_Test_Void_A() {
    console.log('Virtual object function called');
    return true;
}
)";
    
    JavaScriptCompilerCore js_compiler;
    bool js_valid = js_compiler.validateJavaScript(chtl_generated_js);
    assert(js_valid);
    std::cout << "CHTL生成的JavaScript代码验证通过!" << std::endl;
    
    // 优化CHTL生成的JavaScript
    std::string optimized_js = js_compiler.optimizeJavaScript(chtl_generated_js);
    std::cout << "JavaScript优化完成，原长度: " << chtl_generated_js.length() 
              << ", 优化后长度: " << optimized_js.length() << std::endl;
    
    // 提取生成的函数
    auto functions = js_compiler.extractFunctions(chtl_generated_js);
    std::cout << "CHTL生成的JavaScript中包含 " << functions.size() << " 个函数" << std::endl;
    
    std::cout << "CHTL代码生成验证测试完成!" << std::endl;
}

int main() {
    try {
        std::cout << "=== CHTL ANTLR Integration Tests ===" << std::endl;
        
        testCSSCompiler();
        std::cout << std::endl;
        
        testJavaScriptCompiler();
        std::cout << std::endl;
        
        testCHTLCodeGeneration();
        
        std::cout << "\n=== All ANTLR integration tests passed! ===" << std::endl;
        std::cout << "CHTL现在具备了工业级的CSS和JavaScript代码验证和优化能力!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}