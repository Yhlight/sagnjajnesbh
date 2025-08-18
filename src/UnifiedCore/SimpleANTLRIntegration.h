#pragma once
#include "antlr4-runtime.h"
#include "../CSSCompiler/Parser/css3Lexer.h"
#include "../CSSCompiler/Parser/css3Parser.h"
#include "../CSSCompiler/Parser/css3ParserBaseListener.h"
#include "../JavaScriptCompiler/Parser/JavaScriptLexer.h"
#include "../JavaScriptCompiler/Parser/JavaScriptParser.h"
#include "../JavaScriptCompiler/Parser/JavaScriptParserBaseListener.h"
#include <string>

namespace chtl {

// ✅ 超简单的CSS编译器 - 只做必要的事情
class SimpleCSSCompiler : public css3ParserBaseListener {
    std::string output_;
public:
    void enterKnownRuleset(css3Parser::KnownRulesetContext *ctx) override {
        output_ += ctx->getText() + "\n";
    }
    
    std::string compile(const std::string& css_code) {
        output_.clear();
        antlr4::ANTLRInputStream input(css_code);
        css3Lexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);
        css3Parser parser(&tokens);
        
        antlr4::tree::ParseTree *tree = parser.stylesheet();
        antlr4::tree::ParseTreeWalker::DEFAULT.walk(this, tree);
        return output_;
    }
};

// ✅ 超简单的JavaScript编译器
class SimpleJSCompiler : public JavaScriptParserBaseListener {
    std::string output_;
public:
    void enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) override {
        output_ += ctx->getText() + "\n";
    }
    
    std::string compile(const std::string& js_code) {
        output_.clear();
        antlr4::ANTLRInputStream input(js_code);
        JavaScriptLexer lexer(&input);
        antlr4::CommonTokenStream tokens(&lexer);
        JavaScriptParser parser(&tokens);
        
        antlr4::tree::ParseTree *tree = parser.program();
        antlr4::tree::ParseTreeWalker::DEFAULT.walk(this, tree);
        return output_;
    }
};

} // namespace chtl