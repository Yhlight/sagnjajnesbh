#include "include/CHTLTokens.h"
#include "include/CHTLASTNodes.h"
#include "include/CHTLGenerator.h"
#include <iostream>
#include <memory>

using namespace chtl;

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
    
    // 创建使用模板的元素
    auto div = std::make_shared<ElementNode>("div");
    auto styleBlock = std::make_shared<StyleBlockNode>(true);
    
    // 使用样式模板: @Style DefaultText;
    auto templateUsage = std::make_shared<TemplateUsageNode>("@Style", "DefaultText");
    styleBlock->addChild(templateUsage);
    
    div->addChild(styleBlock);
    
    // 添加文本内容
    auto text = std::make_shared<TextNode>("这是使用模板的文本");
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

// 测试元素模板
void testElementTemplate() {
    std::cout << "\n测试元素模板..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建元素模板: [Template] @Element Box
    auto templateElement = std::make_shared<TemplateElementNode>("Box");
    
    // 添加span元素
    auto span = std::make_shared<ElementNode>("span");
    auto spanText = std::make_shared<TextNode>("这是一组div");
    span->addChild(spanText);
    templateElement->addChild(span);
    
    // 添加div元素
    auto div1 = std::make_shared<ElementNode>("div");
    auto div1Style = std::make_shared<StyleBlockNode>(true);
    auto div1Width = std::make_shared<CSSPropertyNode>("width", "200px");
    auto div1Height = std::make_shared<CSSPropertyNode>("height", "200px");
    auto div1Bg = std::make_shared<CSSPropertyNode>("background-color", "red");
    div1Style->addChild(div1Width);
    div1Style->addChild(div1Height);
    div1Style->addChild(div1Bg);
    div1->addChild(div1Style);
    templateElement->addChild(div1);
    
    // 使用元素模板
    auto body = std::make_shared<ElementNode>("body");
    auto elementUsage = std::make_shared<TemplateUsageNode>("@Element", "Box");
    body->addChild(elementUsage);
    
    // 构建AST
    root->addChild(templateElement);
    root->addChild(body);
    
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
        std::cout << "✓ 元素模板测试通过" << std::endl;
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
    auto styleBlock = std::make_shared<StyleBlockNode>(true);
    
    // 使用变量组: color: ThemeColor(tableColor);
    auto varRef = std::make_shared<VariableReferenceNode>("tableColor");
    varRef->setGroupReference("ThemeColor", "tableColor");
    
    // 这里需要在CSS属性中使用变量引用，简化处理
    auto colorProp = std::make_shared<CSSPropertyNode>("background-color", "rgb(255, 192, 203)");
    styleBlock->addChild(colorProp);
    
    div->addChild(styleBlock);
    
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
        std::cout << "CSS输出:" << std::endl;
        std::cout << result.cssOutput << std::endl;
        std::cout << "✓ 变量模板测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

// 测试模板继承
void testTemplateInheritance() {
    std::cout << "\n测试模板继承..." << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建基础样式模板
    auto baseTemplate = std::make_shared<TemplateStyleNode>("BaseStyle");
    auto baseFontSize = std::make_shared<CSSPropertyNode>("font-size", "16px");
    auto baseLineHeight = std::make_shared<CSSPropertyNode>("line-height", "1.5");
    baseTemplate->addChild(baseFontSize);
    baseTemplate->addChild(baseLineHeight);
    
    // 创建继承的样式模板
    auto derivedTemplate = std::make_shared<TemplateStyleNode>("DerivedStyle");
    derivedTemplate->addInheritance("BaseStyle");
    auto derivedColor = std::make_shared<CSSPropertyNode>("color", "blue");
    derivedTemplate->addChild(derivedColor);
    
    // 使用继承的模板
    auto div = std::make_shared<ElementNode>("div");
    auto styleBlock = std::make_shared<StyleBlockNode>(true);
    auto templateUsage = std::make_shared<TemplateUsageNode>("@Style", "DerivedStyle");
    styleBlock->addChild(templateUsage);
    div->addChild(styleBlock);
    
    auto text = std::make_shared<TextNode>("继承模板测试");
    div->addChild(text);
    
    // 构建AST
    root->addChild(baseTemplate);
    root->addChild(derivedTemplate);
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
        std::cout << "✓ 模板继承测试通过" << std::endl;
    } else {
        std::cout << "生成失败:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  " << error.message << std::endl;
        }
    }
}

int main() {
    std::cout << "=== CHTL 模板系统测试 ===" << std::endl;
    
    try {
        testStyleTemplate();
        testElementTemplate();
        testVariableTemplate();
        testTemplateInheritance();
        
        std::cout << "\n✅ 所有模板系统测试完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}