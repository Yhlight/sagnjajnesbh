#include "merger/ResultMerger.h"
#include "utils/Logger.h"
#include <sstream>
#include <algorithm>

namespace chtl {
namespace merger {

ResultMerger::ResultMerger() 
    : m_IndentLevel(0)
    , m_Minify(false)
    , m_InlineStyles(false)
    , m_InlineScripts(false) {}

ResultMerger::~ResultMerger() = default;

std::string ResultMerger::MergeToHTML(const MergeRequest& request) {
    // 重置状态
    Reset();
    
    m_Minify = request.Minify;
    m_InlineStyles = request.InlineStyles;
    m_InlineScripts = request.InlineScripts;
    
    std::stringstream html;
    
    // DOCTYPE
    html << "<!DOCTYPE html>\n";
    
    // HTML标签
    html << "<html";
    if (!request.Language.empty()) {
        html << " lang=\"" << request.Language << "\"";
    }
    html << ">\n";
    
    // Head
    html << "<head>\n";
    m_IndentLevel++;
    
    // Meta标签
    Indent(html);
    html << "<meta charset=\"" << (request.Charset.empty() ? "UTF-8" : request.Charset) << "\">\n";
    
    if (!request.Viewport.empty()) {
        Indent(html);
        html << "<meta name=\"viewport\" content=\"" << request.Viewport << "\">\n";
    }
    
    // 标题
    if (!request.Title.empty()) {
        Indent(html);
        html << "<title>" << EscapeHTML(request.Title) << "</title>\n";
    }
    
    // 其他Meta信息
    for (const auto& meta : request.MetaTags) {
        Indent(html);
        html << "<meta";
        for (const auto& attr : meta) {
            html << " " << attr.first << "=\"" << EscapeHTML(attr.second) << "\"";
        }
        html << ">\n";
    }
    
    // CSS
    if (!request.Styles.empty()) {
        if (m_InlineStyles) {
            Indent(html);
            html << "<style>\n";
            m_IndentLevel++;
            
            for (const auto& style : request.Styles) {
                if (!m_Minify) {
                    Indent(html);
                    html << "/* " << style.Source << " */\n";
                }
                html << style.Content;
                if (!style.Content.empty() && style.Content.back() != '\n') {
                    html << "\n";
                }
            }
            
            m_IndentLevel--;
            Indent(html);
            html << "</style>\n";
        } else {
            // 外部CSS文件
            for (const auto& style : request.Styles) {
                Indent(html);
                html << "<link rel=\"stylesheet\" href=\"" 
                     << style.Source << ".css\">\n";
            }
        }
    }
    
    // Head中的脚本
    for (const auto& script : request.HeadScripts) {
        WriteScript(html, script);
    }
    
    m_IndentLevel--;
    html << "</head>\n";
    
    // Body
    html << "<body>\n";
    m_IndentLevel++;
    
    // Body内容
    if (!request.BodyContent.empty()) {
        html << request.BodyContent;
        if (request.BodyContent.back() != '\n') {
            html << "\n";
        }
    }
    
    // Body脚本
    for (const auto& script : request.BodyScripts) {
        WriteScript(html, script);
    }
    
    m_IndentLevel--;
    html << "</body>\n";
    html << "</html>";
    
    std::string result = html.str();
    
    // 后处理
    if (m_Minify) {
        result = MinifyHTML(result);
    }
    
    return result;
}

bool ResultMerger::MergeAndOutput(const std::string& outputFile,
                                const std::vector<CompileOutput>& outputs) {
    MergeRequest request;
    
    // 从编译输出构建合并请求
    for (const auto& output : outputs) {
        switch (output.Type) {
            case OutputType::HTML:
                request.BodyContent += output.Content;
                break;
                
            case OutputType::CSS:
                request.Styles.push_back({output.Source, output.Content});
                break;
                
            case OutputType::JAVASCRIPT:
                request.BodyScripts.push_back({output.Source, output.Content, false, false});
                break;
                
            default:
                utils::Logger::GetInstance().Warning("未知的输出类型");
        }
    }
    
    // 生成HTML
    std::string html = MergeToHTML(request);
    
    // 写入文件
    std::ofstream file(outputFile);
    if (!file) {
        utils::Logger::GetInstance().Error("无法打开输出文件: " + outputFile);
        return false;
    }
    
    file << html;
    file.close();
    
    utils::Logger::GetInstance().Info("成功生成HTML文件: " + outputFile);
    return true;
}

void ResultMerger::Reset() {
    m_Styles.clear();
    m_Scripts.clear();
    m_IndentLevel = 0;
    m_Minify = false;
    m_InlineStyles = false;
    m_InlineScripts = false;
}

void ResultMerger::AddStyle(const std::string& css, const std::string& source) {
    m_Styles.push_back({source, css});
}

void ResultMerger::AddScript(const std::string& js, const std::string& source, bool isModule) {
    m_Scripts.push_back({source, js, isModule, false});
}

void ResultMerger::SetOption(const std::string& name, const std::string& value) {
    if (name == "minify") {
        m_Minify = (value == "true" || value == "1");
    } else if (name == "inline-styles") {
        m_InlineStyles = (value == "true" || value == "1");
    } else if (name == "inline-scripts") {
        m_InlineScripts = (value == "true" || value == "1");
    }
}

void ResultMerger::Indent(std::stringstream& stream) {
    if (!m_Minify) {
        for (int i = 0; i < m_IndentLevel; ++i) {
            stream << "  ";
        }
    }
}

void ResultMerger::WriteScript(std::stringstream& html, const ScriptInfo& script) {
    Indent(html);
    html << "<script";
    
    if (script.IsModule) {
        html << " type=\"module\"";
    }
    
    if (script.IsAsync) {
        html << " async";
    }
    
    if (m_InlineScripts) {
        html << ">\n";
        m_IndentLevel++;
        
        if (!m_Minify) {
            Indent(html);
            html << "/* " << script.Source << " */\n";
        }
        
        html << script.Content;
        
        if (!script.Content.empty() && script.Content.back() != '\n') {
            html << "\n";
        }
        
        m_IndentLevel--;
        Indent(html);
        html << "</script>\n";
    } else {
        html << " src=\"" << script.Source << ".js\"></script>\n";
    }
}

std::string ResultMerger::EscapeHTML(const std::string& text) {
    std::string escaped;
    escaped.reserve(text.size() * 1.2);
    
    for (char c : text) {
        switch (c) {
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '&': escaped += "&amp;"; break;
            case '"': escaped += "&quot;"; break;
            case '\'': escaped += "&#39;"; break;
            default: escaped += c;
        }
    }
    
    return escaped;
}

std::string ResultMerger::MinifyHTML(const std::string& html) {
    std::string minified;
    minified.reserve(html.size());
    
    bool inTag = false;
    bool inQuote = false;
    char quoteChar = '\0';
    bool lastWasSpace = false;
    bool inPreformatted = false;
    
    for (size_t i = 0; i < html.length(); ++i) {
        char c = html[i];
        char next = (i + 1 < html.length()) ? html[i + 1] : '\0';
        
        // 处理标签
        if (!inQuote && c == '<') {
            inTag = true;
            
            // 检查是否是pre标签
            if (i + 4 < html.length() && 
                html.substr(i, 5) == "<pre>") {
                inPreformatted = true;
            } else if (i + 5 < html.length() && 
                      html.substr(i, 6) == "</pre>") {
                inPreformatted = false;
            }
        } else if (!inQuote && c == '>') {
            inTag = false;
        }
        
        // 处理引号
        if (inTag && (c == '"' || c == '\'')) {
            if (!inQuote) {
                inQuote = true;
                quoteChar = c;
            } else if (c == quoteChar && html[i-1] != '\\') {
                inQuote = false;
            }
        }
        
        // 在预格式化文本中保留所有空白
        if (inPreformatted || inQuote) {
            minified += c;
            lastWasSpace = false;
            continue;
        }
        
        // 压缩空白
        if (std::isspace(c)) {
            if (!lastWasSpace && !minified.empty()) {
                // 在某些情况下需要保留空格
                char last = minified.back();
                if (inTag || (last != '>' && next != '<')) {
                    minified += ' ';
                    lastWasSpace = true;
                }
            }
            continue;
        }
        
        minified += c;
        lastWasSpace = false;
    }
    
    return minified;
}

std::string ResultMerger::ProcessResourcePaths(const std::string& html,
                                              const std::string& basePath) {
    // TODO: 实现资源路径处理
    return html;
}

} // namespace merger
} // namespace chtl