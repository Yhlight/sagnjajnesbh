#include <iostream>
#include <fstream>
#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include "CHTLCompiler/Parser/CHTLContext.h"
#include "CHTLCompiler/State/CHTLState.h"
#include "CHTLCompiler/Parser/CHTLParser.h"

using namespace chtl;

int main() {
    std::cout << "🔍 测试CHTLParser最小化创建" << std::endl;
    std::cout << "=============================" << std::endl;

    try {
        // 1. 测试基础组件创建
        std::cout << "1. 创建CHTLContext..." << std::endl;
        CHTLContext context;
        std::cout << "✅ CHTLContext创建成功" << std::endl;
        
        std::cout << "2. 创建StateManager..." << std::endl;
        StateManager stateManager;
        std::cout << "✅ StateManager创建成功" << std::endl;
        
        // 3. 直接测试CHTLParser创建（这里可能段错误）
        std::cout << "3. 创建CHTLParser..." << std::endl;
        std::cout << "   即将调用 parser::CHTLParser()..." << std::endl;
        
        // 使用new而不是make_unique，更容易定位问题
        parser::CHTLParser* parser = new parser::CHTLParser();
        std::cout << "✅ CHTLParser创建成功" << std::endl;
        
        // 4. 测试简单的token设置
        std::cout << "4. 测试setTokens..." << std::endl;
        std::vector<Token> simpleTokens;
        simpleTokens.emplace_back(TokenType::IDENTIFIER, "html", TokenPosition(1, 1));
        simpleTokens.emplace_back(TokenType::LEFT_BRACE, "{", TokenPosition(1, 6));
        simpleTokens.emplace_back(TokenType::RIGHT_BRACE, "}", TokenPosition(1, 7));
        simpleTokens.emplace_back(TokenType::EOF_TOKEN, "", TokenPosition(1, 8));
        
        parser->setTokens(simpleTokens);
        std::cout << "✅ setTokens成功" << std::endl;
        
        // 5. 测试parseProgram（这里可能段错误）
        std::cout << "5. 测试parseProgram..." << std::endl;
        std::cout << "   即将调用 parser->parseProgram()..." << std::endl;
        
        auto ast = parser->parseProgram();
        
        if (ast) {
            std::cout << "✅ parseProgram成功" << std::endl;
        } else {
            std::cout << "❌ parseProgram返回nullptr" << std::endl;
        }
        
        delete parser;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "🎉 CHTLParser最小化测试完成" << std::endl;
    return 0;
}