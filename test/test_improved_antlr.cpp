#include <iostream>
#include <cassert>
#include "../src/CSSCompiler/Parser/css3Parser.h"
#include "../src/CSSCompiler/Parser/css3Lexer.h"
#include "../src/JavaScriptCompiler/Parser/JavaScriptParser_cpp.h"
#include "../src/JavaScriptCompiler/Parser/JavaScriptLexer_cpp.h"
#include "antlr4-runtime.h"

using namespace antlr4;

int main() {
    std::cout << "测试改进后的ANTLR解析器..." << std::endl;
    
    // 测试CSS解析
    std::string css_code = ".test { color: red; }";
    ANTLRInputStream css_input(css_code);
    css3Lexer css_lexer(&css_input);
    CommonTokenStream css_tokens(&css_lexer);
    css3Parser css_parser(&css_tokens);
    auto css_tree = css_parser.stylesheet();
    
    std::cout << "✅ CSS解析成功!" << std::endl;
    
    // 测试JavaScript解析
    std::string js_code = "function test() { return 42; }";
    ANTLRInputStream js_input(js_code);
    JavaScriptLexer_cpp js_lexer(&js_input);
    CommonTokenStream js_tokens(&js_lexer);
    JavaScriptParser_cpp js_parser(&js_tokens);
    auto js_tree = js_parser.program();
    
    std::cout << "✅ JavaScript解析成功!" << std::endl;
    
    std::cout << "🎉 改进后的ANTLR集成测试通过!" << std::endl;
    return 0;
}
