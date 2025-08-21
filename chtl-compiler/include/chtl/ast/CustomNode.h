#pragma once

#include "chtl/ast/ASTNode.h"
#include "chtl/ast/TemplateNode.h"

namespace chtl {
namespace ast {

/**
 * 自定义节点基类
 * 表示 [Custom] 声明
 */
class CustomNode : public ASTNode {
public:
    enum CustomType {
        STYLE,      // @Style
        ELEMENT,    // @Element
        VAR         // @Var
    };
    
    CustomNode(CustomType type, const std::string& name)
        : ASTNode(GetNodeType(type), name)
        , m_CustomType(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 自定义类型
    CustomType GetCustomType() const { return m_CustomType; }
    
    // 自定义名称
    const std::string& GetCustomName() const { return m_Value; }
    
    // 继承的模板或自定义
    void AddInherited(const std::string& name, bool isTemplate = false);
    const std::vector<std::pair<std::string, bool>>& GetInherited() const { return m_Inherited; }
    
protected:
    CustomType m_CustomType;
    std::vector<std::pair<std::string, bool>> m_Inherited; // name, isTemplate
    
    static ASTNodeType GetNodeType(CustomType type) {
        switch (type) {
            case STYLE: return ASTNodeType::CUSTOM_STYLE;
            case ELEMENT: return ASTNodeType::CUSTOM_ELEMENT;
            case VAR: return ASTNodeType::CUSTOM_VAR;
            default: return ASTNodeType::CUSTOM_STYLE;
        }
    }
};

/**
 * 自定义样式组节点
 * [Custom] @Style Name { }
 */
class CustomStyleNode : public CustomNode {
public:
    CustomStyleNode(const std::string& name)
        : CustomNode(CustomType::STYLE, name) {}
    
    // 无值属性（需要在使用时填充）
    void AddEmptyProperty(const std::string& property);
    const std::vector<std::string>& GetEmptyProperties() const { return m_EmptyProperties; }
    
    // 有值属性
    void AddProperty(const std::string& property, const std::string& value);
    const std::vector<std::pair<std::string, std::string>>& GetProperties() const {
        return m_Properties;
    }
    
    // 样式引用（带特化）
    void AddStyleReference(std::shared_ptr<StyleReferenceNode> reference);
    const std::vector<std::shared_ptr<StyleReferenceNode>>& GetStyleReferences() const {
        return m_StyleReferences;
    }
    
private:
    std::vector<std::string> m_EmptyProperties;
    std::vector<std::pair<std::string, std::string>> m_Properties;
    std::vector<std::shared_ptr<StyleReferenceNode>> m_StyleReferences;
};

/**
 * 自定义元素节点
 * [Custom] @Element Name { }
 */
class CustomElementNode : public CustomNode {
public:
    CustomElementNode(const std::string& name)
        : CustomNode(CustomType::ELEMENT, name) {}
    
    // 包含的元素
    void AddElement(std::shared_ptr<ElementNode> element);
    const std::vector<std::shared_ptr<ElementNode>>& GetElements() const {
        return m_Elements;
    }
    
    // 元素引用
    void AddElementReference(std::shared_ptr<class ElementReferenceNode> reference);
    const std::vector<std::shared_ptr<class ElementReferenceNode>>& GetElementReferences() const {
        return m_ElementReferences;
    }
    
    // 插入操作
    void AddInsertOperation(std::shared_ptr<class InsertNode> insert);
    const std::vector<std::shared_ptr<class InsertNode>>& GetInsertOperations() const {
        return m_InsertOperations;
    }
    
    // 删除操作
    void AddDeleteOperation(std::shared_ptr<class DeleteNode> deleteOp);
    const std::vector<std::shared_ptr<class DeleteNode>>& GetDeleteOperations() const {
        return m_DeleteOperations;
    }
    
private:
    std::vector<std::shared_ptr<ElementNode>> m_Elements;
    std::vector<std::shared_ptr<class ElementReferenceNode>> m_ElementReferences;
    std::vector<std::shared_ptr<class InsertNode>> m_InsertOperations;
    std::vector<std::shared_ptr<class DeleteNode>> m_DeleteOperations;
};

/**
 * 自定义变量组节点
 * [Custom] @Var Name { }
 */
class CustomVarNode : public CustomNode {
public:
    CustomVarNode(const std::string& name)
        : CustomNode(CustomType::VAR, name) {}
    
    // 变量定义
    void AddVariable(const std::string& varName, const std::string& value);
    const std::vector<std::pair<std::string, std::string>>& GetVariables() const {
        return m_Variables;
    }
    
    // 变量引用（带特化）
    void AddVarReference(std::shared_ptr<class VarReferenceNode> reference);
    const std::vector<std::shared_ptr<class VarReferenceNode>>& GetVarReferences() const {
        return m_VarReferences;
    }
    
private:
    std::vector<std::pair<std::string, std::string>> m_Variables;
    std::vector<std::shared_ptr<class VarReferenceNode>> m_VarReferences;
};

} // namespace ast
} // namespace chtl