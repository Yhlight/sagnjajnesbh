#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace CHTL {
namespace AST {

// 前向声明
class RootNode;
class ElementNode;
class TextNode;
class AttributeNode;
class StyleBlockNode;
class ScriptBlockNode;
class TemplateNode;
class CustomNode;
class OriginNode;
class ImportNode;
class NamespaceNode;
class ConfigurationNode;
class InheritanceNode;
class DeletionNode;
class InsertionNode;
class IndexAccessNode;
class ConstraintNode;
class VariableGroupNode;
class VariableReferenceNode;
class TemplateReferenceNode;
class CustomReferenceNode;
class SpecializationNode;
class CommentNode;
class LiteralNode;
class CSSSelectorNode;
class CSSPropertyNode;

/**
 * @brief AST访问者接口
 * 
 * 实现访问者模式，用于遍历和处理AST节点
 */
class ASTVisitor {
public:
    /**
     * @brief 虚析构函数
     */
    virtual ~ASTVisitor() = default;
    
    // 基础节点访问方法
    virtual void VisitRootNode(RootNode& node) = 0;
    virtual void VisitElementNode(ElementNode& node) = 0;
    virtual void VisitTextNode(TextNode& node) = 0;
    virtual void VisitAttributeNode(AttributeNode& node) = 0;
    virtual void VisitCommentNode(CommentNode& node) = 0;
    virtual void VisitLiteralNode(LiteralNode& node) = 0;
    
    // 样式和脚本节点访问方法
    virtual void VisitStyleBlockNode(StyleBlockNode& node) = 0;
    virtual void VisitScriptBlockNode(ScriptBlockNode& node) = 0;
    virtual void VisitCSSSelectorNode(CSSSelectorNode& node) = 0;
    virtual void VisitCSSPropertyNode(CSSPropertyNode& node) = 0;
    
    // 模板系统访问方法
    virtual void VisitTemplateNode(TemplateNode& node) = 0;
    virtual void VisitTemplateReferenceNode(TemplateReferenceNode& node) = 0;
    
    // 自定义系统访问方法
    virtual void VisitCustomNode(CustomNode& node) = 0;
    virtual void VisitCustomReferenceNode(CustomReferenceNode& node) = 0;
    virtual void VisitSpecializationNode(SpecializationNode& node) = 0;
    
    // 原始嵌入访问方法
    virtual void VisitOriginNode(OriginNode& node) = 0;
    
    // 导入和命名空间访问方法
    virtual void VisitImportNode(ImportNode& node) = 0;
    virtual void VisitNamespaceNode(NamespaceNode& node) = 0;
    virtual void VisitConfigurationNode(ConfigurationNode& node) = 0;
    
    // 特殊操作访问方法
    virtual void VisitInheritanceNode(InheritanceNode& node) = 0;
    virtual void VisitDeletionNode(DeletionNode& node) = 0;
    virtual void VisitInsertionNode(InsertionNode& node) = 0;
    virtual void VisitIndexAccessNode(IndexAccessNode& node) = 0;
    virtual void VisitConstraintNode(ConstraintNode& node) = 0;
    
    // 变量相关访问方法
    virtual void VisitVariableGroupNode(VariableGroupNode& node) = 0;
    virtual void VisitVariableReferenceNode(VariableReferenceNode& node) = 0;
};

/**
 * @brief 基础AST访问者实现
 * 
 * 提供默认的空实现，子类可以选择性重写需要的方法
 */
class BaseASTVisitor : public ASTVisitor {
public:
    virtual ~BaseASTVisitor() = default;
    
    void VisitRootNode(RootNode& node) override {}
    void VisitElementNode(ElementNode& node) override {}
    void VisitTextNode(TextNode& node) override {}
    void VisitAttributeNode(AttributeNode& node) override {}
    void VisitCommentNode(CommentNode& node) override {}
    void VisitLiteralNode(LiteralNode& node) override {}
    void VisitStyleBlockNode(StyleBlockNode& node) override {}
    void VisitScriptBlockNode(ScriptBlockNode& node) override {}
    void VisitCSSSelectorNode(CSSSelectorNode& node) override {}
    void VisitCSSPropertyNode(CSSPropertyNode& node) override {}
    void VisitTemplateNode(TemplateNode& node) override {}
    void VisitTemplateReferenceNode(TemplateReferenceNode& node) override {}
    void VisitCustomNode(CustomNode& node) override {}
    void VisitCustomReferenceNode(CustomReferenceNode& node) override {}
    void VisitSpecializationNode(SpecializationNode& node) override {}
    void VisitOriginNode(OriginNode& node) override {}
    void VisitImportNode(ImportNode& node) override {}
    void VisitNamespaceNode(NamespaceNode& node) override {}
    void VisitConfigurationNode(ConfigurationNode& node) override {}
    void VisitInheritanceNode(InheritanceNode& node) override {}
    void VisitDeletionNode(DeletionNode& node) override {}
    void VisitInsertionNode(InsertionNode& node) override {}
    void VisitIndexAccessNode(IndexAccessNode& node) override {}
    void VisitConstraintNode(ConstraintNode& node) override {}
    void VisitVariableGroupNode(VariableGroupNode& node) override {}
    void VisitVariableReferenceNode(VariableReferenceNode& node) override {}
};

/**
 * @brief AST打印访问者
 * 
 * 用于打印AST树结构，便于调试
 */
class ASTPrintVisitor : public BaseASTVisitor {
public:
    ASTPrintVisitor(int indentSize = 2);
    
    void VisitRootNode(RootNode& node) override;
    void VisitElementNode(ElementNode& node) override;
    void VisitTextNode(TextNode& node) override;
    void VisitAttributeNode(AttributeNode& node) override;
    void VisitStyleBlockNode(StyleBlockNode& node) override;
    void VisitScriptBlockNode(ScriptBlockNode& node) override;
    void VisitTemplateNode(TemplateNode& node) override;
    void VisitCustomNode(CustomNode& node) override;
    void VisitOriginNode(OriginNode& node) override;
    void VisitImportNode(ImportNode& node) override;
    void VisitNamespaceNode(NamespaceNode& node) override;
    void VisitConfigurationNode(ConfigurationNode& node) override;
    void VisitCSSSelectorNode(CSSSelectorNode& node) override;
    void VisitCSSPropertyNode(CSSPropertyNode& node) override;
    void VisitCommentNode(CommentNode& node) override;
    void VisitLiteralNode(LiteralNode& node) override;
    void VisitTemplateReferenceNode(TemplateReferenceNode& node) override;
    void VisitCustomReferenceNode(CustomReferenceNode& node) override;
    void VisitSpecializationNode(SpecializationNode& node) override;
    void VisitInheritanceNode(InheritanceNode& node) override;
    void VisitDeletionNode(DeletionNode& node) override;
    void VisitInsertionNode(InsertionNode& node) override;
    void VisitIndexAccessNode(IndexAccessNode& node) override;
    void VisitConstraintNode(ConstraintNode& node) override;
    void VisitVariableGroupNode(VariableGroupNode& node) override;
    void VisitVariableReferenceNode(VariableReferenceNode& node) override;
    
    /**
     * @brief 获取打印结果
     */
    const std::string& GetResult() const { return result_; }
    
    /**
     * @brief 清空结果
     */
    void Clear() { result_.clear(); indent_ = 0; }

private:
    void PrintIndent();
    void IncreaseIndent() { indent_ += indentSize_; }
    void DecreaseIndent() { indent_ = std::max(0, indent_ - indentSize_); }
    
    std::string result_;
    int indent_;
    int indentSize_;
};

/**
 * @brief AST验证访问者
 * 
 * 用于验证AST的正确性和完整性
 */
class ASTValidationVisitor : public BaseASTVisitor {
public:
    ASTValidationVisitor();
    
    void VisitRootNode(RootNode& node) override;
    void VisitElementNode(ElementNode& node) override;
    void VisitTemplateNode(TemplateNode& node) override;
    void VisitCustomNode(CustomNode& node) override;
    void VisitImportNode(ImportNode& node) override;
    void VisitNamespaceNode(NamespaceNode& node) override;
    
    /**
     * @brief 检查是否有错误
     */
    bool HasErrors() const { return !errors_.empty(); }
    
    /**
     * @brief 获取错误列表
     */
    const std::vector<std::string>& GetErrors() const { return errors_; }
    
    /**
     * @brief 清空错误
     */
    void ClearErrors() { errors_.clear(); }

private:
    void AddError(const std::string& error);
    
    std::vector<std::string> errors_;
};

} // namespace AST
} // namespace CHTL