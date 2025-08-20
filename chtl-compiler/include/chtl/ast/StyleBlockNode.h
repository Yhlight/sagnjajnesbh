#pragma once

#include "chtl/ast/ASTNode.h"
#include <unordered_set>

namespace chtl {
namespace ast {

/**
 * 样式块节点
 * 表示 style { } 块
 */
class StyleBlockNode : public ASTNode {
public:
    StyleBlockNode() : ASTNode(ASTNodeType::STYLE_BLOCK) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 是否是局部样式块（在元素内部）
    void SetIsLocal(bool isLocal) { m_IsLocal = isLocal; }
    bool IsLocal() const { return m_IsLocal; }
    
    // 内联样式（直接写在style块中的属性）
    void AddInlineProperty(const std::string& property, const std::string& value);
    const std::vector<std::pair<std::string, std::string>>& GetInlineProperties() const { 
        return m_InlineProperties; 
    }
    
    // 样式规则（选择器和规则）
    void AddStyleRule(std::shared_ptr<class StyleRuleNode> rule);
    const std::vector<std::shared_ptr<class StyleRuleNode>>& GetStyleRules() const {
        return m_StyleRules;
    }
    
    // 模板引用（@Style）
    void AddStyleReference(std::shared_ptr<class StyleReferenceNode> reference);
    const std::vector<std::shared_ptr<class StyleReferenceNode>>& GetStyleReferences() const {
        return m_StyleReferences;
    }
    
    // 自动添加的类名和ID
    void AddAutoClass(const std::string& className) { m_AutoClasses.insert(className); }
    void AddAutoId(const std::string& id) { m_AutoIds.insert(id); }
    const std::unordered_set<std::string>& GetAutoClasses() const { return m_AutoClasses; }
    const std::unordered_set<std::string>& GetAutoIds() const { return m_AutoIds; }
    
private:
    bool m_IsLocal = true;
    std::vector<std::pair<std::string, std::string>> m_InlineProperties;
    std::vector<std::shared_ptr<class StyleRuleNode>> m_StyleRules;
    std::vector<std::shared_ptr<class StyleReferenceNode>> m_StyleReferences;
    std::unordered_set<std::string> m_AutoClasses;
    std::unordered_set<std::string> m_AutoIds;
};

/**
 * 样式规则节点
 * 表示选择器和对应的样式
 */
class StyleRuleNode : public ASTNode {
public:
    StyleRuleNode(const std::string& selector)
        : ASTNode(ASTNodeType::STYLE_RULE, selector) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 选择器
    const std::string& GetSelector() const { return m_Value; }
    
    // 选择器类型
    enum SelectorType {
        CLASS,      // .class
        ID,         // #id
        AMPERSAND,  // &
        TAG,        // 标签选择器
        COMPLEX     // 复杂选择器
    };
    
    void SetSelectorType(SelectorType type) { m_SelectorType = type; }
    SelectorType GetSelectorType() const { return m_SelectorType; }
    
    // 样式属性
    void AddProperty(const std::string& property, const std::string& value);
    const std::vector<std::pair<std::string, std::string>>& GetProperties() const {
        return m_Properties;
    }
    
private:
    SelectorType m_SelectorType = COMPLEX;
    std::vector<std::pair<std::string, std::string>> m_Properties;
};

/**
 * 样式引用节点
 * 表示 @Style 引用
 */
class StyleReferenceNode : public ASTNode {
public:
    StyleReferenceNode(const std::string& styleName)
        : ASTNode(ASTNodeType::STYLE_REFERENCE, styleName) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 引用的样式名
    const std::string& GetStyleName() const { return m_Value; }
    
    // 特化（覆盖的属性）
    void AddOverride(const std::string& property, const std::string& value);
    const std::vector<std::pair<std::string, std::string>>& GetOverrides() const {
        return m_Overrides;
    }
    
    // 删除的属性
    void AddDeletedProperty(const std::string& property) { m_DeletedProperties.insert(property); }
    const std::unordered_set<std::string>& GetDeletedProperties() const { return m_DeletedProperties; }
    
    // 是否删除整个样式继承
    void SetDeleteInheritance(bool deleteIt) { m_DeleteInheritance = deleteIt; }
    bool ShouldDeleteInheritance() const { return m_DeleteInheritance; }
    
private:
    std::vector<std::pair<std::string, std::string>> m_Overrides;
    std::unordered_set<std::string> m_DeletedProperties;
    bool m_DeleteInheritance = false;
};

} // namespace ast
} // namespace chtl