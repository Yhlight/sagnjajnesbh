#include "../src/CSSCompiler/Parser/css3Lexer.h"
#include "../src/CSSCompiler/Parser/css3Parser.h"
#include "antlr4-runtime.h"
#include <iostream>

using namespace antlr4;

int main() {
    try {
        std::cout << "Testing minimal CSS parsing..." << std::endl;
        
        std::string css_code = ".test { color: red; }";
        std::cout << "CSS code: " << css_code << std::endl;
        
        // 创建输入流
        std::cout << "Creating input stream..." << std::endl;
        ANTLRInputStream input(css_code);
        
        // 创建词法分析器
        std::cout << "Creating lexer..." << std::endl;
        css3Lexer lexer(&input);
        
        // 创建token流
        std::cout << "Creating token stream..." << std::endl;
        CommonTokenStream tokens(&lexer);
        
        // 创建解析器
        std::cout << "Creating parser..." << std::endl;
        css3Parser parser(&tokens);
        
        // 解析CSS
        std::cout << "Parsing CSS..." << std::endl;
        auto tree = parser.stylesheet();
        
        std::cout << "CSS parsing completed successfully!" << std::endl;
        std::cout << "Parse tree: " << tree->getText() << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown exception occurred" << std::endl;
        return 1;
    }
}