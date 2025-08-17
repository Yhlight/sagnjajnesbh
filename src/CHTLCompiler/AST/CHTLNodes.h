#pragma once
#include "../Lexer/CHTLToken.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace ast {

// 前向声明
class ASTVisitor;

/**
 * AST节点基类
 */
class ASTNode {
public:
    TokenPosition position;
    std::vector<std::unique_ptr<ASTNode>> children;
    
    explicit ASTNode(const TokenPosition& pos = TokenPosition()) : position(pos) {}
    virtual ~ASTNode() = default;
    
    // 添加子节点
    void addChild(std::unique_ptr<ASTNode> child) {
        children.push_back(std::move(child));
    }
    
    // 获取子节点
    ASTNode* getChild(size_t index) const {
        return index < children.size() ? children[index].get() : nullptr;
    }
    
    size_t getChildCount() const {
        return children.size();
    }
    
    // 访问者模式
    virtual void accept(ASTVisitor& visitor) = 0;
    
    // 调试
    virtual std::string toString() const = 0;
    virtual std::string getNodeType() const = 0;
};

/**
 * 程序节点（根节点）
 */
class ProgramNode : public ASTNode {
public:
    explicit ProgramNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Program"; }
    std::string getNodeType() const override { return "Program"; }
};

/**
 * 模板定义节点
 */
class TemplateNode : public ASTNode {
public:
    std::string name;
    std::string type; // Style, Element, Var等
    
    explicit TemplateNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Template[" + name + "]"; }
    std::string getNodeType() const override { return "Template"; }
};

/**
 * 自定义定义节点
 */
class CustomNode : public ASTNode {
public:
    std::string name;
    std::string type; // Style, Element, Var等
    
    explicit CustomNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Custom[" + name + "]"; }
    std::string getNodeType() const override { return "Custom"; }
};

/**
 * 元素节点
 */
class ElementNode : public ASTNode {
public:
    std::string tag;
    std::vector<std::pair<std::string, std::string>> attributes;
    
    explicit ElementNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Element[" + tag + "]"; }
    std::string getNodeType() const override { return "Element"; }
};

/**
 * 样式块节点
 */
class StyleBlockNode : public ASTNode {
public:
    bool is_global;
    
    explicit StyleBlockNode(bool global = false, const TokenPosition& pos = TokenPosition()) 
        : ASTNode(pos), is_global(global) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return is_global ? "GlobalStyle" : "LocalStyle"; }
    std::string getNodeType() const override { return "StyleBlock"; }
};

/**
 * 脚本块节点
 */
class ScriptBlockNode : public ASTNode {
public:
    bool is_global;
    
    explicit ScriptBlockNode(bool global = false, const TokenPosition& pos = TokenPosition()) 
        : ASTNode(pos), is_global(global) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return is_global ? "GlobalScript" : "LocalScript"; }
    std::string getNodeType() const override { return "ScriptBlock"; }
};

/**
 * 文本节点
 */
class TextNode : public ASTNode {
public:
    std::string content;
    
    explicit TextNode(const std::string& text = "", const TokenPosition& pos = TokenPosition()) 
        : ASTNode(pos), content(text) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Text[" + content + "]"; }
    std::string getNodeType() const override { return "Text"; }
};

/**
 * 导入节点
 */
class ImportNode : public ASTNode {
public:
    std::string type; // @Chtl, @Html, @Style, @JavaScript, @CJmod
    std::string path;
    std::string alias;
    
    explicit ImportNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Import[" + type + ":" + path + "]"; }
    std::string getNodeType() const override { return "Import"; }
};

/**
 * 命名空间节点
 */
class NamespaceNode : public ASTNode {
public:
    std::string name;
    
    explicit NamespaceNode(const std::string& ns_name = "", const TokenPosition& pos = TokenPosition()) 
        : ASTNode(pos), name(ns_name) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Namespace[" + name + "]"; }
    std::string getNodeType() const override { return "Namespace"; }
};

/**
 * 访问者接口
 */
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    virtual void visit(ProgramNode& node) = 0;
    virtual void visit(TemplateNode& node) = 0;
    virtual void visit(CustomNode& node) = 0;
    virtual void visit(ElementNode& node) = 0;
    virtual void visit(StyleBlockNode& node) = 0;
    virtual void visit(ScriptBlockNode& node) = 0;
    virtual void visit(TextNode& node) = 0;
    virtual void visit(ImportNode& node) = 0;
    virtual void visit(NamespaceNode& node) = 0;
};

} // namespace ast
} // namespace chtl