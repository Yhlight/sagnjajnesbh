#include <iostream>
#include <fstream>
#include "CHTLCompiler/Lexer/CHTLLexer.h"

using namespace chtl;

int main() {
    std::cout << "🔍 测试CHTLLexer重复使用" << std::endl;
    std::cout << "========================" << std::endl;

    try {
        CHTLLexer lexer;
        
        std::string testCode = R"(
html {
    head {
        title { text { Test } }
    }
    body {
        div {
            class: container;
            h1 { text { Hello World } }
            p { text { This is a test } }
        }
    }
}
)";
        
        // 第一次使用
        std::cout << "第一次词法分析..." << std::endl;
        lexer.setInput(testCode);
        auto tokens1 = lexer.tokenize();
        std::cout << "✅ 第一次成功，生成 " << tokens1.size() << " 个token" << std::endl;
        
        // 第二次使用同一个lexer
        std::cout << "第二次词法分析（重复使用）..." << std::endl;
        lexer.setInput(testCode);
        auto tokens2 = lexer.tokenize();
        std::cout << "✅ 第二次成功，生成 " << tokens2.size() << " 个token" << std::endl;
        
        // 第三次使用不同的代码
        std::cout << "第三次词法分析（不同代码）..." << std::endl;
        std::string differentCode = "div { h1 { text { Different } } }";
        lexer.setInput(differentCode);
        auto tokens3 = lexer.tokenize();
        std::cout << "✅ 第三次成功，生成 " << tokens3.size() << " 个token" << std::endl;
        
        // 测试大型代码
        std::cout << "第四次词法分析（大型代码）..." << std::endl;
        std::ifstream file("../test_large_webpage_simple.chtl");
        if (file.is_open()) {
            std::string largeCode((std::istreambuf_iterator<char>(file)),
                                  std::istreambuf_iterator<char>());
            file.close();
            
            lexer.setInput(largeCode);
            auto tokens4 = lexer.tokenize();
            std::cout << "✅ 大型代码成功，生成 " << tokens4.size() << " 个token" << std::endl;
        } else {
            std::cout << "❌ 无法读取大型代码文件" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "🎉 CHTLLexer重复使用测试完成" << std::endl;
    return 0;
}