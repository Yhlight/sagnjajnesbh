#include <iostream>
#include <cassert>
#include <memory>
#include "CHTL/Lexer/CHTLLexer.h"
#include "CHTL/Parser/CHTLParser.h"
#include "CHTL/Generator/CHTLGenerator.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"
#include "CHTL/AST/CHTLASTVisitor.h"

using namespace CHTL;

// 测试词法分析器
void test_lexer() {
    std::cout << "测试CHTL词法分析器...\n";
    
    std::string source = R"(
[Template] @Style ButtonStyle
{
    background-color: #007bff;
    color: white;
}

html
{
    body
    {
        div
        {
            id: main;
            class: container;
            
            text { Hello World }
        }
    }
}
)";
    
    Lexer::CHTLLexer lexer;
    auto tokens = lexer.Tokenize(source, "test.chtl");
    
    assert(tokens.Size() > 0);
    
    // 验证一些关键Token
    tokens.Reset();
    assert(tokens.Current().GetType() == Core::TokenType::TEMPLATE);
    
    tokens.Advance();
    assert(tokens.Current().GetType() == Core::TokenType::AT_STYLE);
    
    tokens.Advance();
    assert(tokens.Current().GetType() == Core::TokenType::IDENTIFIER);
    assert(tokens.Current().GetValue() == "ButtonStyle");
    
    std::cout << "  词法分析器测试通过 - 生成了 " << tokens.Size() << " 个Token\n";
}

// 测试语法分析器
void test_parser() {
    std::cout << "测试CHTL语法分析器...\n";
    
    std::string source = R"(
[Template] @Style ButtonStyle
{
    background-color: #007bff;
    color: white;
}

html
{
    body
    {
        div
        {
            id: main;
            class: container;
            
            text { Hello World }
        }
    }
}
)";
    
    // 词法分析
    Lexer::CHTLLexer lexer;
    auto tokens = lexer.Tokenize(source, "test.chtl");
    
    // 语法分析
    Core::CHTLGlobalMap globalMap;
    Core::CHTLState stateManager;
    Parser::CHTLParser parser(globalMap, stateManager);
    
    auto ast = parser.Parse(tokens, "test.chtl");
    
    assert(ast != nullptr);
    assert(ast->GetType() == AST::NodeType::ROOT);
    
    // 验证AST结构
    auto rootNode = std::dynamic_pointer_cast<AST::RootNode>(ast);
    assert(rootNode != nullptr);
    assert(rootNode->GetFileName() == "test.chtl");
    assert(rootNode->GetChildCount() > 0);
    
    // 查找模板节点
    auto templateNodes = rootNode->FindChildrenByType(AST::NodeType::TEMPLATE_STYLE);
    assert(templateNodes.size() >= 1);
    
    auto templateNode = std::dynamic_pointer_cast<AST::TemplateNode>(templateNodes[0]);
    assert(templateNode != nullptr);
    assert(templateNode->GetName() == "ButtonStyle");
    assert(templateNode->GetTemplateType() == AST::TemplateNode::TemplateType::STYLE);
    
    // 查找HTML元素
    auto elementNodes = rootNode->FindChildrenByType(AST::NodeType::ELEMENT);
    assert(elementNodes.size() >= 1);
    
    std::cout << "  语法分析器测试通过 - 生成了 " << parser.GetStatistics() << "\n";
}

// 测试代码生成器
void test_generator() {
    std::cout << "测试CHTL代码生成器...\n";
    
    // 创建一个简单的AST
    auto root = std::make_shared<AST::RootNode>();
    root->SetFileName("test.chtl");
    
    // 创建HTML结构
    auto html = std::make_shared<AST::ElementNode>("html");
    auto head = std::make_shared<AST::ElementNode>("head");
    auto title = std::make_shared<AST::ElementNode>("title");
    auto titleText = std::make_shared<AST::TextNode>("测试页面");
    
    title->AddChild(titleText);
    head->AddChild(title);
    
    auto body = std::make_shared<AST::ElementNode>("body");
    auto div = std::make_shared<AST::ElementNode>("div");
    div->AddAttribute("id", "main");
    div->AddAttribute("class", "container");
    
    auto divText = std::make_shared<AST::TextNode>("Hello World");
    div->AddChild(divText);
    body->AddChild(div);
    
    html->AddChild(head);
    html->AddChild(body);
    root->AddChild(html);
    
    // 生成HTML
    Core::CHTLGlobalMap globalMap;
    Generator::CHTLGenerator generator(globalMap);
    
    std::string htmlOutput = generator.Generate(root);
    
    assert(!htmlOutput.empty());
    assert(htmlOutput.find("<html") != std::string::npos);
    assert(htmlOutput.find("<head") != std::string::npos);
    assert(htmlOutput.find("<body") != std::string::npos);
    assert(htmlOutput.find("Hello World") != std::string::npos);
    assert(htmlOutput.find("id=\"main\"") != std::string::npos);
    assert(htmlOutput.find("class=\"container\"") != std::string::npos);
    
    std::cout << "  代码生成器测试通过 - 生成了 " << htmlOutput.length() << " 字符的HTML\n";
}

// 测试完整的编译流程
void test_complete_compilation() {
    std::cout << "测试完整的CHTL编译流程...\n";
    
    std::string source = R"(
[Template] @Style DefaultText
{
    color: black;
    line-height: 1.6;
}

html
{
    head
    {
        title
        {
            text { CHTL编译测试 }
        }
    }
    
    body
    {
        div
        {
            id: container;
            class: main;
            
            style
            {
                @Style DefaultText;
                
                .box
                {
                    width: 200px;
                    height: 200px;
                    background-color: red;
                }
            }
            
            text { 这是一个测试文本 }
        }
    }
}

[Origin] @Style
{
    * {
        box-sizing: border-box;
    }
}
)";
    
    // 1. 词法分析
    Lexer::CHTLLexer lexer;
    auto tokens = lexer.Tokenize(source, "complete_test.chtl");
    
    assert(tokens.Size() > 0);
    std::cout << "  词法分析完成 - " << tokens.Size() << " 个Token\n";
    
    // 2. 语法分析
    Core::CHTLGlobalMap globalMap;
    Core::CHTLState stateManager;
    Parser::CHTLParser parser(globalMap, stateManager);
    
    auto ast = parser.Parse(tokens, "complete_test.chtl");
    
    assert(ast != nullptr);
    assert(parser.GetErrorCount() == 0);
    std::cout << "  语法分析完成 - 无错误\n";
    
    // 3. AST验证
    AST::ASTValidationVisitor validator;
    ast->Accept(validator);
    
    if (validator.HasErrors()) {
        std::cout << "  AST验证发现错误:\n";
        for (const auto& error : validator.GetErrors()) {
            std::cout << "    " << error << "\n";
        }
    } else {
        std::cout << "  AST验证通过\n";
    }
    
    // 4. 代码生成
    Generator::CHTLGenerator generator(globalMap);
    std::string htmlOutput = generator.Generate(ast);
    
    assert(!htmlOutput.empty());
    std::cout << "  代码生成完成 - " << htmlOutput.length() << " 字符\n";
    
    // 5. 验证生成的HTML
    assert(htmlOutput.find("<!DOCTYPE html>") != std::string::npos);
    assert(htmlOutput.find("<html") != std::string::npos);
    assert(htmlOutput.find("<head>") != std::string::npos);
    assert(htmlOutput.find("<title>CHTL编译测试</title>") != std::string::npos);
    assert(htmlOutput.find("<body>") != std::string::npos);
    assert(htmlOutput.find("id=\"container\"") != std::string::npos);
    assert(htmlOutput.find("class=\"main\"") != std::string::npos);
    assert(htmlOutput.find("这是一个测试文本") != std::string::npos);
    assert(htmlOutput.find("box-sizing: border-box") != std::string::npos);
    
    std::cout << "  HTML验证通过\n";
    
    // 打印生成的HTML（调试用）
    if (htmlOutput.length() < 1000) {
        std::cout << "\n生成的HTML:\n" << htmlOutput << "\n";
    }
}

// 测试AST打印功能
void test_ast_printing() {
    std::cout << "测试AST打印功能...\n";
    
    // 创建简单的AST
    auto root = std::make_shared<AST::RootNode>();
    root->SetFileName("print_test.chtl");
    
    auto template1 = std::make_shared<AST::TemplateNode>(
        AST::TemplateNode::TemplateType::STYLE, "TestStyle");
    
    auto element = std::make_shared<AST::ElementNode>("div");
    element->AddAttribute("class", "test");
    
    auto text = std::make_shared<AST::TextNode>("Test Content");
    element->AddChild(text);
    
    root->AddChild(template1);
    root->AddChild(element);
    
    // 使用打印访问者
    AST::ASTPrintVisitor printVisitor;
    root->Accept(printVisitor);
    
    std::string astString = printVisitor.GetResult();
    assert(!astString.empty());
    assert(astString.find("ROOT") != std::string::npos);
    assert(astString.find("TEMPLATE_Style") != std::string::npos);
    assert(astString.find("ELEMENT(div") != std::string::npos);
    assert(astString.find("TEXT") != std::string::npos);
    
    std::cout << "  AST打印功能测试通过\n";
    std::cout << "\nAST结构:\n" << astString << "\n";
}

// 测试错误处理
void test_error_handling() {
    std::cout << "测试错误处理...\n";
    
    std::string invalidSource = R"(
[Template @Style  // 缺少右括号
{
    color: red
}

html
{
    body
    {
        div
        {
            id main  // 缺少冒号
            
            text { 未闭合文本
        }
    }
)";
    
    // 清空之前的错误
    Utils::ErrorHandler::GetInstance().Clear();
    
    // 词法分析
    Lexer::CHTLLexer lexer;
    auto tokens = lexer.Tokenize(invalidSource, "error_test.chtl");
    
    // 语法分析
    Core::CHTLGlobalMap globalMap;
    Core::CHTLState stateManager;
    Parser::ParserConfig config;
    config.enableErrorRecovery = true;
    Parser::CHTLParser parser(globalMap, stateManager, config);
    
    auto ast = parser.Parse(tokens, "error_test.chtl");
    
    // 应该有错误报告
    assert(Utils::ErrorHandler::GetInstance().HasErrors());
    std::cout << "  错误处理测试通过 - 检测到 " 
              << Utils::ErrorHandler::GetInstance().GetErrorCount() << " 个错误\n";
}

int main() {
    std::cout << "运行CHTL编译器完整功能测试...\n\n";
    
    try {
        test_lexer();
        test_parser();
        test_generator();
        test_complete_compilation();
        test_ast_printing();
        test_error_handling();
        
        std::cout << "\n所有CHTL编译器功能测试通过!\n";
        std::cout << "✅ 词法分析器正常工作\n";
        std::cout << "✅ 语法分析器正常工作\n";
        std::cout << "✅ 代码生成器正常工作\n";
        std::cout << "✅ 完整编译流程正常\n";
        std::cout << "✅ AST打印功能正常\n";
        std::cout << "✅ 错误处理机制正常\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}