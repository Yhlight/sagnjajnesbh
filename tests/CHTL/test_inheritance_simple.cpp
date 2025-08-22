#include <iostream>
#include <cassert>
#include "CHTL/Lexer/CHTLLexer.h"
#include "CHTL/Parser/CHTLParser.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"

using namespace CHTL;

int main() {
    std::cout << "运行简单继承测试...\n";
    
    try {
        std::string source = R"(
[Template] @Style BaseStyle
{
    color: black;
}
)";
        
        // 词法分析
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(source, "inheritance_simple.chtl");
        
        std::cout << "词法分析完成，生成了 " << tokens.Size() << " 个Token\n";
        
        // 打印Token信息
        tokens.Reset();
        for (size_t i = 0; i < std::min(tokens.Size(), size_t(15)); ++i) {
            const auto& token = tokens.Current();
            std::cout << "Token " << i << ": " << Core::CHTLToken::GetTokenTypeName(token.GetType()) 
                      << " = '" << token.GetValue() << "'\n";
            tokens.Advance();
        }
        
        // 语法分析
        tokens.Reset();
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState stateManager;
        Parser::ParserConfig config;
        config.enableDebug = true;
        Parser::CHTLParser parser(globalMap, stateManager, config);
        
        auto ast = parser.Parse(tokens, "inheritance_simple.chtl");
        
        if (ast) {
            std::cout << "语法分析成功\n";
            std::cout << "AST子节点数量: " << ast->GetChildCount() << "\n";
            
            // 检查全局映射表
            const auto& symbols = globalMap.GetAllSymbols();
            std::cout << "符号表中的符号数量: " << symbols.size() << "\n";
            
            for (const auto& symbol : symbols) {
                std::cout << "符号: " << symbol.second.name << " (类型: " 
                          << Core::CHTLGlobalMap::GetSymbolTypeName(symbol.second.type) << ")\n";
            }
        } else {
            std::cout << "语法分析失败\n";
        }
        
        std::cout << "简单继承测试完成!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "测试异常: " << e.what() << std::endl;
        return 1;
    }
}