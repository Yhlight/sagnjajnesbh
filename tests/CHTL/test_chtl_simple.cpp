#include <iostream>
#include <cassert>
#include "CHTL/Lexer/CHTLLexer.h"
#include "CHTL/Parser/CHTLParser.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"

using namespace CHTL;

int main() {
    std::cout << "运行简单CHTL编译器测试...\n";
    
    try {
        // 测试词法分析器
        std::cout << "测试词法分析器...\n";
        std::string source = "html { body { text { Hello } } }";
        
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(source, "simple.chtl");
        
        std::cout << "词法分析完成，生成了 " << tokens.Size() << " 个Token\n";
        
        // 打印前几个Token
        tokens.Reset();
        for (size_t i = 0; i < std::min(tokens.Size(), size_t(10)); ++i) {
            const auto& token = tokens.Current();
            std::cout << "Token " << i << ": " << Core::CHTLToken::GetTokenTypeName(token.GetType()) 
                      << " = '" << token.GetValue() << "'\n";
            tokens.Advance();
        }
        
        // 测试语法分析器
        std::cout << "\n测试语法分析器...\n";
        tokens.Reset();
        
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState stateManager;
        Parser::CHTLParser parser(globalMap, stateManager);
        
        auto ast = parser.Parse(tokens, "simple.chtl");
        
        if (ast) {
            std::cout << "语法分析成功，AST类型: " << AST::ASTNode::GetNodeTypeName(ast->GetType()) << "\n";
            std::cout << "AST子节点数量: " << ast->GetChildCount() << "\n";
        } else {
            std::cout << "语法分析失败\n";
        }
        
        std::cout << "\n简单编译器测试完成!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return 1;
    }
}