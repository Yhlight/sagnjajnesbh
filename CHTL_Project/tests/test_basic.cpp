#include <iostream>
#include <string>
#include <cassert>

#include "Core/CompilerDispatcher.hpp"
#include "CHTL/CHTLLexer.hpp"
#include "CHTL/CHTLToken.hpp"

void TestCHTLLexer() {
    std::cout << "测试CHTL词法分析器..." << std::endl;
    
    CHTL::CHTLLexer lexer;
    
    // 测试基本的CHTL代码
    std::string source = R"(
        div
        {
            id: container;
            class: main-content;
            
            text
            {
                "Hello, CHTL!"
            }
            
            style
            {
                width: 100px;
                height: 200px;
            }
        }
    )";
    
    lexer.SetSource(source, "test.chtl");
    
    std::vector<CHTL::CHTLToken> tokens;
    while (!lexer.IsAtEnd()) {
        CHTL::CHTLToken token = lexer.NextToken();
        tokens.push_back(token);
        
        if (token.type == CHTL::CHTLTokenType::EOF_TOKEN) {
            break;
        }
    }
    
    std::cout << "词法分析完成，共生成 " << tokens.size() << " 个Token" << std::endl;
    
    // 验证一些关键Token
    bool foundDiv = false;
    bool foundText = false;
    bool foundStyle = false;
    
    for (const auto& token : tokens) {
        if (token.type == CHTL::CHTLTokenType::HTML_ELEMENT && token.value == "div") {
            foundDiv = true;
        }
        if (token.type == CHTL::CHTLTokenType::TEXT && token.value == "text") {
            foundText = true;
        }
        if (token.type == CHTL::CHTLTokenType::STYLE && token.value == "style") {
            foundStyle = true;
        }
    }
    
    assert(foundDiv && "应该找到div元素");
    assert(foundText && "应该找到text关键字");
    assert(foundStyle && "应该找到style关键字");
    
    std::cout << "✓ CHTL词法分析器测试通过" << std::endl;
}

void TestCompilerDispatcher() {
    std::cout << "测试编译器调度器..." << std::endl;
    
    CHTL::CompilerDispatcher dispatcher;
    dispatcher.SetCompilationOptions(true, true); // 启用调试和优化
    
    // 测试基本的CHTL代码编译
    std::string source = R"(
        html
        {
            head
            {
                title
                {
                    "CHTL测试页面"
                }
            }
            
            body
            {
                div
                {
                    id: main;
                    class: container;
                    
                    text
                    {
                        "欢迎使用CHTL编译器！"
                    }
                }
            }
        }
    )";
    
    CHTL::CompilationResult result = dispatcher.Compile(source, "test.chtl");
    
    if (result.success) {
        std::cout << "✓ 编译成功" << std::endl;
        std::cout << "生成的HTML长度: " << result.output.length() << " 字符" << std::endl;
        
        // 验证输出包含基本的HTML结构
        assert(result.output.find("<!DOCTYPE html>") != std::string::npos);
        assert(result.output.find("<html>") != std::string::npos);
        assert(result.output.find("</html>") != std::string::npos);
        
    } else {
        std::cout << "✗ 编译失败" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "错误: " << error.message << std::endl;
        }
        assert(false && "编译应该成功");
    }
    
    std::cout << "✓ 编译器调度器测试通过" << std::endl;
}

void TestCHTLTokenTypes() {
    std::cout << "测试CHTL Token类型..." << std::endl;
    
    // 测试Token类型转换
    std::string tokenName = CHTL::TokenTypeToString(CHTL::CHTLTokenType::HTML_ELEMENT);
    assert(tokenName == "HTML_ELEMENT");
    
    // 测试关键字映射
    CHTL::CHTLKeywordMap keywordMap;
    assert(keywordMap.IsKeyword("text"));
    assert(keywordMap.IsKeyword("style"));
    assert(keywordMap.IsKeyword("inherit"));
    assert(!keywordMap.IsKeyword("unknown"));
    
    std::cout << "✓ CHTL Token类型测试通过" << std::endl;
}

int main() {
    std::cout << "开始CHTL编译器测试..." << std::endl;
    std::cout << "==============================" << std::endl;
    
    try {
        TestCHTLTokenTypes();
        TestCHTLLexer();
        TestCompilerDispatcher();
        
        std::cout << "==============================" << std::endl;
        std::cout << "✓ 所有测试通过！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "✗ 测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}