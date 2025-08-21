#include "Generator.h"
#include <algorithm>
#include <vector>

namespace chtl {
namespace simple {

Generator::Generator() 
    : m_PrettyPrint(true), m_IndentSize(2), m_GenerateFullDocument(true) {
}

std::string Generator::Generate(std::shared_ptr<ASTNode> ast) {
    if (!ast) return "";
    
    // 清空之前的样式和脚本
    m_Styles.str("");
    m_Scripts.str("");
    
    std::stringstream body;
    
    // 生成主体内容
    for (const auto& child : ast->children) {
        GenerateNode(child, body, 0);
    }
    
    // 如果不需要完整文档，直接返回主体内容
    if (!m_GenerateFullDocument) {
        return body.str();
    }
    
    // 生成完整的HTML文档
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << Indent(1) << "<meta charset=\"UTF-8\">\n";
    html << Indent(1) << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << Indent(1) << "<title>CHTL Generated Page</title>\n";
    
    // 添加样式
    if (m_Styles.str().length() > 0) {
        html << Indent(1) << "<style>\n";
        html << m_Styles.str();
        html << Indent(1) << "</style>\n";
    }
    
    html << "</head>\n";
    html << "<body>\n";
    
    // 添加主体内容
    html << body.str();
    
    // 添加脚本
    if (m_Scripts.str().length() > 0) {
        html << Indent(1) << "<script>\n";
        html << m_Scripts.str();
        html << Indent(1) << "</script>\n";
    }
    
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

void Generator::GenerateNode(std::shared_ptr<ASTNode> node, std::stringstream& output, int indent) {
    switch (node->type) {
        case ASTNodeType::ELEMENT:
            GenerateElement(node, output, indent);
            break;
        case ASTNodeType::TEXT:
            GenerateText(node, output);
            break;
        case ASTNodeType::STYLE:
            GenerateStyle(node, output);
            break;
        case ASTNodeType::SCRIPT:
            GenerateScript(node, output);
            break;
        default:
            break;
    }
}

void Generator::GenerateElement(std::shared_ptr<ASTNode> node, std::stringstream& output, int indent) {
    std::string tag = node->value;
    bool isInline = IsInlineElement(tag);
    bool isSelfClosing = IsSelfClosingTag(tag);
    
    // 开始标签
    if (m_PrettyPrint && !isInline) {
        output << Indent(indent);
    }
    output << "<" << tag;
    
    // 属性
    for (const auto& attr : node->attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }
    
    // 自闭合标签
    if (isSelfClosing && node->children.empty()) {
        output << " />";
        if (m_PrettyPrint) {
            output << "\n";
        }
        return;
    }
    
    output << ">";
    
    // 子元素
    bool hasBlockChildren = false;
    for (const auto& child : node->children) {
        if (child->type == ASTNodeType::ELEMENT && 
            !IsInlineElement(child->value)) {
            hasBlockChildren = true;
            break;
        }
    }
    
    if (m_PrettyPrint && (hasBlockChildren || !isInline)) {
        output << "\n";
    }
    
    // 生成子元素
    for (const auto& child : node->children) {
        int childIndent = (m_PrettyPrint && hasBlockChildren) ? indent + 1 : 0;
        GenerateNode(child, output, childIndent);
    }
    
    // 结束标签
    if (m_PrettyPrint && (hasBlockChildren || !isInline)) {
        output << Indent(indent);
    }
    output << "</" << tag << ">";
    
    if (m_PrettyPrint) {
        output << "\n";
    }
}

void Generator::GenerateText(std::shared_ptr<ASTNode> node, std::stringstream& output) {
    output << node->value;
}

void Generator::GenerateStyle(std::shared_ptr<ASTNode> node, std::stringstream& output) {
    // 将样式添加到样式收集器
    m_Styles << Indent(2) << node->value << "\n";
}

void Generator::GenerateScript(std::shared_ptr<ASTNode> node, std::stringstream& output) {
    // 将脚本添加到脚本收集器
    m_Scripts << Indent(2) << node->value << "\n";
}

std::string Generator::Indent(int level) {
    if (!m_PrettyPrint) return "";
    return std::string(level * m_IndentSize, ' ');
}

bool Generator::IsInlineElement(const std::string& tag) {
    static std::vector<std::string> inlineTags = {
        "a", "abbr", "b", "bdi", "bdo", "br", "cite", "code", "data",
        "dfn", "em", "i", "kbd", "mark", "q", "s", "samp", "small",
        "span", "strong", "sub", "sup", "time", "u", "var", "wbr",
        "img", "input", "button", "label", "select", "textarea"
    };
    
    return std::find(inlineTags.begin(), inlineTags.end(), tag) != inlineTags.end();
}

bool Generator::IsSelfClosingTag(const std::string& tag) {
    static std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return std::find(selfClosingTags.begin(), selfClosingTags.end(), tag) != selfClosingTags.end();
}

} // namespace simple
} // namespace chtl