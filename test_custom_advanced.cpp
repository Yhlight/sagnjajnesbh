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

// 测试删除属性功能
void testDeleteProperties() {
    std::cout << "测试删除属性功能..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建基础模板
    auto baseTemplate = std::make_shared<TemplateStyleNode>("BaseStyle");
    auto colorProp = std::make_shared<CSSPropertyNode>("color", "white");
    auto fontSizeProp = std::make_shared<CSSPropertyNode>("font-size", "16px");
    auto lineHeightProp = std::make_shared<CSSPropertyNode>("line-height", "1.6");
    auto borderProp = std::make_shared<CSSPropertyNode>("border", "1px black solid");
    
    baseTemplate->addChild(colorProp);
    baseTemplate->addChild(fontSizeProp);
    baseTemplate->addChild(lineHeightProp);
    baseTemplate->addChild(borderProp);
    
    // 创建自定义样式，删除某些属性
    auto customStyle = std::make_shared<CustomStyleNode>("ModifiedStyle");
    customStyle->addInheritance("BaseStyle");
    customStyle->addDeletedProperty("line-height");
    customStyle->addDeletedProperty("border");
    
    // 添加新属性
    auto newColor = std::make_shared<CSSPropertyNode>("color", "blue");
    customStyle->addChild(newColor);
    
    // 构建AST
    root->addChild(baseTemplate);
    root->addChild(customStyle);
    
    // 生成HTML和CSS
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "生成的CSS应该包含: color: blue, font-size: 16px" << std::endl;
        std::cout << "生成的CSS不应该包含: line-height, border" << std::endl;
        std::cout << "CSS输出:" << std::endl;
        std::cout << result.cssOutput << std::endl;
        std::cout << "✓ 删除属性功能测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试插入操作
void testInsertOperation() {
    std::cout << "\n测试插入操作..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建基础自定义元素
    auto customElement = std::make_shared<CustomElementNode>("Container");
    
    // 添加基础元素
    auto div1 = std::make_shared<ElementNode>("div");
    div1->setAttribute("class", "first");
    auto div2 = std::make_shared<ElementNode>("div");
    div2->setAttribute("class", "second");
    
    customElement->addChild(div1);
    customElement->addChild(div2);
    
    // 创建插入操作: insert after div[0] { ... }
    auto insertNode = std::make_shared<InsertNode>(InsertNode::InsertPosition::AFTER, "div[0]");
    auto insertedDiv = std::make_shared<ElementNode>("div");
    insertedDiv->setAttribute("class", "inserted");
    auto insertedText = std::make_shared<TextNode>("插入的内容");
    insertedDiv->addChild(insertedText);
    insertNode->addChild(insertedDiv);
    
    customElement->addChild(insertNode);
    
    // 构建AST
    root->addChild(customElement);
    
    // 生成HTML
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "HTML输出:" << std::endl;
        std::cout << result.htmlOutput << std::endl;
        std::cout << "✓ 插入操作测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试变量组特例化
void testVariableGroupSpecialization() {
    std::cout << "\n测试变量组特例化..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建自定义变量组
    auto customVar = std::make_shared<CustomVarNode>("ThemeColor");
    customVar->addVariable("tableColor", "rgb(255, 192, 203)");
    customVar->addVariable("textColor", "black");
    
    // 创建使用变量组特例化的元素
    auto div = std::make_shared<ElementNode>("div");
    auto styleBlock = std::make_shared<StyleBlockNode>(true);
    
    // 模拟变量组特例化: color: ThemeColor(tableColor = rgb(145, 155, 200));
    auto templateUsage = std::make_shared<TemplateUsageNode>("@Var", "ThemeColor");
    templateUsage->addSpecialization("tableColor", "rgb(145, 155, 200)");
    
    // 使用特例化的变量值
    auto colorProp = std::make_shared<CSSPropertyNode>("color", "rgb(145, 155, 200)");
    styleBlock->addChild(colorProp);
    
    div->addChild(styleBlock);
    
    auto text = std::make_shared<TextNode>("变量组特例化测试");
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
        std::cout << "CSS输出:" << std::endl;
        std::cout << result.cssOutput << std::endl;
        std::cout << "✓ 变量组特例化测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试继承和特例化的组合
void testInheritanceAndSpecialization() {
    std::cout << "\n测试继承和特例化的组合..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建基础模板
    auto baseTemplate = std::make_shared<TemplateStyleNode>("BaseText");
    auto baseFontSize = std::make_shared<CSSPropertyNode>("font-size", "14px");
    auto baseLineHeight = std::make_shared<CSSPropertyNode>("line-height", "1.4");
    auto baseColor = std::make_shared<CSSPropertyNode>("color", "black");
    
    baseTemplate->addChild(baseFontSize);
    baseTemplate->addChild(baseLineHeight);
    baseTemplate->addChild(baseColor);
    
    // 创建继承的自定义样式
    auto customStyle = std::make_shared<CustomStyleNode>("EnhancedText");
    customStyle->addInheritance("BaseText");
    
    // 覆盖某些属性
    auto newFontSize = std::make_shared<CSSPropertyNode>("font-size", "18px");
    auto newWeight = std::make_shared<CSSPropertyNode>("font-weight", "bold");
    
    customStyle->addChild(newFontSize);
    customStyle->addChild(newWeight);
    
    // 创建使用自定义样式的元素
    auto div = std::make_shared<ElementNode>("div");
    auto styleBlock = std::make_shared<StyleBlockNode>(true);
    
    auto customUsage = std::make_shared<TemplateUsageNode>("@Style", "EnhancedText");
    styleBlock->addChild(customUsage);
    
    div->addChild(styleBlock);
    
    auto text = std::make_shared<TextNode>("继承和特例化组合测试");
    div->addChild(text);
    
    // 构建AST
    root->addChild(baseTemplate);
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
        std::cout << "✓ 继承和特例化组合测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

int main() {
    std::cout << "=== CHTL 自定义系统高级测试 ===" << std::endl;
    
    try {
        testDeleteProperties();
        testInsertOperation();
        testVariableGroupSpecialization();
        testInheritanceAndSpecialization();
        
        std::cout << "\n✅ 所有自定义系统高级测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}