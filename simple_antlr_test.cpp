#include "antlr4-runtime.h"
#include "src/CSSCompiler/Parser/css3Lexer.h"
#include "src/CSSCompiler/Parser/css3Parser.h"
#include "src/CSSCompiler/Parser/css3ParserBaseListener.h"
#include <iostream>

using namespace antlr4;

// 最简单的CSS解析器 - 不需要复杂的TreeWalker
class SimpleCSSListener : public css3ParserBaseListener {
public:
    void enterKnownRuleset(css3Parser::KnownRulesetContext *ctx) override {
        std::cout << "找到CSS规则: " << ctx->getText() << std::endl;
    }
};

int main() {
    std::string css_code = R"(
        body { 
            margin: 0; 
            padding: 10px; 
        }
        .container { 
            width: 100%; 
        }
    )";
    
    // ANTLR标准流程
    ANTLRInputStream input(css_code);
    css3Lexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    css3Parser parser(&tokens);
    
    // 创建监听器
    SimpleCSSListener listener;
    
    // 解析并遍历
    tree::ParseTree *tree = parser.stylesheet();
    tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);
    
    std::cout << "CSS解析完成！" << std::endl;
    return 0;
}