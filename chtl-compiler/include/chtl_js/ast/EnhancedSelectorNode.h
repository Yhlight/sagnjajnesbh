#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * 增强选择器节点
 * 表示 {{selector}} 语法
 */
class EnhancedSelectorNode : public CHTLJSASTNode {
public:
    enum SelectorType {
        AUTO,           // {{box}} - 自动判断tag/class/id
        CLASS,          // {{.box}}
        ID,             // {{#box}}
        TAG,            // {{button}}
        DESCENDANT,     // {{.box button}}
        INDEXED         // {{button[0]}}
    };
    
    EnhancedSelectorNode(const std::string& selector)
        : CHTLJSASTNode(CHTLJSNodeType::ENHANCED_SELECTOR, selector) {
        ParseSelectorType();
    }
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 选择器类型
    SelectorType GetSelectorType() const { return m_SelectorType; }
    
    // 选择器内容（去除{{}}）
    const std::string& GetSelector() const { return m_Value; }
    
    // 索引（对于索引选择器）
    void SetIndex(int index) { m_Index = index; }
    int GetIndex() const { return m_Index; }
    bool HasIndex() const { return m_Index >= 0; }
    
    // 是否是复合选择器（如 .box button）
    bool IsCompoundSelector() const { return m_IsCompound; }
    
    // 解析选择器的各个部分
    struct SelectorPart {
        std::string value;
        SelectorType type;
    };
    const std::vector<SelectorPart>& GetParts() const { return m_Parts; }
    
private:
    SelectorType m_SelectorType = AUTO;
    int m_Index = -1;
    bool m_IsCompound = false;
    std::vector<SelectorPart> m_Parts;
    
    void ParseSelectorType();
};

/**
 * 选择器访问节点
 * 表示 {{selector}}[index] 语法
 */
class SelectorAccessNode : public CHTLJSASTNode {
public:
    SelectorAccessNode()
        : CHTLJSASTNode(CHTLJSNodeType::SELECTOR_ACCESS) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 选择器节点
    void SetSelector(std::shared_ptr<EnhancedSelectorNode> selector) {
        m_Selector = selector;
        if (selector) {
            AddChild(selector);
        }
    }
    std::shared_ptr<EnhancedSelectorNode> GetSelector() const { return m_Selector; }
    
    // 索引
    void SetIndex(int index) { m_Index = index; }
    int GetIndex() const { return m_Index; }
    
private:
    std::shared_ptr<EnhancedSelectorNode> m_Selector;
    int m_Index = 0;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl