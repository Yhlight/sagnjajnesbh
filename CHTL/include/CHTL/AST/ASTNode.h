#ifndef CHTL_AST_ASTNODE_H
#define CHTL_AST_ASTNODE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "CHTL/Token.h"

namespace CHTL {
namespace AST {

// AST节点类型
enum class NodeType {
    // 根节点
    Program,
    
    // 元素节点
    Element,
    Attribute,
    TextBlock,
    
    // 样式节点
    StyleBlock,
    StyleRule,
    StyleSelector,
    StyleProperty,
    
    // 脚本节点
    ScriptBlock,
    
    // 模板节点
    TemplateDeclaration,
    CustomDeclaration,
    
    // 特殊节点
    OriginBlock,
    ImportStatement,
    NamespaceDeclaration,
    ConfigurationBlock,
    
    // 表达式节点
    Identifier,
    StringLiteral,
    NumberLiteral,
    UnquotedLiteral,
    
    // 特殊语法节点
    Specialization,
    InsertBlock,
    DeleteBlock,
    ExceptBlock,
    InheritStatement,
    
    // 注释节点
    Comment,
    GeneratorComment
};

// 前向声明
class ASTVisitor;

// AST节点基类
class ASTNode {
public:
    ASTNode(NodeType type, const Token& token);
    virtual ~ASTNode() = default;
    
    // 节点类型
    NodeType GetType() const { return type_; }
    std::string GetTypeName() const;
    
    // 位置信息
    size_t GetLine() const { return line_; }
    size_t GetColumn() const { return column_; }
    const std::string& GetSourceFile() const { return sourceFile_; }
    void SetSourceFile(const std::string& file) { sourceFile_ = file; }
    
    // 子节点管理
    void AddChild(std::shared_ptr<ASTNode> child);
    void RemoveChild(std::shared_ptr<ASTNode> child);
    const std::vector<std::shared_ptr<ASTNode>>& GetChildren() const { return children_; }
    std::shared_ptr<ASTNode> GetChild(size_t index) const;
    size_t GetChildCount() const { return children_.size(); }
    
    // 父节点
    void SetParent(ASTNode* parent) { parent_ = parent; }
    ASTNode* GetParent() const { return parent_; }
    
    // 访问者模式
    virtual void Accept(ASTVisitor* visitor) = 0;
    
    // 调试
    virtual std::string ToString() const;
    virtual void Print(int indent = 0) const;
    
protected:
    NodeType type_;
    size_t line_;
    size_t column_;
    std::string sourceFile_;
    
    ASTNode* parent_ = nullptr;
    std::vector<std::shared_ptr<ASTNode>> children_;
    
    // 辅助方法
    std::string IndentString(int indent) const;
};

// 程序根节点
class ProgramNode : public ASTNode {
public:
    explicit ProgramNode(const Token& token);
    void Accept(ASTVisitor* visitor) override;
};

// 元素节点
class ElementNode : public ASTNode {
public:
    ElementNode(const std::string& tagName, const Token& token);
    
    const std::string& GetTagName() const { return tagName_; }
    
    // 属性管理
    void AddAttribute(const std::string& name, const std::string& value);
    std::string GetAttribute(const std::string& name) const;
    bool HasAttribute(const std::string& name) const;
    const std::unordered_map<std::string, std::string>& GetAttributes() const { return attributes_; }
    
    // 特殊属性
    std::string GetId() const { return GetAttribute("id"); }
    std::string GetClass() const { return GetAttribute("class"); }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string tagName_;
    std::unordered_map<std::string, std::string> attributes_;
};

// 属性节点
class AttributeNode : public ASTNode {
public:
    AttributeNode(const std::string& name, const std::string& value, const Token& token);
    
    const std::string& GetName() const { return name_; }
    const std::string& GetValue() const { return value_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string name_;
    std::string value_;
};

// 文本块节点
class TextBlockNode : public ASTNode {
public:
    explicit TextBlockNode(const Token& token);
    
    void SetContent(const std::string& content) { content_ = content; }
    const std::string& GetContent() const { return content_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string content_;
};

// 样式块节点
class StyleBlockNode : public ASTNode {
public:
    StyleBlockNode(bool isLocal, const Token& token);
    
    bool IsLocal() const { return isLocal_; }
    bool IsGlobal() const { return !isLocal_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    bool isLocal_;
};

// 脚本块节点
class ScriptBlockNode : public ASTNode {
public:
    ScriptBlockNode(bool isLocal, const Token& token);
    
    bool IsLocal() const { return isLocal_; }
    bool IsGlobal() const { return !isLocal_; }
    
    void SetContent(const std::string& content) { content_ = content; }
    const std::string& GetContent() const { return content_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    bool isLocal_;
    std::string content_;
};

// 标识符节点
class IdentifierNode : public ASTNode {
public:
    IdentifierNode(const std::string& name, const Token& token);
    
    const std::string& GetName() const { return name_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string name_;
};

// 字符串字面量节点
class StringLiteralNode : public ASTNode {
public:
    StringLiteralNode(const std::string& value, const Token& token);
    
    const std::string& GetValue() const { return value_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string value_;
};

// AST节点工具类
class ASTNodeUtils {
public:
    static std::string GetNodeTypeName(NodeType type);
    static bool IsExpressionNode(NodeType type);
    static bool IsStatementNode(NodeType type);
    static bool IsDeclarationNode(NodeType type);
    static bool CanHaveChildren(NodeType type);
};

} // namespace AST
} // namespace CHTL

#endif // CHTL_AST_ASTNODE_H