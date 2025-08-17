#include "../src/common/SyntaxConstrainer.h"
#include "../src/ast/CHTLNodes.h"
#include <iostream>
#include <cassert>

using namespace chtl;
using namespace chtl::ast;

void testGlobalStyleConstraints() {
    std::cout << "=== 测试全局样式块约束 ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    
    // 测试允许的语法元素
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::TEMPLATE_VAR));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::CUSTOM_VAR));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::VAR_SPECIALIZATION));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::TEMPLATE_STYLE));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::CUSTOM_STYLE));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::VALUELESS_STYLE));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::STYLE_SPECIALIZATION));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::DELETE_PROPERTY));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::DELETE_INHERITANCE));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::STYLE_INHERITANCE));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::NAMESPACE_FROM));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::QUALIFIED_NAME));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::GENERATOR_COMMENT));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::ORIGIN_EMBED));
    
    // 测试禁止的语法元素
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::CHTL_JS_SELECTOR));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::CHTL_JS_LISTEN));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::CHTL_JS_DELEGATE));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::CHTL_JS_VIR));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::TEMPLATE_ELEMENT));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::CUSTOM_ELEMENT));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::ELEMENT_INSERT));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::ELEMENT_DELETE));
    
    std::cout << "✅ 全局样式块约束测试通过!" << std::endl;
}

void testGlobalScriptConstraints() {
    std::cout << "=== 测试全局script约束 ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    
    // 测试允许的语法元素
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::GENERATOR_COMMENT));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::ORIGIN_EMBED));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::JAVASCRIPT_CODE));
    
    // 注意：{{&}}等特供语法属于CHTL本身功能，应该允许
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CHTL_JS_SELECTOR));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CHTL_JS_ARROW));
    
    // 测试禁止的CHTL语法元素
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::TEMPLATE_VAR));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CUSTOM_VAR));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::VAR_SPECIALIZATION));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::TEMPLATE_STYLE));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CUSTOM_STYLE));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::NAMESPACE_FROM));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CHTL_JS_LISTEN));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CHTL_JS_DELEGATE));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CHTL_JS_ANIMATE));
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CHTL_JS_VIR));
    
    std::cout << "✅ 全局script约束测试通过!" << std::endl;
}

void testLocalStyleConstraints() {
    std::cout << "=== 测试局部样式块约束 ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    
    // 测试允许的语法元素（与全局样式块相同）
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::TEMPLATE_VAR));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::CUSTOM_VAR));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::VAR_SPECIALIZATION));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::TEMPLATE_STYLE));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::CUSTOM_STYLE));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::VALUELESS_STYLE));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::STYLE_SPECIALIZATION));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::DELETE_PROPERTY));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::DELETE_INHERITANCE));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::STYLE_INHERITANCE));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::NAMESPACE_FROM));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::QUALIFIED_NAME));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::GENERATOR_COMMENT));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::ORIGIN_EMBED));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::CSS_PROPERTY));
    
    // 测试禁止的语法元素
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::CHTL_JS_SELECTOR));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::CHTL_JS_LISTEN));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::CHTL_JS_DELEGATE));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::CHTL_JS_VIR));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::ELEMENT_INSERT));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::ELEMENT_DELETE));
    
    std::cout << "✅ 局部样式块约束测试通过!" << std::endl;
}

void testLocalScriptConstraints() {
    std::cout << "=== 测试局部script约束 ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    
    // 测试允许的CHTL语法元素
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::TEMPLATE_VAR));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CUSTOM_VAR));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::VAR_SPECIALIZATION));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::NAMESPACE_FROM));
    
    // 测试允许的特殊存在
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::GENERATOR_COMMENT));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::ORIGIN_EMBED));
    
    // 测试允许的CHTL JS语法（{{&}}等特供语法）
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CHTL_JS_SELECTOR));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CHTL_JS_ARROW));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CHTL_JS_LISTEN));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CHTL_JS_DELEGATE));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CHTL_JS_ANIMATE));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CHTL_JS_VIR));
    
    // 测试允许的JavaScript代码
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::JAVASCRIPT_CODE));
    
    // 测试禁止的样式相关语法
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::TEMPLATE_STYLE));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CUSTOM_STYLE));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::VALUELESS_STYLE));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::STYLE_SPECIALIZATION));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::STYLE_INHERITANCE));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::DELETE_PROPERTY));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::DELETE_INHERITANCE));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CSS_PROPERTY));
    
    // 测试禁止的元素相关语法
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::TEMPLATE_ELEMENT));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CUSTOM_ELEMENT));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::ELEMENT_SPECIALIZATION));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::ELEMENT_INSERT));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::ELEMENT_DELETE));
    
    std::cout << "✅ 局部script约束测试通过!" << std::endl;
}

void testUniversalElements() {
    std::cout << "=== 测试特殊存在元素（可在任意位置使用） ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    
    // 注释和原始嵌入应该在所有上下文中都被允许
    std::vector<ContextType> all_contexts = {
        ContextType::GLOBAL_STYLE,
        ContextType::LOCAL_STYLE,
        ContextType::GLOBAL_SCRIPT,
        ContextType::LOCAL_SCRIPT
    };
    
    for (ContextType context : all_contexts) {
        assert(constrainer.isElementAllowed(context, SyntaxElement::GENERATOR_COMMENT));
        assert(constrainer.isElementAllowed(context, SyntaxElement::ORIGIN_EMBED));
    }
    
    std::cout << "✅ 特殊存在元素测试通过!" << std::endl;
}

void testCHTLJSSpecialSyntax() {
    std::cout << "=== 测试CHTL JS特供语法（{{&}}等） ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    
    // {{&}}等特供语法属于CHTL本身功能，不应该被误禁
    
    // 在全局script中应该允许（因为是CHTL本身功能）
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CHTL_JS_SELECTOR));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CHTL_JS_ARROW));
    
    // 在局部script中也应该允许
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CHTL_JS_SELECTOR));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::CHTL_JS_ARROW));
    
    // 但在样式块中不应该允许
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::CHTL_JS_SELECTOR));
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::CHTL_JS_SELECTOR));
    
    std::cout << "✅ CHTL JS特供语法测试通过!" << std::endl;
}

void testContextInference() {
    std::cout << "=== 测试上下文推断 ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    
    // 创建测试节点
    auto style_block = std::make_unique<StyleBlockNode>();
    auto script_block = std::make_unique<ScriptBlockNode>();
    
    // 测试上下文推断
    ContextType local_style_context = constrainer.inferContextType(style_block.get(), ContextType::ELEMENT_CONTENT);
    assert(local_style_context == ContextType::LOCAL_STYLE);
    
    ContextType global_style_context = constrainer.inferContextType(style_block.get(), ContextType::TEMPLATE_DEF);
    assert(global_style_context == ContextType::GLOBAL_STYLE);
    
    ContextType local_script_context = constrainer.inferContextType(script_block.get(), ContextType::ELEMENT_CONTENT);
    assert(local_script_context == ContextType::LOCAL_SCRIPT);
    
    ContextType global_script_context = constrainer.inferContextType(script_block.get(), ContextType::TEMPLATE_DEF);
    assert(global_script_context == ContextType::GLOBAL_SCRIPT);
    
    std::cout << "✅ 上下文推断测试通过!" << std::endl;
}

void testConstraintViolationReporting() {
    std::cout << "=== 测试约束违规报告 ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    constrainer.setStrictMode(false); // 关闭严格模式以避免stderr输出
    
    // 创建违规场景
    ConstraintViolation violation;
    violation.context = ContextType::GLOBAL_STYLE;
    violation.forbidden_element = SyntaxElement::CHTL_JS_LISTEN;
    violation.element_name = "listen";
    violation.line = 10;
    violation.column = 5;
    violation.message = "测试违规";
    
    constrainer.reportViolation(violation);
    
    auto violations = constrainer.getViolations();
    assert(violations.size() == 1);
    assert(violations[0].context == ContextType::GLOBAL_STYLE);
    assert(violations[0].forbidden_element == SyntaxElement::CHTL_JS_LISTEN);
    
    // 测试建议生成
    std::string suggestion = constrainer.generateSuggestion(violation);
    assert(!suggestion.empty());
    
    constrainer.clearViolations();
    assert(constrainer.getViolations().empty());
    
    std::cout << "✅ 约束违规报告测试通过!" << std::endl;
}

void testConstraintRuleManagement() {
    std::cout << "=== 测试约束规则管理 ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    
    // 测试添加自定义约束规则
    constrainer.addConstraintRule(ContextType::GLOBAL_STYLE, SyntaxElement::JAVASCRIPT_CODE, false);
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::JAVASCRIPT_CODE));
    
    // 测试移除约束规则
    constrainer.removeConstraintRule(ContextType::GLOBAL_STYLE, SyntaxElement::JAVASCRIPT_CODE);
    
    // 测试约束规则检查
    assert(constrainer.hasConstraintRule(ContextType::GLOBAL_STYLE, SyntaxElement::TEMPLATE_VAR));
    
    std::cout << "✅ 约束规则管理测试通过!" << std::endl;
}

void testRealWorldScenarios() {
    std::cout << "=== 测试真实世界场景 ===" << std::endl;
    
    SyntaxConstrainer constrainer;
    
    std::cout << "场景1: 全局样式块中使用模板变量 - 应该允许" << std::endl;
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::TEMPLATE_VAR));
    
    std::cout << "场景2: 全局样式块中使用listen函数 - 应该禁止" << std::endl;
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::CHTL_JS_LISTEN));
    
    std::cout << "场景3: 局部script中使用变量组特例化 - 应该允许" << std::endl;
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::VAR_SPECIALIZATION));
    
    std::cout << "场景4: 局部script中使用样式组 - 应该禁止" << std::endl;
    assert(!constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::TEMPLATE_STYLE));
    
    std::cout << "场景5: 全局script中使用{{&}}选择器 - 应该允许（特供语法）" << std::endl;
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::CHTL_JS_SELECTOR));
    
    std::cout << "场景6: 全局script中使用模板变量 - 应该禁止" << std::endl;
    assert(!constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::TEMPLATE_VAR));
    
    std::cout << "场景7: 任意位置使用生成器注释 - 应该允许" << std::endl;
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::GENERATOR_COMMENT));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::GENERATOR_COMMENT));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::GENERATOR_COMMENT));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::GENERATOR_COMMENT));
    
    std::cout << "场景8: 任意位置使用原始嵌入 - 应该允许" << std::endl;
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_STYLE, SyntaxElement::ORIGIN_EMBED));
    assert(constrainer.isElementAllowed(ContextType::GLOBAL_SCRIPT, SyntaxElement::ORIGIN_EMBED));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_STYLE, SyntaxElement::ORIGIN_EMBED));
    assert(constrainer.isElementAllowed(ContextType::LOCAL_SCRIPT, SyntaxElement::ORIGIN_EMBED));
    
    std::cout << "✅ 真实世界场景测试通过!" << std::endl;
}

void testConstrainerUtils() {
    std::cout << "=== 测试约束器工具函数 ===" << std::endl;
    
    // 测试语法分类
    assert(ConstrainerUtils::isVariableElement(SyntaxElement::TEMPLATE_VAR));
    assert(ConstrainerUtils::isVariableElement(SyntaxElement::CUSTOM_VAR));
    assert(!ConstrainerUtils::isVariableElement(SyntaxElement::TEMPLATE_STYLE));
    
    assert(ConstrainerUtils::isStyleElement(SyntaxElement::TEMPLATE_STYLE));
    assert(ConstrainerUtils::isStyleElement(SyntaxElement::CUSTOM_STYLE));
    assert(!ConstrainerUtils::isStyleElement(SyntaxElement::TEMPLATE_VAR));
    
    // 测试上下文字符串转换
    assert(ConstrainerUtils::contextTypeToString(ContextType::GLOBAL_STYLE) == "全局样式块");
    assert(ConstrainerUtils::contextTypeToString(ContextType::LOCAL_SCRIPT) == "局部script");
    
    // 测试语法元素字符串转换
    assert(ConstrainerUtils::syntaxElementToString(SyntaxElement::TEMPLATE_VAR) == "模板变量");
    assert(ConstrainerUtils::syntaxElementToString(SyntaxElement::CHTL_JS_LISTEN) == "listen函数");
    
    std::cout << "✅ 约束器工具函数测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始测试语法约束器..." << std::endl << std::endl;
        
        testGlobalStyleConstraints();
        std::cout << std::endl;
        
        testGlobalScriptConstraints();
        std::cout << std::endl;
        
        testLocalStyleConstraints();
        std::cout << std::endl;
        
        testLocalScriptConstraints();
        std::cout << std::endl;
        
        testUniversalElements();
        std::cout << std::endl;
        
        testCHTLJSSpecialSyntax();
        std::cout << std::endl;
        
        testContextInference();
        std::cout << std::endl;
        
        testConstraintViolationReporting();
        std::cout << std::endl;
        
        testConstraintRuleManagement();
        std::cout << std::endl;
        
        testRealWorldScenarios();
        std::cout << std::endl;
        
        testConstrainerUtils();
        std::cout << std::endl;
        
        std::cout << "🎉 所有语法约束器测试通过!" << std::endl;
        
        // 打印约束规则统计
        SyntaxConstrainer final_constrainer;
        std::cout << "\n" << final_constrainer.getConstraintStatistics() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}