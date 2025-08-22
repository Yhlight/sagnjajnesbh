#include <iostream>
#include <string>
#include <memory>
#include "CHTL/Core/CHTLToken.h"
#include "CHTL/Lexer/CHTLLexer.h"
#include "CHTL/Parser/CHTLParser.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"

using namespace CHTL;

int main() {
    std::cout << "=== CHTL 命名空间解析测试 ===" << std::endl;
    
    // 测试1：基础命名空间（带大括号）
    std::string test1 = R"(
[Namespace] TestSpace {
    [Custom] @Element Box {
        div {
            class: "test-box";
        }
    }
}
)";
    
    std::cout << "\n--- 测试1：基础命名空间（带大括号） ---" << std::endl;
    std::cout << "输入代码:\n" << test1 << std::endl;
    
    try {
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState state;
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(test1, "test1.chtl");
        Parser::CHTLParser parser(globalMap, state);
        
        auto ast = parser.Parse(tokens, "test1.chtl");
        if (ast) {
            std::cout << "✅ 解析成功：基础命名空间" << std::endl;
        } else {
            std::cout << "❌ 解析失败：基础命名空间" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 异常：" << e.what() << std::endl;
    }
    
    // 测试2：省略大括号的命名空间（关键测试）
    std::string test2 = R"(
[Namespace] SimpleSpace
    [Custom] @Element SimpleBox {
        div {
            class: "simple-box";
        }
    }
)";
    
    std::cout << "\n--- 测试2：省略大括号的命名空间（关键测试） ---" << std::endl;
    std::cout << "输入代码:\n" << test2 << std::endl;
    
    try {
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState state;
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(test2, "test2.chtl");
        Parser::CHTLParser parser(globalMap, state);
        
        auto ast = parser.Parse(tokens, "test2.chtl");
        if (ast) {
            std::cout << "✅ 解析成功：省略大括号的命名空间" << std::endl;
        } else {
            std::cout << "❌ 解析失败：省略大括号的命名空间" << std::endl;
            std::cout << "    这表明当前解析器不支持省略大括号功能" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 异常：" << e.what() << std::endl;
    }
    
    // 测试3：嵌套命名空间省略大括号
    std::string test3 = R"(
[Namespace] Outer {
    [Namespace] Inner
        [Custom] @Element NestedBox {
            div {
                class: "nested-box";
            }
        }
}
)";
    
    std::cout << "\n--- 测试3：嵌套命名空间省略大括号 ---" << std::endl;
    std::cout << "输入代码:\n" << test3 << std::endl;
    
    try {
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState state;
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(test3, "test3.chtl");
        Parser::CHTLParser parser(globalMap, state);
        
        auto ast = parser.Parse(tokens, "test3.chtl");
        if (ast) {
            std::cout << "✅ 解析成功：嵌套命名空间省略大括号" << std::endl;
        } else {
            std::cout << "❌ 解析失败：嵌套命名空间省略大括号" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 异常：" << e.what() << std::endl;
    }
    
    // 测试4：复杂语法组合
    std::string test4 = R"(
[Import] @Chtl from "test";

[Namespace] ComplexSpace {
    [Template] @Style ThemeTemplate {
        .primary { color: blue; }
    }
    
    [Custom] @Element Card {
        div {
            class: "card";
            style {
                padding: 16px;
            }
        }
    }
}

body {
    @Element Card from ComplexSpace;
}
)";
    
    std::cout << "\n--- 测试4：复杂语法组合 ---" << std::endl;
    std::cout << "输入代码:\n" << test4 << std::endl;
    
    try {
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState state;
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(test4, "test4.chtl");
        Parser::CHTLParser parser(globalMap, state);
        
        auto ast = parser.Parse(tokens, "test4.chtl");
        if (ast) {
            std::cout << "✅ 解析成功：复杂语法组合" << std::endl;
        } else {
            std::cout << "❌ 解析失败：复杂语法组合" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "❌ 异常：" << e.what() << std::endl;
    }
    
    std::cout << "\n=== 测试总结 ===" << std::endl;
    std::cout << "如果测试2（省略大括号）失败，说明需要实现命名空间省略大括号功能" << std::endl;
    std::cout << "语法文档第998行明确要求：\"如果仅仅是只有一层关系 或 只有一层平级，可以不用写花括号\"" << std::endl;
    
    return 0;
}