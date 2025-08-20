#include "HTMLGenerator.h"
#include "ASTNodeV2.h"
#include <algorithm>

namespace chtl {
namespace simple {

HTMLGenerator::HTMLGenerator() 
    : m_PrettyPrint(true), m_IndentSize(2), m_CurrentIndent(0), m_GenerateFullDocument(true) {
}

std::string HTMLGenerator::Generate(ASTNode* root) {
    // 清空之前的内容
    m_Output.str("");
    m_Styles.str("");
    m_Scripts.str("");
    m_CurrentIndent = 0;
    
    // 访问 AST
    root->Accept(this);
    
    // 如果不需要完整文档，直接返回主体内容
    if (!m_GenerateFullDocument) {
        return m_Output.str();
    }
    
    // 生成完整的 HTML 文档
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "  <meta charset=\"UTF-8\">\n";
    html << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html << "  <title>CHTL Generated Page</title>\n";
    
    // 添加样式
    if (m_Styles.str().length() > 0) {
        html << "  <style>\n";
        html << m_Styles.str();
        html << "  </style>\n";
    }
    
    html << "</head>\n";
    html << "<body>\n";
    
    // 添加主体内容
    html << m_Output.str();
    
    // 添加脚本
    if (m_Scripts.str().length() > 0) {
        html << "  <script>\n";
        html << m_Scripts.str();
        html << "  </script>\n";
    }
    
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

void HTMLGenerator::VisitDocument(DocumentNode* node) {
    // 访问所有子节点
    VisitChildren(node);
}

void HTMLGenerator::VisitElement(ElementNode* node) {
    std::string tag = node->GetTagName();
    bool isInline = IsInlineElement(tag);
    bool isSelfClosing = IsSelfClosingTag(tag);
    
    // 开始标签
    if (m_PrettyPrint && !isInline) {
        WriteIndent();
    }
    
    Write("<" + tag);
    
    // 属性
    for (const auto& attr : node->GetAttributes()) {
        Write(" " + attr.first + "=\"" + attr.second + "\"");
    }
    
    // 自闭合标签
    if (isSelfClosing && node->GetChildren().empty()) {
        Write(" />");
        if (m_PrettyPrint) {
            WriteLine();
        }
        return;
    }
    
    Write(">");
    
    // 检查是否有块级子元素
    bool hasBlockChildren = false;
    for (const auto& child : node->GetChildren()) {
        if (auto elem = dynamic_cast<ElementNode*>(child.get())) {
            if (!IsInlineElement(elem->GetTagName())) {
                hasBlockChildren = true;
                break;
            }
        }
    }
    
    if (m_PrettyPrint && (hasBlockChildren || !isInline)) {
        WriteLine();
        IncreaseIndent();
    }
    
    // 访问子节点
    VisitChildren(node);
    
    if (m_PrettyPrint && (hasBlockChildren || !isInline)) {
        DecreaseIndent();
        WriteIndent();
    }
    
    // 结束标签
    Write("</" + tag + ">");
    
    if (m_PrettyPrint) {
        WriteLine();
    }
}

void HTMLGenerator::VisitText(TextNode* node) {
    if (m_PrettyPrint && m_CurrentIndent > 0) {
        WriteIndent();
    }
    Write(node->GetText());
    if (m_PrettyPrint && m_CurrentIndent > 0) {
        WriteLine();
    }
}

void HTMLGenerator::VisitStyle(StyleNode* node) {
    m_Styles << "    " << node->GetCSS() << "\n";
}

void HTMLGenerator::VisitScript(ScriptNode* node) {
    m_Scripts << "    " << node->GetJavaScript() << "\n";
}

void HTMLGenerator::VisitTemplate(TemplateNode* node) {
    // 暂时跳过模板节点
}

void HTMLGenerator::VisitCustom(CustomNode* node) {
    // 暂时将自定义节点当作 div 处理
    if (m_PrettyPrint) {
        WriteIndent();
    }
    
    Write("<div class=\"" + node->GetName() + "\"");
    
    // 添加属性
    for (const auto& prop : node->GetProperties()) {
        Write(" data-" + prop.first + "=\"" + prop.second + "\"");
    }
    
    Write(">");
    
    if (m_PrettyPrint) {
        WriteLine();
        IncreaseIndent();
    }
    
    // 访问子节点
    VisitChildren(node);
    
    if (m_PrettyPrint) {
        DecreaseIndent();
        WriteIndent();
    }
    
    Write("</div>");
    
    if (m_PrettyPrint) {
        WriteLine();
    }
}

void HTMLGenerator::WriteIndent() {
    if (!m_PrettyPrint) return;
    m_Output << std::string(m_CurrentIndent * m_IndentSize, ' ');
}

void HTMLGenerator::Write(const std::string& text) {
    m_Output << text;
}

void HTMLGenerator::WriteLine(const std::string& text) {
    m_Output << text;
    if (m_PrettyPrint) {
        m_Output << "\n";
    }
}

bool HTMLGenerator::IsInlineElement(const std::string& tag) {
    static std::vector<std::string> inlineTags = {
        "a", "abbr", "b", "bdi", "bdo", "br", "cite", "code", "data",
        "dfn", "em", "i", "kbd", "mark", "q", "s", "samp", "small",
        "span", "strong", "sub", "sup", "time", "u", "var", "wbr",
        "img", "input", "button", "label", "select", "textarea"
    };
    
    return std::find(inlineTags.begin(), inlineTags.end(), tag) != inlineTags.end();
}

bool HTMLGenerator::IsSelfClosingTag(const std::string& tag) {
    static std::vector<std::string> selfClosingTags = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "link", "meta", "param", "source", "track", "wbr"
    };
    
    return std::find(selfClosingTags.begin(), selfClosingTags.end(), tag) != selfClosingTags.end();
}

} // namespace simple
} // namespace chtl