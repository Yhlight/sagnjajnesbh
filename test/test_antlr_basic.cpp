#include "../src/CSSCompiler/Core/CSSCompilerCore.h"
#include "../src/JavaScriptCompiler/Core/JavaScriptCompilerCore.h"
#include <iostream>

using namespace chtl::css;
using namespace chtl::javascript;

int main() {
    try {
        std::cout << "Testing basic ANTLR initialization..." << std::endl;
        
        // 测试CSS编译器创建
        std::cout << "Creating CSS compiler..." << std::endl;
        CSSCompilerCore css_compiler;
        std::cout << "CSS compiler created successfully!" << std::endl;
        
        // 测试JavaScript编译器创建
        std::cout << "Creating JavaScript compiler..." << std::endl;
        JavaScriptCompilerCore js_compiler;
        std::cout << "JavaScript compiler created successfully!" << std::endl;
        
        // 测试简单的CSS验证
        std::cout << "Testing simple CSS validation..." << std::endl;
        std::string simple_css = ".test { color: red; }";
        bool css_valid = css_compiler.validateCSS(simple_css);
        std::cout << "CSS validation result: " << (css_valid ? "valid" : "invalid") << std::endl;
        
        if (css_compiler.hasErrors()) {
            std::cout << "CSS errors:" << std::endl;
            for (const auto& error : css_compiler.getErrors()) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        // 测试简单的JavaScript验证
        std::cout << "Testing simple JavaScript validation..." << std::endl;
        std::string simple_js = "var x = 5;";
        bool js_valid = js_compiler.validateJavaScript(simple_js);
        std::cout << "JavaScript validation result: " << (js_valid ? "valid" : "invalid") << std::endl;
        
        if (js_compiler.hasErrors()) {
            std::cout << "JavaScript errors:" << std::endl;
            for (const auto& error : js_compiler.getErrors()) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        std::cout << "Basic ANTLR tests completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return 1;
    }
}