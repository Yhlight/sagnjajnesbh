#include "include/CHTLLexer.h"
#include "include/CHTLTokens.h"
#include "include/StateMachine.h"
#include "include/ContextManager.h"
#include <iostream>
#include <string>

using namespace chtl;

int main() {
    // 测试CHTL代码
    std::string testCode = R"(
    // 单行注释
    /* 多行注释 */
    -- 生成器注释
    
    [Template] @Style DefaultButton
    {
        color: red;
        background-color: #ffffff;
    }
    
    [Custom] @Element Box
    {
        div
        {
            style
            {
                width: 100px;
                height: 100px;
            }
        }
    }
    
    html
    {
        body
        {
            text
            {
                "Hello CHTL"
            }
            
            @Element Box;
        }
    }
    
    script
    {
        {{button}}->addEventListener('click', () => {
            console.log('Clicked!');
        });
    }
    )";
    
    try {
        // 创建状态机和上下文管理器
        StateMachine stateMachine;
        ContextManager contextManager(stateMachine);
        
        // 创建词法分析器
        CHTLLexer lexer(testCode, stateMachine, contextManager);
        
        // 进行词法分析
        auto tokens = lexer.tokenize();
        
        std::cout << "=== CHTL 词法分析测试 ===" << std::endl;
        std::cout << "生成的Token数量: " << tokens.size() << std::endl;
        std::cout << std::endl;
        
        // 输出所有Token
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& token = tokens[i];
            std::cout << i + 1 << ". " << token.toString() << std::endl;
        }
        
        // 输出统计信息
        auto stats = lexer.getStatistics();
        std::cout << std::endl << "=== 统计信息 ===" << std::endl;
        std::cout << "总Token数: " << stats.tokensGenerated << std::endl;
        std::cout << "标识符数: " << stats.identifiersFound << std::endl;
        std::cout << "关键字数: " << stats.keywordsFound << std::endl;
        std::cout << "字面量数: " << stats.literalsFound << std::endl;
        std::cout << "跳过的注释数: " << stats.commentsSkipped << std::endl;
        std::cout << "错误数: " << stats.errorsEncountered << std::endl;
        
        // 检查是否有错误
        if (lexer.hasErrors()) {
            std::cout << std::endl << "=== 错误信息 ===" << std::endl;
            for (const auto& error : lexer.getErrors()) {
                std::cout << "错误: " << error.message << " (行 " << error.line << ", 列 " << error.column << ")" << std::endl;
            }
        }
        
        std::cout << std::endl << "词法分析测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}