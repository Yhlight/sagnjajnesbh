#include "CHTL/AST/CHTLASTVisitor.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include "Utils/StringUtils.h"
#include <algorithm>

namespace CHTL {
namespace AST {

// ASTPrintVisitor 实现
ASTPrintVisitor::ASTPrintVisitor(int indentSize) : indent_(0), indentSize_(indentSize) {}

void ASTPrintVisitor::PrintIndent() {
    for (int i = 0; i < indent_; ++i) {
        result_ += " ";
    }
}

void ASTPrintVisitor::VisitRootNode(RootNode& node) {
    PrintIndent();
    result_ += "ROOT";
    if (!node.GetFileName().empty()) {
        result_ += "(" + node.GetFileName() + ")";
    }
    result_ += "\n";
    
    IncreaseIndent();
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
    DecreaseIndent();
}

void ASTPrintVisitor::VisitElementNode(ElementNode& node) {
    PrintIndent();
    result_ += "ELEMENT(" + node.GetTagName();
    
    const auto& attributes = node.GetAttributes();
    if (!attributes.empty()) {
        result_ += " [";
        bool first = true;
        for (const auto& attr : attributes) {
            if (!first) result_ += ", ";
            result_ += attr.first + "=" + attr.second;
            first = false;
        }
        result_ += "]";
    }
    result_ += ")\n";
    
    IncreaseIndent();
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
    DecreaseIndent();
}

void ASTPrintVisitor::VisitTextNode(TextNode& node) {
    PrintIndent();
    result_ += "TEXT(\"" + node.GetContent() + "\")\n";
}

void ASTPrintVisitor::VisitAttributeNode(AttributeNode& node) {
    PrintIndent();
    std::string separator = node.UsesCEEquality() ? "=" : ":";
    result_ += "ATTRIBUTE(" + node.GetName() + separator + node.GetValue() + ")\n";
}

void ASTPrintVisitor::VisitStyleBlockNode(StyleBlockNode& node) {
    PrintIndent();
    result_ += std::string("STYLE_BLOCK(") + (node.IsLocalStyleBlock() ? "local" : "global") + ")\n";
    
    IncreaseIndent();
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
    DecreaseIndent();
}

void ASTPrintVisitor::VisitScriptBlockNode(ScriptBlockNode& node) {
    PrintIndent();
    result_ += std::string("SCRIPT_BLOCK(") + (node.IsLocalScriptBlock() ? "local" : "global") + ")\n";
    
    if (!node.GetScriptContent().empty()) {
        IncreaseIndent();
        PrintIndent();
        result_ += "Content: \"" + node.GetScriptContent() + "\"\n";
        DecreaseIndent();
    }
    
    IncreaseIndent();
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
    DecreaseIndent();
}

void ASTPrintVisitor::VisitTemplateNode(TemplateNode& node) {
    PrintIndent();
    std::string typeStr;
    switch (node.GetTemplateType()) {
        case TemplateNode::TemplateType::STYLE: typeStr = "Style"; break;
        case TemplateNode::TemplateType::ELEMENT: typeStr = "Element"; break;
        case TemplateNode::TemplateType::VAR: typeStr = "Var"; break;
    }
    result_ += "TEMPLATE_" + typeStr + "(" + node.GetName() + ")\n";
    
    IncreaseIndent();
    
    // 打印继承信息
    const auto& inheritances = node.GetInheritances();
    if (!inheritances.empty()) {
        PrintIndent();
        result_ += "Inheritances:\n";
        IncreaseIndent();
        for (const auto& inheritance : inheritances) {
            inheritance->Accept(*this);
        }
        DecreaseIndent();
    }
    
    // 打印子节点
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
    
    DecreaseIndent();
}

void ASTPrintVisitor::VisitCustomNode(CustomNode& node) {
    PrintIndent();
    std::string typeStr;
    switch (node.GetCustomType()) {
        case CustomNode::CustomType::STYLE: typeStr = "Style"; break;
        case CustomNode::CustomType::ELEMENT: typeStr = "Element"; break;
        case CustomNode::CustomType::VAR: typeStr = "Var"; break;
    }
    result_ += "CUSTOM_" + typeStr + "(" + node.GetName() + ")\n";
    
    IncreaseIndent();
    
    // 打印继承信息
    const auto& inheritances = node.GetInheritances();
    if (!inheritances.empty()) {
        PrintIndent();
        result_ += "Inheritances:\n";
        IncreaseIndent();
        for (const auto& inheritance : inheritances) {
            inheritance->Accept(*this);
        }
        DecreaseIndent();
    }
    
    // 打印特例化信息
    const auto& specializations = node.GetSpecializations();
    if (!specializations.empty()) {
        PrintIndent();
        result_ += "Specializations:\n";
        IncreaseIndent();
        for (const auto& specialization : specializations) {
            specialization->Accept(*this);
        }
        DecreaseIndent();
    }
    
    // 打印子节点
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
    
    DecreaseIndent();
}

void ASTPrintVisitor::VisitOriginNode(OriginNode& node) {
    PrintIndent();
    std::string typeStr;
    switch (node.GetOriginType()) {
        case OriginNode::OriginType::HTML: typeStr = "HTML"; break;
        case OriginNode::OriginType::STYLE: typeStr = "STYLE"; break;
        case OriginNode::OriginType::JAVASCRIPT: typeStr = "JAVASCRIPT"; break;
        case OriginNode::OriginType::CUSTOM: 
            typeStr = "CUSTOM(" + node.GetCustomTypeName() + ")"; 
            break;
    }
    
    result_ += "ORIGIN_" + typeStr;
    if (node.IsNamed()) {
        result_ += "(" + node.GetName() + ")";
    }
    result_ += "\n";
    
    if (!node.GetContent().empty()) {
        IncreaseIndent();
        PrintIndent();
        std::string content = node.GetContent();
        if (content.length() > 100) {
            content = content.substr(0, 97) + "...";
        }
        result_ += "Content: \"" + content + "\"\n";
        DecreaseIndent();
    }
}

void ASTPrintVisitor::VisitImportNode(ImportNode& node) {
    PrintIndent();
    result_ += "IMPORT(" + node.ToString() + ")\n";
}

void ASTPrintVisitor::VisitNamespaceNode(NamespaceNode& node) {
    PrintIndent();
    result_ += "NAMESPACE(" + node.GetName() + ")\n";
    
    IncreaseIndent();
    
    // 打印约束
    const auto& constraints = node.GetConstraints();
    if (!constraints.empty()) {
        PrintIndent();
        result_ += "Constraints:\n";
        IncreaseIndent();
        for (const auto& constraint : constraints) {
            constraint->Accept(*this);
        }
        DecreaseIndent();
    }
    
    // 打印子节点
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
    
    DecreaseIndent();
}

void ASTPrintVisitor::VisitConfigurationNode(ConfigurationNode& node) {
    PrintIndent();
    result_ += "CONFIGURATION(" + (node.IsNamed() ? node.GetName() : "unnamed") + ")\n";
    
    const auto& settings = node.GetSettings();
    if (!settings.empty()) {
        IncreaseIndent();
        PrintIndent();
        result_ += "Settings:\n";
        IncreaseIndent();
        for (const auto& setting : settings) {
            PrintIndent();
            result_ += setting.first + " = " + setting.second + "\n";
        }
        DecreaseIndent();
        DecreaseIndent();
    }
    
    IncreaseIndent();
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
    DecreaseIndent();
}

// ASTValidationVisitor 实现
ASTValidationVisitor::ASTValidationVisitor() {}

// 添加缺少的访问者方法实现
void ASTPrintVisitor::VisitCSSSelectorNode(CSSSelectorNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitCSSPropertyNode(CSSPropertyNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitCommentNode(CommentNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitLiteralNode(LiteralNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitTemplateReferenceNode(TemplateReferenceNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitCustomReferenceNode(CustomReferenceNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitSpecializationNode(SpecializationNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitInheritanceNode(InheritanceNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitDeletionNode(DeletionNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitInsertionNode(InsertionNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitIndexAccessNode(IndexAccessNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitConstraintNode(ConstraintNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitVariableGroupNode(VariableGroupNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTPrintVisitor::VisitVariableReferenceNode(VariableReferenceNode& node) {
    PrintIndent();
    result_ += node.ToString() + "\n";
}

void ASTValidationVisitor::AddError(const std::string& error) {
    errors_.push_back(error);
}

void ASTValidationVisitor::VisitRootNode(RootNode& node) {
    // 验证根节点
    if (node.GetFileName().empty()) {
        AddError("根节点缺少文件名");
    }
    
    // 递归验证子节点
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
}

void ASTValidationVisitor::VisitElementNode(ElementNode& node) {
    // 验证元素节点
    if (node.GetTagName().empty()) {
        AddError("元素节点缺少标签名");
    }
    
    // 验证HTML标签名的有效性
    if (!Core::CHTLToken::IsHTMLTagString(node.GetTagName()) && 
        !Utils::StringUtils::IsAlpha(node.GetTagName()[0])) {
        AddError("无效的HTML标签名: " + node.GetTagName());
    }
    
    // 验证属性
    const auto& attributes = node.GetAttributes();
    for (const auto& attr : attributes) {
        if (attr.first.empty()) {
            AddError("属性名不能为空");
        }
        if (attr.first.find(' ') != std::string::npos) {
            AddError("属性名不能包含空格: " + attr.first);
        }
    }
    
    // 递归验证子节点
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
}

void ASTValidationVisitor::VisitTemplateNode(TemplateNode& node) {
    // 验证模板节点
    if (node.GetName().empty()) {
        AddError("模板节点缺少名称");
    }
    
    // 验证名称的有效性
    if (!node.GetName().empty() && !Utils::StringUtils::IsAlpha(node.GetName()[0])) {
        AddError("模板名称必须以字母开头: " + node.GetName());
    }
    
    // 验证继承的有效性
    const auto& inheritances = node.GetInheritances();
    for (const auto& inheritance : inheritances) {
        inheritance->Accept(*this);
    }
    
    // 递归验证子节点
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
}

void ASTValidationVisitor::VisitCustomNode(CustomNode& node) {
    // 验证自定义节点
    if (node.GetName().empty()) {
        AddError("自定义节点缺少名称");
    }
    
    // 验证名称的有效性
    if (!node.GetName().empty() && !Utils::StringUtils::IsAlpha(node.GetName()[0])) {
        AddError("自定义名称必须以字母开头: " + node.GetName());
    }
    
    // 验证继承的有效性
    const auto& inheritances = node.GetInheritances();
    for (const auto& inheritance : inheritances) {
        inheritance->Accept(*this);
    }
    
    // 验证特例化的有效性
    const auto& specializations = node.GetSpecializations();
    for (const auto& specialization : specializations) {
        specialization->Accept(*this);
    }
    
    // 递归验证子节点
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
}

void ASTValidationVisitor::VisitImportNode(ImportNode& node) {
    // 验证导入节点
    if (node.GetPath().empty()) {
        AddError("导入节点缺少路径");
    }
    
    // 验证路径格式
    const std::string& path = node.GetPath();
    if (path.find("..") != std::string::npos) {
        AddError("导入路径不能包含上级目录引用(..)");
    }
    
    // 验证批量导入的名称限制
    if (node.IsBatchImport() && !node.GetName().empty()) {
        AddError("批量导入不支持指定具体名称");
    }
}

void ASTValidationVisitor::VisitNamespaceNode(NamespaceNode& node) {
    // 验证命名空间节点
    if (node.GetName().empty()) {
        AddError("命名空间节点缺少名称");
    }
    
    // 验证命名空间名称格式
    const std::string& name = node.GetName();
    std::vector<std::string> parts = Utils::StringUtils::Split(name, ".");
    for (const auto& part : parts) {
        if (part.empty()) {
            AddError("命名空间名称不能包含空的部分");
            break;
        }
        if (!Utils::StringUtils::IsAlpha(part[0])) {
            AddError("命名空间部分必须以字母开头: " + part);
            break;
        }
    }
    
    // 验证约束
    const auto& constraints = node.GetConstraints();
    for (const auto& constraint : constraints) {
        constraint->Accept(*this);
    }
    
    // 递归验证子节点
    for (const auto& child : node.GetChildren()) {
        child->Accept(*this);
    }
}

} // namespace AST
} // namespace CHTL