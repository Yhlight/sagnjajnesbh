#ifndef CHTL_AST_ASTPRINTER_H
#define CHTL_AST_ASTPRINTER_H

#include "CHTL/AST/ASTVisitor.h"
#include <iostream>
#include <string>
#include <sstream>

namespace CHTL {
namespace AST {

// AST打印器 - 用于调试和可视化AST结构
class ASTPrinter : public BaseASTVisitor {
public:
    ASTPrinter(std::ostream& os = std::cout);
    
    // 设置缩进
    void SetIndentSize(int size) { indentSize_ = size; }
    
    // 访问方法重写
    void VisitProgramNode(ProgramNode* node) override;
    void VisitElementNode(ElementNode* node) override;
    void VisitAttributeNode(AttributeNode* node) override;
    void VisitTextBlockNode(TextBlockNode* node) override;
    void VisitStyleBlockNode(StyleBlockNode* node) override;
    void VisitStyleRuleNode(StyleRuleNode* node) override;
    void VisitStyleSelectorNode(StyleSelectorNode* node) override;
    void VisitStylePropertyNode(StylePropertyNode* node) override;
    void VisitScriptBlockNode(ScriptBlockNode* node) override;
    void VisitTemplateDeclarationNode(TemplateDeclarationNode* node) override;
    void VisitCustomDeclarationNode(CustomDeclarationNode* node) override;
    void VisitOriginBlockNode(OriginBlockNode* node) override;
    void VisitImportStatementNode(ImportStatementNode* node) override;
    void VisitNamespaceDeclarationNode(NamespaceDeclarationNode* node) override;
    void VisitConfigurationBlockNode(ConfigurationBlockNode* node) override;
    void VisitSpecializationNode(SpecializationNode* node) override;
    void VisitInsertBlockNode(InsertBlockNode* node) override;
    void VisitDeleteBlockNode(DeleteBlockNode* node) override;
    void VisitExceptBlockNode(ExceptBlockNode* node) override;
    void VisitInheritStatementNode(InheritStatementNode* node) override;
    void VisitIdentifierNode(IdentifierNode* node) override;
    void VisitStringLiteralNode(StringLiteralNode* node) override;
    
private:
    std::ostream& os_;
    int indent_ = 0;
    int indentSize_ = 2;
    
    // 辅助方法
    void PrintIndent();
    void IncreaseIndent();
    void DecreaseIndent();
    std::string EscapeString(const std::string& str);
    
    // 打印节点信息
    void PrintNodeHeader(const std::string& nodeType, const std::string& info = "");
    void PrintNodeHeaderWithChildren(const std::string& nodeType, const std::string& info = "");
};

// AST到字符串的转换器
class ASTStringifier : public BaseASTVisitor {
public:
    std::string Stringify(ASTNode* node);
    
    // 访问方法
    void VisitProgramNode(ProgramNode* node) override;
    void VisitElementNode(ElementNode* node) override;
    void VisitAttributeNode(AttributeNode* node) override;
    void VisitTextBlockNode(TextBlockNode* node) override;
    void VisitStyleBlockNode(StyleBlockNode* node) override;
    void VisitScriptBlockNode(ScriptBlockNode* node) override;
    void VisitIdentifierNode(IdentifierNode* node) override;
    void VisitStringLiteralNode(StringLiteralNode* node) override;
    
private:
    std::stringstream buffer_;
    int indent_ = 0;
    
    void Indent();
    void IncreaseIndent() { indent_ += 2; }
    void DecreaseIndent() { indent_ -= 2; }
    void Append(const std::string& str);
    void AppendLine(const std::string& str);
};

} // namespace AST
} // namespace CHTL

#endif // CHTL_AST_ASTPRINTER_H