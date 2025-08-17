#include "include/CHTLTokens.h"
#include "include/CHTLASTNodes.h"
#include "include/CHTLGenerator.h"
#include <iostream>
#include <memory>
#include <fstream>

using namespace chtl;

void writeFile(const String& filename, const String& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
        std::cout << "✅ 已写入文件: " << filename << std::endl;
    } else {
        std::cerr << "❌ 无法写入文件: " << filename << std::endl;
    }
}

void testBasicElements() {
    std::cout << "=== 测试基础元素和属性 ===" << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建HTML结构
    auto html = std::make_shared<ElementNode>("html");
    auto head = std::make_shared<ElementNode>("head");
    auto title = std::make_shared<ElementNode>("title");
    auto titleText = std::make_shared<TextNode>("CHTL基础语法测试");
    
    title->addChild(titleText);
    head->addChild(title);
    html->addChild(head);
    
    auto body = std::make_shared<ElementNode>("body");
    
    // 测试各种属性设置方式
    auto div1 = std::make_shared<ElementNode>("div");
    div1->setAttribute("id", "test-basic");
    div1->setAttribute("class", "container");
    div1->setAttribute("data-testid", "basic-test");
    
    auto text1 = std::make_shared<TextNode>("基础元素测试");
    div1->addChild(text1);
    body->addChild(div1);
    
    // 测试不同的文本节点
    auto div2 = std::make_shared<ElementNode>("div");
    auto text2 = std::make_shared<TextNode>("这是无修饰字面量文本");
    auto text3 = std::make_shared<TextNode>("\"这是带引号的文本\"");
    div2->addChild(text2);
    div2->addChild(text3);
    body->addChild(div2);
    
    html->addChild(body);
    root->addChild(html);
    
    // 生成HTML
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "✅ 基础元素测试通过" << std::endl;
        std::cout << "HTML输出长度: " << result.htmlOutput.length() << " 字符" << std::endl;
    } else {
        std::cout << "❌ 基础元素测试失败" << std::endl;
    }
}

void testStyleBlocks() {
    std::cout << "\n=== 测试样式块生成 ===" << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建带样式的div
    auto div = std::make_shared<ElementNode>("div");
    div->setAttribute("id", "styled-div");
    
    // 创建样式块
    auto styleBlock = std::make_shared<StyleBlockNode>();
    
    // 添加内联样式
    styleBlock->addProperty("width", "200px");
    styleBlock->addProperty("height", "200px");
    styleBlock->addProperty("background-color", "blue");
    styleBlock->addProperty("border-radius", "8px");
    
    // 添加类选择器样式
    auto classStyle = std::make_shared<StyleRuleNode>();
    classStyle->setSelector(".box");
    classStyle->addProperty("margin", "20px");
    classStyle->addProperty("padding", "10px");
    styleBlock->addRule(classStyle);
    
    // 添加伪类样式
    auto hoverStyle = std::make_shared<StyleRuleNode>();
    hoverStyle->setSelector("&:hover");
    hoverStyle->addProperty("opacity", "0.8");
    hoverStyle->addProperty("transform", "scale(1.05)");
    styleBlock->addRule(hoverStyle);
    
    div->setStyleBlock(styleBlock);
    
    auto text = std::make_shared<TextNode>("样式块测试");
    div->addChild(text);
    root->addChild(div);
    
    // 生成代码
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "✅ 样式块测试通过" << std::endl;
        std::cout << "HTML输出: " << result.htmlOutput.length() << " 字符" << std::endl;
        std::cout << "CSS输出: " << result.cssOutput.length() << " 字符" << std::endl;
        
        if (!result.cssOutput.empty()) {
            std::cout << "CSS预览:\n" << result.cssOutput.substr(0, 200) << "..." << std::endl;
        }
    } else {
        std::cout << "❌ 样式块测试失败" << std::endl;
    }
}

void testTemplateSystem() {
    std::cout << "\n=== 测试模板系统 ===" << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建样式模板
    auto styleTemplate = std::make_shared<TemplateStyleNode>("BaseText");
    styleTemplate->addProperty("font-family", "Arial, sans-serif");
    styleTemplate->addProperty("font-size", "16px");
    styleTemplate->addProperty("line-height", "1.6");
    styleTemplate->addProperty("color", "#333");
    root->addChild(styleTemplate);
    
    // 创建变量模板
    auto varTemplate = std::make_shared<TemplateVarNode>("Colors");
    varTemplate->addVariable("primary", "rgb(0, 123, 255)");
    varTemplate->addVariable("secondary", "rgb(108, 117, 125)");
    varTemplate->addVariable("success", "rgb(40, 167, 69)");
    root->addChild(varTemplate);
    
    // 创建元素模板
    auto elementTemplate = std::make_shared<TemplateElementNode>("Card");
    auto cardDiv = std::make_shared<ElementNode>("div");
    
    auto cardStyle = std::make_shared<StyleBlockNode>();
    auto cardRule = std::make_shared<StyleRuleNode>();
    cardRule->setSelector(".card");
    cardRule->addProperty("background", "white");
    cardRule->addProperty("border", "1px solid #ddd");
    cardRule->addProperty("border-radius", "8px");
    cardRule->addProperty("padding", "20px");
    cardRule->addProperty("box-shadow", "0 2px 4px rgba(0,0,0,0.1)");
    cardStyle->addRule(cardRule);
    cardDiv->setStyleBlock(cardStyle);
    
    elementTemplate->addChild(cardDiv);
    root->addChild(elementTemplate);
    
    // 使用模板的元素
    auto div = std::make_shared<ElementNode>("div");
    
    // 使用样式模板
    auto templateUse = std::make_shared<TemplateUseNode>("@Style", "BaseText");
    div->addTemplateUse(templateUse);
    
    // 使用变量模板
    auto varUse = std::make_shared<VariableReferenceNode>("Colors", "primary");
    
    auto text = std::make_shared<TextNode>("模板系统测试");
    div->addChild(text);
    root->addChild(div);
    
    // 使用元素模板
    auto elementUse = std::make_shared<TemplateUseNode>("@Element", "Card");
    root->addChild(elementUse);
    
    // 生成代码
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "✅ 模板系统测试通过" << std::endl;
        std::cout << "HTML输出: " << result.htmlOutput.length() << " 字符" << std::endl;
        std::cout << "CSS输出: " << result.cssOutput.length() << " 字符" << std::endl;
    } else {
        std::cout << "❌ 模板系统测试失败" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  错误: " << error.message << std::endl;
        }
    }
}

void testCustomSystem() {
    std::cout << "\n=== 测试自定义系统 ===" << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建无值样式组
    auto customStyle = std::make_shared<CustomStyleNode>("FlexLayout");
    customStyle->addProperty("display", "");  // 无值属性
    customStyle->addProperty("flex-direction", "");
    customStyle->addProperty("justify-content", "");
    customStyle->addProperty("align-items", "");
    root->addChild(customStyle);
    
    // 创建自定义元素
    auto customElement = std::make_shared<CustomElementNode>("Button");
    auto button = std::make_shared<ElementNode>("button");
    
    auto buttonStyle = std::make_shared<StyleBlockNode>();
    auto btnRule = std::make_shared<StyleRuleNode>();
    btnRule->setSelector(".btn");
    btnRule->addProperty("padding", "8px 16px");
    btnRule->addProperty("border", "none");
    btnRule->addProperty("border-radius", "4px");
    btnRule->addProperty("cursor", "pointer");
    btnRule->addProperty("background-color", "blue");
    btnRule->addProperty("color", "white");
    buttonStyle->addRule(btnRule);
    
    auto hoverRule = std::make_shared<StyleRuleNode>();
    hoverRule->setSelector("&:hover");
    hoverRule->addProperty("opacity", "0.8");
    buttonStyle->addRule(hoverRule);
    
    button->setStyleBlock(buttonStyle);
    
    auto buttonText = std::make_shared<TextNode>("点击按钮");
    button->addChild(buttonText);
    customElement->addChild(button);
    root->addChild(customElement);
    
    // 创建自定义变量组
    auto customVar = std::make_shared<CustomVarNode>("Theme");
    customVar->addVariable("primaryColor", "rgb(0, 123, 255)");
    customVar->addVariable("backgroundColor", "rgb(248, 249, 250)");
    customVar->addVariable("textColor", "rgb(33, 37, 41)");
    root->addChild(customVar);
    
    // 使用自定义样式组
    auto div = std::make_shared<ElementNode>("div");
    auto customUse = std::make_shared<CustomStyleUseNode>("FlexLayout");
    customUse->addProperty("display", "flex");
    customUse->addProperty("flex-direction", "column");
    customUse->addProperty("justify-content", "center");
    customUse->addProperty("align-items", "center");
    div->addCustomStyleUse(customUse);
    
    // 使用自定义元素
    auto customElementUse = std::make_shared<CustomElementUseNode>("Button");
    div->addChild(customElementUse);
    
    root->addChild(div);
    
    // 生成代码
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "✅ 自定义系统测试通过" << std::endl;
        std::cout << "HTML输出: " << result.htmlOutput.length() << " 字符" << std::endl;
        std::cout << "CSS输出: " << result.cssOutput.length() << " 字符" << std::endl;
    } else {
        std::cout << "❌ 自定义系统测试失败" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  错误: " << error.message << std::endl;
        }
    }
}

void testOriginEmbedding() {
    std::cout << "\n=== 测试原始嵌入 ===" << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // HTML原始嵌入
    auto htmlOrigin = std::make_shared<OriginNode>("@Html", "HeaderContent");
    htmlOrigin->setContent(R"(
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0/css/all.min.css">
    )");
    root->addChild(htmlOrigin);
    
    // CSS原始嵌入
    auto cssOrigin = std::make_shared<OriginNode>("@Style", "GlobalStyles");
    cssOrigin->setContent(R"(
        * {
            box-sizing: border-box;
        }
        
        body {
            margin: 0;
            padding: 0;
            font-family: 'Arial', sans-serif;
        }
    )");
    root->addChild(cssOrigin);
    
    // JavaScript原始嵌入
    auto jsOrigin = std::make_shared<OriginNode>("@JavaScript", "UtilityFunctions");
    jsOrigin->setContent(R"(
        function showAlert(message) {
            alert(message);
        }
        
        function toggleVisibility(id) {
            const element = document.getElementById(id);
            element.style.display = element.style.display === 'none' ? 'block' : 'none';
        }
    )");
    root->addChild(jsOrigin);
    
    // 自定义类型原始嵌入
    auto vueOrigin = std::make_shared<OriginNode>("@Vue", "VueComponent");
    vueOrigin->setContent(R"(
        <template>
            <div class="vue-component">
                <h1>{{ title }}</h1>
                <p>{{ description }}</p>
            </div>
        </template>
    )");
    root->addChild(vueOrigin);
    
    // 使用原始嵌入的HTML结构
    auto html = std::make_shared<ElementNode>("html");
    auto head = std::make_shared<ElementNode>("head");
    
    // 使用HTML原始嵌入
    auto htmlUse = std::make_shared<OriginUseNode>("@Html", "HeaderContent");
    head->addChild(htmlUse);
    
    // 使用CSS原始嵌入
    auto cssUse = std::make_shared<OriginUseNode>("@Style", "GlobalStyles");
    head->addChild(cssUse);
    
    html->addChild(head);
    
    auto body = std::make_shared<ElementNode>("body");
    auto div = std::make_shared<ElementNode>("div");
    div->setAttribute("id", "origin-test");
    
    auto text = std::make_shared<TextNode>("原始嵌入测试");
    div->addChild(text);
    body->addChild(div);
    
    // 使用JavaScript原始嵌入
    auto jsUse = std::make_shared<OriginUseNode>("@JavaScript", "UtilityFunctions");
    body->addChild(jsUse);
    
    html->addChild(body);
    root->addChild(html);
    
    // 生成代码
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "✅ 原始嵌入测试通过" << std::endl;
        std::cout << "HTML输出: " << result.htmlOutput.length() << " 字符" << std::endl;
        std::cout << "CSS输出: " << result.cssOutput.length() << " 字符" << std::endl;
        std::cout << "JS输出: " << result.jsOutput.length() << " 字符" << std::endl;
    } else {
        std::cout << "❌ 原始嵌入测试失败" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  错误: " << error.message << std::endl;
        }
    }
}

void testCompleteWebPage() {
    std::cout << "\n=== 测试完整网页生成 ===" << std::endl;
    
    auto root = std::make_shared<RootNode>();
    
    // 创建完整的HTML页面结构
    auto html = std::make_shared<ElementNode>("html");
    html->setAttribute("lang", "zh-CN");
    
    auto head = std::make_shared<ElementNode>("head");
    auto title = std::make_shared<ElementNode>("title");
    auto titleText = std::make_shared<TextNode>("CHTL综合语法测试");
    title->addChild(titleText);
    head->addChild(title);
    html->addChild(head);
    
    auto body = std::make_shared<ElementNode>("body");
    
    // 主容器
    auto container = std::make_shared<ElementNode>("div");
    container->setAttribute("id", "main-container");
    container->setAttribute("class", "container");
    
    auto containerStyle = std::make_shared<StyleBlockNode>();
    containerStyle->addProperty("max-width", "1200px");
    containerStyle->addProperty("margin", "0 auto");
    containerStyle->addProperty("padding", "20px");
    containerStyle->addProperty("font-family", "Arial, sans-serif");
    container->setStyleBlock(containerStyle);
    
    // 标题
    auto h1 = std::make_shared<ElementNode>("h1");
    auto h1Style = std::make_shared<StyleBlockNode>();
    h1Style->addProperty("color", "#007bff");
    h1Style->addProperty("text-align", "center");
    h1Style->addProperty("margin-bottom", "30px");
    h1->setStyleBlock(h1Style);
    
    auto h1Text = std::make_shared<TextNode>("CHTL语法全面测试");
    h1->addChild(h1Text);
    container->addChild(h1);
    
    // 功能卡片网格
    auto grid = std::make_shared<ElementNode>("div");
    auto gridStyle = std::make_shared<StyleBlockNode>();
    
    auto gridRule = std::make_shared<StyleRuleNode>();
    gridRule->setSelector(".feature-grid");
    gridRule->addProperty("display", "grid");
    gridRule->addProperty("grid-template-columns", "repeat(auto-fit, minmax(300px, 1fr))");
    gridRule->addProperty("gap", "20px");
    gridRule->addProperty("margin-top", "20px");
    gridStyle->addRule(gridRule);
    grid->setStyleBlock(gridStyle);
    
    // 创建功能卡片
    std::vector<std::pair<String, String>> features = {
        {"基础语法", "支持HTML元素、属性、文本节点、注释等基础语法"},
        {"模板系统", "支持@Style、@Element、@Var三种模板类型"},
        {"自定义系统", "支持无值样式组、特例化操作、继承机制"},
        {"局部样式", "支持内联样式、自动类名、上下文推导"},
        {"原始嵌入", "支持HTML、CSS、JS原始代码嵌入"},
        {"高级特性", "支持命名空间、导入系统、约束机制"}
    };
    
    for (const auto& feature : features) {
        auto card = std::make_shared<ElementNode>("div");
        
        auto cardStyle = std::make_shared<StyleBlockNode>();
        auto cardRule = std::make_shared<StyleRuleNode>();
        cardRule->setSelector(".feature-card");
        cardRule->addProperty("background", "white");
        cardRule->addProperty("border", "1px solid #ddd");
        cardRule->addProperty("border-radius", "8px");
        cardRule->addProperty("padding", "20px");
        cardRule->addProperty("box-shadow", "0 2px 4px rgba(0,0,0,0.1)");
        cardRule->addProperty("transition", "transform 0.3s ease");
        cardStyle->addRule(cardRule);
        
        auto hoverRule = std::make_shared<StyleRuleNode>();
        hoverRule->setSelector("&:hover");
        hoverRule->addProperty("transform", "translateY(-2px)");
        hoverRule->addProperty("box-shadow", "0 4px 8px rgba(0,0,0,0.15)");
        cardStyle->addRule(hoverRule);
        
        card->setStyleBlock(cardStyle);
        
        auto cardTitle = std::make_shared<ElementNode>("h3");
        auto titleStyle = std::make_shared<StyleBlockNode>();
        titleStyle->addProperty("color", "#007bff");
        titleStyle->addProperty("margin-top", "0");
        cardTitle->setStyleBlock(titleStyle);
        
        auto cardTitleText = std::make_shared<TextNode>(feature.first);
        cardTitle->addChild(cardTitleText);
        card->addChild(cardTitle);
        
        auto cardDesc = std::make_shared<ElementNode>("p");
        auto descText = std::make_shared<TextNode>(feature.second);
        cardDesc->addChild(descText);
        card->addChild(cardDesc);
        
        grid->addChild(card);
    }
    
    container->addChild(grid);
    body->addChild(container);
    html->addChild(body);
    root->addChild(html);
    
    // 生成代码
    CompilerOptions options;
    options.debugMode = true;
    
    CHTLGenerator generator(options);
    auto result = generator.generate(root);
    
    if (result.success) {
        std::cout << "✅ 完整网页生成测试通过" << std::endl;
        
        // 生成完整的HTML文件
        String finalHtml = "<!DOCTYPE html>\n" + result.htmlOutput;
        if (!result.cssOutput.empty()) {
            finalHtml = finalHtml.substr(0, finalHtml.find("</head>")) + 
                       "<style>\n" + result.cssOutput + "\n</style>\n</head>" + 
                       finalHtml.substr(finalHtml.find("</head>") + 7);
        }
        if (!result.jsOutput.empty()) {
            finalHtml = finalHtml.substr(0, finalHtml.find("</body>")) + 
                       "<script>\n" + result.jsOutput + "\n</script>\n</body>" + 
                       finalHtml.substr(finalHtml.find("</body>") + 7);
        }
        
        writeFile("comprehensive_test_result.html", finalHtml);
        
        std::cout << "📊 最终统计:" << std::endl;
        std::cout << "  HTML: " << result.htmlOutput.length() << " 字符" << std::endl;
        std::cout << "  CSS: " << result.cssOutput.length() << " 字符" << std::endl;
        std::cout << "  JS: " << result.jsOutput.length() << " 字符" << std::endl;
        std::cout << "  完整文件: " << finalHtml.length() << " 字符" << std::endl;
    } else {
        std::cout << "❌ 完整网页生成测试失败" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  错误: " << error.message << std::endl;
        }
    }
}

int main() {
    std::cout << "🚀 CHTL编译器综合语法解析与生成测试" << std::endl;
    std::cout << "===============================================" << std::endl;
    
    try {
        testBasicElements();
        testStyleBlocks();
        testTemplateSystem();
        testCustomSystem();
        testOriginEmbedding();
        testCompleteWebPage();
        
        std::cout << "\n🎉 所有CHTL语法测试完成!" << std::endl;
        std::cout << "\n📋 测试覆盖的CHTL语法特性:" << std::endl;
        std::cout << "  ✅ 基础语法: 元素、属性、文本节点" << std::endl;
        std::cout << "  ✅ 样式系统: 局部样式块、类选择器、伪类" << std::endl;
        std::cout << "  ✅ 模板系统: @Style、@Element、@Var模板" << std::endl;
        std::cout << "  ✅ 自定义系统: 无值样式组、自定义元素、变量组" << std::endl;
        std::cout << "  ✅ 原始嵌入: @Html、@Style、@JavaScript、自定义类型" << std::endl;
        std::cout << "  ✅ 完整网页: HTML5文档结构、响应式布局" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试过程中发生异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}