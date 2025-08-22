#include <iostream>
#include <cassert>
#include <memory>
#include "CHTLJS/Core/CHTLJSToken.h"
#include "CHTLJS/Core/CHTLJSState.h"
#include "CHTLJS/Lexer/CHTLJSLexer.h"
#include "CHTLJS/Parser/CHTLJSParser.h"

using namespace CHTL::CHTLJS;

int main() {
    std::cout << "运行简单CHTL JS Parser测试...\n";
    
    try {
        std::string source = "{{.box}}";
        
        // 词法分析
        Lexer::CHTLJSLexer lexer;
        auto tokens = lexer.Tokenize(source, "simple_test.chtljs");
        
        std::cout << "词法分析完成，生成了 " << tokens.Size() << " 个Token\n";
        
        // 语法分析
        Core::CHTLJSState stateManager;
        Parser::CHTLJSParser parser(stateManager);
        auto ast = parser.Parse(tokens, "simple_test.chtljs");
        
        if (ast) {
            std::cout << "语法分析成功\n";
            std::cout << "AST子节点数量: " << ast->GetChildCount() << "\n";
            
            if (ast->GetChildCount() > 0) {
                auto child = ast->GetChildren()[0];
                std::cout << "第一个子节点类型: " << static_cast<int>(child->GetType()) << "\n";
                std::cout << "第一个子节点字符串: " << child->ToString() << "\n";
            }
        } else {
            std::cout << "语法分析失败\n";
        }
        
        std::cout << "简单CHTL JS Parser测试完成!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return 1;
    }
}