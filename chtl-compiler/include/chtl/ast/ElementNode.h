#pragma once

#include "chtl/ast/ASTNode.h"
#include <algorithm>

namespace chtl {
namespace ast {

/**
 * 元素节点
 * 表示HTML元素，如 div { }
 */
class ElementNode : public ASTNode {
public:
    ElementNode(const std::string& tagName)
        : ASTNode(ASTNodeType::ELEMENT, tagName) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 标签名
    const std::string& GetTagName() const { return m_Value; }
    
    // 属性管理
    void AddAttribute(std::shared_ptr<class AttributeNode> attribute);
    std::vector<std::shared_ptr<class AttributeNode>> GetAttributes() const;
    std::shared_ptr<class AttributeNode> GetAttribute(const std::string& name) const;
    bool HasAttribute(const std::string& name) const;
    
    // 样式块
    void SetStyleBlock(std::shared_ptr<class StyleBlockNode> styleBlock);
    std::shared_ptr<class StyleBlockNode> GetStyleBlock() const { return m_StyleBlock; }
    bool HasStyleBlock() const { return m_StyleBlock != nullptr; }
    
    // 脚本块
    void SetScriptBlock(std::shared_ptr<class ScriptBlockNode> scriptBlock);
    std::shared_ptr<class ScriptBlockNode> GetScriptBlock() const { return m_ScriptBlock; }
    bool HasScriptBlock() const { return m_ScriptBlock != nullptr; }
    
    // 获取所有子元素
    std::vector<std::shared_ptr<ElementNode>> GetChildElements() const;
    
    // 获取所有文本节点
    std::vector<std::shared_ptr<class TextNode>> GetTextNodes() const;
    
    // 是否是自闭合标签
    void SetSelfClosing(bool selfClosing) { m_SelfClosing = selfClosing; }
    bool IsSelfClosing() const { return m_SelfClosing; }
    
private:
    std::shared_ptr<class StyleBlockNode> m_StyleBlock;
    std::shared_ptr<class ScriptBlockNode> m_ScriptBlock;
    bool m_SelfClosing = false;
};

} // namespace ast
} // namespace chtl