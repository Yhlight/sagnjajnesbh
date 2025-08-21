#ifndef CHTL_AST_STYLENODES_H
#define CHTL_AST_STYLENODES_H

#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/ASTVisitor.h"

namespace CHTL {
namespace AST {

// 样式规则节点
class StyleRuleNode : public ASTNode {
public:
    explicit StyleRuleNode(const Token& token);
    
    // 选择器管理
    void SetSelector(std::shared_ptr<ASTNode> selector);
    std::shared_ptr<ASTNode> GetSelector() const { return selector_; }
    
    // 是否为内联样式（无选择器）
    bool IsInlineStyle() const { return selector_ == nullptr; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::shared_ptr<ASTNode> selector_;  // 可以是StyleSelectorNode或标识符等
};

// 样式选择器节点
class StyleSelectorNode : public ASTNode {
public:
    enum class SelectorType {
        Class,          // .className
        Id,             // #id
        Element,        // element
        Universal,      // *
        Attribute,      // [attr=value]
        PseudoClass,    // :hover
        PseudoElement,  // ::before
        Combinator,     // 组合选择器
        Ampersand       // & (上下文推导)
    };
    
    StyleSelectorNode(SelectorType type, const std::string& value, const Token& token);
    
    SelectorType GetSelectorType() const { return selectorType_; }
    const std::string& GetValue() const { return value_; }
    
    // 组合选择器
    void SetLeft(std::shared_ptr<ASTNode> left) { left_ = left; }
    void SetRight(std::shared_ptr<ASTNode> right) { right_ = right; }
    std::shared_ptr<ASTNode> GetLeft() const { return left_; }
    std::shared_ptr<ASTNode> GetRight() const { return right_; }
    
    // 组合器类型
    enum class CombinatorType {
        Descendant,     // 空格
        Child,          // >
        Adjacent,       // +
        Sibling         // ~
    };
    
    void SetCombinatorType(CombinatorType type) { combinatorType_ = type; }
    CombinatorType GetCombinatorType() const { return combinatorType_; }
    
    std::string GetSelectorTypeName() const;
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    SelectorType selectorType_;
    std::string value_;
    CombinatorType combinatorType_ = CombinatorType::Descendant;
    
    // 用于组合选择器
    std::shared_ptr<ASTNode> left_;
    std::shared_ptr<ASTNode> right_;
};

// 样式属性节点
class StylePropertyNode : public ASTNode {
public:
    StylePropertyNode(const std::string& name, const Token& token);
    
    const std::string& GetName() const { return name_; }
    
    // 值管理（可以是多个值，如 margin: 10px 20px）
    void AddValue(std::shared_ptr<ASTNode> value);
    const std::vector<std::shared_ptr<ASTNode>>& GetValues() const { return values_; }
    
    // 重要性
    void SetImportant(bool important) { isImportant_ = important; }
    bool IsImportant() const { return isImportant_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string name_;
    std::vector<std::shared_ptr<ASTNode>> values_;
    bool isImportant_ = false;
};

// 样式值节点（用于复杂的样式值）
class StyleValueNode : public ASTNode {
public:
    enum class ValueType {
        Literal,        // 字面值
        Function,       // 函数调用，如 rgb()
        Variable,       // 变量引用
        Calc            // 计算表达式
    };
    
    StyleValueNode(ValueType type, const std::string& value, const Token& token);
    
    ValueType GetValueType() const { return valueType_; }
    const std::string& GetValue() const { return value_; }
    
    // 函数参数（仅用于Function类型）
    void AddArgument(std::shared_ptr<ASTNode> arg);
    const std::vector<std::shared_ptr<ASTNode>>& GetArguments() const { return arguments_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    ValueType valueType_;
    std::string value_;
    std::vector<std::shared_ptr<ASTNode>> arguments_;
};

// 样式变量使用节点
class StyleVariableUseNode : public ASTNode {
public:
    StyleVariableUseNode(const std::string& varGroupName, const std::string& varName, const Token& token);
    
    const std::string& GetVarGroupName() const { return varGroupName_; }
    const std::string& GetVarName() const { return varName_; }
    
    // 特例化
    void SetSpecialization(const std::string& value) { specialization_ = value; }
    const std::string& GetSpecialization() const { return specialization_; }
    bool HasSpecialization() const { return !specialization_.empty(); }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string varGroupName_;
    std::string varName_;
    std::string specialization_;
};

// 工具函数
std::string GetSelectorTypeName(StyleSelectorNode::SelectorType type);
std::string GetCombinatorTypeName(StyleSelectorNode::CombinatorType type);
std::string GetStyleValueTypeName(StyleValueNode::ValueType type);

} // namespace AST
} // namespace CHTL

#endif // CHTL_AST_STYLENODES_H