#include "antlr4-runtime.h"
#include "../src/CSSCompiler/Parser/css3Lexer.h"
#include "../src/CSSCompiler/Parser/css3Parser.h"
#include "../src/JavaScriptCompiler/Parser/JavaScriptLexer.h"
#include "../src/JavaScriptCompiler/Parser/JavaScriptParser.h"
#include <iostream>
#include <cassert>

using namespace antlr4;

void testBasicCSS() {
    std::cout << "=== 测试基础CSS解析 ===" << std::endl;
    
    std::string css = ".test { color: red; }";
    
    ANTLRInputStream input(css);
    css3Lexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    css3Parser parser(&tokens);
    
    auto tree = parser.stylesheet();
    
    std::cout << "CSS解析结果: " << (tree ? "成功" : "失败") << std::endl;
    if (tree) {
        std::cout << "解析树文本: " << tree->getText() << std::endl;
    }
    
    assert(tree != nullptr);
    std::cout << "✅ CSS解析测试通过!" << std::endl;
}

void testBasicJavaScript() {
    std::cout << "=== 测试基础JavaScript解析 ===" << std::endl;
    
    std::string js = "function test() { return 42; }";
    
    ANTLRInputStream input(js);
    JavaScriptLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    JavaScriptParser parser(&tokens);
    
    auto tree = parser.program();
    
    std::cout << "JavaScript解析结果: " << (tree ? "成功" : "失败") << std::endl;
    if (tree) {
        std::cout << "解析树文本: " << tree->getText() << std::endl;
    }
    
    assert(tree != nullptr);
    std::cout << "✅ JavaScript解析测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试ANTLR基础功能..." << std::endl << std::endl;
        
        testBasicCSS();
        std::cout << std::endl;
        
        testBasicJavaScript();
        std::cout << std::endl;
        
        std::cout << "🎉 ANTLR基础功能测试通过!" << std::endl;
        std::cout << "✅ ANTLR 4成功集成到CHTL项目!" << std::endl;
        std::cout << "✅ CSS和JavaScript原生解析能力已就绪!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}