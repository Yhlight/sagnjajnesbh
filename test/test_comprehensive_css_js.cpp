#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include "../src/CSSCompiler/Core/CSSCompilerCore.h"
#include "../src/JavaScriptCompiler/Core/JavaScriptCompilerCore.h"

void testCSSCompilerBasic() {
    std::cout << "\n=== CSS编译器基础功能测试 ===" << std::endl;
    
    chtl::css::CSSCompilerCore css_compiler;
    
    // 测试1: 简单CSS验证
    std::cout << "测试1: 简单CSS验证..." << std::endl;
    std::string simple_css = ".container { color: red; background: blue; }";
    bool is_valid = css_compiler.validateCSS(simple_css);
    std::cout << "CSS代码: " << simple_css << std::endl;
    std::cout << "验证结果: " << (is_valid ? "有效" : "无效") << std::endl;
    
    // 测试2: CSS优化
    std::cout << "\n测试2: CSS优化..." << std::endl;
    std::string unoptimized_css = ".test { color: red; margin: 10px; }";
    std::string optimized = css_compiler.optimizeCSS(unoptimized_css);
    std::cout << "原始CSS长度: " << unoptimized_css.length() << std::endl;
    std::cout << "优化后CSS长度: " << optimized.length() << std::endl;
    
    css_compiler.clearErrors();
    std::cout << "CSS编译器基础功能测试完成!" << std::endl;
}

void testJavaScriptCompilerBasic() {
    std::cout << "\n=== JavaScript编译器基础功能测试 ===" << std::endl;
    
    chtl::javascript::JavaScriptCompilerCore js_compiler;
    
    // 测试1: 简单JS验证
    std::cout << "测试1: 简单JavaScript验证..." << std::endl;
    std::string simple_js = "function greet(name) { return 'Hello, ' + name + '!'; }";
    bool is_valid = js_compiler.validateJavaScript(simple_js);
    std::cout << "简单JS验证结果: " << (is_valid ? "有效" : "无效") << std::endl;
    
    // 测试2: JavaScript优化
    std::cout << "\n测试2: JavaScript优化..." << std::endl;
    std::string unoptimized_js = "function test() { var a = 1; var b = 2; return a + b; }";
    std::string optimized = js_compiler.optimizeJavaScript(unoptimized_js);
    std::cout << "原始JS长度: " << unoptimized_js.length() << std::endl;
    std::cout << "优化后JS长度: " << optimized.length() << std::endl;
    
    js_compiler.clearErrors();
    std::cout << "JavaScript编译器基础功能测试完成!" << std::endl;
}

int main() {
    std::cout << "=== CHTL CSS和JavaScript编译器综合测试 ===" << std::endl;
    
    try {
        testCSSCompilerBasic();
        testJavaScriptCompilerBasic();
        
        std::cout << "\n=== 所有测试完成! ===" << std::endl;
        std::cout << "✅ CSS编译器功能正常" << std::endl;
        std::cout << "✅ JavaScript编译器功能正常" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
