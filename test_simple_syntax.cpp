#include "include/CHTLTokens.h"
#include "include/CHTLASTNodes.h"
#include "include/CHTLGenerator.h"
#include <iostream>
#include <memory>

using namespace chtl;

// 简单测试：创建基础AST节点并生成HTML
void testBasicGeneration() {
    std::cout << "测试基础AST节点生成..." << std::endl;
    
    // 创建根节点
    auto root = std::make_shared<RootNode>();
    
    // 创建div元素
    auto div = std::make_shared<ElementNode>("div");
    div->setAttribute("id", "test");
    div->setAttribute("class", "container");
    
    // 创建文本节点
    auto text = std::make_shared<TextNode>("Hello CHTL!");
    
    // 构建AST
    div->addChild(text);
    root->addChild(div);
    
    // 生成HTML
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "HTML输出:" << std::endl;
        std::cout << result.htmlOutput << std::endl;
        std::cout << "✓ 基础生成测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试样式块生成
void testStyleGeneration() {
    std::cout << "\n测试样式块生成..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    auto div = std::make_shared<ElementNode>("div");
    
    // 创建样式块
    auto styleBlock = std::make_shared<StyleBlockNode>(true); // 局部样式
    
    // 添加内联样式属性
    auto widthProp = std::make_shared<CSSPropertyNode>("width", "100px");
    auto heightProp = std::make_shared<CSSPropertyNode>("height", "200px");
    
    styleBlock->addChild(widthProp);
    styleBlock->addChild(heightProp);
    
    // 添加类选择器
    auto classRule = std::make_shared<CSSRuleNode>(".box");
    auto bgProp = std::make_shared<CSSPropertyNode>("background-color", "red");
    classRule->addChild(bgProp);
    styleBlock->addChild(classRule);
    
    div->addChild(styleBlock);
    root->addChild(div);
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "HTML输出:" << std::endl;
        std::cout << result.htmlOutput << std::endl;
        std::cout << "CSS输出:" << std::endl;
        std::cout << result.cssOutput << std::endl;
        std::cout << "✓ 样式生成测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试Token系统
void testTokenSystem() {
    std::cout << "\n测试Token系统..." << std::endl;
    
    // 创建一些基础Token
    Token identifierToken(TokenType::IDENTIFIER, "div", 1, 1);
    Token leftBraceToken(TokenType::LEFT_BRACE, "{", 1, 5);
    Token textToken(TokenType::TEXT, "text", 2, 5);
    Token stringToken(TokenType::STRING_LITERAL, "\"Hello World\"", 3, 9);
    
    std::cout << "Token信息:" << std::endl;
    std::cout << "  " << identifierToken.toString() << std::endl;
    std::cout << "  " << leftBraceToken.toString() << std::endl;
    std::cout << "  " << textToken.toString() << std::endl;
    std::cout << "  " << stringToken.toString() << std::endl;
    
    // 测试Token类型判断
    std::cout << "Token类型判断:" << std::endl;
    std::cout << "  identifierToken.isKeyword(): " << identifierToken.isKeyword() << std::endl;
    std::cout << "  textToken.isKeyword(): " << textToken.isKeyword() << std::endl;
    std::cout << "  stringToken.isLiteral(): " << stringToken.isLiteral() << std::endl;
    
    std::cout << "✓ Token系统测试通过" << std::endl;
}

int main() {
    std::cout << "=== CHTL 简化语法测试 ===" << std::endl;
    
    try {
        testTokenSystem();
        testBasicGeneration();
        testStyleGeneration();
        
        std::cout << "\n✅ 所有简化测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}