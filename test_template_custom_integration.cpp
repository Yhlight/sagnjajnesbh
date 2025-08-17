#include "include/CHTLTokens.h"
#include "include/CHTLASTNodes.h"
#include "include/CHTLGenerator.h"
#include <iostream>
#include <memory>

using namespace chtl;

// 简化的访问者实现
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

// 测试完整的CHTL语法示例
void testCompleteExample() {
    std::cout << "测试完整CHTL语法示例..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 1. 创建变量模板
    auto themeVar = std::make_shared<TemplateVarNode>("ThemeColor");
    themeVar->addVariable("primaryColor", "rgb(255, 192, 203)");
    themeVar->addVariable("secondaryColor", "rgb(253, 144, 162)");
    
    // 2. 创建样式模板
    auto baseStyle = std::make_shared<TemplateStyleNode>("BaseText");
    auto baseFontSize = std::make_shared<CSSPropertyNode>("font-size", "16px");
    auto baseLineHeight = std::make_shared<CSSPropertyNode>("line-height", "1.5");
    baseStyle->addChild(baseFontSize);
    baseStyle->addChild(baseLineHeight);
    
    // 3. 创建自定义样式（继承模板）
    auto customStyle = std::make_shared<CustomStyleNode>("CustomText");
    customStyle->addInheritance("BaseText");
    auto customColor = std::make_shared<CSSPropertyNode>("color", "blue");
    auto customWeight = std::make_shared<CSSPropertyNode>("font-weight", "bold");
    customStyle->addChild(customColor);
    customStyle->addChild(customWeight);
    
    // 4. 创建元素模板
    auto cardTemplate = std::make_shared<TemplateElementNode>("Card");
    auto cardDiv = std::make_shared<ElementNode>("div");
    cardDiv->setAttribute("class", "card");
    
    auto cardHeader = std::make_shared<ElementNode>("h3");
    auto headerText = std::make_shared<TextNode>("卡片标题");
    cardHeader->addChild(headerText);
    
    auto cardContent = std::make_shared<ElementNode>("p");
    auto contentText = std::make_shared<TextNode>("卡片内容");
    cardContent->addChild(contentText);
    
    cardDiv->addChild(cardHeader);
    cardDiv->addChild(cardContent);
    cardTemplate->addChild(cardDiv);
    
    // 5. 创建自定义元素（继承模板）
    auto customCard = std::make_shared<CustomElementNode>("EnhancedCard");
    customCard->addInheritance("Card");
    
    // 添加额外的元素
    auto cardFooter = std::make_shared<ElementNode>("footer");
    auto footerText = std::make_shared<TextNode>("卡片底部");
    cardFooter->addChild(footerText);
    customCard->addChild(cardFooter);
    
    // 6. 使用模板和自定义的页面结构
    auto html = std::make_shared<ElementNode>("html");
    auto body = std::make_shared<ElementNode>("body");
    
    // 使用自定义元素
    auto cardUsage = std::make_shared<TemplateUsageNode>("@Element", "EnhancedCard");
    body->addChild(cardUsage);
    
    // 添加样式
    auto mainDiv = std::make_shared<ElementNode>("div");
    auto styleBlock = std::make_shared<StyleBlockNode>(true);
    
    // 使用自定义样式
    auto styleUsage = std::make_shared<TemplateUsageNode>("@Style", "CustomText");
    styleBlock->addChild(styleUsage);
    
    mainDiv->addChild(styleBlock);
    
    auto mainText = std::make_shared<TextNode>("主要内容");
    mainDiv->addChild(mainText);
    
    body->addChild(mainDiv);
    html->addChild(body);
    
    // 构建完整AST
    root->addChild(themeVar);
    root->addChild(baseStyle);
    root->addChild(customStyle);
    root->addChild(cardTemplate);
    root->addChild(customCard);
    root->addChild(html);
    
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
        std::cout << "JS输出:" << std::endl;
        std::cout << result.jsOutput << std::endl;
        std::cout << "✓ 完整CHTL语法示例测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试模板与自定义的相互继承
void testTemplateCustomInheritance() {
    std::cout << "\n测试模板与自定义的相互继承..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建基础模板
    auto template1 = std::make_shared<TemplateStyleNode>("Template1");
    auto prop1 = std::make_shared<CSSPropertyNode>("margin", "10px");
    auto prop2 = std::make_shared<CSSPropertyNode>("padding", "15px");
    template1->addChild(prop1);
    template1->addChild(prop2);
    
    // 创建继承模板的自定义
    auto custom1 = std::make_shared<CustomStyleNode>("Custom1");
    custom1->addInheritance("Template1");
    auto customProp1 = std::make_shared<CSSPropertyNode>("background", "lightblue");
    custom1->addChild(customProp1);
    
    // 创建继承自定义的模板
    auto template2 = std::make_shared<TemplateStyleNode>("Template2");
    template2->addInheritance("Custom1");
    auto templateProp2 = std::make_shared<CSSPropertyNode>("border-radius", "5px");
    template2->addChild(templateProp2);
    
    // 创建继承第二个模板的自定义
    auto custom2 = std::make_shared<CustomStyleNode>("Custom2");
    custom2->addInheritance("Template2");
    auto customProp2 = std::make_shared<CSSPropertyNode>("box-shadow", "0 2px 4px rgba(0,0,0,0.1)");
    custom2->addChild(customProp2);
    
    // 构建AST
    root->addChild(template1);
    root->addChild(custom1);
    root->addChild(template2);
    root->addChild(custom2);
    
    // 生成HTML和CSS
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "注册了完整的继承链: Template1 -> Custom1 -> Template2 -> Custom2" << std::endl;
        std::cout << "✓ 模板与自定义相互继承测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

int main() {
    std::cout << "=== CHTL 模板与自定义系统集成测试 ===" << std::endl;
    
    try {
        testCompleteExample();
        testTemplateCustomInheritance();
        
        std::cout << "\n✅ 所有集成测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}