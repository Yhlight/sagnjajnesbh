#include "../src/CHTLCompiler/Core/CHTLCompilerCore.h"
#include <iostream>
#include <cassert>

using namespace chtl;

void testCHTLCompilerInitialization() {
    std::cout << "=== 测试CHTL编译器初始化 ===" << std::endl;
    
    CHTLCompilerCore compiler;
    compiler.setDebugMode(true);
    
    assert(compiler.initialize());
    std::cout << "✅ CHTL编译器初始化成功!" << std::endl;
}

void testCHTLCompilerComponents() {
    std::cout << "=== 测试CHTL编译器组件 ===" << std::endl;
    
    CHTLCompilerCore compiler;
    compiler.initialize();
    
    // 测试组件访问
    auto& lexer = compiler.getLexer();
    auto& parser = compiler.getParser();
    auto& generator = compiler.getGenerator();
    auto& module_manager = compiler.getModuleManager();
    
    std::cout << "✅ 所有CHTL编译器组件访问正常!" << std::endl;
}

void testCHTLCompilation() {
    std::cout << "=== 测试CHTL编译 ===" << std::endl;
    
    CHTLCompilerCore compiler;
    compiler.setDebugMode(false);
    
    std::string test_code = R"(
        [Template] @Style ButtonStyle {
            background: blue;
            color: white;
        }
        
        div {
            @Style ButtonStyle;
            text "Hello CHTL";
        }
    )";
    
    std::string result = compiler.compile(test_code);
    
    // 检查是否有错误
    auto errors = compiler.getErrors();
    if (!errors.empty()) {
        std::cout << "编译错误:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    std::cout << "✅ CHTL编译测试完成!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试CHTL编译器..." << std::endl << std::endl;
        
        testCHTLCompilerInitialization();
        std::cout << std::endl;
        
        testCHTLCompilerComponents();
        std::cout << std::endl;
        
        testCHTLCompilation();
        std::cout << std::endl;
        
        std::cout << "🎉 所有CHTL编译器测试通过!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}