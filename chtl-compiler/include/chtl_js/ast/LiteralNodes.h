#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * 字面量节点基类
 */
class LiteralNode : public CHTLJSASTNode {
public:
    LiteralNode(CHTLJSNodeType type, const std::string& value)
        : CHTLJSASTNode(type, value) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
};

/**
 * 标识符节点
 * 表示变量名、函数名等标识符
 */
class IdentifierNode : public LiteralNode {
public:
    IdentifierNode(const std::string& name)
        : LiteralNode(CHTLJSNodeType::IDENTIFIER, name) {}
    
    const std::string& GetName() const { return m_Value; }
};

/**
 * 字符串字面量节点
 */
class StringLiteralNode : public LiteralNode {
public:
    StringLiteralNode(const std::string& value, char quoteChar = '"')
        : LiteralNode(CHTLJSNodeType::STRING_LITERAL, value)
        , m_QuoteChar(quoteChar) {}
    
    char GetQuoteChar() const { return m_QuoteChar; }
    
private:
    char m_QuoteChar;
};

/**
 * 数字字面量节点
 */
class NumberLiteralNode : public LiteralNode {
public:
    NumberLiteralNode(double value)
        : LiteralNode(CHTLJSNodeType::NUMBER_LITERAL, std::to_string(value))
        , m_NumericValue(value) {}
    
    double GetNumericValue() const { return m_NumericValue; }
    
private:
    double m_NumericValue;
};

/**
 * 布尔字面量节点
 */
class BooleanLiteralNode : public LiteralNode {
public:
    BooleanLiteralNode(bool value)
        : LiteralNode(CHTLJSNodeType::BOOLEAN_LITERAL, value ? "true" : "false")
        , m_BoolValue(value) {}
    
    bool GetBoolValue() const { return m_BoolValue; }
    
private:
    bool m_BoolValue;
};

/**
 * 数组字面量节点
 * 表示 [item1, item2, ...]
 */
class ArrayLiteralNode : public CHTLJSASTNode {
public:
    ArrayLiteralNode()
        : CHTLJSASTNode(CHTLJSNodeType::ARRAY_LITERAL) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 添加元素
    void AddElement(std::shared_ptr<CHTLJSASTNode> element) {
        if (element) {
            m_Elements.push_back(element);
            AddChild(element);
        }
    }
    
    // 获取所有元素
    const std::vector<std::shared_ptr<CHTLJSASTNode>>& GetElements() const {
        return m_Elements;
    }
    
    size_t GetElementCount() const { return m_Elements.size(); }
    
private:
    std::vector<std::shared_ptr<CHTLJSASTNode>> m_Elements;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl