#include <iostream>
#include <string>
#include "CHTL/Lexer.h"
#include "CHTL/Token.h"

int main() {
    std::string chtlCode = R"(
div {
    id: container;
    
    text {
        "Hello World"
    }
}
    )";
    
    std::cout << "CHTL Lexer测试" << std::endl;
    std::cout << "===================" << std::endl;
    
    CHTL::Lexer lexer;
    lexer.SetInput(chtlCode, "test.chtl");
    
    // 读取所有Token
    CHTL::Token token;
    int tokenCount = 0;
    
    do {
        token = lexer.NextToken();
        tokenCount++;
        
        std::cout << "Token #" << tokenCount << ": ";
        std::cout << "Type=" << CHTL::TokenUtils::GetTokenTypeName(token.type);
        std::cout << ", Value=\"" << token.value << "\"";
        std::cout << ", Line=" << token.line;
        std::cout << ", Column=" << token.column << std::endl;
        
    } while (token.type != CHTL::TokenType::Eof);
    
    std::cout << "===================" << std::endl;
    std::cout << "总Token数: " << tokenCount << std::endl;
    
    return 0;
}