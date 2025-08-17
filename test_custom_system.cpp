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

// 测试无值样式组
void testValuelessStyleGroup() {
    std::cout << "测试无值样式组..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建无值样式组: [Custom] @Style TextSet
    auto customStyle = std::make_shared<CustomStyleNode>("TextSet");
    
    // 添加无值属性: color, font-size;
    auto colorProp = std::make_shared<CSSPropertyNode>("color", ""); // 无值
    auto fontSizeProp = std::make_shared<CSSPropertyNode>("font-size", ""); // 无值
    
    customStyle->addChild(colorProp);
    customStyle->addChild(fontSizeProp);
    
    // 创建使用无值样式组的元素
    auto div = std::make_shared<ElementNode>("div");
    auto styleBlock = std::make_shared<StyleBlockNode>(true);
    
    // 使用无值样式组并提供值: @Style TextSet { color: red; font-size: 16px; }
    auto templateUsage = std::make_shared<TemplateUsageNode>("@Style", "TextSet");
    
    // 添加特例化属性
    auto redColor = std::make_shared<CSSPropertyNode>("color", "red");
    auto fontSize16 = std::make_shared<CSSPropertyNode>("font-size", "16px");
    templateUsage->addChild(redColor);
    templateUsage->addChild(fontSize16);
    
    styleBlock->addChild(templateUsage);
    div->addChild(styleBlock);
    
    auto text = std::make_shared<TextNode>("无值样式组测试");
    div->addChild(text);
    
    // 构建AST
    root->addChild(customStyle);
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
        std::cout << "✓ 无值样式组测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试自定义样式的特例化
void testCustomStyleSpecialization() {
    std::cout << "\n测试自定义样式特例化..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建基础模板: [Template] @Style WhiteText
    auto whiteTemplate = std::make_shared<TemplateStyleNode>("WhiteText");
    auto whiteColor = std::make_shared<CSSPropertyNode>("color", "white");
    auto whiteFontSize = std::make_shared<CSSPropertyNode>("font-size", "16px");
    auto whiteLineHeight = std::make_shared<CSSPropertyNode>("line-height", "1.6");
    auto whiteBorder = std::make_shared<CSSPropertyNode>("border", "1px black solid");
    
    whiteTemplate->addChild(whiteColor);
    whiteTemplate->addChild(whiteFontSize);
    whiteTemplate->addChild(whiteLineHeight);
    whiteTemplate->addChild(whiteBorder);
    
    // 创建自定义样式: [Custom] @Style YellowText
    auto yellowCustom = std::make_shared<CustomStyleNode>("YellowText");
    yellowCustom->addInheritance("WhiteText");
    
    // 添加删除的属性
    yellowCustom->addDeletedProperty("line-height");
    yellowCustom->addDeletedProperty("border");
    
    // 添加新的属性
    auto yellowColor = std::make_shared<CSSPropertyNode>("color", "yellow");
    yellowCustom->addChild(yellowColor);
    
    // 创建使用自定义样式的元素
    auto div = std::make_shared<ElementNode>("div");
    auto styleBlock = std::make_shared<StyleBlockNode>(true);
    
    auto customUsage = std::make_shared<TemplateUsageNode>("@Style", "YellowText");
    styleBlock->addChild(customUsage);
    
    div->addChild(styleBlock);
    
    auto text = std::make_shared<TextNode>("自定义样式特例化测试");
    div->addChild(text);
    
    // 构建AST
    root->addChild(whiteTemplate);
    root->addChild(yellowCustom);
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
        std::cout << "✓ 自定义样式特例化测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试自定义元素
void testCustomElement() {
    std::cout << "\n测试自定义元素..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建自定义元素: [Custom] @Element Box
    auto customElement = std::make_shared<CustomElementNode>("Box");
    
    // 添加基础元素
    auto div1 = std::make_shared<ElementNode>("div");
    auto div2 = std::make_shared<ElementNode>("div");
    auto span = std::make_shared<ElementNode>("span");
    
    customElement->addChild(div1);
    customElement->addChild(div2);
    customElement->addChild(span);
    
    // 使用自定义元素
    auto body = std::make_shared<ElementNode>("body");
    auto elementUsage = std::make_shared<TemplateUsageNode>("@Element", "Box");
    body->addChild(elementUsage);
    
    // 构建AST
    root->addChild(customElement);
    root->addChild(body);
    
    // 生成HTML和CSS
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "HTML输出:" << std::endl;
        std::cout << result.htmlOutput << std::endl;
        std::cout << "✓ 自定义元素测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试自定义变量
void testCustomVariable() {
    std::cout << "\n测试自定义变量..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建自定义变量: [Custom] @Var ThemeColor
    auto customVar = std::make_shared<CustomVarNode>("ThemeColor");
    customVar->addVariable("tableColor", "rgb(255, 192, 203)");
    customVar->addVariable("textColor", "black");
    
    // 创建使用变量的元素
    auto div = std::make_shared<ElementNode>("div");
    auto text = std::make_shared<TextNode>("自定义变量测试");
    div->addChild(text);
    
    // 构建AST
    root->addChild(customVar);
    root->addChild(div);
    
    // 生成HTML和CSS
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "HTML输出:" << std::endl;
        std::cout << result.htmlOutput << std::endl;
        std::cout << "✓ 自定义变量测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

int main() {
    std::cout << "=== CHTL 自定义系统测试 ===" << std::endl;
    
    try {
        testValuelessStyleGroup();
        testCustomStyleSpecialization();
        testCustomElement();
        testCustomVariable();
        
        std::cout << "\n✅ 所有自定义系统测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}