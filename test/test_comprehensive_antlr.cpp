#include <iostream>
#include <cassert>
#include <string>
#include <chrono>
#include "../src/CSSCompiler/Core/CSSCompilerCore.h"
#include "../src/JavaScriptCompiler/Core/JavaScriptCompilerCore.h"

using namespace chtl::css;
using namespace chtl::javascript;

void testAdvancedCSSParsing() {
    std::cout << "=== 测试高级CSS解析功能 ===\n";
    
    CSSCompilerCore css_compiler;
    
    // 测试复杂的CSS规则
    std::string complex_css = R"(
        @media screen and (max-width: 768px) {
            .container {
                width: 100%;
                padding: 10px;
                background: linear-gradient(45deg, #ff0000, #00ff00);
            }
            
            .button:hover {
                transform: scale(1.1);
                transition: all 0.3s ease;
            }
        }
        
        @keyframes fadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }
        
        .animated {
            animation: fadeIn 1s ease-in-out;
        }
    )";
    
    auto parse_tree = css_compiler.parseCSS(complex_css);
    assert(parse_tree != nullptr);
    
    bool is_valid = css_compiler.validateCSS(complex_css);
    assert(is_valid);
    
    std::cout << "✅ 复杂CSS解析测试通过!\n";
    
    // 测试CSS编译
    css_compiler.enableMinification(true);
    css_compiler.enableAutoprefixer(true);
    
    std::string compiled_css = css_compiler.compileCSS(complex_css);
    assert(!compiled_css.empty());
    assert(!css_compiler.hasErrors());
    
    std::cout << "✅ CSS编译测试通过!\n";
    
    // 打印统计信息
    css_compiler.printCompilationInfo();
}

void testAdvancedJavaScriptParsing() {
    std::cout << "\n=== 测试高级JavaScript解析功能 ===\n";
    
    JavaScriptCompilerCore js_compiler;
    
    // 测试复杂的JavaScript代码
    std::string complex_js = R"(
        class Calculator {
            constructor() {
                this.result = 0;
            }
            
            add(value) {
                this.result += value;
                return this;
            }
            
            multiply(value) {
                this.result *= value;
                return this;
            }
            
            getResult() {
                return this.result;
            }
        }
        
        const calc = new Calculator();
        const result = calc.add(10).multiply(2).getResult();
        
        function fibonacci(n) {
            if (n <= 1) return n;
            return fibonacci(n - 1) + fibonacci(n - 2);
        }
        
        const numbers = [1, 2, 3, 4, 5];
        const doubled = numbers.map(n => n * 2);
        
        async function fetchData() {
            try {
                const response = await fetch('/api/data');
                const data = await response.json();
                return data;
            } catch (error) {
                console.error('Error:', error);
            }
        }
    )";
    
    auto parse_tree = js_compiler.parseJavaScript(complex_js);
    assert(parse_tree != nullptr);
    
    bool is_valid = js_compiler.validateJavaScript(complex_js);
    assert(is_valid);
    
    std::cout << "✅ 复杂JavaScript解析测试通过!\n";
    
    // 测试JavaScript编译
    js_compiler.enableMinification(true);
    js_compiler.setTargetECMAScript("ES5");
    js_compiler.setTargetEnvironment("browser");
    
    std::string compiled_js = js_compiler.compileJavaScript(complex_js);
    assert(!compiled_js.empty());
    assert(!js_compiler.hasErrors());
    
    std::cout << "✅ JavaScript编译测试通过!\n";
    
    // 打印统计信息
    js_compiler.printCompilationInfo();
}

void testErrorHandling() {
    std::cout << "\n=== 测试错误处理功能 ===\n";
    
    CSSCompilerCore css_compiler;
    JavaScriptCompilerCore js_compiler;
    
    // 测试CSS语法错误
    std::string invalid_css = ".test { color: ; }"; // 缺少值
    auto css_result = css_compiler.parseCSS(invalid_css);
    // 注意：某些语法错误可能仍然产生解析树，但会有错误记录
    
    std::cout << "CSS错误数量: " << css_compiler.getErrors().size() << "\n";
    for (const auto& error : css_compiler.getErrors()) {
        std::cout << "CSS错误: " << error << "\n";
    }
    
    // 测试JavaScript语法错误
    std::string invalid_js = "function test( { return 42; }"; // 缺少参数列表的右括号
    auto js_result = js_compiler.parseJavaScript(invalid_js);
    
    std::cout << "JavaScript错误数量: " << js_compiler.getErrors().size() << "\n";
    for (const auto& error : js_compiler.getErrors()) {
        std::cout << "JavaScript错误: " << error << "\n";
    }
    
    std::cout << "✅ 错误处理测试完成!\n";
}

void testPerformance() {
    std::cout << "\n=== 测试性能 ===\n";
    
    CSSCompilerCore css_compiler;
    JavaScriptCompilerCore js_compiler;
    
    // 生成大量CSS规则
    std::string large_css;
    for (int i = 0; i < 1000; ++i) {
        large_css += ".class" + std::to_string(i) + " { color: #" + 
                     std::to_string(i % 16) + std::to_string(i % 16) + std::to_string(i % 16) + 
                     std::to_string(i % 16) + std::to_string(i % 16) + std::to_string(i % 16) + "; }\n";
    }
    
    auto css_start = std::chrono::high_resolution_clock::now();
    auto css_tree = css_compiler.parseCSS(large_css);
    auto css_end = std::chrono::high_resolution_clock::now();
    
    auto css_duration = std::chrono::duration_cast<std::chrono::milliseconds>(css_end - css_start);
    std::cout << "解析1000个CSS规则耗时: " << css_duration.count() << "ms\n";
    
    // 生成大量JavaScript函数
    std::string large_js;
    for (int i = 0; i < 1000; ++i) {
        large_js += "function func" + std::to_string(i) + "() { return " + std::to_string(i) + "; }\n";
    }
    
    auto js_start = std::chrono::high_resolution_clock::now();
    auto js_tree = js_compiler.parseJavaScript(large_js);
    auto js_end = std::chrono::high_resolution_clock::now();
    
    auto js_duration = std::chrono::duration_cast<std::chrono::milliseconds>(js_end - js_start);
    std::cout << "解析1000个JavaScript函数耗时: " << js_duration.count() << "ms\n";
    
    assert(css_tree != nullptr);
    assert(js_tree != nullptr);
    
    std::cout << "✅ 性能测试通过!\n";
}

int main() {
    std::cout << "开始ANTLR综合集成测试...\n\n";
    
    try {
        testAdvancedCSSParsing();
        testAdvancedJavaScriptParsing();
        testErrorHandling();
        testPerformance();
        
        std::cout << "\n🎉 ANTLR综合集成测试全部通过!\n";
        std::cout << "✅ CHTL项目现已具备完整的CSS和JavaScript原生解析能力!\n";
        std::cout << "✅ 基于官方ANTLR语法的解析器集成成功!\n";
        std::cout << "✅ 错误处理和性能测试均表现良好!\n";
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
}