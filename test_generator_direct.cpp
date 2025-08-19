#include <iostream>
#include "CHTLCompiler/Generator/CHTLGenerator.h"
#include "CHTLCompiler/AST/CHTLNodes.h"

using namespace chtl;

int main() {
    std::cout << "🔧 直接测试HTML生成器" << std::endl;
    std::cout << "=====================" << std::endl;

    try {
        // 手动创建AST结构
        auto root = std::make_unique<ast::ProgramNode>();
        
        // 创建html元素
        auto htmlNode = std::make_unique<ast::ElementNode>();
        htmlNode->tag = "html";
        
        // 创建body元素
        auto bodyNode = std::make_unique<ast::ElementNode>();
        bodyNode->tag = "body";
        
        // 创建div元素
        auto divNode = std::make_unique<ast::ElementNode>();
        divNode->tag = "div";
        divNode->attributes.emplace_back("class", "container");
        
        // 创建h1元素
        auto h1Node = std::make_unique<ast::ElementNode>();
        h1Node->tag = "h1";
        
        // 创建文本节点
        auto textNode = std::make_unique<ast::TextNode>();
        textNode->content = "Hello World";
        
        // 构建AST层次结构
        h1Node->addChild(std::move(textNode));
        divNode->addChild(std::move(h1Node));
        bodyNode->addChild(std::move(divNode));
        htmlNode->addChild(std::move(bodyNode));
        root->addChild(std::move(htmlNode));
        
        std::cout << "✅ AST结构创建成功" << std::endl;
        
        // 使用HTML生成器
        generator::HTMLGenerator generator;
        std::string html = generator.generateHTML(root.get());
        
        std::cout << "✅ HTML生成完成" << std::endl;
        std::cout << "\n生成的HTML:" << std::endl;
        std::cout << "================================" << std::endl;
        std::cout << html << std::endl;
        std::cout << "================================" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}