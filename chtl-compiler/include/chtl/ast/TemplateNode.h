#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 模板节点基类
 * 表示 [Template] 声明
 */
class TemplateNode : public ASTNode {
public:
    enum TemplateType {
        STYLE,      // @Style
        ELEMENT,    // @Element
        VAR         // @Var
    };
    
    TemplateNode(TemplateType type, const std::string& name)
        : ASTNode(GetNodeType(type), name)
        , m_TemplateType(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 模板类型
    TemplateType GetTemplateType() const { return m_TemplateType; }
    
    // 模板名称
    const std::string& GetTemplateName() const { return m_Value; }
    
    // 继承的模板
    void AddInheritedTemplate(const std::string& templateName);
    const std::vector<std::string>& GetInheritedTemplates() const { return m_InheritedTemplates; }
    
protected:
    TemplateType m_TemplateType;
    std::vector<std::string> m_InheritedTemplates;
    
    static ASTNodeType GetNodeType(TemplateType type) {
        switch (type) {
            case STYLE: return ASTNodeType::TEMPLATE_STYLE;
            case ELEMENT: return ASTNodeType::TEMPLATE_ELEMENT;
            case VAR: return ASTNodeType::TEMPLATE_VAR;
            default: return ASTNodeType::TEMPLATE_STYLE;
        }
    }
};

/**
 * 样式组模板节点
 * [Template] @Style Name { }
 */
class TemplateStyleNode : public TemplateNode {
public:
    TemplateStyleNode(const std::string& name)
        : TemplateNode(TemplateType::STYLE, name) {}
    
    // 样式属性
    void AddProperty(const std::string& property, const std::string& value);
    const std::vector<std::pair<std::string, std::string>>& GetProperties() const {
        return m_Properties;
    }
    
    // 继承的样式引用
    void AddStyleReference(std::shared_ptr<StyleReferenceNode> reference);
    const std::vector<std::shared_ptr<StyleReferenceNode>>& GetStyleReferences() const {
        return m_StyleReferences;
    }
    
private:
    std::vector<std::pair<std::string, std::string>> m_Properties;
    std::vector<std::shared_ptr<StyleReferenceNode>> m_StyleReferences;
};

/**
 * 元素模板节点
 * [Template] @Element Name { }
 */
class TemplateElementNode : public TemplateNode {
public:
    TemplateElementNode(const std::string& name)
        : TemplateNode(TemplateType::ELEMENT, name) {}
    
    // 模板包含的元素
    void AddElement(std::shared_ptr<ElementNode> element);
    const std::vector<std::shared_ptr<ElementNode>>& GetElements() const {
        return m_Elements;
    }
    
    // 元素引用
    void AddElementReference(std::shared_ptr<class ElementReferenceNode> reference);
    const std::vector<std::shared_ptr<class ElementReferenceNode>>& GetElementReferences() const {
        return m_ElementReferences;
    }
    
private:
    std::vector<std::shared_ptr<ElementNode>> m_Elements;
    std::vector<std::shared_ptr<class ElementReferenceNode>> m_ElementReferences;
};

/**
 * 变量组模板节点
 * [Template] @Var Name { }
 */
class TemplateVarNode : public TemplateNode {
public:
    TemplateVarNode(const std::string& name)
        : TemplateNode(TemplateType::VAR, name) {}
    
    // 变量定义
    void AddVariable(const std::string& varName, const std::string& value);
    const std::vector<std::pair<std::string, std::string>>& GetVariables() const {
        return m_Variables;
    }
    
    // 获取特定变量的值
    std::string GetVariableValue(const std::string& varName) const;
    bool HasVariable(const std::string& varName) const;
    
private:
    std::vector<std::pair<std::string, std::string>> m_Variables;
};

} // namespace ast
} // namespace chtl