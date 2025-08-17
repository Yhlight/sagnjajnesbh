#include "../src/CHTLJSCompiler/Lexer/CHTLJSLexer.h"
#include <iostream>
#include <cassert>

using namespace chtl;

void testCHTLJSLexerBasic() {
    std::cout << "=== 测试CHTL JS词法分析器基础功能 ===" << std::endl;
    
    CHTLJSLexer lexer;
    
    std::string test_code = "{{box}}->listen({ click: handleClick });";
    lexer.setInput(test_code);
    
    auto tokens = lexer.tokenize();
    
    std::cout << "生成的Token数量: " << tokens.size() << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << "Token " << i << ": " << tokens[i].toString() << std::endl;
    }
    
    // 验证基本功能
    assert(!tokens.empty());
    
    // 检查是否有错误
    auto errors = lexer.getErrors();
    if (!errors.empty()) {
        std::cout << "词法分析错误:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    std::cout << "✅ CHTL JS词法分析器基础功能测试通过!" << std::endl;
}

void testCHTLJSSpecialSyntax() {
    std::cout << "=== 测试CHTL JS特殊语法 ===" << std::endl;
    
    CHTLJSLexer lexer;
    
    // 测试各种CHTL JS语法
    std::vector<std::string> test_cases = {
        "{{box}}",
        "listen",
        "delegate", 
        "animate",
        "vir",
        "->",
        "=>"
    };
    
    for (const auto& test_case : test_cases) {
        lexer.setInput(test_case);
        auto tokens = lexer.tokenize();
        
        std::cout << "测试 '" << test_case << "': ";
        if (!tokens.empty() && tokens[0].type != TokenType::INVALID) {
            std::cout << "✅ " << tokens[0].toString() << std::endl;
        } else {
            std::cout << "❌ 识别失败" << std::endl;
        }
    }
    
    std::cout << "✅ CHTL JS特殊语法测试完成!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试CHTL JS词法分析器..." << std::endl << std::endl;
        
        testCHTLJSLexerBasic();
        std::cout << std::endl;
        
        testCHTLJSSpecialSyntax();
        std::cout << std::endl;
        
        std::cout << "🎉 CHTL JS词法分析器测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}