// ✅ 正确的ANTLR使用方式 - 简单直接
#include "antlr4-runtime.h"
#include "src/JavaScriptCompiler/Parser/JavaScriptLexer.h"
#include "src/JavaScriptCompiler/Parser/JavaScriptParser.h"
#include "src/JavaScriptCompiler/Parser/JavaScriptParserBaseListener.h"

class SimpleJSCompiler : public JavaScriptParserBaseListener {
    std::string output_;
    
public:
    // 只需要实现你关心的方法！
    void enterFunctionDeclaration(JavaScriptParser::FunctionDeclarationContext *ctx) override {
        output_ += "// 找到函数: " + ctx->getText() + "\n";
    }
    
    void enterVariableDeclaration(JavaScriptParser::VariableDeclarationContext *ctx) override {
        output_ += "// 找到变量: " + ctx->getText() + "\n";
    }
    
    std::string getOutput() const { return output_; }
};

// 使用方式 - 就是这么简单！
std::string compileJS(const std::string& js_code) {
    antlr4::ANTLRInputStream input(js_code);
    JavaScriptLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    JavaScriptParser parser(&tokens);
    
    SimpleJSCompiler compiler;
    antlr4::tree::ParseTree *tree = parser.program();
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&compiler, tree);
    
    return compiler.getOutput();
}