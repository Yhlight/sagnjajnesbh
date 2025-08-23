#include <iostream>
#include <cassert>
#include <memory>
#include "CHTL/Constraints/CHTLConstraintValidator.h"
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
    auto templateVar = std::make_shared<AST::VariableGroupNode>("[Template] @Var TestVar", Core::CHTLToken());
    auto originBlock = std::make_shared<AST::OriginNode>(AST::OriginNode::OriginType::HTML, "<div>原始HTML</div>", "", Core::CHTLToken());
    
    // 测试全局样式块约束（目标规划.ini第137行）
    std::cout << "  测试全局样式块约束...\n";
    
    // 模板变量应该被允许
    bool templateVarAllowed = validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_STYLE);
    std::cout << "    模板变量在全局样式块中: " << (templateVarAllowed ? "允许" : "禁止") << "\n";
    assert(templateVarAllowed);
    
    // HTML元素应该被禁止
    bool htmlElementAllowed = validator.ValidateNode(htmlElement, SyntaxContext::GLOBAL_STYLE);
    std::cout << "    HTML元素在全局样式块中: " << (htmlElementAllowed ? "允许" : "禁止") << "\n";
    assert(!htmlElementAllowed);
    
    // 原始嵌入总是被允许
    bool originAllowed = validator.ValidateNode(originBlock, SyntaxContext::GLOBAL_STYLE);
    std::cout << "    原始嵌入在全局样式块中: " << (originAllowed ? "允许" : "禁止") << "\n";
    assert(originAllowed);
    
    std::cout << "    ✅ 全局样式块约束验证正确\n";
    
    // 测试全局script约束（目标规划.ini第139行）
    std::cout << "  测试全局script约束...\n";
    
    // 模板变量应该被禁止
    bool templateVarInScript = validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_SCRIPT);
    std::cout << "    模板变量在全局script中: " << (templateVarInScript ? "允许" : "禁止") << "\n";
    assert(!templateVarInScript);
    
    // 原始嵌入应该被允许
    bool originInScript = validator.ValidateNode(originBlock, SyntaxContext::GLOBAL_SCRIPT);
    std::cout << "    原始嵌入在全局script中: " << (originInScript ? "允许" : "禁止") << "\n";
    assert(originInScript);
    
    std::cout << "    ✅ 全局script约束验证正确\n";
    
    // 测试局部script约束（目标规划.ini第143行）
    std::cout << "  测试局部script约束...\n";
    
    // 模板变量应该被允许
    bool templateVarInLocalScript = validator.ValidateNode(templateVar, SyntaxContext::LOCAL_SCRIPT);
    std::cout << "    模板变量在局部script中: " << (templateVarInLocalScript ? "允许" : "禁止") << "\n";
    assert(templateVarInLocalScript);
    
    // HTML元素应该被禁止
    bool htmlInLocalScript = validator.ValidateNode(htmlElement, SyntaxContext::LOCAL_SCRIPT);
    std::cout << "    HTML元素在局部script中: " << (htmlInLocalScript ? "允许" : "禁止") << "\n";
    assert(!htmlInLocalScript);
    
    std::cout << "    ✅ 局部script约束验证正确\n";
    
    std::cout << "  ✅ 基本约束验证器测试通过\n";
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
    
    std::cout << "    精确约束规则: " << preciseRule.description << "\n";
    
    // 测试类型约束规则构建
    auto typeRule = ConstraintRuleBuilder::CreateTypeRule(
        ConstraintTarget::CUSTOM_TYPE, "禁止使用[Custom]类型");
    
    assert(typeRule.type == ConstraintType::TYPE);
    assert(typeRule.target == ConstraintTarget::CUSTOM_TYPE);
    assert(typeRule.description.find("Custom") != std::string::npos);
    assert(typeRule.isActive);
    
    std::cout << "    类型约束规则: " << typeRule.description << "\n";
    
    // 测试全局约束规则构建
    auto globalRule = ConstraintRuleBuilder::CreateGlobalRule(
        ConstraintTarget::TEMPLATE_TYPE, "在命名空间中禁止[Template]");
    
    assert(globalRule.type == ConstraintType::GLOBAL);
    assert(globalRule.target == ConstraintTarget::TEMPLATE_TYPE);
    assert(globalRule.description.find("Template") != std::string::npos);
    assert(globalRule.isActive);
    
    std::cout << "    全局约束规则: " << globalRule.description << "\n";
    
    std::cout << "  ✅ 约束规则构建器测试通过\n";
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
    
    std::cout << "    样式块上下文: " << (styleContext == SyntaxContext::GLOBAL_STYLE ? "全局样式" : "局部样式") << "\n";
    std::cout << "    脚本块上下文: " << (scriptContext == SyntaxContext::GLOBAL_SCRIPT ? "全局脚本" : "局部脚本") << "\n";
    std::cout << "    命名空间上下文: " << (namespaceContext == SyntaxContext::NAMESPACE_BODY ? "命名空间主体" : "其他") << "\n";
    
    assert(styleContext == SyntaxContext::GLOBAL_STYLE || styleContext == SyntaxContext::LOCAL_STYLE);
    assert(scriptContext == SyntaxContext::GLOBAL_SCRIPT || scriptContext == SyntaxContext::LOCAL_SCRIPT);
    assert(namespaceContext == SyntaxContext::NAMESPACE_BODY);
    
    std::cout << "  ✅ 语法上下文检测器测试通过\n";
}

// 测试except约束规则
void test_except_constraint_rules() {
    std::cout << "测试except约束规则...\n";
    
    CHTLConstraintValidator validator;
    
    // 测试添加精确约束：禁止使用div元素
    std::cout << "  测试精确约束...\n";
    ConstraintRule preciseRule = ConstraintRuleBuilder::CreatePreciseRule(
        ConstraintTarget::HTML_ELEMENT, "div", "测试：禁止使用div元素");
    validator.AddExceptRule(SyntaxContext::ELEMENT_BODY, preciseRule);
    
    // 验证精确约束
    bool divAllowed = validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "div");
    bool spanAllowed = validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "span");
    
    std::cout << "    div元素允许状态: " << (divAllowed ? "允许" : "禁止") << "\n";
    std::cout << "    span元素允许状态: " << (spanAllowed ? "允许" : "禁止") << "\n";
    
    assert(!divAllowed);  // div应该被禁止
    assert(spanAllowed);  // span应该被允许
    
    // 测试添加类型约束：禁止使用[Custom]类型
    std::cout << "  测试类型约束...\n";
    ConstraintRule typeRule = ConstraintRuleBuilder::CreateTypeRule(
        ConstraintTarget::CUSTOM_TYPE, "测试：禁止使用[Custom]类型");
    validator.AddExceptRule(SyntaxContext::ELEMENT_BODY, typeRule);
    
    // 验证类型约束
    bool customTypeAllowed = validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::CUSTOM_TYPE);
    bool templateTypeAllowed = validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::TEMPLATE_TYPE);
    
    std::cout << "    [Custom]类型允许状态: " << (customTypeAllowed ? "允许" : "禁止") << "\n";
    std::cout << "    [Template]类型允许状态: " << (templateTypeAllowed ? "允许" : "禁止") << "\n";
    
    assert(!customTypeAllowed);    // [Custom]应该被禁止
    assert(templateTypeAllowed);   // [Template]应该被允许
    
    // 测试约束规则移除
    std::cout << "  测试约束规则移除...\n";
    validator.RemoveExceptRule(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "div");
    
    bool divAllowedAfterRemove = validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT, "div");
    std::cout << "    移除约束后div元素允许状态: " << (divAllowedAfterRemove ? "允许" : "禁止") << "\n";
    assert(divAllowedAfterRemove); // div应该重新被允许
    
    std::cout << "  ✅ except约束规则测试通过\n";
}

// 测试目标规划.ini约束要求
void test_target_planning_constraints() {
    std::cout << "测试目标规划.ini约束要求...\n";
    
    CHTLConstraintValidator validator;
    
    // 创建各种测试节点
    auto htmlElement = std::make_shared<AST::ElementNode>("div", Core::CHTLToken());
    auto templateVar = std::make_shared<AST::VariableGroupNode>("[Template] @Var TestVar", Core::CHTLToken());
    auto customVar = std::make_shared<AST::VariableGroupNode>("[Custom] @Var TestVar", Core::CHTLToken());
    auto templateStyle = std::make_shared<AST::StyleBlockNode>(Core::CHTLToken());
    auto originEmbed = std::make_shared<AST::OriginNode>(AST::OriginNode::OriginType::HTML, "<div>test</div>", "", Core::CHTLToken());
    
    // 目标规划.ini第137行：全局样式块约束测试
    std::cout << "  测试全局样式块约束（目标规划.ini第137行）...\n";
    
    bool templateVarInGlobalStyle = validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_STYLE);
    bool customVarInGlobalStyle = validator.ValidateNode(customVar, SyntaxContext::GLOBAL_STYLE);
    bool templateStyleInGlobalStyle = validator.ValidateNode(templateStyle, SyntaxContext::GLOBAL_STYLE);
    bool originEmbedInGlobalStyle = validator.ValidateNode(originEmbed, SyntaxContext::GLOBAL_STYLE);
    bool htmlElementInGlobalStyle = validator.ValidateNode(htmlElement, SyntaxContext::GLOBAL_STYLE);
    
    std::cout << "    模板变量: " << (templateVarInGlobalStyle ? "✅ 允许" : "❌ 禁止") << "\n";
    std::cout << "    自定义变量: " << (customVarInGlobalStyle ? "✅ 允许" : "❌ 禁止") << "\n";
    std::cout << "    模板样式组: " << (templateStyleInGlobalStyle ? "✅ 允许" : "❌ 禁止") << "\n";
    std::cout << "    原始嵌入: " << (originEmbedInGlobalStyle ? "✅ 允许" : "❌ 禁止") << "\n";
    std::cout << "    HTML元素: " << (htmlElementInGlobalStyle ? "❌ 意外允许" : "✅ 正确禁止") << "\n";
    
    assert(templateVarInGlobalStyle);    // 模板变量 ✅
    assert(customVarInGlobalStyle);      // 自定义变量 ✅
    assert(templateStyleInGlobalStyle);  // 模板样式组 ✅
    assert(originEmbedInGlobalStyle);    // 原始嵌入 ✅
    assert(!htmlElementInGlobalStyle);   // HTML元素 ❌
    
    std::cout << "    ✅ 全局样式块约束符合要求\n";
    
    // 目标规划.ini第139行：全局script约束测试
    std::cout << "  测试全局script约束（目标规划.ini第139行）...\n";
    
    bool originEmbedInGlobalScript = validator.ValidateNode(originEmbed, SyntaxContext::GLOBAL_SCRIPT);
    bool templateVarInGlobalScript = validator.ValidateNode(templateVar, SyntaxContext::GLOBAL_SCRIPT);
    bool customVarInGlobalScript = validator.ValidateNode(customVar, SyntaxContext::GLOBAL_SCRIPT);
    bool htmlElementInGlobalScript = validator.ValidateNode(htmlElement, SyntaxContext::GLOBAL_SCRIPT);
    
    std::cout << "    原始嵌入: " << (originEmbedInGlobalScript ? "✅ 允许" : "❌ 禁止") << "\n";
    std::cout << "    模板变量: " << (templateVarInGlobalScript ? "❌ 意外允许" : "✅ 正确禁止") << "\n";
    std::cout << "    自定义变量: " << (customVarInGlobalScript ? "❌ 意外允许" : "✅ 正确禁止") << "\n";
    std::cout << "    HTML元素: " << (htmlElementInGlobalScript ? "❌ 意外允许" : "✅ 正确禁止") << "\n";
    
    assert(originEmbedInGlobalScript);    // 原始嵌入 ✅
    assert(!templateVarInGlobalScript);   // 模板变量 ❌
    assert(!customVarInGlobalScript);     // 自定义变量 ❌
    assert(!htmlElementInGlobalScript);   // HTML元素 ❌
    
    std::cout << "    ✅ 全局script约束符合要求\n";
    
    // 目标规划.ini第143行：局部script约束测试
    std::cout << "  测试局部script约束（目标规划.ini第143行）...\n";
    
    bool templateVarInLocalScript = validator.ValidateNode(templateVar, SyntaxContext::LOCAL_SCRIPT);
    bool customVarInLocalScript = validator.ValidateNode(customVar, SyntaxContext::LOCAL_SCRIPT);
    bool originEmbedInLocalScript = validator.ValidateNode(originEmbed, SyntaxContext::LOCAL_SCRIPT);
    bool htmlElementInLocalScript = validator.ValidateNode(htmlElement, SyntaxContext::LOCAL_SCRIPT);
    
    std::cout << "    模板变量: " << (templateVarInLocalScript ? "✅ 允许" : "❌ 禁止") << "\n";
    std::cout << "    自定义变量组: " << (customVarInLocalScript ? "✅ 允许" : "❌ 禁止") << "\n";
    std::cout << "    原始嵌入: " << (originEmbedInLocalScript ? "✅ 允许" : "❌ 禁止") << "\n";
    std::cout << "    HTML元素: " << (htmlElementInLocalScript ? "❌ 意外允许" : "✅ 正确禁止") << "\n";
    
    assert(templateVarInLocalScript);     // 模板变量 ✅
    assert(customVarInLocalScript);       // 自定义变量组 ✅
    assert(originEmbedInLocalScript);     // 原始嵌入 ✅
    assert(!htmlElementInLocalScript);    // HTML元素 ❌
    
    std::cout << "    ✅ 局部script约束符合要求\n";
    
    std::cout << "  ✅ 目标规划.ini约束要求测试通过\n";
}

// 测试IsAllowed方法
void test_is_allowed_method() {
    std::cout << "测试IsAllowed方法...\n";
    
    CHTLConstraintValidator validator;
    
    // 测试原始嵌入总是被允许
    assert(validator.IsAllowed(SyntaxContext::GLOBAL_STYLE, ConstraintTarget::ORIGIN_EMBED));
    assert(validator.IsAllowed(SyntaxContext::GLOBAL_SCRIPT, ConstraintTarget::ORIGIN_EMBED));
    assert(validator.IsAllowed(SyntaxContext::LOCAL_STYLE, ConstraintTarget::ORIGIN_EMBED));
    assert(validator.IsAllowed(SyntaxContext::LOCAL_SCRIPT, ConstraintTarget::ORIGIN_EMBED));
    
    std::cout << "    ✅ 原始嵌入在所有上下文中都被允许\n";
    
    // 测试模板变量的上下文允许性
    assert(validator.IsAllowed(SyntaxContext::GLOBAL_STYLE, ConstraintTarget::TEMPLATE_VAR));  // 全局样式：允许
    assert(!validator.IsAllowed(SyntaxContext::GLOBAL_SCRIPT, ConstraintTarget::TEMPLATE_VAR)); // 全局script：禁止
    assert(validator.IsAllowed(SyntaxContext::LOCAL_STYLE, ConstraintTarget::TEMPLATE_VAR));   // 局部样式：允许
    assert(validator.IsAllowed(SyntaxContext::LOCAL_SCRIPT, ConstraintTarget::TEMPLATE_VAR));  // 局部script：允许
    
    std::cout << "    ✅ 模板变量的上下文约束正确\n";
    
    // 测试HTML元素的上下文允许性
    assert(!validator.IsAllowed(SyntaxContext::GLOBAL_STYLE, ConstraintTarget::HTML_ELEMENT));  // 全局样式：禁止
    assert(!validator.IsAllowed(SyntaxContext::GLOBAL_SCRIPT, ConstraintTarget::HTML_ELEMENT)); // 全局script：禁止
    assert(!validator.IsAllowed(SyntaxContext::LOCAL_STYLE, ConstraintTarget::HTML_ELEMENT));   // 局部样式：禁止
    assert(!validator.IsAllowed(SyntaxContext::LOCAL_SCRIPT, ConstraintTarget::HTML_ELEMENT));  // 局部script：禁止
    assert(validator.IsAllowed(SyntaxContext::ELEMENT_BODY, ConstraintTarget::HTML_ELEMENT));   // 元素主体：允许
    
    std::cout << "    ✅ HTML元素的上下文约束正确\n";
    
    std::cout << "  ✅ IsAllowed方法测试通过\n";
}

int main() {
    std::cout << "运行CHTL语法约束验证器简化测试...\n\n";
    
    try {
        test_basic_constraint_validator();
        test_constraint_rule_builder();
        test_syntax_context_detector();
        test_except_constraint_rules();
        test_target_planning_constraints();
        test_is_allowed_method();
        
        std::cout << "\n🎉 所有语法约束验证器简化测试通过!\n";
        std::cout << "✅ 基本约束验证器正常\n";
        std::cout << "✅ 约束规则构建器正常\n";
        std::cout << "✅ 语法上下文检测器正常\n";
        std::cout << "✅ except约束规则正常\n";
        std::cout << "✅ 目标规划.ini约束要求符合\n";
        std::cout << "✅ IsAllowed方法正常\n";
        
        std::cout << "\n📋 约束验证器功能总结:\n";
        std::cout << "🔒 全局样式块：仅允许模板变量、自定义变量、样式组等\n";
        std::cout << "🔒 全局script：禁止所有CHTL语法，仅允许原始嵌入\n";
        std::cout << "🔓 局部样式块：允许模板变量、自定义变量、样式组等\n";
        std::cout << "🔓 局部script：允许模板变量、自定义变量组等\n";
        std::cout << "🚫 except约束：支持精确约束、类型约束、全局约束\n";
        std::cout << "✨ 原始嵌入：在任何地方都被允许\n";
        
        std::cout << "\n🎯 严格按照目标规划.ini第137-143行实现:\n";
        std::cout << "📝 第137行：全局样式块约束 - 完整实现\n";
        std::cout << "📝 第139行：全局script约束 - 完整实现\n";
        std::cout << "📝 第141行：局部样式块约束 - 完整实现\n";
        std::cout << "📝 第143行：局部script约束 - 完整实现\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}