#include <iostream>
#include <cassert>
#include <memory>
#include "chtl/ast/DocumentNode.h"
#include "chtl/ast/ElementNode.h"
#include "chtl/ast/TextNode.h"
#include "chtl/ast/AttributeNode.h"
#include "chtl/ast/StyleBlockNode.h"
#include "chtl/ast/TemplateNode.h"
#include "chtl/ast/CustomNode.h"
#include "chtl/ast/ImportNode.h"
#include "chtl/ast/NamespaceNode.h"
#include "chtl/ast/ConfigurationNode.h"
#include "chtl/ast/OriginNode.h"
#include "chtl/ast/ExceptNode.h"
#include "chtl/ast/ASTBuilder.h"
#include "utils/Logger.h"

using namespace chtl::ast;

void TestBasicNodes() {
    std::cout << "测试基本AST节点..." << std::endl;
    
    // 创建文档节点
    auto doc = std::make_shared<DocumentNode>();
    doc->SetFilename("test.chtl");
    assert(doc->GetFilename() == "test.chtl");
    assert(doc->GetType() == ASTNodeType::DOCUMENT);
    
    // 创建元素节点
    auto div = std::make_shared<ElementNode>("div");
    assert(div->GetTagName() == "div");
    assert(div->GetType() == ASTNodeType::ELEMENT);
    
    // 创建属性节点
    auto idAttr = std::make_shared<AttributeNode>("id", "main");
    assert(idAttr->GetName() == "id");
    assert(idAttr->GetAttributeValue() == "main");
    
    // 创建文本节点
    auto text = std::make_shared<TextNode>();
    text->SetContent("Hello CHTL");
    assert(text->GetContent() == "Hello CHTL");
    
    // 构建树结构
    div->AddAttribute(idAttr);
    div->AddChild(text);
    doc->AddChild(div);
    
    assert(doc->GetChildCount() == 1);
    assert(div->GetChildCount() == 2); // 属性 + 文本
    assert(div->HasAttribute("id"));
    
    std::cout << "基本AST节点测试通过!" << std::endl;
}

void TestStyleNodes() {
    std::cout << "测试样式相关节点..." << std::endl;
    
    // 创建样式块
    auto styleBlock = std::make_shared<StyleBlockNode>();
    styleBlock->SetIsLocal(true);
    
    // 添加内联样式
    styleBlock->AddInlineProperty("width", "100px");
    styleBlock->AddInlineProperty("height", "200px");
    
    auto inlineProps = styleBlock->GetInlineProperties();
    assert(inlineProps.size() == 2);
    assert(inlineProps[0].first == "width");
    assert(inlineProps[0].second == "100px");
    
    // 创建样式规则
    auto rule = std::make_shared<StyleRuleNode>(".box");
    rule->SetSelectorType(StyleRuleNode::CLASS);
    rule->AddProperty("color", "red");
    rule->AddProperty("font-size", "16px");
    
    styleBlock->AddStyleRule(rule);
    styleBlock->AddAutoClass("box");
    
    assert(styleBlock->GetStyleRules().size() == 1);
    assert(styleBlock->GetAutoClasses().count("box") == 1);
    
    // 创建样式引用
    auto styleRef = std::make_shared<StyleReferenceNode>("DefaultTheme");
    styleRef->AddOverride("color", "blue");
    styleRef->AddDeletedProperty("border");
    
    styleBlock->AddStyleReference(styleRef);
    
    assert(styleBlock->GetStyleReferences().size() == 1);
    assert(styleRef->GetDeletedProperties().count("border") == 1);
    
    std::cout << "样式节点测试通过!" << std::endl;
}

void TestTemplateNodes() {
    std::cout << "测试模板节点..." << std::endl;
    
    // 创建样式组模板
    auto templateStyle = std::make_shared<TemplateStyleNode>("DefaultText");
    templateStyle->AddProperty("color", "black");
    templateStyle->AddProperty("line-height", "1.6");
    templateStyle->AddInheritedTemplate("BaseStyle");
    
    assert(templateStyle->GetTemplateName() == "DefaultText");
    assert(templateStyle->GetTemplateType() == TemplateNode::STYLE);
    assert(templateStyle->GetProperties().size() == 2);
    assert(templateStyle->GetInheritedTemplates().size() == 1);
    
    // 创建元素模板
    auto templateElement = std::make_shared<TemplateElementNode>("Box");
    auto div1 = std::make_shared<ElementNode>("div");
    auto div2 = std::make_shared<ElementNode>("div");
    templateElement->AddElement(div1);
    templateElement->AddElement(div2);
    
    assert(templateElement->GetElements().size() == 2);
    
    // 创建变量组模板
    auto templateVar = std::make_shared<TemplateVarNode>("ThemeColor");
    templateVar->AddVariable("tableColor", "rgb(255, 192, 203)");
    templateVar->AddVariable("textColor", "black");
    
    assert(templateVar->HasVariable("tableColor"));
    assert(templateVar->GetVariableValue("textColor") == "black");
    
    std::cout << "模板节点测试通过!" << std::endl;
}

void TestCustomNodes() {
    std::cout << "测试自定义节点..." << std::endl;
    
    // 创建自定义样式组
    auto customStyle = std::make_shared<CustomStyleNode>("TextSet");
    customStyle->AddEmptyProperty("color");
    customStyle->AddEmptyProperty("font-size");
    customStyle->AddProperty("line-height", "1.5");
    
    assert(customStyle->GetEmptyProperties().size() == 2);
    assert(customStyle->GetProperties().size() == 1);
    
    // 创建自定义元素
    auto customElement = std::make_shared<CustomElementNode>("Card");
    auto header = std::make_shared<ElementNode>("header");
    auto body = std::make_shared<ElementNode>("section");
    customElement->AddElement(header);
    customElement->AddElement(body);
    
    assert(customElement->GetElements().size() == 2);
    
    // 创建自定义变量组
    auto customVar = std::make_shared<CustomVarNode>("Colors");
    customVar->AddVariable("primary", "#007bff");
    customVar->AddVariable("secondary", "#6c757d");
    
    assert(customVar->GetVariables().size() == 2);
    
    std::cout << "自定义节点测试通过!" << std::endl;
}

void TestImportNode() {
    std::cout << "测试导入节点..." << std::endl;
    
    auto import = std::make_shared<ImportNode>();
    import->SetImportType(ImportNode::CHTL);
    import->SetFromPath("components/button.chtl");
    import->SetAlias("Button");
    
    assert(import->GetImportType() == ImportNode::CHTL);
    assert(import->GetFromPath() == "components/button.chtl");
    assert(import->HasAlias());
    assert(import->GetAlias() == "Button");
    
    // 测试特定导入
    auto specificImport = std::make_shared<ImportNode>();
    specificImport->SetImportType(ImportNode::SPECIFIC);
    specificImport->SetIsCustom(true);
    specificImport->SetSpecificType("@Element");
    specificImport->SetSpecificName("Box");
    specificImport->SetFromPath("layouts.chtl");
    
    assert(specificImport->IsCustom());
    assert(specificImport->GetSpecificType() == "@Element");
    assert(specificImport->GetSpecificName() == "Box");
    
    std::cout << "导入节点测试通过!" << std::endl;
}

void TestNamespaceNode() {
    std::cout << "测试命名空间节点..." << std::endl;
    
    auto ns1 = std::make_shared<NamespaceNode>("app");
    assert(ns1->GetNamespaceName() == "app");
    assert(ns1->GetFullyQualifiedName() == "app");
    
    auto ns2 = std::make_shared<NamespaceNode>("components");
    ns2->SetIsNested(true);
    ns2->SetParentNamespace("app");
    
    assert(ns2->IsNested());
    assert(ns2->GetFullyQualifiedName() == "app.components");
    
    std::cout << "命名空间节点测试通过!" << std::endl;
}

void TestConfigurationNode() {
    std::cout << "测试配置节点..." << std::endl;
    
    auto config = std::make_shared<ConfigurationNode>("Basic");
    config->AddProperty("INDEX_INITIAL_COUNT", "0");
    config->AddProperty("DEBUG_MODE", "false");
    
    assert(config->IsNamed());
    assert(config->GetConfigName() == "Basic");
    assert(config->GetProperty("INDEX_INITIAL_COUNT") == "0");
    assert(config->HasProperty("DEBUG_MODE"));
    
    // 创建[Name]块
    auto nameBlock = std::make_shared<ConfigNameBlockNode>();
    nameBlock->AddNameMapping("CUSTOM_STYLE", std::vector<std::string>{"@Style", "@style", "@CSS"});
    nameBlock->AddNameMapping("KEYWORD_TEXT", "text");
    nameBlock->SetOptionCount(3);
    
    config->SetNameBlock(nameBlock);
    
    assert(config->GetNameBlock() != nullptr);
    assert(nameBlock->GetMapping("CUSTOM_STYLE").size() == 3);
    assert(nameBlock->GetOptionCount() == 3);
    
    // 创建[OriginType]块
    auto originType = std::make_shared<ConfigOriginTypeNode>();
    originType->AddOriginType("ORIGINTYPE_VUE", "@Vue");
    originType->AddOriginType("ORIGINTYPE_REACT", "@React");
    
    config->SetOriginTypeBlock(originType);
    
    assert(config->GetOriginTypeBlock() != nullptr);
    assert(originType->GetOriginType("ORIGINTYPE_VUE") == "@Vue");
    assert(originType->HasOriginType("ORIGINTYPE_REACT"));
    
    std::cout << "配置节点测试通过!" << std::endl;
}

void TestOriginNode() {
    std::cout << "测试原始嵌入节点..." << std::endl;
    
    auto origin = std::make_shared<OriginNode>();
    origin->SetOriginType("@Html");
    origin->SetOriginName("header");
    origin->SetContent("<header>Custom HTML</header>");
    
    assert(origin->GetOriginType() == "@Html");
    assert(origin->IsNamed());
    assert(origin->GetOriginName() == "header");
    assert(!origin->IsReference());
    
    // 测试原始嵌入引用
    auto originRef = std::make_shared<OriginNode>();
    originRef->SetOriginType("@Html");
    originRef->SetOriginName("header");
    originRef->SetIsReference(true);
    
    assert(originRef->IsReference());
    
    std::cout << "原始嵌入节点测试通过!" << std::endl;
}

void TestExceptNode() {
    std::cout << "测试约束节点..." << std::endl;
    
    auto except = std::make_shared<ExceptNode>();
    except->SetConstraintType(ExceptNode::ELEMENT);
    except->AddTarget("span");
    except->AddTarget("br");
    
    assert(except->GetConstraintType() == ExceptNode::ELEMENT);
    assert(except->GetTargets().size() == 2);
    assert(except->GetTargets().count("span") == 1);
    
    // 测试类型约束
    auto typeExcept = std::make_shared<ExceptNode>();
    typeExcept->SetConstraintType(ExceptNode::TYPE);
    ExceptNode::ConstraintInfo info;
    info.isCustom = true;
    info.specificType = "@Element";
    typeExcept->SetConstraintInfo(info);
    
    assert(typeExcept->GetConstraintInfo().isCustom);
    assert(typeExcept->GetConstraintInfo().specificType == "@Element");
    
    std::cout << "约束节点测试通过!" << std::endl;
}

void TestASTStructure() {
    std::cout << "测试完整的AST结构..." << std::endl;
    
    // 构建一个完整的CHTL文档AST
    auto doc = std::make_shared<DocumentNode>();
    doc->SetFilename("complete.chtl");
    
    // 添加模板
    auto template1 = std::make_shared<TemplateStyleNode>("Theme");
    template1->AddProperty("color", "black");
    doc->AddChild(template1);
    
    // 添加HTML结构
    auto html = std::make_shared<ElementNode>("html");
    auto head = std::make_shared<ElementNode>("head");
    auto title = std::make_shared<ElementNode>("title");
    auto titleText = std::make_shared<TextNode>();
    titleText->SetContent("CHTL Test");
    
    title->AddChild(titleText);
    head->AddChild(title);
    html->AddChild(head);
    
    auto body = std::make_shared<ElementNode>("body");
    auto div = std::make_shared<ElementNode>("div");
    
    // 添加属性
    auto idAttr = std::make_shared<AttributeNode>("id", "main");
    div->AddAttribute(idAttr);
    
    // 添加样式块
    auto styleBlock = std::make_shared<StyleBlockNode>();
    styleBlock->AddInlineProperty("width", "100%");
    
    auto styleRule = std::make_shared<StyleRuleNode>(".container");
    styleRule->AddProperty("margin", "0 auto");
    styleBlock->AddStyleRule(styleRule);
    
    div->SetStyleBlock(styleBlock);
    
    body->AddChild(div);
    html->AddChild(body);
    doc->AddChild(html);
    
    // 验证结构
    assert(doc->GetChildCount() == 2); // template + html
    assert(html->GetChildCount() == 2); // head + body
    assert(div->HasStyleBlock());
    assert(div->GetStyleBlock()->GetInlineProperties().size() == 1);
    assert(div->GetStyleBlock()->GetStyleRules().size() == 1);
    
    std::cout << "完整AST结构测试通过!" << std::endl;
}

int main() {
    // 设置日志级别
    chtl::utils::Logger::GetInstance().SetLevel(chtl::utils::LogLevel::INFO);
    
    std::cout << "运行CHTL AST节点测试..." << std::endl;
    
    TestBasicNodes();
    TestStyleNodes();
    TestTemplateNodes();
    TestCustomNodes();
    TestImportNode();
    TestNamespaceNode();
    TestConfigurationNode();
    TestOriginNode();
    TestExceptNode();
    TestASTStructure();
    
    std::cout << "\n所有CHTL AST测试通过!" << std::endl;
    return 0;
}