#include <iostream>
#include <string>
#include "CHTLCompiler/CHTLCompilerCore.h"
#include "CHTLCompiler/Lexer/CHTLLexer.h"
#include "CHTLCompiler/Parser/CHTLParser.h"

using namespace chtl;

void printAST(ast::ASTNode* node, int depth = 0) {
    if (!node) return;
    
    std::string indent(depth * 2, ' ');
    std::cout << indent << "Node: " << typeid(*node).name() << std::endl;
    
    // 如果是ElementNode，打印标签和属性
    if (auto elem = dynamic_cast<ast::ElementNode*>(node)) {
        std::cout << indent << "  Tag: " << elem->tag << std::endl;
        for (const auto& attr : elem->attributes) {
            std::cout << indent << "  Attr: " << attr.first << " = " << attr.second << std::endl;
        }
    }
    
    // 如果是TextNode，打印内容
    if (auto text = dynamic_cast<ast::TextNode*>(node)) {
        std::cout << indent << "  Content: " << text->content << std::endl;
    }
    
    // 递归打印子节点
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        if (auto child = node->getChild(i)) {
            printAST(child, depth + 1);
        }
    }
}

int main() {
    std::cout << "🔍 CHTL AST结构调试" << std::endl;
    std::cout << "==================" << std::endl;

    try {
        std::string testCode = "html { body { div { h1 { text { Hello } } } } }";
        
        std::cout << "1. 词法分析..." << std::endl;
        CHTLLexer lexer;
        lexer.setInput(testCode);
        auto tokens = lexer.tokenize();
        std::cout << "✅ 生成 " << tokens.size() << " 个token" << std::endl;
        
        std::cout << "\n2. 语法分析..." << std::endl;
        parser::CHTLParser parser;
        parser.setTokens(tokens);
        auto ast = parser.parseProgram();
        
        if (ast) {
            std::cout << "✅ AST构建成功" << std::endl;
            std::cout << "\n3. AST结构:" << std::endl;
            printAST(ast.get());
        } else {
            std::cout << "❌ AST构建失败" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}