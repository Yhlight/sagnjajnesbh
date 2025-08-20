#include <iostream>
#include <cassert>
#include <memory>
#include "chtl/constraint/ConstraintSystem.h"
#include "chtl/constraint/ConstraintChecker.h"
#include "chtl/ast/DocumentNode.h"
#include "chtl/ast/StyleBlockNode.h"
#include "chtl/ast/ScriptBlockNode.h"
#include "chtl/ast/ElementNode.h"
#include "chtl/ast/TemplateNode.h"
#include "chtl/ast/CustomNode.h"
#include "chtl/ast/CommentNode.h"
#include "chtl/ast/OriginNode.h"
#include "utils/Logger.h"

using namespace chtl;
using namespace chtl::constraint;
using namespace chtl::ast;

// 创建测试AST
std::shared_ptr<DocumentNode> CreateTestDocument() {
    auto doc = std::make_shared<DocumentNode>();
    
    // 创建一个div元素
    auto div = std::make_shared<ElementNode>("div");
    
    // 添加局部样式块
    auto localStyle = std::make_shared<StyleBlockNode>();
    localStyle->SetLocation("test.chtl");
    
    // 在局部样式块中使用模板变量（允许）
    auto templateVar = std::make_shared<TemplateVarNode>("ThemeColor");
    localStyle->AddChild(templateVar);
    
    div->AddChild(localStyle);
    
    // 添加局部script块
    auto localScript = std::make_shared<ScriptBlockNode>();
    localScript->SetLocation("test.chtl");
    
    // 在局部script中使用自定义变量（允许）
    auto customVar = std::make_shared<CustomVarNode>("MyVar");
    localScript->AddChild(customVar);
    
    div->AddChild(localScript);
    
    doc->AddChild(div);
    
    // 添加全局样式块
    auto globalStyle = std::make_shared<StyleBlockNode>();
    globalStyle->SetLocation("test.chtl");
    
    // 在全局样式块中使用样式组继承（允许）
    auto styleInherit = std::make_shared<InheritNode>();
    styleInherit->SetTargetType("@Style");
    styleInherit->SetTargetName("BaseStyle");
    globalStyle->AddChild(styleInherit);
    
    doc->AddChild(globalStyle);
    
    // 添加全局script块
    auto globalScript = std::make_shared<ScriptBlockNode>();
    globalScript->SetLocation("test.chtl");
    
    // 在全局script中只添加注释（允许）
    auto comment = std::make_shared<CommentNode>("-- 这是一个注释");
    globalScript->AddChild(comment);
    
    doc->AddChild(globalScript);
    
    return doc;
}

// 创建违反约束的AST
std::shared_ptr<DocumentNode> CreateViolatingDocument() {
    auto doc = std::make_shared<DocumentNode>();
    
    // 全局script块
    auto globalScript = std::make_shared<ScriptBlockNode>();
    globalScript->SetLocation("test.chtl");
    
    // 在全局script中使用模板变量（违反约束）
    auto templateVar = std::make_shared<TemplateVarNode>("ThemeColor");
    globalScript->AddChild(templateVar);
    
    doc->AddChild(globalScript);
    
    // 全局样式块
    auto globalStyle = std::make_shared<StyleBlockNode>();
    globalStyle->SetLocation("test.chtl");
    
    // 在全局样式块中使用HTML元素（违反约束）
    auto htmlElement = std::make_shared<ElementNode>("div");
    globalStyle->AddChild(htmlElement);
    
    doc->AddChild(globalStyle);
    
    return doc;
}

void TestConstraintRule() {
    std::cout << "测试约束规则..." << std::endl;
    
    // 创建全局样式块规则
    ConstraintRule globalStyleRule(ContextType::GLOBAL_STYLE);
    
    // 添加允许的元素
    globalStyleRule.AddAllowedElement(SyntaxElement::TEMPLATE_VAR);
    globalStyleRule.AddAllowedElement(SyntaxElement::CUSTOM_VAR);
    globalStyleRule.AddAllowedElement(SyntaxElement::TEMPLATE_STYLE);
    
    // 测试允许的元素
    assert(globalStyleRule.IsAllowed(SyntaxElement::TEMPLATE_VAR));
    assert(globalStyleRule.IsAllowed(SyntaxElement::CUSTOM_VAR));
    
    // 测试不允许的元素
    assert(!globalStyleRule.IsAllowed(SyntaxElement::HTML_ELEMENT));
    
    // 测试特殊允许的元素（注释和原始嵌入）
    assert(globalStyleRule.IsAllowed(SyntaxElement::GENERATOR_COMMENT));
    assert(globalStyleRule.IsAllowed(SyntaxElement::ORIGIN_EMBED));
    
    std::cout << "  约束规则测试通过" << std::endl;
}

void TestConstraintManager() {
    std::cout << "\n测试约束管理器..." << std::endl;
    
    ConstraintManager manager;
    manager.InitializeDefaultRules();
    
    // 测试获取规则
    {
        const ConstraintRule* globalStyleRule = manager.GetRule(ContextType::GLOBAL_STYLE);
        assert(globalStyleRule != nullptr);
        assert(globalStyleRule->GetContext() == ContextType::GLOBAL_STYLE);
        
        const ConstraintRule* localScriptRule = manager.GetRule(ContextType::LOCAL_SCRIPT);
        assert(localScriptRule != nullptr);
        assert(localScriptRule->GetContext() == ContextType::LOCAL_SCRIPT);
        
        std::cout << "  规则获取测试通过" << std::endl;
    }
    
    // 测试默认规则内容
    {
        const ConstraintRule* globalScriptRule = manager.GetRule(ContextType::GLOBAL_SCRIPT);
        assert(globalScriptRule != nullptr);
        
        // 全局script只允许注释和原始嵌入
        assert(globalScriptRule->IsAllowed(SyntaxElement::GENERATOR_COMMENT));
        assert(globalScriptRule->IsAllowed(SyntaxElement::ORIGIN_EMBED));
        assert(!globalScriptRule->IsAllowed(SyntaxElement::TEMPLATE_VAR));
        assert(!globalScriptRule->IsAllowed(SyntaxElement::CUSTOM_VAR));
        
        std::cout << "  默认规则内容测试通过" << std::endl;
    }
    
    // 测试创建验证器
    {
        auto validator = manager.CreateValidator();
        assert(validator != nullptr);
        
        std::cout << "  验证器创建测试通过" << std::endl;
    }
}

void TestConstraintChecker() {
    std::cout << "\n测试约束检查器..." << std::endl;
    
    ConstraintChecker checker;
    
    // 测试符合约束的文档
    {
        auto validDoc = CreateTestDocument();
        assert(checker.CheckDocument(validDoc.get()));
        assert(checker.GetErrors().empty());
        
        std::cout << "  符合约束的文档测试通过" << std::endl;
    }
    
    // 测试违反约束的文档
    {
        checker.ClearErrors();
        auto violatingDoc = CreateViolatingDocument();
        assert(!checker.CheckDocument(violatingDoc.get()));
        assert(!checker.GetErrors().empty());
        
        std::cout << "  违反约束的文档测试通过" << std::endl;
        std::cout << "  检测到 " << checker.GetErrors().size() << " 个约束违反" << std::endl;
        
        for (const auto& error : checker.GetErrors()) {
            std::cout << "    - " << error << std::endl;
        }
    }
    
    // 测试获取建议
    {
        auto suggestions = checker.GetSuggestions(ContextType::GLOBAL_STYLE);
        assert(!suggestions.empty());
        
        std::cout << "  全局样式块允许的元素:" << std::endl;
        for (const auto& elem : suggestions) {
            std::cout << "    - " << ConstraintUtils::ElementToString(elem) << std::endl;
        }
        
        std::cout << "  建议获取测试通过" << std::endl;
    }
}

void TestConstraintReporter() {
    std::cout << "\n测试约束报告生成器..." << std::endl;
    
    ConstraintChecker checker;
    ConstraintReporter reporter;
    
    // 检查违反约束的文档
    auto violatingDoc = CreateViolatingDocument();
    checker.CheckDocument(violatingDoc.get());
    
    // 生成报告
    std::string report = reporter.GenerateReport(checker);
    
    // 验证报告内容
    assert(!report.empty());
    assert(report.find("约束检查报告") != std::string::npos);
    assert(report.find("约束违反") != std::string::npos);
    
    std::cout << "  报告生成测试通过" << std::endl;
    
    // 生成上下文指南
    std::string guide = reporter.GenerateContextGuide();
    assert(!guide.empty());
    assert(guide.find("上下文使用指南") != std::string::npos);
    
    std::cout << "  上下文指南生成测试通过" << std::endl;
}

void TestConstraintUtils() {
    std::cout << "\n测试约束工具类..." << std::endl;
    
    // 测试元素转字符串
    {
        assert(ConstraintUtils::ElementToString(SyntaxElement::TEMPLATE_VAR) == "模板变量");
        assert(ConstraintUtils::ElementToString(SyntaxElement::CUSTOM_STYLE) == "自定义样式组");
        assert(ConstraintUtils::ElementToString(SyntaxElement::GENERATOR_COMMENT) == "生成器注释");
        
        std::cout << "  元素名称转换测试通过" << std::endl;
    }
    
    // 测试上下文转字符串
    {
        assert(ConstraintUtils::ContextToString(ContextType::GLOBAL_STYLE) == "全局样式块");
        assert(ConstraintUtils::ContextToString(ContextType::LOCAL_SCRIPT) == "局部script块");
        
        std::cout << "  上下文名称转换测试通过" << std::endl;
    }
    
    // 测试通用允许元素
    {
        assert(ConstraintUtils::IsUniversallyAllowed(SyntaxElement::GENERATOR_COMMENT));
        assert(ConstraintUtils::IsUniversallyAllowed(SyntaxElement::ORIGIN_EMBED));
        assert(!ConstraintUtils::IsUniversallyAllowed(SyntaxElement::TEMPLATE_VAR));
        
        std::cout << "  通用允许元素测试通过" << std::endl;
    }
}

void TestComplexScenarios() {
    std::cout << "\n测试复杂场景..." << std::endl;
    
    ConstraintChecker checker;
    
    // 场景1：嵌套的样式块和script块
    {
        auto doc = std::make_shared<DocumentNode>();
        
        auto div = std::make_shared<ElementNode>("div");
        
        // 局部样式块内嵌套
        auto localStyle = std::make_shared<StyleBlockNode>();
        
        // 使用无值样式组（允许）
        auto valuelessStyle = std::make_shared<CustomStyleNode>("TextSet");
        valuelessStyle->SetValueless(true);
        localStyle->AddChild(valuelessStyle);
        
        // 使用delete属性（允许）
        auto deleteNode = std::make_shared<DeleteNode>();
        deleteNode->AddDeleteItem("line-height");
        localStyle->AddChild(deleteNode);
        
        div->AddChild(localStyle);
        
        // 局部script块
        auto localScript = std::make_shared<ScriptBlockNode>();
        
        // 使用命名空间from（允许）
        auto namespaceFrom = std::make_shared<VarReferenceNode>();
        namespaceFrom->SetFrom("MyNamespace");
        localScript->AddChild(namespaceFrom);
        
        div->AddChild(localScript);
        
        doc->AddChild(div);
        
        assert(checker.CheckDocument(doc.get()));
        assert(checker.GetErrors().empty());
        
        std::cout << "  嵌套场景测试通过" << std::endl;
    }
    
    // 场景2：原始嵌入在各种上下文中（都应该允许）
    {
        checker.ClearErrors();
        auto doc = std::make_shared<DocumentNode>();
        
        // 在顶层
        auto topOrigin = std::make_shared<OriginNode>();
        topOrigin->SetOriginType("@Html");
        doc->AddChild(topOrigin);
        
        // 在全局样式块中
        auto globalStyle = std::make_shared<StyleBlockNode>();
        auto styleOrigin = std::make_shared<OriginNode>();
        styleOrigin->SetOriginType("@Style");
        globalStyle->AddChild(styleOrigin);
        doc->AddChild(globalStyle);
        
        // 在全局script块中
        auto globalScript = std::make_shared<ScriptBlockNode>();
        auto scriptOrigin = std::make_shared<OriginNode>();
        scriptOrigin->SetOriginType("@Javascript");
        globalScript->AddChild(scriptOrigin);
        doc->AddChild(globalScript);
        
        assert(checker.CheckDocument(doc.get()));
        assert(checker.GetErrors().empty());
        
        std::cout << "  原始嵌入场景测试通过" << std::endl;
    }
}

int main() {
    // 设置日志级别
    utils::Logger::GetInstance().SetLevel(utils::LogLevel::INFO);
    
    std::cout << "运行约束系统测试...\n" << std::endl;
    
    try {
        TestConstraintRule();
        TestConstraintManager();
        TestConstraintChecker();
        TestConstraintReporter();
        TestConstraintUtils();
        TestComplexScenarios();
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "\n所有测试完成!" << std::endl;
    std::cout << "\n约束系统实现了：" << std::endl;
    std::cout << "  - 全局样式块约束（只允许样式相关语法）" << std::endl;
    std::cout << "  - 局部样式块约束（与全局样式块相同）" << std::endl;
    std::cout << "  - 全局script块约束（只允许注释和原始嵌入）" << std::endl;
    std::cout << "  - 局部script块约束（允许变量相关语法）" << std::endl;
    std::cout << "  - 注释和原始嵌入的特殊处理（任何地方都允许）" << std::endl;
    std::cout << "  - 约束违反检测和报告" << std::endl;
    std::cout << "  - 上下文感知的语法建议" << std::endl;
    std::cout << "  - 详细的约束检查报告生成" << std::endl;
    
    return 0;
}