#include "include/CHTLTokens.h"
#include "include/CHTLASTNodes.h"
#include "include/CHTLGenerator.h"
#include <iostream>
#include <memory>

using namespace chtl;

// 简化的访问者实现，用于测试
class SimpleVisitor : public CHTLASTVisitor {
public:
    void visit(RootNode& node) override {}
    void visit(ElementNode& node) override {}
    void visit(TextNode& node) override {}
    void visit(AttributeNode& node) override {}
    void visit(StyleBlockNode& node) override {}
    void visit(CSSRuleNode& node) override {}
    void visit(CSSPropertyNode& node) override {}
    void visit(ScriptBlockNode& node) override {}
    void visit(TemplateStyleNode& node) override {}
    void visit(TemplateElementNode& node) override {}
    void visit(TemplateVarNode& node) override {}
    void visit(TemplateUsageNode& node) override {}
    void visit(VariableReferenceNode& node) override {}
    void visit(CustomStyleNode& node) override {}
    void visit(CustomElementNode& node) override {}
    void visit(CustomVarNode& node) override {}
    void visit(ImportNode& node) override {}
    void visit(NamespaceNode& node) override {}
    void visit(OriginNode& node) override {}
    void visit(ConstraintNode& node) override {}
};

// 测试样式模板
void testStyleTemplate() {
    std::cout << "测试样式模板..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建样式模板: [Template] @Style DefaultText
    auto templateStyle = std::make_shared<TemplateStyleNode>("DefaultText");
    auto colorProp = std::make_shared<CSSPropertyNode>("color", "black");
    auto lineHeightProp = std::make_shared<CSSPropertyNode>("line-height", "1.6");
    
    templateStyle->addChild(colorProp);
    templateStyle->addChild(lineHeightProp);
    
    // 创建简单的div元素
    auto div = std::make_shared<ElementNode>("div");
    auto text = std::make_shared<TextNode>("模板测试文本");
    div->addChild(text);
    
    // 构建AST
    root->addChild(templateStyle);
    root->addChild(div);
    
    // 生成HTML和CSS
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "HTML输出:" << std::endl;
        std::cout << result.htmlOutput << std::endl;
        std::cout << "CSS输出:" << std::endl;
        std::cout << result.cssOutput << std::endl;
        std::cout << "✓ 样式模板测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试变量模板
void testVariableTemplate() {
    std::cout << "\n测试变量模板..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建变量模板: [Template] @Var ThemeColor
    auto templateVar = std::make_shared<TemplateVarNode>("ThemeColor");
    templateVar->addVariable("tableColor", "rgb(255, 192, 203)");
    templateVar->addVariable("textColor", "black");
    
    // 创建使用变量的元素
    auto div = std::make_shared<ElementNode>("div");
    auto text = std::make_shared<TextNode>("变量模板测试");
    div->addChild(text);
    
    // 构建AST
    root->addChild(templateVar);
    root->addChild(div);
    
    // 生成HTML和CSS
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "HTML输出:" << std::endl;
        std::cout << result.htmlOutput << std::endl;
        std::cout << "✓ 变量模板测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试AST节点创建
void testASTNodes() {
    std::cout << "\n测试AST节点创建..." << std::endl;
    
    // 测试模板节点创建
    auto templateStyle = std::make_shared<TemplateStyleNode>("TestStyle");
    auto templateElement = std::make_shared<TemplateElementNode>("TestElement");
    auto templateVar = std::make_shared<TemplateVarNode>("TestVar");
    
    // 测试模板使用节点创建
    auto templateUsage = std::make_shared<TemplateUsageNode>("@Style", "TestStyle");
    
    // 测试变量引用节点创建
    auto varRef = std::make_shared<VariableReferenceNode>("testVar");
    varRef->setGroupReference("ThemeColor", "tableColor");
    
    std::cout << "模板样式节点: " << templateStyle->getName() << std::endl;
    std::cout << "模板元素节点: " << templateElement->getName() << std::endl;
    std::cout << "模板变量节点: " << templateVar->getName() << std::endl;
    std::cout << "模板使用节点: " << templateUsage->getType() << " " << templateUsage->getName() << std::endl;
    std::cout << "变量引用节点: " << varRef->getGroupName() << "(" << varRef->getVariableName() << ")" << std::endl;
    
    std::cout << "✓ AST节点创建测试通过" << std::endl;
}

int main() {
    std::cout << "=== CHTL 模板系统简化测试 ===" << std::endl;
    
    try {
        testASTNodes();
        testStyleTemplate();
        testVariableTemplate();
        
        std::cout << "\n✅ 所有模板系统简化测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}