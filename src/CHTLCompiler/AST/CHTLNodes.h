#pragma once
#include "../Lexer/CHTLToken.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

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
    std::string customType; // 兼容字段
    
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
    std::string tagName; // 兼容字段
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
 * 原始嵌入节点
 */
class OriginNode : public ASTNode {
public:
    std::string type; // @Html, @Style, @JavaScript, @Vue等
    std::string originType; // 兼容字段
    std::string name; // 带名原始嵌入的名称
    std::string content; // 原始内容
    bool isNamed = false; // 是否为带名原始嵌入
    bool isUsage = false; // 是否为使用而非定义
    
    explicit OriginNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Origin[" + type + (name.empty() ? "" : ":" + name) + "]"; }
    std::string getNodeType() const override { return "Origin"; }
};

/**
 * 配置组节点
 */
class ConfigurationNode : public ASTNode {
public:
    std::string name; // 配置组名称，空字符串表示无名配置组
    std::unordered_map<std::string, std::string> basicConfig; // 基础配置项
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> sections; // 配置节
    
    explicit ConfigurationNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Configuration[" + (name.empty() ? "unnamed" : name) + "]"; }
    std::string getNodeType() const override { return "Configuration"; }
};

/**
 * 约束节点
 */
class ConstraintNode : public ASTNode {
public:
    std::string constraintType; // except
    std::string type; // 兼容字段
    std::vector<std::string> targets; // 约束目标
    bool isGlobal; // 是否为全局约束
    
    explicit ConstraintNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos), isGlobal(false) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Constraint[" + constraintType + "]"; }
    std::string getNodeType() const override { return "Constraint"; }
};

/**
 * 特例化操作节点
 */
class SpecializationNode : public ASTNode {
public:
    std::string operation; // inherit, delete, insert, replace
    std::string target; // 操作目标
    std::string position; // after, before, at top, at bottom
    std::string selector; // 选择器（用于insert）
    std::vector<std::unique_ptr<ast::ASTNode>> content; // 插入或替换的内容
    
    explicit SpecializationNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "Specialization[" + operation + ":" + target + "]"; }
    std::string getNodeType() const override { return "Specialization"; }
};

/**
 * 索引访问节点
 */
class IndexAccessNode : public ASTNode {
public:
    std::string elementType; // div, span等
    int index; // 索引值
    
    explicit IndexAccessNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos), index(0) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "IndexAccess[" + elementType + "[" + std::to_string(index) + "]]"; }
    std::string getNodeType() const override { return "IndexAccess"; }
};

/**
 * 变量组节点
 */
class VariableGroupNode : public ASTNode {
public:
    std::string name;
    std::string type; // Template, Custom
    std::unordered_map<std::string, std::string> variables;
    
    explicit VariableGroupNode(const TokenPosition& pos = TokenPosition()) : ASTNode(pos) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override { return "VariableGroup[" + type + ":" + name + "]"; }
    std::string getNodeType() const override { return "VariableGroup"; }
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
    virtual void visit(OriginNode& node) = 0;
    virtual void visit(ConfigurationNode& node) = 0;
    virtual void visit(ConstraintNode& node) = 0;
    virtual void visit(SpecializationNode& node) = 0;
    virtual void visit(IndexAccessNode& node) = 0;
    virtual void visit(VariableGroupNode& node) = 0;
};

} // namespace ast
} // namespace chtl