#ifndef CHTL_AST_TEMPLATENODES_H
#define CHTL_AST_TEMPLATENODES_H

#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/ASTVisitor.h"

namespace CHTL {
namespace AST {

// 模板类型枚举
enum class TemplateType {
    Style,      // @Style
    Element,    // @Element
    Var         // @Var
};

// 模板声明节点
class TemplateDeclarationNode : public ASTNode {
public:
    TemplateDeclarationNode(TemplateType templateType, const std::string& name, const Token& token);
    
    TemplateType GetTemplateType() const { return templateType_; }
    const std::string& GetName() const { return name_; }
    std::string GetTemplateTypeName() const;
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    TemplateType templateType_;
    std::string name_;
};

// 自定义声明节点
class CustomDeclarationNode : public ASTNode {
public:
    CustomDeclarationNode(TemplateType customType, const std::string& name, const Token& token);
    
    TemplateType GetCustomType() const { return customType_; }
    const std::string& GetName() const { return name_; }
    std::string GetCustomTypeName() const;
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    TemplateType customType_;
    std::string name_;
};

// 模板/自定义使用节点
class TemplateUseNode : public ASTNode {
public:
    TemplateUseNode(TemplateType type, const std::string& name, bool isCustom, const Token& token);
    
    TemplateType GetTemplateType() const { return type_; }
    const std::string& GetName() const { return name_; }
    bool IsCustom() const { return isCustom_; }
    
    // 特例化参数
    void AddSpecialization(const std::string& key, const std::string& value);
    const std::unordered_map<std::string, std::string>& GetSpecializations() const { return specializations_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    TemplateType type_;
    std::string name_;
    bool isCustom_;
    std::unordered_map<std::string, std::string> specializations_;
};

// 继承语句节点
class InheritStatementNode : public ASTNode {
public:
    InheritStatementNode(const std::string& target, const Token& token);
    
    const std::string& GetTarget() const { return target_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string target_;
};

// 特例化节点
class SpecializationNode : public ASTNode {
public:
    explicit SpecializationNode(const Token& token);
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
};

// 删除块节点
class DeleteBlockNode : public ASTNode {
public:
    explicit DeleteBlockNode(const Token& token);
    
    // 删除目标
    void AddDeleteTarget(const std::string& target);
    const std::vector<std::string>& GetDeleteTargets() const { return deleteTargets_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::vector<std::string> deleteTargets_;
};

// 插入块节点
class InsertBlockNode : public ASTNode {
public:
    enum class InsertPosition {
        After,
        Before,
        Replace,
        AtTop,
        AtBottom
    };
    
    InsertBlockNode(InsertPosition position, const std::string& target, const Token& token);
    
    InsertPosition GetPosition() const { return position_; }
    const std::string& GetTarget() const { return target_; }
    std::string GetPositionName() const;
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    InsertPosition position_;
    std::string target_;
};

// 工具函数
std::string GetTemplateTypeName(TemplateType type);
std::string GetInsertPositionName(InsertBlockNode::InsertPosition position);

} // namespace AST
} // namespace CHTL

#endif // CHTL_AST_TEMPLATENODES_H