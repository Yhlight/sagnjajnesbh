#pragma once

#include "chtl/ast/ASTNode.h"
#include <unordered_set>

namespace chtl {
namespace ast {

/**
 * 约束节点
 * 表示 except 语句
 */
class ExceptNode : public ASTNode {
public:
    ExceptNode() : ASTNode(ASTNodeType::EXCEPT) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    enum ConstraintType {
        ELEMENT,        // 元素约束（如 span）
        TYPE,           // 类型约束（如 @Html, [Custom]）
        SPECIFIC,       // 特定约束（如 [Custom] @Element Box）
        GLOBAL          // 全局约束（在命名空间内）
    };
    
    void SetConstraintType(ConstraintType type) { m_ConstraintType = type; }
    ConstraintType GetConstraintType() const { return m_ConstraintType; }
    
    // 约束目标
    void AddTarget(const std::string& target) { m_Targets.insert(target); }
    const std::unordered_set<std::string>& GetTargets() const { return m_Targets; }
    
    // 是否是全局约束
    void SetIsGlobal(bool global) { m_IsGlobal = global; }
    bool IsGlobal() const { return m_IsGlobal; }
    
    // 约束的具体类型信息
    struct ConstraintInfo {
        bool isTemplate = false;
        bool isCustom = false;
        bool isOrigin = false;
        std::string specificType;  // @Style, @Element, @Var
        std::string specificName;  // 具体名称
    };
    
    void SetConstraintInfo(const ConstraintInfo& info) { m_ConstraintInfo = info; }
    const ConstraintInfo& GetConstraintInfo() const { return m_ConstraintInfo; }
    
private:
    ConstraintType m_ConstraintType = ELEMENT;
    std::unordered_set<std::string> m_Targets;
    bool m_IsGlobal = false;
    ConstraintInfo m_ConstraintInfo;
};

} // namespace ast
} // namespace chtl