#include "include/CHTLParser.h"
#include "include/CHTLGenerator.h"
#include "include/CHTLCompiler.h"
#include <iostream>
#include <cassert>

using namespace chtl;

void testBasicTextNode() {
    std::cout << "测试基础文本节点..." << std::endl;
    
    String testCode = R"(
text
{
    "这是一段文本"
}
)";
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "test_text.chtl");
    
    if (!parseResult.success) {
        std::cout << "解析失败:" << std::endl;
        for (const auto& error : parseResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : genResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    std::cout << "HTML输出:" << std::endl;
    std::cout << genResult.htmlOutput << std::endl;
    
    std::cout << "✓ 基础文本节点测试通过" << std::endl;
}

void testBasicElement() {
    std::cout << "测试基础元素节点..." << std::endl;
    
    String testCode = R"(
div
{
    id: box;
    class: welcome;
    
    text
    {
        HelloWorld
    }
}
)";
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "test_element.chtl");
    
    if (!parseResult.success) {
        std::cout << "解析失败:" << std::endl;
        for (const auto& error : parseResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : genResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    std::cout << "HTML输出:" << std::endl;
    std::cout << genResult.htmlOutput << std::endl;
    
    std::cout << "✓ 基础元素节点测试通过" << std::endl;
}

void testLocalStyleBlock() {
    std::cout << "测试局部样式块..." << std::endl;
    
    String testCode = R"(
div
{
    style
    {
        width: 100px;
        height: 200px;
        
        .box
        {
            background-color: red;
        }
    }
    
    text
    {
        样式测试
    }
}
)";
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "test_style.chtl");
    
    if (!parseResult.success) {
        std::cout << "解析失败:" << std::endl;
        for (const auto& error : parseResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : genResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    std::cout << "HTML输出:" << std::endl;
    std::cout << genResult.htmlOutput << std::endl;
    std::cout << "CSS输出:" << std::endl;
    std::cout << genResult.cssOutput << std::endl;
    
    std::cout << "✓ 局部样式块测试通过" << std::endl;
}

void testUnquotedLiterals() {
    std::cout << "测试无修饰字面量..." << std::endl;
    
    String testCode = R"(
div
{
    style
    {
        color: red;
        font-size: 16px;
    }
    
    text
    {
        这是无修饰字面量文本
    }
}
)";
    
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLParser parser(options);
    auto parseResult = parser.parse(testCode, "test_unquoted.chtl");
    
    if (!parseResult.success) {
        std::cout << "解析失败:" << std::endl;
        for (const auto& error : parseResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    CHTLGenerator generator(options);
    auto genResult = generator.generate(parseResult.ast);
    
    if (!genResult.success) {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : genResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return;
    }
    
    std::cout << "HTML输出:" << std::endl;
    std::cout << genResult.htmlOutput << std::endl;
    std::cout << "CSS输出:" << std::endl;
    std::cout << genResult.cssOutput << std::endl;
    
    std::cout << "✓ 无修饰字面量测试通过" << std::endl;
}

int main() {
    std::cout << "=== CHTL 基础语法测试 ===" << std::endl;
    
    try {
        testBasicTextNode();
        std::cout << std::endl;
        
        testBasicElement();
        std::cout << std::endl;
        
        testLocalStyleBlock();
        std::cout << std::endl;
        
        testUnquotedLiterals();
        std::cout << std::endl;
        
        std::cout << "✅ 所有基础语法测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}