#include "../src/CHTLCompiler/AST/CHTLNodes.h"
#include <iostream>
#include <cassert>

using namespace chtl::ast;

// 简单的访问者实现用于测试
class TestVisitor : public ASTVisitor {
public:
    int visit_count = 0;
    
    void visitProgram(ProgramNode& node) override {
        visit_count++;
        std::cout << "访问程序节点: " << node.toString() << std::endl;
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            node.getChild(i)->accept(*this);
        }
    }
    
    void visitElement(ElementNode& node) override {
        visit_count++;
        std::cout << "访问元素节点: " << node.toString() << std::endl;
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            node.getChild(i)->accept(*this);
        }
    }
    
    void visitText(TextNode& node) override {
        visit_count++;
        std::cout << "访问文本节点: " << node.toString() << std::endl;
    }
    
    void visitAttribute(AttributeNode& node) override {
        visit_count++;
        std::cout << "访问属性节点: " << node.toString() << std::endl;
    }
    
    void visitStyleBlock(StyleBlockNode& node) override {
        visit_count++;
        std::cout << "访问样式块节点: " << node.toString() << std::endl;
        for (size_t i = 0; i < node.getChildCount(); ++i) {
            node.getChild(i)->accept(*this);
        }
    }
    
    void visitScriptBlock(ScriptBlockNode& node) override {
        visit_count++;
        std::cout << "访问脚本块节点: " << node.toString() << std::endl;
    }
    
    void visitTemplateDefinition(TemplateDefinitionNode& node) override {
        visit_count++;
        std::cout << "访问模板定义节点: " << node.toString() << std::endl;
    }
    
    void visitCustomDefinition(CustomDefinitionNode& node) override {
        visit_count++;
        std::cout << "访问自定义定义节点: " << node.toString() << std::endl;
    }
    
    void visitOriginBlock(OriginBlockNode& node) override {
        visit_count++;
        std::cout << "访问原始嵌入节点: " << node.toString() << std::endl;
    }
    
    void visitImportStatement(ImportStatementNode& node) override {
        visit_count++;
        std::cout << "访问导入语句节点: " << node.toString() << std::endl;
    }
    
    void visitNamespaceDefinition(NamespaceDefinitionNode& node) override {
        visit_count++;
        std::cout << "访问命名空间定义节点: " << node.toString() << std::endl;
    }
    
    void visitCHTLJSSelector(CHTLJSSelectorNode& node) override {
        visit_count++;
        std::cout << "访问CHTL JS选择器节点: " << node.toString() << std::endl;
    }
    
    void visitComment(CommentNode& node) override {
        visit_count++;
        std::cout << "访问注释节点: " << node.toString() << std::endl;
    }
    
    void visitLiteral(LiteralNode& node) override {
        visit_count++;
        std::cout << "访问字面量节点: " << node.toString() << std::endl;
    }
    
    void visitIdentifier(IdentifierNode& node) override {
        visit_count++;
        std::cout << "访问标识符节点: " << node.toString() << std::endl;
    }
};

void testBasicNodes() {
    std::cout << "=== 测试基础AST节点 ===" << std::endl;
    
    // 测试程序节点
    auto program = std::make_unique<ProgramNode>();
    assert(program->type == NodeType::PROGRAM);
    assert(program->getChildCount() == 0);
    
    // 测试元素节点
    auto element = std::make_unique<ElementNode>("div");
    assert(element->type == NodeType::ELEMENT);
    assert(element->tag_name == "div");
    assert(element->is_self_closing == false);
    
    // 测试文本节点
    auto text = std::make_unique<TextNode>("Hello World");
    assert(text->type == NodeType::TEXT);
    assert(text->content == "Hello World");
    
    // 测试属性节点
    auto attribute = std::make_unique<AttributeNode>("id", "test");
    assert(attribute->type == NodeType::ATTRIBUTE);
    assert(attribute->name == "id");
    assert(attribute->value == "test");
    
    // 测试节点关系
    element->addChild(std::move(text));
    element->addChild(std::move(attribute));
    program->addChild(std::move(element));
    
    assert(program->getChildCount() == 1);
    assert(program->getChild(0)->type == NodeType::ELEMENT);
    
    std::cout << "基础AST节点测试通过!" << std::endl;
}

void testCHTLNodes() {
    std::cout << "=== 测试CHTL特殊节点 ===" << std::endl;
    
    // 测试样式块节点
    auto style_block = std::make_unique<StyleBlockNode>(true);
    assert(style_block->type == NodeType::STYLE_BLOCK);
    assert(style_block->is_local == true);
    
    // 测试脚本块节点
    auto script_block = std::make_unique<ScriptBlockNode>(false);
    assert(script_block->type == NodeType::SCRIPT_BLOCK);
    assert(script_block->is_local == false);
    
    // 测试模板定义节点
    auto template_def = std::make_unique<TemplateDefinitionNode>("@Style", "DefaultText");
    assert(template_def->type == NodeType::TEMPLATE_DEFINITION);
    assert(template_def->template_type == "@Style");
    assert(template_def->template_name == "DefaultText");
    
    // 测试自定义定义节点
    auto custom_def = std::make_unique<CustomDefinitionNode>("@Element", "Box");
    assert(custom_def->type == NodeType::CUSTOM_DEFINITION);
    assert(custom_def->custom_type == "@Element");
    assert(custom_def->custom_name == "Box");
    
    // 测试原始嵌入节点
    auto origin_block = std::make_unique<OriginBlockNode>("@Html");
    assert(origin_block->type == NodeType::ORIGIN_BLOCK);
    assert(origin_block->origin_type == "@Html");
    
    // 测试导入语句节点
    auto import_stmt = std::make_unique<ImportStatementNode>("@Chtl");
    import_stmt->import_target = "test";
    import_stmt->source_path = "test.chtl";
    import_stmt->alias = "TestModule";
    assert(import_stmt->type == NodeType::IMPORT_STATEMENT);
    assert(import_stmt->import_type == "@Chtl");
    
    // 测试命名空间定义节点
    auto namespace_def = std::make_unique<NamespaceDefinitionNode>("TestNamespace");
    assert(namespace_def->type == NodeType::NAMESPACE_DEFINITION);
    assert(namespace_def->namespace_name == "TestNamespace");
    
    // 测试CHTL JS选择器节点
    auto chtl_js_selector = std::make_unique<CHTLJSSelectorNode>("box");
    assert(chtl_js_selector->type == NodeType::CHTL_JS_SELECTOR);
    assert(chtl_js_selector->selector_content == "box");
    
    std::cout << "CHTL特殊节点测试通过!" << std::endl;
}

void testSpecializedNodes() {
    std::cout << "=== 测试特殊化节点 ===" << std::endl;
    
    // 测试CSS属性节点
    auto css_prop = std::make_unique<CSSPropertyNode>("color", "red");
    assert(css_prop->type == NodeType::CSS_PROPERTY);
    assert(css_prop->property_name == "color");
    assert(css_prop->property_value == "red");
    assert(css_prop->is_important == false);
    
    // 测试CSS选择器节点
    auto css_selector = std::make_unique<CSSSelectorNode>(".box");
    assert(css_selector->type == NodeType::CSS_SELECTOR);
    assert(css_selector->selector_text == ".box");
    assert(css_selector->auto_generated == false);
    
    // 测试样式继承节点
    auto style_inherit = std::make_unique<StyleInheritanceNode>("DefaultStyle", true);
    assert(style_inherit->type == NodeType::STYLE_INHERITANCE);
    assert(style_inherit->inherited_style == "DefaultStyle");
    assert(style_inherit->is_explicit == true);
    
    // 测试样式删除节点
    auto style_delete = std::make_unique<StyleDeleteNode>();
    style_delete->addDeletedProperty("color");
    style_delete->addDeletedProperty("background");
    style_delete->addDeletedInheritance("@Style DefaultStyle");
    assert(style_delete->type == NodeType::STYLE_DELETE);
    assert(style_delete->deleted_properties.size() == 2);
    assert(style_delete->deleted_inheritance.size() == 1);
    
    // 测试变量引用节点
    auto var_ref = std::make_unique<VariableReferenceNode>("ThemeColor", "primaryColor");
    assert(var_ref->type == NodeType::VARIABLE_REFERENCE);
    assert(var_ref->variable_group == "ThemeColor");
    assert(var_ref->variable_name == "primaryColor");
    
    // 测试变量特例化节点
    auto var_spec = std::make_unique<VariableSpecializationNode>("ThemeColor");
    var_spec->addSpecialization("primaryColor", "blue");
    var_spec->addSpecialization("secondaryColor", "green");
    assert(var_spec->type == NodeType::VARIABLE_SPECIALIZATION);
    assert(var_spec->specializations.size() == 2);
    
    // 测试元素插入节点
    auto element_insert = std::make_unique<ElementInsertNode>(
        ElementInsertNode::AFTER, "div[0]");
    assert(element_insert->type == NodeType::ELEMENT_INSERT);
    assert(element_insert->insert_position == ElementInsertNode::AFTER);
    assert(element_insert->target_selector == "div[0]");
    
    // 测试元素删除节点
    auto element_delete = std::make_unique<ElementDeleteNode>("span");
    assert(element_delete->type == NodeType::ELEMENT_DELETE);
    assert(element_delete->target_selector == "span");
    
    // 测试元素索引节点
    auto element_index = std::make_unique<ElementIndexNode>("div", 1);
    assert(element_index->type == NodeType::ELEMENT_INDEX);
    assert(element_index->element_name == "div");
    assert(element_index->index == 1);
    
    // 测试CHTL JS Listen节点
    auto chtl_js_listen = std::make_unique<CHTLJSListenNode>("{{box}}");
    chtl_js_listen->addEventHandler("click", "handleClick");
    chtl_js_listen->addEventHandler("hover", "handleHover");
    assert(chtl_js_listen->type == NodeType::CHTL_JS_LISTEN);
    assert(chtl_js_listen->target_selector == "{{box}}");
    assert(chtl_js_listen->event_handlers.size() == 2);
    
    // 测试CHTL JS Delegate节点
    auto chtl_js_delegate = std::make_unique<CHTLJSDelegateNode>("{{parent}}");
    chtl_js_delegate->addTargetSelector("{{child1}}");
    chtl_js_delegate->addTargetSelector("{{child2}}");
    chtl_js_delegate->addEventHandler("click", "handleDelegatedClick");
    assert(chtl_js_delegate->type == NodeType::CHTL_JS_DELEGATE);
    assert(chtl_js_delegate->parent_selector == "{{parent}}");
    assert(chtl_js_delegate->target_selectors.size() == 2);
    
    // 测试CHTL JS Vir节点
    auto chtl_js_vir = std::make_unique<CHTLJSVirNode>("Test", "listen");
    chtl_js_vir->addFunction("click", "function() { console.log('clicked'); }");
    assert(chtl_js_vir->type == NodeType::CHTL_JS_VIR);
    assert(chtl_js_vir->vir_name == "Test");
    assert(chtl_js_vir->function_name == "listen");
    
    // 测试约束节点
    auto except_constraint = std::make_unique<ExceptConstraintNode>();
    except_constraint->addExceptElement("span");
    except_constraint->addExceptElement("div");
    except_constraint->addExceptType("@Html");
    assert(except_constraint->type == NodeType::EXCEPT_CONSTRAINT);
    assert(except_constraint->except_elements.size() == 2);
    assert(except_constraint->except_types.size() == 1);
    
    std::cout << "特殊化节点测试通过!" << std::endl;
}

void testVisitorPattern() {
    std::cout << "=== 测试访问者模式 ===" << std::endl;
    
    // 创建AST树
    auto program = std::make_unique<ProgramNode>();
    auto element = std::make_unique<ElementNode>("div");
    auto text = std::make_unique<TextNode>("Hello World");
    auto attribute = std::make_unique<AttributeNode>("class", "container");
    auto style_block = std::make_unique<StyleBlockNode>();
    
    element->addChild(std::move(text));
    element->addChild(std::move(attribute));
    element->addChild(std::move(style_block));
    program->addChild(std::move(element));
    
    // 使用访问者遍历AST
    TestVisitor visitor;
    program->accept(visitor);
    
    assert(visitor.visit_count == 5); // program + element + text + attribute + style_block
    
    std::cout << "访问者模式测试通过!" << std::endl;
}

void testCloning() {
    std::cout << "=== 测试节点克隆 ===" << std::endl;
    
    // 创建原始节点
    auto original_element = std::make_unique<ElementNode>("div");
    original_element->is_self_closing = true;
    original_element->setAttribute("test", "value");
    
    auto original_text = std::make_unique<TextNode>("Test Content");
    original_element->addChild(std::move(original_text));
    
    // 克隆节点
    auto cloned = original_element->clone();
    auto cloned_element = dynamic_cast<ElementNode*>(cloned.get());
    
    assert(cloned_element != nullptr);
    assert(cloned_element->tag_name == "div");
    assert(cloned_element->is_self_closing == true);
    assert(cloned_element->getAttribute("test") == "value");
    assert(cloned_element->getChildCount() == 1);
    assert(cloned_element->getChild(0)->type == NodeType::TEXT);
    
    // 验证是深拷贝
    assert(cloned_element != original_element.get());
    assert(cloned_element->getChild(0) != original_element->getChild(0));
    
    std::cout << "节点克隆测试通过!" << std::endl;
}

void testUtilityFunctions() {
    std::cout << "=== 测试工具函数 ===" << std::endl;
    
    // 测试节点类型转换
    assert(nodeTypeToString(NodeType::PROGRAM) == "Program");
    assert(nodeTypeToString(NodeType::ELEMENT) == "Element");
    assert(nodeTypeToString(NodeType::CHTL_JS_SELECTOR) == "CHTLJSSelector");
    
    // 测试节点分类函数
    assert(isTemplateNode(NodeType::TEMPLATE_DEFINITION) == true);
    assert(isTemplateNode(NodeType::TEMPLATE_STYLE) == true);
    assert(isTemplateNode(NodeType::ELEMENT) == false);
    
    assert(isCustomNode(NodeType::CUSTOM_DEFINITION) == true);
    assert(isCustomNode(NodeType::CUSTOM_ELEMENT) == true);
    assert(isCustomNode(NodeType::TEMPLATE_STYLE) == false);
    
    assert(isCHTLJSNode(NodeType::CHTL_JS_SELECTOR) == true);
    assert(isCHTLJSNode(NodeType::CHTL_JS_LISTEN) == true);
    assert(isCHTLJSNode(NodeType::ELEMENT) == false);
    
    assert(isStyleNode(NodeType::STYLE_BLOCK) == true);
    assert(isStyleNode(NodeType::CSS_PROPERTY) == true);
    assert(isStyleNode(NodeType::SCRIPT_BLOCK) == false);
    
    std::cout << "工具函数测试通过!" << std::endl;
}

int main() {
    try {
        std::cout << "开始CHTL AST节点测试..." << std::endl;
        
        testBasicNodes();
        testCHTLNodes();
        testSpecializedNodes();
        testVisitorPattern();
        testCloning();
        testUtilityFunctions();
        
        std::cout << std::endl << "=== 所有AST测试通过! ===" << std::endl;
        std::cout << "第二个Todo (实现CHTL和CHTL JS的AST节点定义) 已完成!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}