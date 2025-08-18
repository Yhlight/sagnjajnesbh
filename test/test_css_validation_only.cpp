#include "../src/CSSCompiler/Core/CSSCompilerCore.h"
#include <iostream>

using namespace chtl::css;

int main() {
    try {
        std::cout << "Testing CSS validation only..." << std::endl;
        
        CSSCompilerCore css_compiler;
        
        // 测试基本CSS验证（不使用树遍历器）
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
        
        std::cout << "Testing CSS validation..." << std::endl;
        bool is_valid = css_compiler.validateCSS(valid_css);
        std::cout << "CSS validation result: " << (is_valid ? "valid" : "invalid") << std::endl;
        
        if (css_compiler.hasErrors()) {
            std::cout << "Errors found:" << std::endl;
            for (const auto& error : css_compiler.getErrors()) {
                std::cout << "  - " << error << std::endl;
            }
        } else {
            std::cout << "No errors found!" << std::endl;
        }
        
        // 测试无效CSS
        std::string invalid_css = R"(
.button {
    color red;  // 缺少冒号
    background-color: ;  // 缺少值
}
)";
        
        std::cout << "Testing invalid CSS..." << std::endl;
        css_compiler.clearErrors();
        bool invalid_result = css_compiler.validateCSS(invalid_css);
        std::cout << "Invalid CSS validation result: " << (invalid_result ? "valid" : "invalid") << std::endl;
        
        if (css_compiler.hasErrors()) {
            std::cout << "Errors found (expected):" << std::endl;
            for (const auto& error : css_compiler.getErrors()) {
                std::cout << "  - " << error << std::endl;
            }
        }
        
        std::cout << "CSS validation tests completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return 1;
    }
}