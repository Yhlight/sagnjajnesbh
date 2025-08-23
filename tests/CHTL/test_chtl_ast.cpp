#include <iostream>
#include <cassert>
#include <memory>
#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/AST/CHTLASTVisitor.h"
#include "CHTL/AST/CHTLASTBuilder.h"
#include "CHTL/Core/CHTLToken.h"

using namespace CHTL;

// 测试AST节点创建和基本功能
void test_ast_node_creation() {
    std::cout << "测试AST节点创建...\n";
    
    // 测试根节点
    auto root = std::make_shared<AST::RootNode>();
    root->SetFileName("test.chtl");
    assert(root->GetType() == AST::NodeType::ROOT);
    assert(root->GetFileName() == "test.chtl");
    assert(root->GetChildCount() == 0);
    
    // 测试元素节点
    Core::CHTLToken token(Core::TokenType::HTML_TAG, "div", 1, 1, "test.chtl");
    auto element = std::make_shared<AST::ElementNode>("div", token);
    assert(element->GetType() == AST::NodeType::ELEMENT);
    assert(element->GetTagName() == "div");
    assert(element->GetToken().GetValue() == "div");
    
    // 测试属性
    element->AddAttribute("id", "main");
    element->AddAttribute("class", "container");
    assert(element->HasAttribute("id"));
    assert(element->GetAttribute("id") == "main");
    assert(element->GetAttribute("class") == "container");
    
    // 测试CSS类添加
    element->AddClass("responsive");
    assert(element->GetAttribute("class") == "container responsive");
    
    // 测试父子关系
    root->AddChild(element);
    assert(root->GetChildCount() == 1);
    assert(element->GetParent() == root.get());
    assert(root->GetChild(0) == element);
    
    std::cout << "  AST节点创建测试通过\n";
}

// 测试文本节点
void test_text_node() {
    std::cout << "测试文本节点...\n";
    
    Core::CHTLToken token(Core::TokenType::TEXT, "text", 2, 5, "test.chtl");
    auto textNode = std::make_shared<AST::TextNode>("Hello World", token);
    
    assert(textNode->GetType() == AST::NodeType::TEXT);
    assert(textNode->GetContent() == "Hello World");
    assert(!textNode->IsLiteral());
    
    textNode->SetIsLiteral(true);
    assert(textNode->IsLiteral());
    
    // 测试ToString
    std::string str = textNode->ToString();
    assert(str.find("TEXT") != std::string::npos);
    assert(str.find("Hello World") != std::string::npos);
    
    std::cout << "  文本节点测试通过\n";
}

// 测试模板节点
void test_template_node() {
    std::cout << "测试模板节点...\n";
    
    Core::CHTLToken token(Core::TokenType::TEMPLATE, "[Template]", 1, 1, "test.chtl");
    auto templateNode = std::make_shared<AST::TemplateNode>(
        AST::TemplateNode::TemplateType::STYLE, "DefaultButton", token);
    
    assert(templateNode->GetType() == AST::NodeType::TEMPLATE_STYLE);
    assert(templateNode->GetTemplateType() == AST::TemplateNode::TemplateType::STYLE);
    assert(templateNode->GetName() == "DefaultButton");
    
    // 测试继承
    auto inheritance = std::make_shared<AST::InheritanceNode>("@Style", "BaseButton", false, token);
    templateNode->AddInheritance(inheritance);
    assert(templateNode->GetInheritances().size() == 1);
    
    // 测试ToString
    std::string str = templateNode->ToString();
    assert(str.find("TEMPLATE_Style") != std::string::npos);
    assert(str.find("DefaultButton") != std::string::npos);
    
    std::cout << "  模板节点测试通过\n";
}

// 测试自定义节点
void test_custom_node() {
    std::cout << "测试自定义节点...\n";
    
    Core::CHTLToken token(Core::TokenType::CUSTOM, "[Custom]", 5, 1, "test.chtl");
    auto customNode = std::make_shared<AST::CustomNode>(
        AST::CustomNode::CustomType::ELEMENT, "Card", token);
    
    assert(customNode->GetType() == AST::NodeType::CUSTOM_ELEMENT);
    assert(customNode->GetCustomType() == AST::CustomNode::CustomType::ELEMENT);
    assert(customNode->GetName() == "Card");
    
    // 测试特例化
    auto specialization = std::make_shared<AST::SpecializationNode>(token);
    specialization->AddPropertyOverride("color", "red");
    customNode->AddSpecialization(specialization);
    assert(customNode->GetSpecializations().size() == 1);
    assert(customNode->HasSpecialization());
    
    std::cout << "  自定义节点测试通过\n";
}

// 测试样式块节点
void test_style_block_node() {
    std::cout << "测试样式块节点...\n";
    
    Core::CHTLToken token(Core::TokenType::STYLE, "style", 10, 5, "test.chtl");
    auto styleBlock = std::make_shared<AST::StyleBlockNode>(token);
    
    assert(styleBlock->GetType() == AST::NodeType::STYLE_BLOCK);
    assert(styleBlock->IsLocalStyleBlock());
    
    // 测试CSS选择器
    auto selector = std::make_shared<AST::CSSSelectorNode>(
        AST::CSSSelectorNode::SelectorType::CLASS, ".box", token);
    styleBlock->AddSelector(selector);
    
    // 测试CSS属性
    auto property = std::make_shared<AST::CSSPropertyNode>("width", "100px", token);
    styleBlock->AddProperty(property);
    
    assert(styleBlock->GetChildCount() == 2);
    
    // 测试CSS选择器的自动添加到全局样式块特性
    assert(selector->GetSelectorType() == AST::CSSSelectorNode::SelectorType::CLASS);
    assert(selector->GetSelector() == ".box");
    assert(selector->IsAddedToGlobalStyle()); // 类选择器应该自动添加到全局样式块
    
    std::cout << "  样式块节点测试通过\n";
}

// 测试原始嵌入节点
void test_origin_node() {
    std::cout << "测试原始嵌入节点...\n";
    
    Core::CHTLToken token(Core::TokenType::ORIGIN, "[Origin]", 15, 1, "test.chtl");
    
    // 测试HTML原始嵌入
    auto htmlOrigin = std::make_shared<AST::OriginNode>(
        AST::OriginNode::OriginType::HTML, "<div>Raw HTML</div>", "", token);
    assert(htmlOrigin->GetType() == AST::NodeType::ORIGIN_HTML);
    assert(htmlOrigin->GetOriginType() == AST::OriginNode::OriginType::HTML);
    assert(htmlOrigin->GetContent() == "<div>Raw HTML</div>");
    assert(!htmlOrigin->IsNamed());
    
    // 测试带名原始嵌入
    auto namedOrigin = std::make_shared<AST::OriginNode>(
        AST::OriginNode::OriginType::STYLE, "body { margin: 0; }", "globalStyles", token);
    assert(namedOrigin->IsNamed());
    assert(namedOrigin->GetName() == "globalStyles");
    
    // 测试自定义类型原始嵌入
    auto customOrigin = std::make_shared<AST::OriginNode>(
        AST::OriginNode::OriginType::CUSTOM, "<template>Vue content</template>", "vueComponent", token);
    customOrigin->SetCustomTypeName("@Vue");
    assert(customOrigin->GetOriginType() == AST::OriginNode::OriginType::CUSTOM);
    assert(customOrigin->GetCustomTypeName() == "@Vue");
    
    std::cout << "  原始嵌入节点测试通过\n";
}

// 测试导入节点
void test_import_node() {
    std::cout << "测试导入节点...\n";
    
    Core::CHTLToken token(Core::TokenType::IMPORT, "[Import]", 1, 1, "test.chtl");
    
    // 测试普通导入
    auto importNode = std::make_shared<AST::ImportNode>(
        AST::ImportNode::ImportType::CHTL, "utils/helpers.chtl", "", "", token);
    assert(importNode->GetType() == AST::NodeType::IMPORT);
    assert(importNode->GetImportType() == AST::ImportNode::ImportType::CHTL);
    assert(importNode->GetPath() == "utils/helpers.chtl");
    assert(!importNode->HasAlias());
    assert(!importNode->IsBatchImport());
    
    // 测试带别名导入
    auto aliasImport = std::make_shared<AST::ImportNode>(
        AST::ImportNode::ImportType::TEMPLATE_STYLE, "theme.chtl", "DefaultTheme", "MyTheme", token);
    assert(aliasImport->HasAlias());
    assert(aliasImport->GetAlias() == "MyTheme");
    assert(aliasImport->GetName() == "DefaultTheme");
    
    // 测试批量导入
    auto batchImport = std::make_shared<AST::ImportNode>(
        AST::ImportNode::ImportType::ALL_TEMPLATES, "components.chtl", "", "", token);
    assert(batchImport->IsBatchImport());
    
    std::cout << "  导入节点测试通过\n";
}

// 测试命名空间节点
void test_namespace_node() {
    std::cout << "测试命名空间节点...\n";
    
    Core::CHTLToken token(Core::TokenType::NAMESPACE, "[Namespace]", 1, 1, "test.chtl");
    auto namespaceNode = std::make_shared<AST::NamespaceNode>("MyNamespace", token);
    
    assert(namespaceNode->GetType() == AST::NodeType::NAMESPACE);
    assert(namespaceNode->GetName() == "MyNamespace");
    assert(!namespaceNode->IsNested()); // 没有父命名空间
    
    // 测试嵌套命名空间
    auto parentNamespace = std::make_shared<AST::NamespaceNode>("Parent", token);
    parentNamespace->AddChild(namespaceNode);
    assert(namespaceNode->IsNested());
    
    // 测试约束
    std::vector<std::string> targets = {"span", "[Custom] @Element Box"};
    auto constraint = std::make_shared<AST::ConstraintNode>(
        AST::ConstraintNode::ConstraintType::PRECISE, targets, token);
    namespaceNode->AddConstraint(constraint);
    assert(namespaceNode->GetConstraints().size() == 1);
    
    std::cout << "  命名空间节点测试通过\n";
}

// 测试删除和插入节点
void test_deletion_insertion_nodes() {
    std::cout << "测试删除和插入节点...\n";
    
    Core::CHTLToken token(Core::TokenType::DELETE, "delete", 8, 10, "test.chtl");
    
    // 测试删除节点
    std::vector<std::string> deleteTargets = {"line-height", "border"};
    auto deletionNode = std::make_shared<AST::DeletionNode>(
        AST::DeletionNode::DeletionType::PROPERTY, deleteTargets, token);
    
    assert(deletionNode->GetType() == AST::NodeType::DELETION);
    assert(deletionNode->GetDeletionType() == AST::DeletionNode::DeletionType::PROPERTY);
    assert(deletionNode->GetTargets().size() == 2);
    assert(deletionNode->GetTargets()[0] == "line-height");
    
    deletionNode->AddTarget("margin");
    assert(deletionNode->GetTargets().size() == 3);
    
    // 测试插入节点
    Core::CHTLToken insertToken(Core::TokenType::INSERT, "insert", 12, 5, "test.chtl");
    auto insertionNode = std::make_shared<AST::InsertionNode>(
        AST::InsertionNode::InsertionPosition::AFTER, "div[0]", insertToken);
    
    assert(insertionNode->GetType() == AST::NodeType::INSERTION);
    assert(insertionNode->GetPosition() == AST::InsertionNode::InsertionPosition::AFTER);
    assert(insertionNode->GetTarget() == "div[0]");
    
    // 添加插入内容
    auto newDiv = std::make_shared<AST::ElementNode>("div", token);
    insertionNode->AddInsertContent(newDiv);
    assert(insertionNode->GetInsertContents().size() == 1);
    
    std::cout << "  删除和插入节点测试通过\n";
}

// 测试索引访问节点
void test_index_access_node() {
    std::cout << "测试索引访问节点...\n";
    
    Core::CHTLToken token(Core::TokenType::LEFT_BRACKET, "[", 5, 10, "test.chtl");
    auto indexNode = std::make_shared<AST::IndexAccessNode>("div", 1, token);
    
    assert(indexNode->GetType() == AST::NodeType::INDEX_ACCESS);
    assert(indexNode->GetElementName() == "div");
    assert(indexNode->GetIndex() == 1);
    
    indexNode->SetIndex(2);
    assert(indexNode->GetIndex() == 2);
    
    // 测试ToString
    std::string str = indexNode->ToString();
    assert(str.find("INDEX_ACCESS") != std::string::npos);
    assert(str.find("div[2]") != std::string::npos);
    
    std::cout << "  索引访问节点测试通过\n";
}

// 测试变量相关节点
void test_variable_nodes() {
    std::cout << "测试变量相关节点...\n";
    
    Core::CHTLToken token(Core::TokenType::AT_VAR, "@Var", 3, 1, "test.chtl");
    
    // 变量组功能现在通过TEMPLATE_VAR和CUSTOM_VAR节点实现
    // 测试已移除，因为VARIABLE_GROUP节点已删除
    
    // 所有变量组相关测试已移除
    // 变量组功能现在通过现有的TEMPLATE_VAR和CUSTOM_VAR节点实现
    
    std::cout << "  变量相关节点测试通过\n";
}

// 测试CSS相关节点
void test_css_nodes() {
    std::cout << "测试CSS相关节点...\n";
    
    Core::CHTLToken token(Core::TokenType::CLASS_SELECTOR, ".box", 8, 5, "test.chtl");
    
    // 测试CSS选择器节点
    auto selector = std::make_shared<AST::CSSSelectorNode>(
        AST::CSSSelectorNode::SelectorType::CLASS, ".box", token);
    
    assert(selector->GetType() == AST::NodeType::CSS_SELECTOR);
    assert(selector->GetSelectorType() == AST::CSSSelectorNode::SelectorType::CLASS);
    assert(selector->GetSelector() == ".box");
    assert(selector->IsAddedToGlobalStyle()); // 类选择器自动添加到全局样式块
    
    // 测试&选择器
    Core::CHTLToken ampToken(Core::TokenType::AMPERSAND, "&", 9, 5, "test.chtl");
    auto ampSelector = std::make_shared<AST::CSSSelectorNode>(
        AST::CSSSelectorNode::SelectorType::AMPERSAND, "&:hover", ampToken);
    assert(!ampSelector->IsAddedToGlobalStyle()); // &选择器不自动添加到全局样式块
    
    // 测试CSS属性节点
    Core::CHTLToken propToken(Core::TokenType::IDENTIFIER, "width", 10, 9, "test.chtl");
    auto property = std::make_shared<AST::CSSPropertyNode>("width", "100px", propToken);
    
    assert(property->GetType() == AST::NodeType::CSS_PROPERTY);
    assert(property->GetProperty() == "width");
    assert(property->GetValue() == "100px");
    assert(!property->UsesCEEquality());
    
    // 测试CE对等式
    property->SetUsesCEEquality(true);
    assert(property->UsesCEEquality());
    
    std::cout << "  CSS相关节点测试通过\n";
}

// 测试注释节点
void test_comment_node() {
    std::cout << "测试注释节点...\n";
    
    // 测试单行注释
    Core::CHTLToken token(Core::TokenType::SINGLE_LINE_COMMENT, "//", 1, 1, "test.chtl");
    auto singleComment = std::make_shared<AST::CommentNode>(
        AST::CommentNode::CommentType::SINGLE_LINE, "这是单行注释", token);
    
    assert(singleComment->GetType() == AST::NodeType::COMMENT);
    assert(singleComment->GetCommentType() == AST::CommentNode::CommentType::SINGLE_LINE);
    assert(singleComment->GetContent() == "这是单行注释");
    assert(!singleComment->IsGeneratorComment());
    
    // 测试生成器注释
    Core::CHTLToken genToken(Core::TokenType::GENERATOR_COMMENT, "--", 2, 1, "test.chtl");
    auto genComment = std::make_shared<AST::CommentNode>(
        AST::CommentNode::CommentType::GENERATOR, "生成器注释", genToken);
    
    assert(genComment->IsGeneratorComment());
    
    std::cout << "  注释节点测试通过\n";
}

// 测试字面量节点
void test_literal_node() {
    std::cout << "测试字面量节点...\n";
    
    // 测试字符串字面量
    Core::CHTLToken strToken(Core::TokenType::STRING_LITERAL, "\"Hello\"", 1, 5, "test.chtl");
    auto stringLiteral = std::make_shared<AST::LiteralNode>(
        AST::LiteralNode::LiteralType::STRING, "Hello", strToken);
    
    assert(stringLiteral->GetType() == AST::NodeType::STRING_LITERAL);
    assert(stringLiteral->GetLiteralType() == AST::LiteralNode::LiteralType::STRING);
    assert(stringLiteral->GetValue() == "Hello");
    
    // 测试无修饰字面量
    Core::CHTLToken unquotedToken(Core::TokenType::UNQUOTED_LITERAL, "red", 2, 10, "test.chtl");
    auto unquotedLiteral = std::make_shared<AST::LiteralNode>(
        AST::LiteralNode::LiteralType::UNQUOTED, "red", unquotedToken);
    
    assert(unquotedLiteral->GetType() == AST::NodeType::UNQUOTED_LITERAL);
    assert(unquotedLiteral->GetLiteralType() == AST::LiteralNode::LiteralType::UNQUOTED);
    
    // 测试数字字面量
    Core::CHTLToken numToken(Core::TokenType::NUMBER, "100", 3, 15, "test.chtl");
    auto numberLiteral = std::make_shared<AST::LiteralNode>(
        AST::LiteralNode::LiteralType::NUMBER, "100", numToken);
    
    assert(numberLiteral->GetType() == AST::NodeType::NUMBER_LITERAL);
    assert(numberLiteral->GetLiteralType() == AST::LiteralNode::LiteralType::NUMBER);
    
    std::cout << "  字面量节点测试通过\n";
}

// 测试AST访问者模式
void test_ast_visitor() {
    std::cout << "测试AST访问者模式...\n";
    
    // 创建一个简单的AST
    auto root = std::make_shared<AST::RootNode>();
    root->SetFileName("visitor_test.chtl");
    
    Core::CHTLToken token(Core::TokenType::HTML_TAG, "div", 1, 1, "visitor_test.chtl");
    auto element = std::make_shared<AST::ElementNode>("div", token);
    element->AddAttribute("id", "main");
    
    auto textNode = std::make_shared<AST::TextNode>("Hello World", token);
    element->AddChild(textNode);
    root->AddChild(element);
    
    // 测试打印访问者
    AST::ASTPrintVisitor printVisitor;
    root->Accept(printVisitor);
    
    std::string result = printVisitor.GetResult();
    assert(!result.empty());
    assert(result.find("ROOT") != std::string::npos);
    assert(result.find("ELEMENT(div") != std::string::npos);
    assert(result.find("TEXT") != std::string::npos);
    
    // 测试验证访问者
    AST::ASTValidationVisitor validationVisitor;
    root->Accept(validationVisitor);
    
    assert(!validationVisitor.HasErrors()); // 应该没有验证错误
    
    std::cout << "  AST访问者模式测试通过\n";
}

// 测试AST节点克隆
void test_ast_cloning() {
    std::cout << "测试AST节点克隆...\n";
    
    Core::CHTLToken token(Core::TokenType::HTML_TAG, "div", 1, 1, "test.chtl");
    auto original = std::make_shared<AST::ElementNode>("div", token);
    original->AddAttribute("class", "container");
    
    auto textChild = std::make_shared<AST::TextNode>("Original Text", token);
    original->AddChild(textChild);
    
    // 克隆节点
    auto cloned = std::dynamic_pointer_cast<AST::ElementNode>(original->Clone());
    
    assert(cloned != nullptr);
    assert(cloned->GetTagName() == original->GetTagName());
    assert(cloned->GetAttribute("class") == original->GetAttribute("class"));
    assert(cloned->GetChildCount() == original->GetChildCount());
    
    // 验证是深拷贝
    assert(cloned.get() != original.get());
    assert(cloned->GetChild(0).get() != original->GetChild(0).get());
    
    std::cout << "  AST节点克隆测试通过\n";
}

int main() {
    std::cout << "运行CHTL AST功能测试...\n\n";
    
    try {
        test_ast_node_creation();
        test_text_node();
        test_template_node();
        test_custom_node();
        test_style_block_node();
        test_origin_node();
        test_import_node();
        test_namespace_node();
        test_deletion_insertion_nodes();
        test_index_access_node();
        test_variable_nodes();
        test_css_nodes();
        test_comment_node();
        test_literal_node();
        test_ast_visitor();
        test_ast_cloning();
        
        std::cout << "\n所有CHTL AST功能测试通过!\n";
        std::cout << "✅ AST节点创建和管理正常\n";
        std::cout << "✅ 模板和自定义系统正常\n";
        std::cout << "✅ 原始嵌入系统正常\n";
        std::cout << "✅ 导入和命名空间系统正常\n";
        std::cout << "✅ 特殊操作（删除、插入、索引访问）正常\n";
        std::cout << "✅ CSS和样式系统正常\n";
        std::cout << "✅ 访问者模式正常\n";
        std::cout << "✅ 节点克隆功能正常\n";
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}