#include "CHTL/AST/ASTPrinter.h"
#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/TemplateNodes.h"
#include "CHTL/AST/SpecialNodes.h"
#include "CHTL/AST/StyleNodes.h"
#include <sstream>

namespace CHTL {
namespace AST {

// ASTPrinter实现
ASTPrinter::ASTPrinter(std::ostream& os) : os_(os) {}

void ASTPrinter::PrintIndent() {
    os_ << std::string(indent_, ' ');
}

void ASTPrinter::IncreaseIndent() {
    indent_ += indentSize_;
}

void ASTPrinter::DecreaseIndent() {
    indent_ = (indent_ >= indentSize_) ? indent_ - indentSize_ : 0;
}

std::string ASTPrinter::EscapeString(const std::string& str) {
    std::stringstream ss;
    for (char ch : str) {
        switch (ch) {
            case '\n': ss << "\\n"; break;
            case '\t': ss << "\\t"; break;
            case '\r': ss << "\\r"; break;
            case '\\': ss << "\\\\"; break;
            case '"': ss << "\\\""; break;
            default: ss << ch; break;
        }
    }
    return ss.str();
}

void ASTPrinter::PrintNodeHeader(const std::string& nodeType, const std::string& info) {
    PrintIndent();
    os_ << nodeType;
    if (!info.empty()) {
        os_ << " " << info;
    }
    os_ << std::endl;
}

void ASTPrinter::PrintNodeHeaderWithChildren(const std::string& nodeType, const std::string& info) {
    PrintNodeHeader(nodeType, info);
    IncreaseIndent();
}

// 访问方法实现
void ASTPrinter::VisitProgramNode(ProgramNode* node) {
    PrintNodeHeaderWithChildren("Program");
    BaseASTVisitor::VisitProgramNode(node);
    DecreaseIndent();
}

void ASTPrinter::VisitElementNode(ElementNode* node) {
    std::stringstream info;
    info << "<" << node->GetTagName();
    if (!node->GetId().empty()) {
        info << " id=\"" << node->GetId() << "\"";
    }
    if (!node->GetClass().empty()) {
        info << " class=\"" << node->GetClass() << "\"";
    }
    info << ">";
    
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("Element", info.str());
        BaseASTVisitor::VisitElementNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("Element", info.str());
    }
}

void ASTPrinter::VisitAttributeNode(AttributeNode* node) {
    std::stringstream info;
    info << node->GetName() << "=\"" << EscapeString(node->GetValue()) << "\"";
    PrintNodeHeader("Attribute", info.str());
}

void ASTPrinter::VisitTextBlockNode(TextBlockNode* node) {
    std::stringstream info;
    info << "\"" << EscapeString(node->GetContent()) << "\"";
    PrintNodeHeader("TextBlock", info.str());
}

void ASTPrinter::VisitStyleBlockNode(StyleBlockNode* node) {
    std::string info = node->IsLocal() ? "(local)" : "(global)";
    
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("StyleBlock", info);
        BaseASTVisitor::VisitStyleBlockNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("StyleBlock", info);
    }
}

void ASTPrinter::VisitStyleRuleNode(StyleRuleNode* node) {
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("StyleRule");
        BaseASTVisitor::VisitStyleRuleNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("StyleRule");
    }
}

void ASTPrinter::VisitStyleSelectorNode(StyleSelectorNode* node) {
    std::stringstream info;
    info << node->GetSelectorTypeName();
    if (!node->GetValue().empty()) {
        info << " \"" << node->GetValue() << "\"";
    }
    
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("StyleSelector", info.str());
        BaseASTVisitor::VisitStyleSelectorNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("StyleSelector", info.str());
    }
}

void ASTPrinter::VisitStylePropertyNode(StylePropertyNode* node) {
    std::stringstream info;
    info << node->GetName();
    if (node->IsImportant()) {
        info << " !important";
    }
    
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("StyleProperty", info.str());
        BaseASTVisitor::VisitStylePropertyNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("StyleProperty", info.str());
    }
}

void ASTPrinter::VisitScriptBlockNode(ScriptBlockNode* node) {
    std::string info = node->IsLocal() ? "(local)" : "(global)";
    if (!node->GetContent().empty()) {
        info += " [" + std::to_string(node->GetContent().length()) + " chars]";
    }
    PrintNodeHeader("ScriptBlock", info);
}

void ASTPrinter::VisitTemplateDeclarationNode(TemplateDeclarationNode* node) {
    std::stringstream info;
    info << node->GetTemplateTypeName() << " " << node->GetName();
    
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("TemplateDeclaration", info.str());
        BaseASTVisitor::VisitTemplateDeclarationNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("TemplateDeclaration", info.str());
    }
}

void ASTPrinter::VisitCustomDeclarationNode(CustomDeclarationNode* node) {
    std::stringstream info;
    info << node->GetCustomTypeName() << " " << node->GetName();
    
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("CustomDeclaration", info.str());
        BaseASTVisitor::VisitCustomDeclarationNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("CustomDeclaration", info.str());
    }
}

void ASTPrinter::VisitOriginBlockNode(OriginBlockNode* node) {
    std::stringstream info;
    info << node->GetOriginTypeName() << " " << node->GetName();
    if (!node->GetContent().empty()) {
        info << " [" << node->GetContent().length() << " chars]";
    }
    PrintNodeHeader("OriginBlock", info.str());
}

void ASTPrinter::VisitImportStatementNode(ImportStatementNode* node) {
    std::stringstream info;
    info << node->GetImportTypeName();
    if (!node->GetTarget().empty()) {
        info << " " << node->GetTarget();
    }
    info << " from \"" << node->GetFromPath() << "\"";
    if (node->HasAsName()) {
        info << " as " << node->GetAsName();
    }
    PrintNodeHeader("ImportStatement", info.str());
}

void ASTPrinter::VisitNamespaceDeclarationNode(NamespaceDeclarationNode* node) {
    std::stringstream info;
    info << node->GetName();
    
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("NamespaceDeclaration", info.str());
        BaseASTVisitor::VisitNamespaceDeclarationNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("NamespaceDeclaration", info.str());
    }
}

void ASTPrinter::VisitConfigurationBlockNode(ConfigurationBlockNode* node) {
    std::stringstream info;
    if (node->IsNamed()) {
        info << node->GetName();
    }
    
    PrintNodeHeaderWithChildren("ConfigurationBlock", info.str());
    
    // 打印配置项
    for (const auto& [key, value] : node->GetConfigs()) {
        IncreaseIndent();
        PrintNodeHeader("Config", key + " = \"" + EscapeString(value) + "\"");
        DecreaseIndent();
    }
    
    BaseASTVisitor::VisitConfigurationBlockNode(node);
    DecreaseIndent();
}

void ASTPrinter::VisitSpecializationNode(SpecializationNode* node) {
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("Specialization");
        BaseASTVisitor::VisitSpecializationNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("Specialization");
    }
}

void ASTPrinter::VisitInsertBlockNode(InsertBlockNode* node) {
    std::stringstream info;
    info << node->GetPositionName();
    if (!node->GetTarget().empty()) {
        info << " " << node->GetTarget();
    }
    
    if (node->GetChildCount() > 0) {
        PrintNodeHeaderWithChildren("InsertBlock", info.str());
        BaseASTVisitor::VisitInsertBlockNode(node);
        DecreaseIndent();
    } else {
        PrintNodeHeader("InsertBlock", info.str());
    }
}

void ASTPrinter::VisitDeleteBlockNode(DeleteBlockNode* node) {
    PrintNodeHeaderWithChildren("DeleteBlock");
    
    // 打印删除目标
    for (const auto& target : node->GetDeleteTargets()) {
        IncreaseIndent();
        PrintNodeHeader("DeleteTarget", target);
        DecreaseIndent();
    }
    
    BaseASTVisitor::VisitDeleteBlockNode(node);
    DecreaseIndent();
}

void ASTPrinter::VisitExceptBlockNode(ExceptBlockNode* node) {
    std::stringstream info;
    info << GetExceptTypeName(node->GetExceptType());
    
    PrintNodeHeaderWithChildren("ExceptBlock", info.str());
    
    // 打印约束目标
    for (const auto& target : node->GetExceptTargets()) {
        IncreaseIndent();
        PrintNodeHeader("ExceptTarget", target);
        DecreaseIndent();
    }
    
    BaseASTVisitor::VisitExceptBlockNode(node);
    DecreaseIndent();
}

void ASTPrinter::VisitInheritStatementNode(InheritStatementNode* node) {
    PrintNodeHeader("InheritStatement", node->GetTarget());
}

void ASTPrinter::VisitIdentifierNode(IdentifierNode* node) {
    PrintNodeHeader("Identifier", node->GetName());
}

void ASTPrinter::VisitStringLiteralNode(StringLiteralNode* node) {
    PrintNodeHeader("StringLiteral", "\"" + EscapeString(node->GetValue()) + "\"");
}

// ASTStringifier实现
std::string ASTStringifier::Stringify(ASTNode* node) {
    buffer_.str("");
    buffer_.clear();
    indent_ = 0;
    
    if (node) {
        node->Accept(this);
    }
    
    return buffer_.str();
}

void ASTStringifier::Indent() {
    buffer_ << std::string(indent_, ' ');
}

void ASTStringifier::Append(const std::string& str) {
    buffer_ << str;
}

void ASTStringifier::AppendLine(const std::string& str) {
    Indent();
    buffer_ << str << std::endl;
}

void ASTStringifier::VisitProgramNode(ProgramNode* node) {
    BaseASTVisitor::VisitProgramNode(node);
}

void ASTStringifier::VisitElementNode(ElementNode* node) {
    Indent();
    Append(node->GetTagName());
    
    // 输出属性
    bool hasAttributes = false;
    for (const auto& [name, value] : node->GetAttributes()) {
        if (!hasAttributes) {
            Append(" {");
            IncreaseIndent();
            hasAttributes = true;
        }
        AppendLine("");
        Indent();
        Append(name + ": " + value + ";");
    }
    
    if (hasAttributes) {
        DecreaseIndent();
        AppendLine("");
        Indent();
        Append("}");
    }
    
    // 输出子节点
    if (node->GetChildCount() > 0) {
        Append(" {");
        IncreaseIndent();
        AppendLine("");
        
        BaseASTVisitor::VisitElementNode(node);
        
        DecreaseIndent();
        Indent();
        Append("}");
    }
    
    AppendLine("");
}

void ASTStringifier::VisitAttributeNode(AttributeNode* node) {
    Append(node->GetName() + ": " + node->GetValue());
}

void ASTStringifier::VisitTextBlockNode(TextBlockNode* node) {
    AppendLine("text { \"" + node->GetContent() + "\" }");
}

void ASTStringifier::VisitStyleBlockNode(StyleBlockNode* node) {
    AppendLine("style {");
    IncreaseIndent();
    
    BaseASTVisitor::VisitStyleBlockNode(node);
    
    DecreaseIndent();
    AppendLine("}");
}

void ASTStringifier::VisitScriptBlockNode(ScriptBlockNode* node) {
    AppendLine("script {");
    IncreaseIndent();
    AppendLine(node->GetContent());
    DecreaseIndent();
    AppendLine("}");
}

void ASTStringifier::VisitIdentifierNode(IdentifierNode* node) {
    Append(node->GetName());
}

void ASTStringifier::VisitStringLiteralNode(StringLiteralNode* node) {
    Append("\"" + node->GetValue() + "\"");
}

} // namespace AST
} // namespace CHTL