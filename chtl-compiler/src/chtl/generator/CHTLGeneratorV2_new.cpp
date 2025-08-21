#include "CHTLGeneratorV2.h"
#include <sstream>
#include <iomanip>

namespace chtl {

CHTLGeneratorV2::CHTLGeneratorV2() : m_PrettyPrint(true), m_Minify(false), m_IndentLevel(0) {}

std::string CHTLGeneratorV2::Generate(std::shared_ptr<ast::DocumentNode> document, const GeneratorOptions& options) {
    m_PrettyPrint = options.prettyPrint;
    m_Minify = options.minify;
    m_FragmentOnly = options.fragmentOnly;
    m_Output.str("");
    m_GlobalStyles.str("");
    m_GlobalScripts.str("");
    m_LocalStyleRules.clear();
    m_Templates.clear();
    m_Variables.clear();
    m_IndentLevel = 0;
    
    // 访问文档收集信息
    document->Accept(this);
    
    // 生成最终输出
    return GenerateFinalOutput();
}

std::string CHTLGeneratorV2::GenerateFinalOutput() {
    if (m_FragmentOnly) {
        return m_Output.str();
    }
    
    std::stringstream final_output;
    
    // HTML5 文档类型
    final_output << "<!DOCTYPE html>\n";
    final_output << "<html>\n";
    final_output << "<head>\n";
    final_output << "    <meta charset=\"UTF-8\">\n";
    
    // 生成样式
    std::string styles = GenerateStyleContent();
    if (!styles.empty()) {
        final_output << "    <style>\n";
        if (!m_Minify) {
            std::istringstream styleStream(styles);
            std::string line;
            while (std::getline(styleStream, line)) {
                final_output << "        " << line << "\n";
            }
        } else {
            final_output << styles;
        }
        final_output << "    </style>\n";
    }
    
    final_output << "</head>\n";
    final_output << "<body>\n";
    
    // 主体内容
    std::istringstream contentStream(m_Output.str());
    std::string line;
    while (std::getline(contentStream, line)) {
        if (!line.empty()) {
            final_output << "    " << line << "\n";
        }
    }
    
    // 生成脚本
    std::string scripts = GenerateScriptContent();
    if (!scripts.empty()) {
        final_output << "    <script>\n";
        if (!m_Minify) {
            std::istringstream scriptStream(scripts);
            while (std::getline(scriptStream, line)) {
                final_output << "        " << line << "\n";
            }
        } else {
            final_output << scripts;
        }
        final_output << "    </script>\n";
    }
    
    final_output << "</body>\n";
    final_output << "</html>\n";
    
    return final_output.str();
}

// 访问器方法实现

void CHTLGeneratorV2::VisitDocument(ast::DocumentNode* node) {
    VisitChildren(node);
}

void CHTLGeneratorV2::VisitText(ast::TextNode* node) {
    WriteIndent();
    Write(EscapeHtml(node->GetContent()));
    if (m_PrettyPrint) WriteLine();
}

void CHTLGeneratorV2::VisitElement(ast::ElementNode* node) {
    const std::string& tag = node->GetTagName();
    
    // 是否是自闭合标签
    bool isSelfClosing = (tag == "br" || tag == "hr" || tag == "img" || 
                         tag == "input" || tag == "meta" || tag == "link");
    
    // 是否是内联元素
    bool isInline = IsInlineElement(tag);
    
    WriteIndent();
    Write("<" + tag);
    
    // 属性
    for (const auto& attr : node->GetAttributes()) {
        Write(" " + attr.first);
        if (!attr.second.empty()) {
            Write("=\"" + EscapeAttribute(attr.second) + "\"");
        }
    }
    
    // 内联样式（来自局部样式块）
    std::string inlineStyle;
    for (const auto& child : node->GetChildren()) {
        if (auto style = std::dynamic_pointer_cast<ast::StyleNode>(child)) {
            if (style->GetType() == ast::StyleNode::LOCAL) {
                // 收集模板样式
                std::unordered_map<std::string, std::string> mergedProps;
                
                // 处理样式模板引用
                for (const auto& styleChild : style->GetChildren()) {
                    if (auto custom = std::dynamic_pointer_cast<ast::CustomNode>(styleChild)) {
                        if (custom->GetType() == ast::CustomNode::STYLE) {
                            auto tmplIt = m_Templates.find(custom->GetName());
                            if (tmplIt != m_Templates.end()) {
                                auto tmpl = tmplIt->second;
                                if (tmpl->GetType() == ast::TemplateNode::STYLE) {
                                    for (const auto& tmplChild : tmpl->GetChildren()) {
                                        if (auto tmplStyle = std::dynamic_pointer_cast<ast::StyleNode>(tmplChild)) {
                                            for (const auto& prop : tmplStyle->GetInlineProperties()) {
                                                mergedProps[prop.first] = prop.second;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                // 合并本地内联属性
                for (const auto& prop : style->GetInlineProperties()) {
                    mergedProps[prop.first] = prop.second;
                }
                
                // 生成内联样式字符串
                for (const auto& prop : mergedProps) {
                    if (!inlineStyle.empty()) inlineStyle += "; ";
                    inlineStyle += prop.first + ": " + prop.second;
                }
                
                break;
            }
        }
    }
    
    if (!inlineStyle.empty()) {
        // 检查是否已有style属性
        auto styleAttr = node->GetAttributes().find("style");
        if (styleAttr != node->GetAttributes().end()) {
            Write(" style=\"" + styleAttr->second + "; " + inlineStyle + "\"");
        } else {
            Write(" style=\"" + inlineStyle + "\"");
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
            // 样式模板实例化 - 在父样式块中处理
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
    }
}

void CHTLGeneratorV2::VisitAttribute(ast::AttributeNode* node) {
    // 属性在元素中处理
}

void CHTLGeneratorV2::VisitComment(ast::CommentNode* node) {
    if (node->GetType() == ast::CommentNode::HTML_COMMENT) {
        WriteIndent();
        Write("<!-- " + node->GetContent() + " -->");
        if (m_PrettyPrint) WriteLine();
    }
    // 其他类型的注释不输出到HTML
}

void CHTLGeneratorV2::VisitVar(ast::VarNode* node) {
    m_Variables[node->GetName()] = node->GetValue();
    // 变量定义不生成输出
}

void CHTLGeneratorV2::VisitImport(ast::ImportNode* node) {
    // TODO: 实现导入逻辑
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
    m_Templates[tmpl->GetName()] = tmpl;
}

// 辅助方法

void CHTLGeneratorV2::Write(const std::string& text) {
    m_Output << text;
}

void CHTLGeneratorV2::WriteLine(const std::string& text) {
    m_Output << text << "\n";
}

void CHTLGeneratorV2::WriteIndent() {
    if (!m_PrettyPrint || m_Minify) return;
    for (int i = 0; i < m_IndentLevel; ++i) {
        m_Output << "    ";
    }
}

void CHTLGeneratorV2::IncreaseIndent() {
    m_IndentLevel++;
}

void CHTLGeneratorV2::DecreaseIndent() {
    if (m_IndentLevel > 0) m_IndentLevel--;
}

std::string CHTLGeneratorV2::EscapeHtml(const std::string& text) {
    std::string result;
    for (char c : text) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c;
        }
    }
    return result;
}

std::string CHTLGeneratorV2::EscapeAttribute(const std::string& text) {
    return EscapeHtml(text);
}

bool CHTLGeneratorV2::IsInlineElement(const std::string& tag) {
    return tag == "span" || tag == "a" || tag == "strong" || tag == "em" ||
           tag == "b" || tag == "i" || tag == "u" || tag == "code" ||
           tag == "small" || tag == "sub" || tag == "sup";
}

void CHTLGeneratorV2::VisitChildren(ast::ASTNode* node) {
    for (const auto& child : node->GetChildren()) {
        child->Accept(this);
    }
}

// 其他访问器实现...

void CHTLGeneratorV2::VisitInherit(ast::InheritNode* node) {
    // TODO: 实现继承逻辑
}

void CHTLGeneratorV2::VisitDelete(ast::DeleteNode* node) {
    // TODO: 实现删除逻辑
}

void CHTLGeneratorV2::VisitInsert(ast::InsertNode* node) {
    // TODO: 实现插入逻辑
    for (const auto& child : node->GetChildren()) {
        child->Accept(this);
    }
}

} // namespace chtl