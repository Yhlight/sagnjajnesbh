#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 继承节点
 * 表示 inherit 语句
 */
class InheritNode : public ASTNode {
public:
    InheritNode() : ASTNode(ASTNodeType::INHERIT) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 继承类型（@Style, @Element, @Var）
    void SetInheritType(const std::string& type) { m_InheritType = type; }
    const std::string& GetInheritType() const { return m_InheritType; }
    
    // 继承的名称
    void SetInheritName(const std::string& name) { m_Value = name; }
    const std::string& GetInheritName() const { return m_Value; }
    
    // 是否是模板还是自定义
    void SetIsTemplate(bool isTemplate) { m_IsTemplate = isTemplate; }
    bool IsTemplate() const { return m_IsTemplate; }
    
private:
    std::string m_InheritType;
    bool m_IsTemplate = true;
};

/**
 * 删除节点
 * 表示 delete 语句
 */
class DeleteNode : public ASTNode {
public:
    DeleteNode() : ASTNode(ASTNodeType::DELETE) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    enum DeleteType {
        PROPERTY,       // 删除属性
        ELEMENT,        // 删除元素
        INHERITANCE,    // 删除继承
        STYLE_REF,      // 删除样式引用
        ELEMENT_REF     // 删除元素引用
    };
    
    void SetDeleteType(DeleteType type) { m_DeleteType = type; }
    DeleteType GetDeleteType() const { return m_DeleteType; }
    
    // 删除目标（属性名、元素选择器等）
    void AddTarget(const std::string& target) { m_Targets.push_back(target); }
    const std::vector<std::string>& GetTargets() const { return m_Targets; }
    
    // 删除的索引（用于元素）
    void SetIndex(int index) { m_Index = index; }
    int GetIndex() const { return m_Index; }
    bool HasIndex() const { return m_Index >= 0; }
    
private:
    DeleteType m_DeleteType = PROPERTY;
    std::vector<std::string> m_Targets;
    int m_Index = -1;
};

/**
 * 插入节点
 * 表示 insert 语句
 */
class InsertNode : public ASTNode {
public:
    InsertNode() : ASTNode(ASTNodeType::INSERT) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    enum InsertPosition {
        AFTER,      // after
        BEFORE,     // before
        REPLACE,    // replace
        AT_TOP,     // at top
        AT_BOTTOM   // at bottom
    };
    
    void SetPosition(InsertPosition pos) { m_Position = pos; }
    InsertPosition GetPosition() const { return m_Position; }
    
    // 插入位置的选择器（如 div[0]）
    void SetTargetSelector(const std::string& selector) { m_TargetSelector = selector; }
    const std::string& GetTargetSelector() const { return m_TargetSelector; }
    
    // 插入的内容（元素或引用）
    void AddContent(std::shared_ptr<ASTNode> content) { m_Contents.push_back(content); }
    const std::vector<std::shared_ptr<ASTNode>>& GetContents() const { return m_Contents; }
    
private:
    InsertPosition m_Position = AFTER;
    std::string m_TargetSelector;
    std::vector<std::shared_ptr<ASTNode>> m_Contents;
};

/**
 * 元素引用节点
 * 表示 @Element 引用
 */
class ElementReferenceNode : public ASTNode {
public:
    ElementReferenceNode(const std::string& elementName)
        : ASTNode(ASTNodeType::ELEMENT_REFERENCE, elementName) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 引用的元素名
    const std::string& GetElementName() const { return m_Value; }
    
    // 特化（修改子元素）
    void AddSpecialization(const std::string& selector, std::shared_ptr<ASTNode> spec);
    const std::vector<std::pair<std::string, std::shared_ptr<ASTNode>>>& GetSpecializations() const {
        return m_Specializations;
    }
    
    // 删除操作
    void AddDeleteOperation(std::shared_ptr<DeleteNode> deleteOp);
    const std::vector<std::shared_ptr<DeleteNode>>& GetDeleteOperations() const {
        return m_DeleteOperations;
    }
    
    // 插入操作
    void AddInsertOperation(std::shared_ptr<InsertNode> insert);
    const std::vector<std::shared_ptr<InsertNode>>& GetInsertOperations() const {
        return m_InsertOperations;
    }
    
private:
    std::vector<std::pair<std::string, std::shared_ptr<ASTNode>>> m_Specializations;
    std::vector<std::shared_ptr<DeleteNode>> m_DeleteOperations;
    std::vector<std::shared_ptr<InsertNode>> m_InsertOperations;
};

/**
 * 变量引用节点
 * 表示变量组引用和调用
 */
class VarReferenceNode : public ASTNode {
public:
    VarReferenceNode(const std::string& varGroupName)
        : ASTNode(ASTNodeType::VAR_REFERENCE, varGroupName) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 变量组名
    const std::string& GetVarGroupName() const { return m_Value; }
    
    // 变量调用（如 ThemeColor(tableColor)）
    void SetVarCall(const std::string& varName) { m_VarCall = varName; }
    const std::string& GetVarCall() const { return m_VarCall; }
    bool IsVarCall() const { return !m_VarCall.empty(); }
    
    // 变量特化（如 tableColor = rgb(145, 155, 200)）
    void AddSpecialization(const std::string& varName, const std::string& value);
    const std::vector<std::pair<std::string, std::string>>& GetSpecializations() const {
        return m_Specializations;
    }
    
private:
    std::string m_VarCall;
    std::vector<std::pair<std::string, std::string>> m_Specializations;
};

/**
 * 索引访问节点
 * 表示 element[index] 访问
 */
class IndexAccessNode : public ASTNode {
public:
    IndexAccessNode(const std::string& element, int index)
        : ASTNode(ASTNodeType::INDEX_ACCESS, element)
        , m_Index(index) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 元素名或选择器
    const std::string& GetElement() const { return m_Value; }
    
    // 索引
    int GetIndex() const { return m_Index; }
    
private:
    int m_Index;
};

} // namespace ast
} // namespace chtl