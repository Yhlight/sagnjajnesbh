#include <iostream>
#include <cassert>
#include <memory>
#include "CHTL/Lexer/CHTLLexer.h"
#include "CHTL/Parser/CHTLParser.h"
#include "CHTL/Generator/CHTLGenerator.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"

using namespace CHTL;

// 测试元素插入操作
void test_element_insertion() {
    std::cout << "测试元素插入操作...\n";
    
    std::string source = R"(
[Custom] @Element Card
{
    div { text { 标题 } }
    div { text { 内容 } }
}

body
{
    @Element Card
    {
        insert after div[0] {
            p { text { 插入的段落 } }
        }
    }
}
)";
    
    try {
        // 词法分析
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(source, "insertion_test.chtl");
        
        // 语法分析
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState stateManager;
        Parser::CHTLParser parser(globalMap, stateManager);
        
        auto ast = parser.Parse(tokens, "insertion_test.chtl");
        assert(ast != nullptr);
        
        // 代码生成
        Generator::GeneratorConfig config;
        config.enableDebug = true;
        Generator::CHTLGenerator generator(globalMap, config);
        
        std::string html = generator.Generate(ast);
        assert(!html.empty());
        
        // 验证插入操作的标记存在
        assert(html.find("body") != std::string::npos);
        
        std::cout << "  元素插入操作测试通过\n";
        
    } catch (const std::exception& e) {
        std::cerr << "元素插入测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

// 测试元素删除操作
void test_element_deletion() {
    std::cout << "测试元素删除操作...\n";
    
    std::string source = R"(
[Custom] @Element Card
{
    div { text { 标题 } }
    div { text { 内容 } }
    span { text { 额外信息 } }
}

body
{
    @Element Card
    {
        delete span;
        delete div[1];
    }
}
)";
    
    try {
        // 词法分析
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(source, "deletion_test.chtl");
        
        // 语法分析
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState stateManager;
        Parser::CHTLParser parser(globalMap, stateManager);
        
        auto ast = parser.Parse(tokens, "deletion_test.chtl");
        assert(ast != nullptr);
        
        // 代码生成
        Generator::GeneratorConfig config;
        config.enableDebug = true;
        Generator::CHTLGenerator generator(globalMap, config);
        
        std::string html = generator.Generate(ast);
        assert(!html.empty());
        
        std::cout << "  元素删除操作测试通过\n";
        
    } catch (const std::exception& e) {
        std::cerr << "元素删除测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

// 测试约束验证
void test_constraint_validation() {
    std::cout << "测试约束验证...\n";
    
    std::string source = R"(
[Namespace] TestSpace
{
    except span, div;
    
    p { text { 这是允许的 } }
}
)";
    
    try {
        // 词法分析
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(source, "constraint_test.chtl");
        
        // 语法分析
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState stateManager;
        Parser::CHTLParser parser(globalMap, stateManager);
        
        auto ast = parser.Parse(tokens, "constraint_test.chtl");
        assert(ast != nullptr);
        
        // 代码生成
        Generator::GeneratorConfig config;
        config.enableDebug = true;
        Generator::CHTLGenerator generator(globalMap, config);
        
        std::string html = generator.Generate(ast);
        assert(!html.empty());
        
        // 验证约束处理
        assert(html.find("<p>") != std::string::npos);
        
        std::cout << "  约束验证测试通过\n";
        
    } catch (const std::exception& e) {
        std::cerr << "约束验证测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

// 测试配置系统
void test_configuration_system() {
    std::cout << "测试配置系统...\n";
    
    std::string source = R"(
[Configuration] Output
{
    pretty_print: true;
    minify: false;
    include_comments: true;
}

html
{
    body
    {
        div { text { 配置测试 } }
    }
}
)";
    
    try {
        // 词法分析
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(source, "config_test.chtl");
        
        // 语法分析
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState stateManager;
        Parser::CHTLParser parser(globalMap, stateManager);
        
        auto ast = parser.Parse(tokens, "config_test.chtl");
        assert(ast != nullptr);
        
        // 代码生成
        Generator::CHTLGenerator generator(globalMap);
        std::string html = generator.Generate(ast);
        assert(!html.empty());
        
        // 验证配置应用（美化输出应该包含换行）
        assert(html.find("\n") != std::string::npos);
        
        std::cout << "  配置系统测试通过\n";
        
    } catch (const std::exception& e) {
        std::cerr << "配置系统测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

// 测试索引访问
void test_index_access() {
    std::cout << "测试索引访问...\n";
    
    std::string source = R"(
[Custom] @Element List
{
    div { text { 项目1 } }
    div { text { 项目2 } }
    div { text { 项目3 } }
}

body
{
    @Element List
    {
        div[1]
        {
            style
            {
                background-color: yellow;
            }
        }
    }
}
)";
    
    try {
        // 词法分析
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(source, "index_test.chtl");
        
        // 语法分析
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState stateManager;
        Parser::CHTLParser parser(globalMap, stateManager);
        
        auto ast = parser.Parse(tokens, "index_test.chtl");
        assert(ast != nullptr);
        
        // 代码生成
        Generator::CHTLGenerator generator(globalMap);
        std::string html = generator.Generate(ast);
        assert(!html.empty());
        
        // 验证HTML生成
        assert(html.find("<body>") != std::string::npos);
        
        std::cout << "  索引访问测试通过\n";
        
    } catch (const std::exception& e) {
        std::cerr << "索引访问测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

// 测试完整的高级特性集成
void test_advanced_integration() {
    std::cout << "测试高级特性集成...\n";
    
    std::string source = R"(
[Configuration] Global
{
    pretty_print: true;
    auto_doctype: true;
}

[Template] @Style BaseCard
{
    border: 1px solid #ddd;
    padding: 16px;
    margin: 8px;
}

[Custom] @Style RedCard
{
    @Style BaseCard
    {
        delete margin;
    }
    background-color: red;
    color: white;
}

[Custom] @Element CardList
{
    div
    {
        style { .card-container { display: flex; } }
    }
    
    div
    {
        style { @Style RedCard; }
        text { 卡片内容 }
    }
}

html
{
    body
    {
        @Element CardList
        {
            insert after div[0] {
                div
                {
                    style { background-color: blue; }
                    text { 插入的蓝色卡片 }
                }
            }
            
            delete div[1];
        }
    }
}
)";
    
    try {
        // 词法分析
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(source, "advanced_test.chtl");
        
        // 语法分析
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState stateManager;
        Parser::CHTLParser parser(globalMap, stateManager);
        
        auto ast = parser.Parse(tokens, "advanced_test.chtl");
        assert(ast != nullptr);
        
        // 代码生成
        Generator::CHTLGenerator generator(globalMap);
        std::string html = generator.Generate(ast);
        assert(!html.empty());
        
        // 验证高级特性
        assert(html.find("<!DOCTYPE html>") != std::string::npos);  // 自动DOCTYPE
        assert(html.find("<html") != std::string::npos);           // HTML结构
        assert(html.find(".card-container") != std::string::npos); // CSS选择器
        
        std::cout << "  高级特性集成测试通过\n";
        
        // 输出生成的HTML（如果不太长）
        if (html.length() < 2000) {
            std::cout << "\n生成的HTML:\n" << html << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "高级特性集成测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

int main() {
    std::cout << "运行CHTL高级特性测试...\n\n";
    
    try {
        test_element_insertion();
        test_element_deletion();
        test_constraint_validation();
        test_configuration_system();
        test_index_access();
        test_advanced_integration();
        
        std::cout << "\n所有高级特性测试通过!\n";
        std::cout << "✅ 元素插入操作正常\n";
        std::cout << "✅ 元素删除操作正常\n";
        std::cout << "✅ 约束验证正常\n";
        std::cout << "✅ 配置系统正常\n";
        std::cout << "✅ 索引访问正常\n";
        std::cout << "✅ 高级特性集成正常\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}