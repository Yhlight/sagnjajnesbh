#include <iostream>
#include <cassert>
#include <memory>
#include "CHTL/Constraints/CHTLConstraintValidator.h"
#include "CHTL/Constraints/ExceptConstraintParser.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/Core/CHTLToken.h"

using namespace CHTL;
using namespace CHTL::Constraints;

// 测试基本约束验证器
void test_basic_constraint_validator() {
    std::cout << "测试基本约束验证器...\n";
    
    CHTLConstraintValidator validator;
    
    // 创建测试节点
    auto htmlElement = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    auto customElement = std::make_shared<AST::ElementNode>("[Custom] @Element Box", Core::CHTLToken());
    auto templateVar = std::make_shared<AST::VariableGroupNode>("[Template] @Var TestVar", Core::CHTLToken());
    auto originBlock = std::make_shared<AST::OriginHtmlNode>("<div>原始HTML</div>", Core::CHTLToken());
    
    // 测试全局样式块约束（目标规划.ini第137行）
    std::cout << "  测试全局样式块约束...\n";
    
    // 模板变量应该被允许
    assert(validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_STYLE));
    
    // HTML元素应该被禁止
    assert(!validator.ValidateNode(htmlElement, SyntaxContext::GLOBAL_STYLE));
    
    // 原始嵌入总是被允许
    assert(validator.ValidateNode(originBlock, SyntaxContext::GLOBAL_STYLE));
    
    std::cout << "    ✅ 全局样式块约束验证正确\n";
    
    // 测试全局script约束（目标规划.ini第139行）
    std::cout << "  测试全局script约束...\n";
    
    // 模板变量应该被禁止
    assert(!validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_SCRIPT));
    
    // 原始嵌入应该被允许
    assert(validator.ValidateNode(originBlock, SyntaxContext::GLOBAL_SCRIPT));
    
    std::cout << "    ✅ 全局script约束验证正确\n";
    
    // 测试局部script约束（目标规划.ini第143行）
    std::cout << "  测试局部script约束...\n";
    
    // 模板变量应该被允许
    assert(validator.ValidateNode(templateVar, SyntaxContext::LOCAL_SCRIPT));
    
    // HTML元素应该被禁止
    assert(!validator.ValidateNode(htmlElement, SyntaxContext::LOCAL_SCRIPT));
    
    std::cout << "    ✅ 局部script约束验证正确\n";
    
    std::cout << "  ✅ 基本约束验证器测试通过\n";
}

// 测试except约束解析器
void test_except_constraint_parser() {
    std::cout << "测试except约束解析器...\n";
    
    // 创建测试Token流
    std::vector<Core::CHTLToken> tokens = {
        Core::CHTLToken(Core::TokenType::EXCEPT, "except"),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "span"),
        Core::CHTLToken(Core::TokenType::COMMA, ","),
        Core::CHTLToken(Core::TokenType::LEFT_BRACKET, "["),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Custom"),
        Core::CHTLToken(Core::TokenType::RIGHT_BRACKET, "]"),
        Core::CHTLToken(Core::TokenType::AT, "@"),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Element"),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Box"),
        Core::CHTLToken(Core::TokenType::SEMICOLON, ";")
    };
    
    ExceptConstraintParser parser(tokens);
    size_t pos = 0;
    
    // 解析except约束
    auto rules = parser.ParseExceptConstraints(pos);
    
    assert(rules.size() == 2); // 应该解析出两个约束规则
    
    // 验证第一个规则（精确约束：span）
    assert(rules[0].type == ConstraintType::PRECISE);
    assert(rules[0].target == ConstraintTarget::HTML_ELEMENT);
    assert(rules[0].specificName == "span");
    
    // 验证第二个规则（类型约束：[Custom] @Element Box）
    assert(rules[1].type == ConstraintType::TYPE);
    assert(rules[1].target == ConstraintTarget::CUSTOM_ELEMENT);
    assert(rules[1].specificName.find("Custom") != std::string::npos);
    assert(rules[1].specificName.find("Box") != std::string::npos);
    
    std::cout << "  ✅ except约束解析器测试通过\n";
    std::cout << "    解析出约束规则数量: " << rules.size() << "\n";
    std::cout << "    第一个规则: " << rules[0].description << "\n";
    std::cout << "    第二个规则: " << rules[1].description << "\n";
}

// 测试类型约束解析
void test_type_constraint_parsing() {
    std::cout << "测试类型约束解析...\n";
    
    // 测试 except @Html; 语法
    std::vector<Core::CHTLToken> tokens1 = {
        Core::CHTLToken(Core::TokenType::EXCEPT, "except", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::AT, "@", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Html", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::SEMICOLON, ";", Core::TokenPosition())
    };
    
    ExceptConstraintParser parser1(tokens1);
    size_t pos1 = 0;
    auto rules1 = parser1.ParseExceptConstraints(pos1);
    
    assert(rules1.size() == 1);
    assert(rules1[0].type == ConstraintType::TYPE);
    assert(rules1[0].target == ConstraintTarget::HTML_TYPE);
    assert(rules1[0].specificName == "@Html");
    
    // 测试 except [Template] @Var; 语法
    std::vector<Core::CHTLToken> tokens2 = {
        Core::CHTLToken(Core::TokenType::EXCEPT, "except", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::LEFT_BRACKET, "[", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Template", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::RIGHT_BRACKET, "]", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::AT, "@", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Var", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::SEMICOLON, ";", Core::TokenPosition())
    };
    
    ExceptConstraintParser parser2(tokens2);
    size_t pos2 = 0;
    auto rules2 = parser2.ParseExceptConstraints(pos2);
    
    assert(rules2.size() == 1);
    assert(rules2[0].type == ConstraintType::TYPE);
    assert(rules2[0].target == ConstraintTarget::TEMPLATE_VAR);
    assert(rules2[0].specificName.find("Template") != std::string::npos);
    assert(rules2[0].specificName.find("@Var") != std::string::npos);
    
    // 测试 except [Custom]; 语法
    std::vector<Core::CHTLToken> tokens3 = {
        Core::CHTLToken(Core::TokenType::EXCEPT, "except", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::LEFT_BRACKET, "[", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Custom", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::RIGHT_BRACKET, "]", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::SEMICOLON, ";", Core::TokenPosition())
    };
    
    ExceptConstraintParser parser3(tokens3);
    size_t pos3 = 0;
    auto rules3 = parser3.ParseExceptConstraints(pos3);
    
    assert(rules3.size() == 1);
    assert(rules3[0].type == ConstraintType::TYPE);
    assert(rules3[0].target == ConstraintTarget::CUSTOM_TYPE);
    assert(rules3[0].specificName == "[Custom]");
    
    std::cout << "  ✅ 类型约束解析测试通过\n";
    std::cout << "    @Html约束解析正确\n";
    std::cout << "    [Template] @Var约束解析正确\n";
    std::cout << "    [Custom]约束解析正确\n";
}

// 测试约束集成器
void test_constraint_integrator() {
    std::cout << "测试约束集成器...\n";
    
    CHTLConstraintValidator validator;
    ExceptConstraintIntegrator integrator(validator);
    
    // 测试集成精确约束
    std::vector<Core::CHTLToken> tokens = {
        Core::CHTLToken(Core::TokenType::EXCEPT, "except", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "span", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::COMMA, ",", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "div", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::SEMICOLON, ";", Core::TokenPosition())
    };
    
    bool success = integrator.ProcessExceptConstraints(tokens, SyntaxContext::ELEMENT_BODY, 0);
    assert(success);
    
    // 验证约束是否生效
    assert(!validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "span"));
    assert(!validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "div"));
    assert(validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "p")); // 未被约束的元素
    
    std::cout << "  ✅ 约束集成器测试通过\n";
    std::cout << "    精确约束集成成功\n";
    std::cout << "    约束验证生效\n";
}

// 测试语法上下文检测器
void test_syntax_context_detector() {
    std::cout << "测试语法上下文检测器...\n";
    
    // 创建测试节点
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
    auto scriptBlock = std::make_shared<AST::ScriptBlockNode>(Core::CHTLToken());
    auto namespaceNode = std::make_shared<AST::NamespaceNode>("testSpace", Core::CHTLToken());
    
    // 测试上下文检测
    auto styleContext = SyntaxContextDetector::DetectContext(styleBlock, SyntaxContext::ELEMENT_BODY);
    auto scriptContext = SyntaxContextDetector::DetectContext(scriptBlock, SyntaxContext::ELEMENT_BODY);
    auto namespaceContext = SyntaxContextDetector::DetectContext(namespaceNode, SyntaxContext::ELEMENT_BODY);
    
    assert(styleContext == SyntaxContext::GLOBAL_STYLE || styleContext == SyntaxContext::LOCAL_STYLE);
    assert(scriptContext == SyntaxContext::GLOBAL_SCRIPT || scriptContext == SyntaxContext::LOCAL_SCRIPT);
    assert(namespaceContext == SyntaxContext::NAMESPACE_BODY);
    
    std::cout << "  ✅ 语法上下文检测器测试通过\n";
}

// 测试完整的约束工作流程
void test_complete_constraint_workflow() {
    std::cout << "测试完整的约束工作流程...\n";
    
    try {
        // 1. 创建约束验证器
        CHTLConstraintValidator validator;
        
        // 2. 测试默认约束规则
        std::cout << "  测试默认约束规则...\n";
        
        // 创建各种测试节点
        auto htmlDiv = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
        auto customElement = std::make_shared<AST::ElementNode>("[Custom] @Element MyBox", Core::CHTLToken());
        auto templateVar = std::make_shared<AST::VariableGroupNode>("[Template] @Var MyVar", Core::CHTLToken());
        auto originEmbed = std::make_shared<AST::OriginBlockNode>("html", "<span>原始</span>", Core::CHTLToken());
        
        // 全局样式块测试
        assert(!validator.ValidateNode(htmlDiv, SyntaxContext::GLOBAL_STYLE));      // HTML元素应被禁止
        assert(validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_STYLE));   // 模板变量应被允许
        assert(validator.ValidateNode(originEmbed, SyntaxContext::GLOBAL_STYLE));   // 原始嵌入总是允许
        
        // 全局script块测试
        assert(!validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_SCRIPT)); // 模板变量应被禁止
        assert(validator.ValidateNode(originEmbed, SyntaxContext::GLOBAL_SCRIPT));  // 原始嵌入总是允许
        
        // 局部script块测试
        assert(validator.ValidateNode(templateVar, SyntaxContext::LOCAL_SCRIPT));   // 模板变量应被允许
        assert(!validator.ValidateNode(htmlDiv, SyntaxContext::LOCAL_SCRIPT));      // HTML元素应被禁止
        
        std::cout << "    ✅ 默认约束规则验证正确\n";
        
        // 3. 测试except约束规则
        std::cout << "  测试except约束规则...\n";
        
        // 添加精确约束：禁止使用div元素
        ConstraintRule preciseRule = ConstraintRuleBuilder::CreatePreciseRule(
            ConstraintTarget::HTML_ELEMENT, "div", "测试：禁止使用div元素");
        validator.AddExceptRule(SyntaxContext::ELEMENT_BODY, preciseRule);
        
        // 验证精确约束
        assert(!validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "div"));
        assert(validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "span")); // 其他元素仍允许
        
        // 添加类型约束：禁止使用[Custom]类型
        ConstraintRule typeRule = ConstraintRuleBuilder::CreateTypeRule(
            ConstraintTarget::CUSTOM_TYPE, "测试：禁止使用[Custom]类型");
        validator.AddExceptRule(SyntaxContext::ELEMENT_BODY, typeRule);
        
        // 验证类型约束
        assert(!validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::CUSTOM_TYPE));
        assert(validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::TEMPLATE_TYPE)); // [Template]仍允许
        
        std::cout << "    ✅ except约束规则验证正确\n";
        
        // 4. 测试约束规则移除
        std::cout << "  测试约束规则移除...\n";
        
        validator.RemoveExceptRule(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "div");
        assert(validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "div")); // div应该重新被允许
        
        std::cout << "    ✅ 约束规则移除正确\n";
        
        std::cout << "  ✅ 完整约束工作流程测试通过\n";
        
    } catch (const std::exception& e) {
        std::cerr << "约束工作流程测试异常: " << e.what() << std::endl;
        assert(false);
    }
}

// 测试语法文档示例的约束解析
void test_syntax_document_examples() {
    std::cout << "测试语法文档示例的约束解析...\n";
    
    CHTLConstraintValidator validator;
    ExceptConstraintIntegrator integrator(validator);
    
    // 语法文档第1071行示例：except span, [Custom] @Element Box;
    std::cout << "  测试精确约束示例...\n";
    std::vector<Core::CHTLToken> example1 = {
        Core::CHTLToken(Core::TokenType::EXCEPT, "except", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "span", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::COMMA, ",", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::LEFT_BRACKET, "[", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Custom", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::RIGHT_BRACKET, "]", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::AT, "@", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Element", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Box", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::SEMICOLON, ";", Core::TokenPosition())
    };
    
    bool success1 = integrator.ProcessExceptConstraints(example1, SyntaxContext::ELEMENT_BODY, 0);
    assert(success1);
    
    // 验证约束效果
    assert(!validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "span"));
    assert(!validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::CUSTOM_ELEMENT, "[Custom] @Element Box"));
    
    std::cout << "    ✅ 精确约束示例解析正确\n";
    
    // 语法文档第1081行示例：except @Html;
    std::cout << "  测试类型约束示例...\n";
    std::vector<Core::CHTLToken> example2 = {
        Core::CHTLToken(Core::TokenType::EXCEPT, "except", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::AT, "@", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Html", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::SEMICOLON, ";", Core::TokenPosition())
    };
    
    CHTLConstraintValidator validator2;
    ExceptConstraintIntegrator integrator2(validator2);
    
    bool success2 = integrator2.ProcessExceptConstraints(example2, SyntaxContext::ELEMENT_BODY, 0);
    assert(success2);
    
    // 验证约束效果
    assert(!validator2.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_TYPE));
    
    std::cout << "    ✅ 类型约束示例解析正确\n";
    
    // 语法文档第1093行示例：except [Template];（全局约束）
    std::cout << "  测试全局约束示例...\n";
    std::vector<Core::CHTLToken> example3 = {
        Core::CHTLToken(Core::TokenType::EXCEPT, "except", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::LEFT_BRACKET, "[", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::IDENTIFIER, "Template", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::RIGHT_BRACKET, "]", Core::TokenPosition()),
        Core::CHTLToken(Core::TokenType::SEMICOLON, ";", Core::TokenPosition())
    };
    
    CHTLConstraintValidator validator3;
    ExceptConstraintIntegrator integrator3(validator3);
    
    bool success3 = integrator3.ProcessExceptConstraints(example3, SyntaxContext::NAMESPACE_BODY, 0);
    assert(success3);
    
    // 验证约束效果
    assert(!validator3.IsAllowed(SyntaxContext::NAMESPACE_BODY, ConstraintTarget::TEMPLATE_TYPE));
    
    std::cout << "    ✅ 全局约束示例解析正确\n";
    
    std::cout << "  ✅ 语法文档示例约束解析测试通过\n";
}

// 测试约束规则构建器
void test_constraint_rule_builder() {
    std::cout << "测试约束规则构建器...\n";
    
    // 测试精确约束规则构建
    auto preciseRule = ConstraintRuleBuilder::CreatePreciseRule(
        ConstraintTarget::HTML_ELEMENT, "span", "禁止使用span元素");
    
    assert(preciseRule.type == ConstraintType::PRECISE);
    assert(preciseRule.target == ConstraintTarget::HTML_ELEMENT);
    assert(preciseRule.specificName == "span");
    assert(preciseRule.description.find("span") != std::string::npos);
    assert(preciseRule.isActive);
    
    // 测试类型约束规则构建
    auto typeRule = ConstraintRuleBuilder::CreateTypeRule(
        ConstraintTarget::CUSTOM_TYPE, "禁止使用[Custom]类型");
    
    assert(typeRule.type == ConstraintType::TYPE);
    assert(typeRule.target == ConstraintTarget::CUSTOM_TYPE);
    assert(typeRule.description.find("Custom") != std::string::npos);
    assert(typeRule.isActive);
    
    // 测试全局约束规则构建
    auto globalRule = ConstraintRuleBuilder::CreateGlobalRule(
        ConstraintTarget::TEMPLATE_TYPE, "在命名空间中禁止[Template]");
    
    assert(globalRule.type == ConstraintType::GLOBAL);
    assert(globalRule.target == ConstraintTarget::TEMPLATE_TYPE);
    assert(globalRule.description.find("Template") != std::string::npos);
    assert(globalRule.isActive);
    
    std::cout << "  ✅ 约束规则构建器测试通过\n";
    std::cout << "    精确约束规则构建正确\n";
    std::cout << "    类型约束规则构建正确\n";
    std::cout << "    全局约束规则构建正确\n";
}

// 测试目标规划.ini约束要求
void test_target_planning_constraints() {
    std::cout << "测试目标规划.ini约束要求...\n";
    
    CHTLConstraintValidator validator;
    
    // 创建各种测试节点
    auto htmlElement = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    auto templateVar = std::make_shared<AST::VariableGroupNode>("[Template] @Var TestVar", Core::CHTLToken());
    auto customVar = std::make_shared<AST::VariableGroupNode>("[Custom] @Var TestVar", Core::CHTLToken());
    auto templateStyle = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken()); // 假设为模板样式
    auto originEmbed = std::make_shared<AST::OriginBlockNode>("html", "<div>test</div>", Core::CHTLToken());
    
    // 目标规划.ini第137行：全局样式块约束测试
    std::cout << "  测试全局样式块约束（目标规划.ini第137行）...\n";
    
    assert(validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_STYLE));    // 模板变量 ✅
    assert(validator.ValidateNode(customVar, SyntaxContext::GLOBAL_STYLE));      // 自定义变量 ✅
    assert(validator.ValidateNode(templateStyle, SyntaxContext::GLOBAL_STYLE));  // 模板样式组 ✅
    assert(validator.ValidateNode(originEmbed, SyntaxContext::GLOBAL_STYLE));    // 原始嵌入 ✅
    assert(!validator.ValidateNode(htmlElement, SyntaxContext::GLOBAL_STYLE));   // HTML元素 ❌
    
    std::cout << "    ✅ 全局样式块约束符合要求\n";
    
    // 目标规划.ini第139行：全局script约束测试
    std::cout << "  测试全局script约束（目标规划.ini第139行）...\n";
    
    assert(validator.ValidateNode(originEmbed, SyntaxContext::GLOBAL_SCRIPT));    // 原始嵌入 ✅
    assert(!validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_SCRIPT));   // 模板变量 ❌
    assert(!validator.ValidateNode(customVar, SyntaxContext::GLOBAL_SCRIPT));     // 自定义变量 ❌
    assert(!validator.ValidateNode(htmlElement, SyntaxContext::GLOBAL_SCRIPT));   // HTML元素 ❌
    
    std::cout << "    ✅ 全局script约束符合要求\n";
    
    // 目标规划.ini第143行：局部script约束测试
    std::cout << "  测试局部script约束（目标规划.ini第143行）...\n";
    
    assert(validator.ValidateNode(templateVar, SyntaxContext::LOCAL_SCRIPT));     // 模板变量 ✅
    assert(validator.ValidateNode(customVar, SyntaxContext::LOCAL_SCRIPT));       // 自定义变量组 ✅
    assert(validator.ValidateNode(originEmbed, SyntaxContext::LOCAL_SCRIPT));     // 原始嵌入 ✅
    assert(!validator.ValidateNode(htmlElement, SyntaxContext::LOCAL_SCRIPT));    // HTML元素 ❌
    
    std::cout << "    ✅ 局部script约束符合要求\n";
    
    std::cout << "  ✅ 目标规划.ini约束要求测试通过\n";
}

int main() {
    std::cout << "运行CHTL语法约束验证器测试...\n\n";
    
    try {
        test_basic_constraint_validator();
        test_except_constraint_parser();
        test_type_constraint_parsing();
        test_constraint_integrator();
        test_syntax_context_detector();
        test_constraint_rule_builder();
        test_target_planning_constraints();
        test_complete_constraint_workflow();
        
        std::cout << "\n🎉 所有语法约束验证器测试通过!\n";
        std::cout << "✅ 基本约束验证器正常\n";
        std::cout << "✅ except约束解析器正常\n";
        std::cout << "✅ 类型约束解析正常\n";
        std::cout << "✅ 约束集成器正常\n";
        std::cout << "✅ 语法上下文检测器正常\n";
        std::cout << "✅ 约束规则构建器正常\n";
        std::cout << "✅ 目标规划.ini约束要求符合\n";
        std::cout << "✅ 完整约束工作流程正常\n";
        
        std::cout << "\n📋 约束验证器功能总结:\n";
        std::cout << "🔒 全局样式块：仅允许模板变量、自定义变量、样式组等\n";
        std::cout << "🔒 全局script：禁止所有CHTL语法，仅允许原始嵌入\n";
        std::cout << "🔓 局部样式块：允许模板变量、自定义变量、样式组等\n";
        std::cout << "🔓 局部script：允许模板变量、自定义变量组等\n";
        std::cout << "🚫 except约束：支持精确约束、类型约束、全局约束\n";
        std::cout << "✨ 原始嵌入：在任何地方都被允许\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}