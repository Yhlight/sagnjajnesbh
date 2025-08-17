#include "include/CHTLLexer.h"
#include "include/CHTLParser.h"
#include "include/CHTLGenerator.h"
#include "include/StateMachine.h"
#include "include/ContextManager.h"
#include <iostream>
#include <string>

using namespace chtl;

int main() {
    // 测试CHTL完整代码
    std::string testCode = R"(
    // CHTL 完整语法测试
    [Template] @Style DefaultButton
    {
        padding: 10px 20px;
        border: 1px solid #ccc;
        background-color: #f8f8f8;
        border-radius: 4px;
    }
    
    [Template] @Element Card
    {
        div
        {
            style
            {
                @Style DefaultButton;
                box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            }
        }
    }
    
    html
    {
        head
        {
            text
            {
                "CHTL测试页面"
            }
        }
        
        body
        {
            div
            {
                id: container;
                class: main-wrapper;
                
                style
                {
                    width: 100%;
                    max-width: 1200px;
                    margin: 0 auto;
                    
                    .header
                    {
                        background-color: #333;
                        color: white;
                        padding: 20px;
                    }
                    
                    &:hover
                    {
                        background-color: #f5f5f5;
                    }
                }
                
                div
                {
                    style
                    {
                        .header;
                    }
                    
                    text
                    {
                        "欢迎使用CHTL"
                    }
                }
                
                @Element Card;
                
                div
                {
                    text
                    {
                        "这是一个使用CHTL语法编写的页面"
                    }
                }
            }
        }
    }
    )";
    
    try {
        std::cout << "=== CHTL 完整编译测试 ===" << std::endl;
        std::cout << "源代码长度: " << testCode.length() << " 字符" << std::endl;
        std::cout << std::endl;
        
        // 第一步：词法分析
        std::cout << "1. 词法分析..." << std::endl;
        StateMachine stateMachine;
        ContextManager contextManager(stateMachine);
        CHTLLexer lexer(testCode, stateMachine, contextManager);
        
        auto tokens = lexer.tokenize();
        std::cout << "   生成Token数量: " << tokens.size() << std::endl;
        
        if (lexer.hasErrors()) {
            std::cout << "   词法分析错误:" << std::endl;
            for (const auto& error : lexer.getErrors()) {
                std::cout << "   - " << error.message << std::endl;
            }
            return 1;
        }
        
        // 第二步：语法解析
        std::cout << "2. 语法解析..." << std::endl;
        CompilerOptions options;
        options.debugMode = false;
        CHTLParser parser(options);
        
        auto parseResult = parser.parseFromTokens(tokens, "test.chtl");
        
        if (!parseResult.success) {
            std::cout << "   语法解析错误:" << std::endl;
            for (const auto& error : parseResult.errors) {
                std::cout << "   - " << error.message << std::endl;
            }
            return 1;
        }
        
        std::cout << "   AST根节点类型: " << static_cast<int>(parseResult.ast->getType()) << std::endl;
        std::cout << "   子节点数量: " << parseResult.ast->getChildren().size() << std::endl;
        
        // 第三步：代码生成
        std::cout << "3. 代码生成..." << std::endl;
        CHTLGenerator generator(options);
        
        auto generateResult = generator.generate(parseResult.ast);
        
        if (!generateResult.success) {
            std::cout << "   代码生成错误:" << std::endl;
            for (const auto& error : generateResult.errors) {
                std::cout << "   - " << error.message << std::endl;
            }
            return 1;
        }
        
        // 输出生成的HTML
        std::cout << std::endl << "=== 生成的HTML ===" << std::endl;
        std::cout << generateResult.htmlOutput << std::endl;
        
        // 输出生成的CSS
        if (!generateResult.cssOutput.empty()) {
            std::cout << std::endl << "=== 生成的CSS ===" << std::endl;
            std::cout << generateResult.cssOutput << std::endl;
        }
        
        // 输出生成的JS
        if (!generateResult.jsOutput.empty()) {
            std::cout << std::endl << "=== 生成的JS ===" << std::endl;
            std::cout << generateResult.jsOutput << std::endl;
        }
        
        // 统计信息
        std::cout << std::endl << "=== 编译统计 ===" << std::endl;
        std::cout << "HTML输出长度: " << generateResult.htmlOutput.length() << " 字符" << std::endl;
        std::cout << "CSS输出长度: " << generateResult.cssOutput.length() << " 字符" << std::endl;
        std::cout << "JS输出长度: " << generateResult.jsOutput.length() << " 字符" << std::endl;
        std::cout << "警告数量: " << generateResult.warnings.size() << std::endl;
        
        std::cout << std::endl << "✅ CHTL编译成功完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 编译失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}