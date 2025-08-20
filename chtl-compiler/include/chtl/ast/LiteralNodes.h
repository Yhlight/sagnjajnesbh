#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 字面量节点基类
 */
class LiteralNode : public ASTNode {
public:
    LiteralNode(ASTNodeType type, const std::string& value)
        : ASTNode(type, value) {}
    
    void Accept(ASTVisitor* visitor) override;
};

/**
 * 字符串字面量节点
 * 表示 "string" 或 'string'
 */
class StringLiteralNode : public LiteralNode {
public:
    StringLiteralNode(const std::string& value, char quoteChar = '"')
        : LiteralNode(ASTNodeType::STRING_LITERAL, value)
        , m_QuoteChar(quoteChar) {}
    
    // 引号字符（" 或 '）
    char GetQuoteChar() const { return m_QuoteChar; }
    
private:
    char m_QuoteChar;
};

/**
 * 无引号字面量节点
 * 表示无引号的值
 */
class UnquotedLiteralNode : public LiteralNode {
public:
    UnquotedLiteralNode(const std::string& value)
        : LiteralNode(ASTNodeType::UNQUOTED_LITERAL, value) {}
};

/**
 * 数字字面量节点
 */
class NumberLiteralNode : public LiteralNode {
public:
    NumberLiteralNode(double value)
        : LiteralNode(ASTNodeType::NUMBER_LITERAL, std::to_string(value))
        , m_NumericValue(value) {}
    
    double GetNumericValue() const { return m_NumericValue; }
    
private:
    double m_NumericValue;
};

/**
 * 标识符节点
 */
class IdentifierNode : public ASTNode {
public:
    IdentifierNode(const std::string& name)
        : ASTNode(ASTNodeType::IDENTIFIER, name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 标识符名称
    const std::string& GetName() const { return m_Value; }
};

/**
 * 引用节点基类
 */
class ReferenceNode : public ASTNode {
public:
    ReferenceNode(ASTNodeType type, const std::string& refName)
        : ASTNode(type, refName) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 引用名称
    const std::string& GetReferenceName() const { return m_Value; }
    
    // 命名空间（用于 from 语法）
    void SetNamespace(const std::string& ns) { m_Namespace = ns; }
    const std::string& GetNamespace() const { return m_Namespace; }
    bool HasNamespace() const { return !m_Namespace.empty(); }
    
private:
    std::string m_Namespace;
};



} // namespace ast
} // namespace chtl