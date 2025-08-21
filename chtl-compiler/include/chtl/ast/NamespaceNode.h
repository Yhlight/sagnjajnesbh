#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 命名空间节点
 * 表示 [Namespace] 声明
 */
class NamespaceNode : public ASTNode {
public:
    NamespaceNode(const std::string& name = "")
        : ASTNode(ASTNodeType::NAMESPACE, name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 命名空间名称
    const std::string& GetNamespaceName() const { return m_Value; }
    
    // 是否是嵌套命名空间
    void SetIsNested(bool nested) { m_IsNested = nested; }
    bool IsNested() const { return m_IsNested; }
    
    // 父命名空间（用于嵌套）
    void SetParentNamespace(const std::string& parent) { m_ParentNamespace = parent; }
    const std::string& GetParentNamespace() const { return m_ParentNamespace; }
    
    // 获取完全限定名
    std::string GetFullyQualifiedName() const;
    
    // 命名空间内的内容通过子节点管理
    
private:
    bool m_IsNested = false;
    std::string m_ParentNamespace;
};

/**
 * from 子句节点
 * 用于 from namespace 语法
 */
class FromClauseNode : public ASTNode {
public:
    FromClauseNode(const std::string& namespaceName)
        : ASTNode(ASTNodeType::FROM_CLAUSE, namespaceName) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 命名空间名称
    const std::string& GetNamespaceName() const { return m_Value; }
};

} // namespace ast
} // namespace chtl