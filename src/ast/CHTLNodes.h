#pragma once
#include "ASTNode.h"

namespace chtl {
namespace ast {

// 程序根节点
class ProgramNode : public ASTNode {
public:
    ProgramNode(const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::PROGRAM, pos) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitProgram(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<ProgramNode>(position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
};

// HTML元素节点
class ElementNode : public ASTNode {
public:
    std::string tag_name;           // 标签名
    bool is_self_closing;           // 是否自闭合标签
    
    ElementNode(const std::string& tag, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::ELEMENT, pos), tag_name(tag), is_self_closing(false) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitElement(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<ElementNode>(tag_name, position);
        cloned->is_self_closing = is_self_closing;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "Element<" + tag_name + "> " + ASTNode::toString();
    }
};

// 文本节点
class TextNode : public ASTNode {
public:
    std::string content;            // 文本内容
    
    TextNode(const std::string& text, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::TEXT, pos), content(text) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitText(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<TextNode>(content, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "Text(\"" + content + "\") " + ASTNode::toString();
    }
};

// 属性节点
class AttributeNode : public ASTNode {
public:
    std::string name;               // 属性名
    std::string value;              // 属性值
    bool is_quoted;                 // 值是否有引号
    
    AttributeNode(const std::string& attr_name, const std::string& attr_value = "",
                  const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::ATTRIBUTE, pos), name(attr_name), value(attr_value), is_quoted(false) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitAttribute(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<AttributeNode>(name, value, position);
        cloned->is_quoted = is_quoted;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "Attribute(" + name + "=" + value + ") " + ASTNode::toString();
    }
};

// 样式块节点
class StyleBlockNode : public ASTNode {
public:
    bool is_local;                  // 是否为局部样式块
    
    StyleBlockNode(bool local = true, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::STYLE_BLOCK, pos), is_local(local) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitStyleBlock(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<StyleBlockNode>(is_local, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return (is_local ? "LocalStyleBlock " : "GlobalStyleBlock ") + ASTNode::toString();
    }
};

// 脚本块节点
class ScriptBlockNode : public ASTNode {
public:
    bool is_local;                  // 是否为局部脚本块
    bool contains_chtl_js;          // 是否包含CHTL JS语法
    
    ScriptBlockNode(bool local = true, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::SCRIPT_BLOCK, pos), is_local(local), contains_chtl_js(false) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitScriptBlock(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<ScriptBlockNode>(is_local, position);
        cloned->contains_chtl_js = contains_chtl_js;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return (is_local ? "LocalScriptBlock " : "GlobalScriptBlock ") + ASTNode::toString();
    }
};

// 模板定义节点
class TemplateDefinitionNode : public ASTNode {
public:
    std::string template_type;      // 模板类型 (@Style, @Element, @Var)
    std::string template_name;      // 模板名称
    
    TemplateDefinitionNode(const std::string& type, const std::string& name,
                          const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::TEMPLATE_DEFINITION, pos), template_type(type), template_name(name) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitTemplateDefinition(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<TemplateDefinitionNode>(template_type, template_name, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "Template[" + template_type + " " + template_name + "] " + ASTNode::toString();
    }
};

// 自定义定义节点
class CustomDefinitionNode : public ASTNode {
public:
    std::string custom_type;        // 自定义类型 (@Style, @Element, @Var)
    std::string custom_name;        // 自定义名称
    
    CustomDefinitionNode(const std::string& type, const std::string& name,
                        const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::CUSTOM_DEFINITION, pos), custom_type(type), custom_name(name) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitCustomDefinition(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<CustomDefinitionNode>(custom_type, custom_name, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "Custom[" + custom_type + " " + custom_name + "] " + ASTNode::toString();
    }
};

// 原始嵌入块节点
class OriginBlockNode : public ASTNode {
public:
    std::string origin_type;        // 原始类型 (@Html, @Style, @JavaScript, 或自定义)
    std::string block_name;         // 块名称（可选）
    std::string raw_content;        // 原始内容
    
    OriginBlockNode(const std::string& type, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::ORIGIN_BLOCK, pos), origin_type(type) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitOriginBlock(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<OriginBlockNode>(origin_type, position);
        cloned->block_name = block_name;
        cloned->raw_content = raw_content;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "Origin[" + origin_type + 
               (block_name.empty() ? "" : " " + block_name) + "] " + ASTNode::toString();
    }
};

// 导入语句节点
class ImportStatementNode : public ASTNode {
public:
    std::string import_type;        // 导入类型
    std::string import_target;      // 导入目标
    std::string source_path;        // 源路径
    std::string alias;              // 别名（可选）
    
    ImportStatementNode(const std::string& type, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::IMPORT_STATEMENT, pos), import_type(type) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitImportStatement(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<ImportStatementNode>(import_type, position);
        cloned->import_target = import_target;
        cloned->source_path = source_path;
        cloned->alias = alias;
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "Import[" + import_type + " " + import_target + " from " + source_path + 
               (alias.empty() ? "" : " as " + alias) + "] " + ASTNode::toString();
    }
};

// 命名空间定义节点
class NamespaceDefinitionNode : public ASTNode {
public:
    std::string namespace_name;     // 命名空间名称
    
    NamespaceDefinitionNode(const std::string& name, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::NAMESPACE_DEFINITION, pos), namespace_name(name) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitNamespaceDefinition(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<NamespaceDefinitionNode>(namespace_name, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "Namespace[" + namespace_name + "] " + ASTNode::toString();
    }
};

// CHTL JS选择器节点
class CHTLJSSelectorNode : public ASTNode {
public:
    std::string selector_content;   // 选择器内容 ({{...}} 中间的部分)
    
    CHTLJSSelectorNode(const std::string& content, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::CHTL_JS_SELECTOR, pos), selector_content(content) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitCHTLJSSelector(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<CHTLJSSelectorNode>(selector_content, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "CHTLJSSelector({{" + selector_content + "}}) " + ASTNode::toString();
    }
};

// 注释节点
class CommentNode : public ASTNode {
public:
    std::string comment_content;    // 注释内容
    
    CommentNode(NodeType comment_type, const std::string& content, 
               const TokenPosition& pos = TokenPosition())
        : ASTNode(comment_type, pos), comment_content(content) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitComment(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<CommentNode>(type, comment_content, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return getNodeTypeName() + "(\"" + comment_content + "\") " + ASTNode::toString();
    }
};

// 字面量节点
class LiteralNode : public ASTNode {
public:
    std::string literal_value;      // 字面量值
    
    LiteralNode(NodeType literal_type, const std::string& value,
               const TokenPosition& pos = TokenPosition())
        : ASTNode(literal_type, pos), literal_value(value) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitLiteral(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<LiteralNode>(type, literal_value, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return getNodeTypeName() + "(\"" + literal_value + "\") " + ASTNode::toString();
    }
};

// 标识符节点
class IdentifierNode : public ASTNode {
public:
    std::string identifier_name;    // 标识符名称
    
    IdentifierNode(const std::string& name, const TokenPosition& pos = TokenPosition())
        : ASTNode(NodeType::IDENTIFIER, pos), identifier_name(name) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitIdentifier(*this);
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto cloned = std::make_unique<IdentifierNode>(identifier_name, position);
        cloneBaseProperties(cloned.get());
        return std::move(cloned);
    }
    
    std::string toString() const override {
        return "Identifier(" + identifier_name + ") " + ASTNode::toString();
    }
};

} // namespace ast
} // namespace chtl