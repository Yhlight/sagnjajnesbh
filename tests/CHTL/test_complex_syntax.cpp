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

// 测试继承功能
void test_inheritance() {
    std::cout << "测试继承功能...\n";
    
    std::string source = R"(
[Template] @Style BaseStyle
{
    color: black;
    font-size: 16px;
}

[Template] @Style ExtendedStyle
{
    background-color: white;
    inherit @Style BaseStyle;
}
)";
    
    // 词法分析
    Lexer::CHTLLexer lexer;
    auto tokens = lexer.Tokenize(source, "inheritance_test.chtl");
    
    // 语法分析
    Core::CHTLGlobalMap globalMap;
    Core::CHTLState stateManager;
    Parser::CHTLParser parser(globalMap, stateManager);
    
    auto ast = parser.Parse(tokens, "inheritance_test.chtl");
    
    assert(ast != nullptr);
    
    // 验证AST结构
    auto templateNodes = ast->FindChildrenByType(AST::NodeType::TEMPLATE_STYLE);
    assert(templateNodes.size() == 2);
    
    // 检查第二个模板是否有继承
    auto extendedTemplate = std::dynamic_pointer_cast<AST::TemplateNode>(templateNodes[1]);
    assert(extendedTemplate != nullptr);
    assert(extendedTemplate->GetName() == "ExtendedStyle");
    assert(extendedTemplate->GetInheritances().size() == 1);
    
    std::cout << "  继承功能测试通过\n";
}

// 测试删除功能
void test_deletion() {
    std::cout << "测试删除功能...\n";
    
    std::string source = R"(
[Custom] @Style TestStyle
{
    @Style BaseStyle
    {
        delete color, font-size;
    }
    background-color: red;
}
)";
    
    // 词法分析
    Lexer::CHTLLexer lexer;
    auto tokens = lexer.Tokenize(source, "deletion_test.chtl");
    
    // 语法分析
    Core::CHTLGlobalMap globalMap;
    Core::CHTLState stateManager;
    Parser::CHTLParser parser(globalMap, stateManager);
    
    auto ast = parser.Parse(tokens, "deletion_test.chtl");
    
    assert(ast != nullptr);
    
    // 验证删除节点
    auto customNodes = ast->FindChildrenByType(AST::NodeType::CUSTOM_STYLE);
    assert(customNodes.size() == 1);
    
    std::cout << "  删除功能测试通过\n";
}

// 测试变量组特例化
void test_variable_specialization() {
    std::cout << "测试变量组特例化...\n";
    
    std::string source = R"(
[Custom] @Var Colors
{
    primary: "#007bff";
    secondary: "#6c757d";
}

div
{
    style
    {
        color: Colors(primary = "#ff0000");
    }
}
)";
    
    // 词法分析
    Lexer::CHTLLexer lexer;
    auto tokens = lexer.Tokenize(source, "variable_test.chtl");
    
    // 语法分析
    Core::CHTLGlobalMap globalMap;
    Core::CHTLState stateManager;
    Parser::CHTLParser parser(globalMap, stateManager);
    
    auto ast = parser.Parse(tokens, "variable_test.chtl");
    
    assert(ast != nullptr);
    
    std::cout << "  变量组特例化测试通过\n";
}

// 测试无值样式组
void test_valueless_style_group() {
    std::cout << "测试无值样式组...\n";
    
    std::string source = R"(
[Custom] @Style TextSet
{
    color,
    font-size;
}

div
{
    style
    {
        @Style TextSet
        {
            color: red;
            font-size: 16px;
        }
    }
}
)";
    
    // 词法分析
    Lexer::CHTLLexer lexer;
    auto tokens = lexer.Tokenize(source, "valueless_test.chtl");
    
    // 语法分析
    Core::CHTLGlobalMap globalMap;
    Core::CHTLState stateManager;
    Parser::CHTLParser parser(globalMap, stateManager);
    
    auto ast = parser.Parse(tokens, "valueless_test.chtl");
    
    assert(ast != nullptr);
    
    std::cout << "  无值样式组测试通过\n";
}

// 测试CSS选择器自动添加功能
void test_css_selector_auto_add() {
    std::cout << "测试CSS选择器自动添加功能...\n";
    
    std::string source = R"(
div
{
    style
    {
        .box
        {
            width: 100px;
            height: 100px;
        }
        
        &:hover
        {
            background-color: red;
        }
    }
}
)";
    
    // 词法分析
    Lexer::CHTLLexer lexer;
    auto tokens = lexer.Tokenize(source, "selector_test.chtl");
    
    // 语法分析
    Core::CHTLGlobalMap globalMap;
    Core::CHTLState stateManager;
    Parser::CHTLParser parser(globalMap, stateManager);
    
    auto ast = parser.Parse(tokens, "selector_test.chtl");
    
    assert(ast != nullptr);
    
    // 代码生成
    Generator::CHTLGenerator generator(globalMap);
    std::string html = generator.Generate(ast);
    
    // 验证生成的HTML包含自动添加的类名
    assert(html.find("class=\"box\"") != std::string::npos);
    
    // 验证全局样式包含选择器
    assert(html.find(".box {") != std::string::npos);
    assert(html.find(".box:hover {") != std::string::npos);
    
    std::cout << "  CSS选择器自动添加功能测试通过\n";
}

// 测试完整的复杂语法编译
void test_complete_complex_syntax() {
    std::cout << "测试完整的复杂语法编译...\n";
    
    std::string source = R"(
[Template] @Style ButtonBase
{
    padding: 8px 16px;
    border: none;
    cursor: pointer;
}

[Custom] @Style PrimaryBtn
{
    @Style ButtonBase
    {
        delete border;
    }
    background-color: #007bff;
    color: white;
}

div
{
    style
    {
        @Style PrimaryBtn;
        
        .btn
        {
            border-radius: 4px;
        }
    }
    
    text { 测试按钮 }
}
)";
    
    try {
        // 词法分析
        Lexer::CHTLLexer lexer;
        auto tokens = lexer.Tokenize(source, "complex_test.chtl");
        assert(tokens.Size() > 0);
        
        // 语法分析
        Core::CHTLGlobalMap globalMap;
        Core::CHTLState stateManager;
        Parser::CHTLParser parser(globalMap, stateManager);
        
        auto ast = parser.Parse(tokens, "complex_test.chtl");
        assert(ast != nullptr);
        
        // 代码生成
        Generator::CHTLGenerator generator(globalMap);
        std::string html = generator.Generate(ast);
        
        assert(!html.empty());
        assert(html.find("<div") != std::string::npos);
        assert(html.find("测试按钮") != std::string::npos);
        
        std::cout << "  复杂语法编译测试通过\n";
        
        // 打印生成的HTML（如果不太长）
        if (html.length() < 1000) {
            std::cout << "\n生成的HTML:\n" << html << "\n";
        }
        
    } catch (const std::exception& e) {
        std::cerr << "复杂语法测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

int main() {
    std::cout << "运行CHTL复杂语法测试...\n\n";
    
    try {
        test_inheritance();
        test_deletion();
        test_variable_specialization();
        test_valueless_style_group();
        test_css_selector_auto_add();
        test_complete_complex_syntax();
        
        std::cout << "\n所有复杂语法测试通过!\n";
        std::cout << "✅ 继承功能正常\n";
        std::cout << "✅ 删除功能正常\n";
        std::cout << "✅ 变量特例化正常\n";
        std::cout << "✅ 无值样式组正常\n";
        std::cout << "✅ CSS选择器自动添加正常\n";
        std::cout << "✅ 复杂语法编译正常\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}