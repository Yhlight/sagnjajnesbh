#ifndef CHTL_AST_SPECIALNODES_H
#define CHTL_AST_SPECIALNODES_H

#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/ASTVisitor.h"

namespace CHTL {
namespace AST {

// 原始嵌入块节点
class OriginBlockNode : public ASTNode {
public:
    enum class OriginType {
        Html,       // @Html
        Style,      // @Style
        JavaScript, // @JavaScript
        Custom      // 自定义类型
    };
    
    OriginBlockNode(OriginType type, const std::string& name, const Token& token);
    OriginBlockNode(const std::string& customType, const std::string& name, const Token& token);
    
    OriginType GetOriginType() const { return originType_; }
    const std::string& GetCustomTypeName() const { return customTypeName_; }
    const std::string& GetName() const { return name_; }
    const std::string& GetContent() const { return content_; }
    void SetContent(const std::string& content) { content_ = content; }
    
    bool IsCustomType() const { return originType_ == OriginType::Custom; }
    std::string GetOriginTypeName() const;
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    OriginType originType_;
    std::string customTypeName_;  // 仅在自定义类型时使用
    std::string name_;
    std::string content_;
};

// 导入语句节点
class ImportStatementNode : public ASTNode {
public:
    enum class ImportType {
        Html,           // @Html
        Style,          // @Style
        JavaScript,     // @JavaScript
        Chtl,           // @Chtl
        CJmod,          // @CJmod
        Config,         // @Config
        Template,       // [Template]
        Custom,         // [Custom]
        Origin,         // [Origin]
        TemplateStyle,  // [Template] @Style
        TemplateElement,// [Template] @Element
        TemplateVar,    // [Template] @Var
        CustomStyle,    // [Custom] @Style
        CustomElement,  // [Custom] @Element
        CustomVar       // [Custom] @Var
    };
    
    ImportStatementNode(ImportType type, const Token& token);
    
    ImportType GetImportType() const { return importType_; }
    const std::string& GetTarget() const { return target_; }
    const std::string& GetFromPath() const { return fromPath_; }
    const std::string& GetAsName() const { return asName_; }
    
    void SetTarget(const std::string& target) { target_ = target; }
    void SetFromPath(const std::string& path) { fromPath_ = path; }
    void SetAsName(const std::string& name) { asName_ = name; }
    
    bool HasAsName() const { return !asName_.empty(); }
    std::string GetImportTypeName() const;
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    ImportType importType_;
    std::string target_;      // 要导入的具体项（可为空）
    std::string fromPath_;    // 导入路径
    std::string asName_;      // 别名（可选）
};

// 命名空间声明节点
class NamespaceDeclarationNode : public ASTNode {
public:
    NamespaceDeclarationNode(const std::string& name, const Token& token);
    
    const std::string& GetName() const { return name_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string name_;
};

// 配置块节点
class ConfigurationBlockNode : public ASTNode {
public:
    ConfigurationBlockNode(const std::string& name, const Token& token);
    
    const std::string& GetName() const { return name_; }
    bool IsNamed() const { return !name_.empty(); }
    
    // 配置项管理
    void AddConfig(const std::string& key, const std::string& value);
    std::string GetConfig(const std::string& key) const;
    bool HasConfig(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& GetConfigs() const { return configs_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string name_;
    std::unordered_map<std::string, std::string> configs_;
};

// 约束块节点
class ExceptBlockNode : public ASTNode {
public:
    enum class ExceptType {
        Precise,    // 精确约束
        Type,       // 类型约束
        Global      // 全局约束
    };
    
    explicit ExceptBlockNode(const Token& token);
    
    ExceptType GetExceptType() const { return exceptType_; }
    void SetExceptType(ExceptType type) { exceptType_ = type; }
    
    // 约束目标
    void AddExceptTarget(const std::string& target);
    const std::vector<std::string>& GetExceptTargets() const { return exceptTargets_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    ExceptType exceptType_ = ExceptType::Precise;
    std::vector<std::string> exceptTargets_;
};

// from语句节点（用于从命名空间获取符号）
class FromStatementNode : public ASTNode {
public:
    FromStatementNode(const std::string& symbol, const std::string& namespaceName, const Token& token);
    
    const std::string& GetSymbol() const { return symbol_; }
    const std::string& GetNamespace() const { return namespaceName_; }
    
    void Accept(ASTVisitor* visitor) override;
    std::string ToString() const override;
    
private:
    std::string symbol_;
    std::string namespaceName_;
};

// 工具函数
std::string GetOriginTypeName(OriginBlockNode::OriginType type);
std::string GetImportTypeName(ImportStatementNode::ImportType type);
std::string GetExceptTypeName(ExceptBlockNode::ExceptType type);

} // namespace AST
} // namespace CHTL

#endif // CHTL_AST_SPECIALNODES_H