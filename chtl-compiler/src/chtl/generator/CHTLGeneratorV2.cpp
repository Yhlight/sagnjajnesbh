#include "CHTLGeneratorV2.h"
#include <algorithm>
#include <regex>

namespace chtl {

// 内联元素列表
static const std::vector<std::string> s_InlineElements = {
    "a", "abbr", "b", "bdi", "bdo", "br", "cite", "code", "data",
    "dfn", "em", "i", "kbd", "mark", "q", "s", "samp", "small",
    "span", "strong", "sub", "sup", "time", "u", "var", "wbr",
    "img", "input", "button", "label", "select", "textarea"
};

// 自闭合标签列表
static const std::vector<std::string> s_SelfClosingTags = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

CHTLGeneratorV2::CHTLGeneratorV2() 
    : m_PrettyPrint(true), m_IndentSize(2), m_CurrentIndent(0), 
      m_GenerateFullDocument(true) {
}

std::string CHTLGeneratorV2::Generate(std::shared_ptr<ast::ASTNode> root) {
    // 清空状态
    m_Output.str("");
    m_GlobalStyles.str("");
    m_GlobalScripts.str("");
    m_LocalStyleRules.clear();
    m_Templates.clear();
    m_Variables.clear();
    m_Configuration.clear();
    m_CurrentIndent = 0;
    
    // 访问 AST
    root->Accept(this);
    
    // 如果不需要完整文档，直接返回内容
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
    
    // 标题（可以从配置中获取）
    auto titleIt = m_Configuration.find("title");
    if (titleIt != m_Configuration.end()) {
        html << "  <title>" << EscapeHTML(titleIt->second) << "</title>\n";
    } else {
        html << "  <title>CHTL Generated Page</title>\n";
    }
    
    // 样式
    std::string styleContent = GenerateStyleContent();
    if (!styleContent.empty()) {
        html << "  <style>\n";
        html << styleContent;
        html << "  </style>\n";
    }
    
    html << "</head>\n";
    html << "<body>\n";
    
    // 主体内容
    html << m_Output.str();
    
    // 脚本
    std::string scriptContent = GenerateScriptContent();
    if (!scriptContent.empty()) {
        html << "  <script>\n";
        html << scriptContent;
        html << "  </script>\n";
    }
    
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

// 访问者方法实现

void CHTLGeneratorV2::VisitDocument(ast::DocumentNode* node) {
    VisitChildren(node);
}

void CHTLGeneratorV2::VisitComment(ast::CommentNode* node) {
    // 只有 HTML 注释会被输出
    if (node->GetType() == ast::CommentNode::HTML_COMMENT) {
        WriteIndent();
        Write("<!-- " + node->GetContent() + " -->");
        if (m_PrettyPrint) WriteLine();
    }
}

void CHTLGeneratorV2::VisitText(ast::TextNode* node) {
    std::string content = ReplaceVariables(node->GetContent());
    Write(EscapeHTML(content));
}

void CHTLGeneratorV2::VisitElement(ast::ElementNode* node) {
    const std::string& tag = node->GetTagName();
    bool isInline = IsInlineElement(tag);
    bool isSelfClosing = IsSelfClosingTag(tag);
    
    // 开始标签
    if (m_PrettyPrint && !isInline) {
        WriteIndent();
    }
    
    Write("<" + tag);
    
    // 属性
    for (const auto& attr : node->GetAttributes()) {
        Write(" " + attr.first + "=\"" + EscapeHTML(attr.second) + "\"");
    }
    
    // 处理局部样式生成的内联样式
    for (const auto& child : node->GetChildren()) {
        if (auto style = std::dynamic_pointer_cast<ast::StyleNode>(child)) {
            if (style->GetType() == ast::StyleNode::LOCAL) {
                auto& inlineProps = style->GetInlineProperties();
                if (!inlineProps.empty()) {
                    Write(" style=\"");
                    bool first = true;
                    for (const auto& prop : inlineProps) {
                        if (!first) Write(" ");
                        Write(prop.first + ": " + prop.second + ";");
                        first = false;
                    }
                    Write("\"");
                }
            }
        }
    }
    
    // 自闭合标签
    if (isSelfClosing && node->GetChildren().empty()) {
        Write(" />");
        if (m_PrettyPrint) WriteLine();
        return;
    }
    
    Write(">");
    
    // 检查是否有块级子元素
    bool hasBlockChildren = false;
    for (const auto& child : node->GetChildren()) {
        if (auto elem = std::dynamic_pointer_cast<ast::ElementNode>(child)) {
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
    
    // 访问子节点（跳过已处理的局部样式）
    for (const auto& child : node->GetChildren()) {
        if (auto style = std::dynamic_pointer_cast<ast::StyleNode>(child)) {
            if (style->GetType() == ast::StyleNode::LOCAL) {
                ProcessLocalStyle(style.get());
                continue;
            }
        }
        if (auto script = std::dynamic_pointer_cast<ast::ScriptNode>(child)) {
            if (script->GetType() == ast::ScriptNode::LOCAL) {
                ProcessLocalScript(script.get());
                continue;
            }
        }
        child->Accept(this);
    }
    
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

void CHTLGeneratorV2::VisitStyle(ast::StyleNode* node) {
    if (node->GetType() == ast::StyleNode::GLOBAL) {
        CollectGlobalStyle(node);
    }
    // 局部样式在元素访问时处理
}

void CHTLGeneratorV2::VisitScript(ast::ScriptNode* node) {
    if (node->GetType() == ast::ScriptNode::GLOBAL) {
        CollectGlobalScript(node);
    }
    // 局部脚本在元素访问时处理
}

void CHTLGeneratorV2::VisitTemplate(ast::TemplateNode* node) {
    RegisterTemplate(node);
    // 模板定义不生成输出
}

void CHTLGeneratorV2::VisitCustom(ast::CustomNode* node) {
    // 检查是否是模板实例
    auto tmplIt = m_Templates.find(node->GetName());
    if (tmplIt != m_Templates.end()) {
        auto tmpl = tmplIt->second;
        
        // 根据模板类型处理
        if (node->GetType() == ast::CustomNode::STYLE && 
            tmpl->GetType() == ast::TemplateNode::STYLE) {
            // 样式模板实例化
            for (const auto& child : tmpl->GetChildren()) {
                if (auto styleNode = std::dynamic_pointer_cast<ast::StyleNode>(child)) {
                    // 合并样式属性
                    for (const auto& prop : styleNode->GetInlineProperties()) {
                        // TODO: 这些属性应该被添加到父元素的style中
                    }
                }
            }
        } else if (node->GetType() == ast::CustomNode::ELEMENT && 
                   tmpl->GetType() == ast::TemplateNode::ELEMENT) {
            // 元素模板实例化 - 直接访问模板的子节点
            for (const auto& child : tmpl->GetChildren()) {
                child->Accept(this);
            }
        } else if (node->GetType() == ast::CustomNode::VAR && 
                   tmpl->GetType() == ast::TemplateNode::VAR) {
            // 变量模板实例化
            for (const auto& child : tmpl->GetChildren()) {
                if (auto varNode = std::dynamic_pointer_cast<ast::VarNode>(child)) {
                    m_Variables[varNode->GetName()] = varNode->GetValue();
                }
            }
        }
        
        // 处理自定义节点的子节点（覆盖或扩展）
        VisitChildren(node);
    } else {
        // 未找到模板，作为普通元素处理
        WriteIndent();
        Write("<div class=\"custom-" + node->GetName() + "\">");
        if (m_PrettyPrint) {
            WriteLine();
            IncreaseIndent();
        }
        
        VisitChildren(node);
        
        if (m_PrettyPrint) {
            DecreaseIndent();
            WriteIndent();
        }
        Write("</div>");
        if (m_PrettyPrint) WriteLine();
    }
}

void CHTLGeneratorV2::VisitVar(ast::VarNode* node) {
    // 注册变量
    if (!node->GetValue().empty()) {
        m_Variables[node->GetName()] = node->GetValue();
    } else if (node->GetExpression()) {
        // TODO: 计算表达式值
        m_Variables[node->GetName()] = "[expression]";
    }
}

void CHTLGeneratorV2::VisitInherit(ast::InheritNode* node) {
    // TODO: 实现继承
}

void CHTLGeneratorV2::VisitDelete(ast::DeleteNode* node) {
    // TODO: 实现删除
}

void CHTLGeneratorV2::VisitInsert(ast::InsertNode* node) {
    // TODO: 实现插入
}

void CHTLGeneratorV2::VisitImport(ast::ImportNode* node) {
    // TODO: 实现导入
    // 这里应该读取文件并根据类型处理
}

void CHTLGeneratorV2::VisitNamespace(ast::NamespaceNode* node) {
    std::string oldNamespace = m_CurrentNamespace;
    m_CurrentNamespace = node->GetName();
    
    VisitChildren(node);
    
    m_CurrentNamespace = oldNamespace;
}

void CHTLGeneratorV2::VisitConfiguration(ast::ConfigurationNode* node) {
    // 收集配置
    for (const auto& config : node->GetConfigs()) {
        m_Configuration[config.first] = config.second;
    }
}

void CHTLGeneratorV2::VisitOrigin(ast::OriginNode* node) {
    switch (node->GetType()) {
        case ast::OriginNode::HTML:
            // 直接输出HTML内容，保持缩进
            if (m_PrettyPrint) {
                WriteIndent();
            }
            Write(node->GetContent());
            if (m_PrettyPrint) {
                WriteLine();
            }
            break;
        case ast::OriginNode::STYLE:
            m_GlobalStyles << node->GetContent() << "\n";
            break;
        case ast::OriginNode::JAVASCRIPT:
            m_GlobalScripts << node->GetContent() << "\n";
            break;
        case ast::OriginNode::CUSTOM:
            // TODO: 处理自定义类型
            break;
    }
}

void CHTLGeneratorV2::VisitExcept(ast::ExceptNode* node) {
    // TODO: 实现约束例外
}

// 样式处理

void CHTLGeneratorV2::ProcessLocalStyle(ast::StyleNode* style) {
    // 首先处理样式模板使用（@Style引用）
    std::unordered_map<std::string, std::string> mergedProperties;
    
    // 收集模板样式属性
    for (const auto& child : style->GetChildren()) {
        if (auto custom = std::dynamic_pointer_cast<ast::CustomNode>(child)) {
            if (custom->GetType() == ast::CustomNode::STYLE) {
                // 查找对应的模板
                auto tmplIt = m_Templates.find(custom->GetName());
                if (tmplIt != m_Templates.end()) {
                    auto tmpl = tmplIt->second;
                    if (tmpl->GetType() == ast::TemplateNode::STYLE) {
                        // 合并模板中的样式属性
                        for (const auto& tmplChild : tmpl->GetChildren()) {
                            if (auto tmplStyle = std::dynamic_pointer_cast<ast::StyleNode>(tmplChild)) {
                                for (const auto& prop : tmplStyle->GetInlineProperties()) {
                                    mergedProperties[prop.first] = prop.second;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // 合并本地内联属性（覆盖模板属性）
    for (const auto& prop : style->GetInlineProperties()) {
        mergedProperties[prop.first] = prop.second;
    }
    
    // 将合并后的属性添加到内联样式
    style->ClearInlineProperties();
    for (const auto& prop : mergedProperties) {
        style->AddInlineProperty(prop.first, prop.second);
    }
    
    // 收集需要提升到全局的规则
    for (const auto& rule : style->GetRules()) {
        std::string selector = rule.first;
        
        // 替换 & 为实际的选择器
        if (!style->GetAutoClassName().empty()) {
            size_t pos = 0;
            while ((pos = selector.find("&", pos)) != std::string::npos) {
                selector.replace(pos, 1, "." + style->GetAutoClassName());
                pos += style->GetAutoClassName().length() + 1;
            }
        } else if (!style->GetAutoId().empty()) {
            size_t pos = 0;
            while ((pos = selector.find("&", pos)) != std::string::npos) {
                selector.replace(pos, 1, "#" + style->GetAutoId());
                pos += style->GetAutoId().length() + 1;
            }
        }
        
        m_LocalStyleRules.push_back(selector + " { " + rule.second + " }");
    }
}

void CHTLGeneratorV2::CollectGlobalStyle(ast::StyleNode* style) {
    for (const auto& rule : style->GetRules()) {
        m_GlobalStyles << rule.first << " { " << rule.second << " }\n";
    }
}

std::string CHTLGeneratorV2::GenerateStyleContent() {
    std::stringstream styles;
    
    // 全局样式
    if (m_GlobalStyles.str().length() > 0) {
        styles << m_GlobalStyles.str();
    }
    
    // 局部样式规则
    for (const auto& rule : m_LocalStyleRules) {
        styles << "    " << rule << "\n";
    }
    
    return styles.str();
}

// 脚本处理

void CHTLGeneratorV2::ProcessLocalScript(ast::ScriptNode* script) {
    // 局部脚本直接添加到全局脚本中
    m_GlobalScripts << script->GetContent() << "\n";
}

void CHTLGeneratorV2::CollectGlobalScript(ast::ScriptNode* script) {
    m_GlobalScripts << script->GetContent() << "\n";
}

std::string CHTLGeneratorV2::GenerateScriptContent() {
    return m_GlobalScripts.str();
}

// 模板处理

void CHTLGeneratorV2::RegisterTemplate(ast::TemplateNode* tmpl) {
    m_Templates[tmpl->GetName()] = std::make_shared<ast::TemplateNode>(*tmpl);
}

// 辅助方法

void CHTLGeneratorV2::Write(const std::string& text) {
    m_Output << text;
}

void CHTLGeneratorV2::WriteLine(const std::string& text) {
    m_Output << text;
    if (m_PrettyPrint) {
        m_Output << "\n";
    }
}

void CHTLGeneratorV2::WriteIndent() {
    if (m_PrettyPrint) {
        m_Output << std::string(m_CurrentIndent * m_IndentSize, ' ');
    }
}

void CHTLGeneratorV2::VisitChildren(ast::ASTNode* node) {
    for (const auto& child : node->GetChildren()) {
        child->Accept(this);
    }
}

bool CHTLGeneratorV2::IsInlineElement(const std::string& tag) const {
    return std::find(s_InlineElements.begin(), s_InlineElements.end(), tag) 
           != s_InlineElements.end();
}

bool CHTLGeneratorV2::IsSelfClosingTag(const std::string& tag) const {
    return std::find(s_SelfClosingTags.begin(), s_SelfClosingTags.end(), tag) 
           != s_SelfClosingTags.end();
}

std::string CHTLGeneratorV2::EscapeHTML(const std::string& text) const {
    std::string escaped;
    escaped.reserve(text.length());
    
    for (char c : text) {
        switch (c) {
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '&': escaped += "&amp;"; break;
            case '"': escaped += "&quot;"; break;
            case '\'': escaped += "&#39;"; break;
            default: escaped += c; break;
        }
    }
    
    return escaped;
}

std::string CHTLGeneratorV2::ReplaceVariables(const std::string& text) {
    std::string result = text;
    
    // 简单的变量替换 $varname
    for (const auto& var : m_Variables) {
        std::string placeholder = "$" + var.first;
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    return result;
}

} // namespace chtl