#pragma once

#include "CHTLCommon.h"
#include "CHTLTokens.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <optional>

namespace chtl {

// AST节点基类
class CHTLASTNode {
public:
    CHTLASTNode(ASTNodeType type, size_t line = 0, size_t column = 0)
        : nodeType_(type), line_(line), column_(column) {}
    
    virtual ~CHTLASTNode() = default;
    
    // 基础信息
    ASTNodeType getType() const { return nodeType_; }
    size_t getLine() const { return line_; }
    size_t getColumn() const { return column_; }
    
    // 子节点管理
    void addChild(std::shared_ptr<CHTLASTNode> child);
    const std::vector<std::shared_ptr<CHTLASTNode>>& getChildren() const { return children_; }
    
    // 属性管理
    void setAttribute(const String& key, const String& value);
    std::optional<String> getAttribute(const String& key) const;
    const StringMap& getAttributes() const { return attributes_; }
    
    // 访问者模式支持
    virtual void accept(class CHTLASTVisitor& visitor) = 0;
    
    // 转换为字符串
    virtual String toString() const;

protected:
    ASTNodeType nodeType_;
    size_t line_;
    size_t column_;
    std::vector<std::shared_ptr<CHTLASTNode>> children_;
    StringMap attributes_;
};

// 根节点
class RootNode : public CHTLASTNode {
public:
    RootNode() : CHTLASTNode(ASTNodeType::ROOT) {}
    void accept(class CHTLASTVisitor& visitor) override;
};

// 元素节点
class ElementNode : public CHTLASTNode {
public:
    ElementNode(const String& tagName, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::ELEMENT, line, column), tagName_(tagName) {}
    
    const String& getTagName() const { return tagName_; }
    void setTagName(const String& tagName) { tagName_ = tagName; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String tagName_;
};

// 文本节点
class TextNode : public CHTLASTNode {
public:
    TextNode(const String& content, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::TEXT_NODE, line, column), content_(content) {}
    
    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String content_;
};

// 属性节点
class AttributeNode : public CHTLASTNode {
public:
    AttributeNode(const String& name, const String& value, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::ATTRIBUTE, line, column), name_(name), value_(value) {}
    
    const String& getName() const { return name_; }
    const String& getValue() const { return value_; }
    void setName(const String& name) { name_ = name; }
    void setValue(const String& value) { value_ = value; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    String value_;
};

// 样式块节点
class StyleBlockNode : public CHTLASTNode {
public:
    StyleBlockNode(bool isLocal = true, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::STYLE_BLOCK, line, column), isLocal_(isLocal) {}
    
    bool isLocal() const { return isLocal_; }
    void setLocal(bool local) { isLocal_ = local; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    bool isLocal_; // true表示局部样式块，false表示全局样式块
};

// CSS规则节点
class CSSRuleNode : public CHTLASTNode {
public:
    CSSRuleNode(const String& selector, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::CSS_RULE, line, column), selector_(selector) {}
    
    const String& getSelector() const { return selector_; }
    void setSelector(const String& selector) { selector_ = selector; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String selector_;
};

// CSS属性节点
class CSSPropertyNode : public CHTLASTNode {
public:
    CSSPropertyNode(const String& property, const String& value, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::CSS_PROPERTY, line, column), property_(property), value_(value) {}
    
    const String& getProperty() const { return property_; }
    const String& getValue() const { return value_; }
    void setProperty(const String& property) { property_ = property; }
    void setValue(const String& value) { value_ = value; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String property_;
    String value_;
};

// 脚本块节点
class ScriptBlockNode : public CHTLASTNode {
public:
    ScriptBlockNode(bool isLocal = true, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::SCRIPT_BLOCK, line, column), isLocal_(isLocal) {}
    
    bool isLocal() const { return isLocal_; }
    void setLocal(bool local) { isLocal_ = local; }
    
    // 内容管理
    const String& getContent() const { return content_; }
    void setContent(const String& content) { content_ = content; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    bool isLocal_; // true表示局部脚本块，false表示全局脚本块
    String content_; // 脚本内容
};

// 模板样式节点
class TemplateStyleNode : public CHTLASTNode {
public:
    TemplateStyleNode(const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::TEMPLATE_STYLE, line, column), name_(name) {}
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    // 继承相关
    void addInheritance(const String& templateName) { inheritedTemplates_.push_back(templateName); }
    const StringList& getInheritedTemplates() const { return inheritedTemplates_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    StringList inheritedTemplates_;
};

// 模板元素节点
class TemplateElementNode : public CHTLASTNode {
public:
    TemplateElementNode(const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::TEMPLATE_ELEMENT, line, column), name_(name) {}
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    void addInheritance(const String& templateName) { inheritedTemplates_.push_back(templateName); }
    const StringList& getInheritedTemplates() const { return inheritedTemplates_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    StringList inheritedTemplates_;
};

// 模板变量节点
class TemplateVarNode : public CHTLASTNode {
public:
    TemplateVarNode(const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::TEMPLATE_VAR, line, column), name_(name) {}
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    // 变量定义
    void addVariable(const String& varName, const String& value) { variables_[varName] = value; }
    std::optional<String> getVariable(const String& varName) const;
    const StringMap& getVariables() const { return variables_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    StringMap variables_;
};

// 模板使用节点
class TemplateUsageNode : public CHTLASTNode {
public:
    TemplateUsageNode(const String& type, const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column), type_(type), name_(name) {}
    
    const String& getType() const { return type_; }
    const String& getName() const { return name_; }
    void setType(const String& type) { type_ = type; }
    void setName(const String& name) { name_ = name; }
    
    // 特例化参数
    void addSpecialization(const String& key, const String& value) { specializations_[key] = value; }
    const StringMap& getSpecializations() const { return specializations_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String type_; // @Style, @Element, @Var
    String name_; // 模板名称
    StringMap specializations_; // 特例化参数
};

// 变量引用节点
class VariableReferenceNode : public CHTLASTNode {
public:
    VariableReferenceNode(const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column), name_(name) {}
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    // 变量组引用 ThemeColor(tableColor)
    const String& getGroupName() const { return groupName_; }
    const String& getVariableName() const { return variableName_; }
    void setGroupReference(const String& group, const String& variable) {
        groupName_ = group;
        variableName_ = variable;
    }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    String groupName_;    // 变量组名称
    String variableName_; // 组内变量名称
};

// 自定义样式节点
class CustomStyleNode : public CHTLASTNode {
public:
    CustomStyleNode(const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::CUSTOM_STYLE, line, column), name_(name) {}
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    // 继承和特例化
    void addInheritance(const String& templateName) { inheritedTemplates_.push_back(templateName); }
    const StringList& getInheritedTemplates() const { return inheritedTemplates_; }
    
    void addDeletedProperty(const String& property) { deletedProperties_.push_back(property); }
    const StringList& getDeletedProperties() const { return deletedProperties_; }
    
    void addDeletedInheritance(const String& templateName) { deletedInheritances_.push_back(templateName); }
    const StringList& getDeletedInheritances() const { return deletedInheritances_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    StringList inheritedTemplates_;
    StringList deletedProperties_;
    StringList deletedInheritances_;
};

// 自定义元素节点
class CustomElementNode : public CHTLASTNode {
public:
    CustomElementNode(const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::CUSTOM_ELEMENT, line, column), name_(name) {}
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    void addInheritance(const String& templateName) { inheritedTemplates_.push_back(templateName); }
    const StringList& getInheritedTemplates() const { return inheritedTemplates_; }
    
    void addDeletedElement(const String& elementSelector) { deletedElements_.push_back(elementSelector); }
    const StringList& getDeletedElements() const { return deletedElements_; }
    
    void addDeletedInheritance(const String& templateName) { deletedInheritances_.push_back(templateName); }
    const StringList& getDeletedInheritances() const { return deletedInheritances_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    StringList inheritedTemplates_;
    StringList deletedElements_;
    StringList deletedInheritances_;
};

// 自定义变量节点
class CustomVarNode : public CHTLASTNode {
public:
    CustomVarNode(const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::CUSTOM_VAR, line, column), name_(name) {}
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    void addVariable(const String& varName, const String& value) { variables_[varName] = value; }
    std::optional<String> getVariable(const String& varName) const;
    const StringMap& getVariables() const { return variables_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    StringMap variables_;
};

// 变量节点
class VariableNode : public CHTLASTNode {
public:
    VariableNode(const String& name, const String& value, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column), name_(name), value_(value) {}
    
    const String& getName() const { return name_; }
    const String& getValue() const { return value_; }
    void setName(const String& name) { name_ = name; }
    void setValue(const String& value) { value_ = value; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    String value_;
};

// 删除节点 (delete语法)
class DeleteNode : public CHTLASTNode {
public:
    DeleteNode(const String& target, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column), target_(target) {}
    
    const String& getTarget() const { return target_; }
    void setTarget(const String& target) { target_ = target; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String target_;
};

// 继承节点 (inherit语法)
class InheritNode : public CHTLASTNode {
public:
    InheritNode(const String& target, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column), target_(target) {}
    
    const String& getTarget() const { return target_; }
    void setTarget(const String& target) { target_ = target; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String target_;
};

// 异常节点 (except语法)
class ExceptNode : public CHTLASTNode {
public:
    ExceptNode(const String& target, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column), target_(target) {}
    
    const String& getTarget() const { return target_; }
    void setTarget(const String& target) { target_ = target; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String target_;
};

// 类选择器节点
class ClassSelectorNode : public CHTLASTNode {
public:
    ClassSelectorNode(const String& className, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::CSS_RULE, line, column), className_(className) {}
    
    const String& getClassName() const { return className_; }
    void setClassName(const String& className) { className_ = className; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String className_;
};

// ID选择器节点
class IdSelectorNode : public CHTLASTNode {
public:
    IdSelectorNode(const String& idName, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::CSS_RULE, line, column), idName_(idName) {}
    
    const String& getIdName() const { return idName_; }
    void setIdName(const String& idName) { idName_ = idName; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String idName_;
};

// 伪选择器节点
class PseudoSelectorNode : public CHTLASTNode {
public:
    PseudoSelectorNode(const String& selector, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::CSS_RULE, line, column), selector_(selector) {}
    
    const String& getSelector() const { return selector_; }
    void setSelector(const String& selector) { selector_ = selector; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String selector_;
};

// 样式属性节点 (兼容StylePropertyNode)
using StylePropertyNode = CSSPropertyNode;

// 插入操作节点 (用于自定义元素的insert操作)
class InsertNode : public CHTLASTNode {
public:
    enum class InsertPosition {
        AFTER,      // after
        BEFORE,     // before
        REPLACE,    // replace
        AT_TOP,     // at top
        AT_BOTTOM   // at bottom
    };
    
    InsertNode(InsertPosition position, const String& selector, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column), position_(position), selector_(selector) {}
    
    InsertPosition getPosition() const { return position_; }
    const String& getSelector() const { return selector_; }
    void setPosition(InsertPosition position) { position_ = position; }
    void setSelector(const String& selector) { selector_ = selector; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    InsertPosition position_;
    String selector_;
};

// 特例化使用节点 (用于自定义系统的特例化操作)
class SpecializationNode : public CHTLASTNode {
public:
    SpecializationNode(const String& type, const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column), type_(type), name_(name) {}
    
    const String& getType() const { return type_; }
    const String& getName() const { return name_; }
    void setType(const String& type) { type_ = type; }
    void setName(const String& name) { name_ = name; }
    
    // 删除操作
    void addDeletedProperty(const String& property) { deletedProperties_.push_back(property); }
    const StringList& getDeletedProperties() const { return deletedProperties_; }
    
    void addDeletedInheritance(const String& inheritance) { deletedInheritances_.push_back(inheritance); }
    const StringList& getDeletedInheritances() const { return deletedInheritances_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String type_; // @Style, @Element, @Var
    String name_; // 模板或自定义名称
    StringList deletedProperties_;    // 删除的属性列表
    StringList deletedInheritances_;  // 删除的继承列表
};

// 导入节点
class ImportNode : public CHTLASTNode {
public:
    enum class ImportType {
        HTML,           // @Html
        STYLE,          // @Style  
        JAVASCRIPT,     // @JavaScript
        CHTL,           // @Chtl
        CJMOD,          // @CJmod
        CUSTOM_STYLE,   // [Custom] @Style
        CUSTOM_ELEMENT, // [Custom] @Element
        CUSTOM_VAR,     // [Custom] @Var
        TEMPLATE_STYLE, // [Template] @Style
        TEMPLATE_ELEMENT, // [Template] @Element
        TEMPLATE_VAR    // [Template] @Var
    };
    
    ImportNode(ImportType type, const String& path, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::IMPORT_STATEMENT, line, column), importType_(type), path_(path) {}
    
    ImportType getImportType() const { return importType_; }
    const String& getPath() const { return path_; }
    const String& getAlias() const { return alias_; }
    const String& getItemName() const { return itemName_; }
    
    void setAlias(const String& alias) { alias_ = alias; }
    void setItemName(const String& itemName) { itemName_ = itemName; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    ImportType importType_;
    String path_;
    String alias_;      // as 别名
    String itemName_;   // 具体导入的项目名称
};

// 命名空间节点
class NamespaceNode : public CHTLASTNode {
public:
    NamespaceNode(const String& name, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::NAMESPACE_DECLARATION, line, column), name_(name) {}
    
    const String& getName() const { return name_; }
    void setName(const String& name) { name_ = name; }
    
    // 嵌套命名空间支持
    void addNestedNamespace(std::shared_ptr<NamespaceNode> ns) { nestedNamespaces_.push_back(ns); }
    const std::vector<std::shared_ptr<NamespaceNode>>& getNestedNamespaces() const { return nestedNamespaces_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    String name_;
    std::vector<std::shared_ptr<NamespaceNode>> nestedNamespaces_;
};

// 原始嵌入节点
class OriginNode : public CHTLASTNode {
public:
    enum class OriginType {
        HTML,       // @Html
        STYLE,      // @Style
        JAVASCRIPT, // @JavaScript
        CUSTOM      // 自定义类型如@Vue
    };
    
    OriginNode(OriginType type, const String& content, size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column), originType_(type), content_(content) {}
    
    OriginType getOriginType() const { return originType_; }
    const String& getContent() const { return content_; }
    const String& getName() const { return name_; }
    const String& getCustomTypeName() const { return customTypeName_; }
    
    void setContent(const String& content) { content_ = content; }
    void setName(const String& name) { name_ = name; }
    void setCustomTypeName(const String& typeName) { customTypeName_ = typeName; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    OriginType originType_;
    String content_;
    String name_;           // 带名原始嵌入的名称
    String customTypeName_; // 自定义类型名称(如Vue)
};

// 约束节点 (except语法)
class ConstraintNode : public CHTLASTNode {
public:
    ConstraintNode(size_t line = 0, size_t column = 0)
        : CHTLASTNode(ASTNodeType::UNKNOWN_NODE, line, column) {}
    
    void addConstraint(const String& constraint) { constraints_.push_back(constraint); }
    const StringList& getConstraints() const { return constraints_; }
    
    void accept(class CHTLASTVisitor& visitor) override;

private:
    StringList constraints_;
};

// 访问者模式接口
class CHTLASTVisitor {
public:
    virtual ~CHTLASTVisitor() = default;
    
    virtual void visit(RootNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(AttributeNode& node) = 0;
    virtual void visit(StyleBlockNode& node) = 0;
    virtual void visit(CSSRuleNode& node) = 0;
    virtual void visit(CSSPropertyNode& node) = 0;
    virtual void visit(ScriptBlockNode& node) = 0;
    virtual void visit(TemplateStyleNode& node) = 0;
    virtual void visit(TemplateElementNode& node) = 0;
    virtual void visit(TemplateVarNode& node) = 0;
    virtual void visit(TemplateUsageNode& node) = 0;
    virtual void visit(VariableReferenceNode& node) = 0;
    virtual void visit(CustomStyleNode& node) = 0;
    virtual void visit(CustomElementNode& node) = 0;
    virtual void visit(CustomVarNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
    virtual void visit(NamespaceNode& node) = 0;
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ConstraintNode& node) = 0;
};

} // namespace chtl