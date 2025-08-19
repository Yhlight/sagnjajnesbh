#include "src/CHTLCompiler/CHTLCompilerCore.h"
#include <iostream>
#include <memory>

using namespace chtl;

int main() {
    std::cout << "🔍 CHTL AST调试程序" << std::endl;
    std::cout << "==================" << std::endl;
    
    CHTLCompilerCore compiler;
    compiler.setDebugMode(true);
    
    std::string testCode = "html { head { title { text { 测试 } } } body { div { text { Hello } } } }";
    
    std::cout << "\n📋 测试代码: " << testCode << std::endl;
    
    // 1. 词法分析
    auto& lexer = compiler.getLexer();
    lexer.setInput(testCode);
    auto tokens = lexer.tokenize();
    
    std::cout << "\n🔤 词法分析结果:" << std::endl;
    for (size_t i = 0; i < tokens.size() && i < 20; ++i) {
        std::cout << "  [" << i << "] " << tokens[i].toString() << std::endl;
    }
    
    // 2. 语法分析
    std::cout << "\n🌳 语法分析..." << std::endl;
    auto& parser = compiler.getParser();
    parser.setTokens(tokens);
    auto ast = parser.parseProgram();
    
    if (ast) {
        std::cout << "✅ AST生成成功" << std::endl;
        std::cout << "AST根节点: " << ast->toString() << std::endl;
        std::cout << "子节点数量: " << ast->getChildCount() << std::endl;
        
        // 遍历AST结构
        std::function<void(ast::ASTNode*, int)> printAST = [&](ast::ASTNode* node, int depth) {
            if (!node) return;
            
            std::string indent(depth * 2, ' ');
            std::cout << indent << "- " << node->getNodeType() << ": " << node->toString() << std::endl;
            
            for (size_t i = 0; i < node->getChildCount(); ++i) {
                printAST(node->getChild(i), depth + 1);
            }
        };
        
        std::cout << "\n🌳 AST结构:" << std::endl;
        printAST(ast.get(), 0);
        
        // 3. HTML生成
        std::cout << "\n🏗️ HTML生成..." << std::endl;
        auto& generator = compiler.getGenerator();
        std::string html = generator.generateHTML(ast.get());
        
        std::cout << "\n📄 生成的HTML:" << std::endl;
        std::cout << "================================" << std::endl;
        std::cout << html << std::endl;
        std::cout << "================================" << std::endl;
        
    } else {
        std::cout << "❌ AST生成失败" << std::endl;
        auto errors = parser.getErrors();
        for (const auto& error : errors) {
            std::cout << "  错误: " << error << std::endl;
        }
    }
    
    return 0;
}