#include "CommentNode.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace chtl {
namespace ast {

// CommentNode 实现
CommentNode::CommentNode(const std::string& content, CommentType type, CommentContext context)
    : content_(content), type_(type), context_(context) {}

CommentNode::CommentNode(const std::string& content, CommentType type)
    : content_(content), type_(type), context_(CommentContext::UNKNOWN) {}

std::string CommentNode::generateComment() const {
    return generateComment(context_);
}

std::string CommentNode::generateComment(CommentContext target_context) const {
    switch (target_context) {
        case CommentContext::CHTL:
        case CommentContext::HTML:
            // 生成器注释在CHTL上下文中保持原样
            if (type_ == CommentType::GENERATOR) {
                return generateGeneratorComment();
            }
            return generateHTMLComment();
            
        case CommentContext::CSS:
            return generateCSSComment();
            
        case CommentContext::CHTL_JS:
        case CommentContext::JAVASCRIPT:
            if (type_ == CommentType::MULTI_LINE) {
                return generateMultiLineComment();
            } else {
                return generateSingleLineComment();
            }
            
        case CommentContext::UNKNOWN:
        default:
            // 根据原始类型生成
            switch (type_) {
                case CommentType::SINGLE_LINE:
                    return generateSingleLineComment();
                case CommentType::MULTI_LINE:
                    return generateMultiLineComment();
                case CommentType::GENERATOR:
                    return generateGeneratorComment();
                case CommentType::HTML:
                    return generateHTMLComment();
                case CommentType::CSS:
                    return generateCSSComment();
                default:
                    return generateSingleLineComment();
            }
    }
}

std::string CommentNode::generateSingleLineComment() const {
    std::string clean_content = cleanContent();
    if (clean_content.empty()) {
        return "//";
    }
    return "// " + clean_content;
}

std::string CommentNode::generateMultiLineComment() const {
    std::string clean_content = cleanContent();
    if (clean_content.empty()) {
        return "/* */";
    }
    
    // 处理多行内容
    std::istringstream iss(clean_content);
    std::string line;
    std::ostringstream result;
    
    if (clean_content.find('\n') != std::string::npos) {
        // 多行注释
        result << "/*\n";
        while (std::getline(iss, line)) {
            result << " * " << line << "\n";
        }
        result << " */";
    } else {
        // 单行多行注释
        result << "/* " << clean_content << " */";
    }
    
    return result.str();
}

std::string CommentNode::generateGeneratorComment() const {
    std::string clean_content = cleanContent();
    if (clean_content.empty()) {
        return "--";
    }
    return "-- " + clean_content;
}

std::string CommentNode::generateHTMLComment() const {
    std::string clean_content = cleanContent();
    if (clean_content.empty()) {
        return "<!-- -->";
    }
    
    // 转义HTML注释中的特殊字符
    std::string escaped_content = escapeContent(CommentContext::HTML);
    return "<!-- " + escaped_content + " -->";
}

std::string CommentNode::generateCSSComment() const {
    std::string clean_content = cleanContent();
    if (clean_content.empty()) {
        return "/* */";
    }
    return "/* " + clean_content + " */";
}

std::string CommentNode::cleanContent() const {
    std::string result = content_;
    
    // 移除前后空白
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), result.end());
    
    return result;
}

std::string CommentNode::escapeContent(CommentContext target_context) const {
    std::string content = cleanContent();
    
    switch (target_context) {
        case CommentContext::HTML:
            // 转义HTML注释中的 -->
            {
                std::string result = content;
                size_t pos = 0;
                while ((pos = result.find("-->", pos)) != std::string::npos) {
                    result.replace(pos, 3, "--&gt;");
                    pos += 6;
                }
                return result;
            }
            
        case CommentContext::CSS:
        case CommentContext::CHTL_JS:
        case CommentContext::JAVASCRIPT:
            // 转义多行注释中的 */
            {
                std::string result = content;
                size_t pos = 0;
                while ((pos = result.find("*/", pos)) != std::string::npos) {
                    result.replace(pos, 2, "*\\/");
                    pos += 3;
                }
                return result;
            }
            
        default:
            return content;
    }
}

void CommentNode::inferContext(const std::string& surrounding_code) {
    context_ = inferContextFromCode(surrounding_code);
}

CommentContext CommentNode::inferContextFromCode(const std::string& code) {
    // 检查代码特征来推断上下文
    if (code.find("{{") != std::string::npos || code.find("}}") != std::string::npos) {
        return CommentContext::CHTL_JS;
    }
    
    if (code.find("selector") != std::string::npos || 
        code.find("color:") != std::string::npos ||
        code.find("margin:") != std::string::npos ||
        code.find("padding:") != std::string::npos) {
        return CommentContext::CSS;
    }
    
    if (code.find("function") != std::string::npos ||
        code.find("var ") != std::string::npos ||
        code.find("let ") != std::string::npos ||
        code.find("const ") != std::string::npos) {
        return CommentContext::JAVASCRIPT;
    }
    
    if (code.find("<") != std::string::npos || code.find(">") != std::string::npos) {
        return CommentContext::HTML;
    }
    
    return CommentContext::CHTL;
}

std::string CommentNode::convertToContext(CommentContext target_context) const {
    return generateComment(target_context);
}

bool CommentNode::canConvertToContext(CommentContext target_context) const {
    // 所有注释类型都可以转换到任何上下文
    return true;
}

bool CommentNode::isMultiLine() const {
    return type_ == CommentType::MULTI_LINE || content_.find('\n') != std::string::npos;
}

bool CommentNode::shouldAppearInOutput() const {
    // 只有生成器注释和HTML注释应该出现在最终输出中
    return type_ == CommentType::GENERATOR || type_ == CommentType::HTML;
}

std::string CommentNode::getFormattedContent() const {
    return cleanContent();
}

std::string CommentNode::toString() const {
    std::ostringstream oss;
    oss << "CommentNode{type=" << getTypeString() 
        << ", context=" << getContextString() 
        << ", content=\"" << getFormattedContent() << "\"}";
    return oss.str();
}

std::string CommentNode::getTypeString() const {
    return CommentUtils::commentTypeToString(type_);
}

std::string CommentNode::getContextString() const {
    return CommentUtils::commentContextToString(context_);
}

// CommentFactory 实现
std::unique_ptr<CommentNode> CommentFactory::createFromText(const std::string& text, CommentContext context) {
    CommentType type = detectCommentType(text);
    std::string content = extractCommentContent(text, type);
    return std::make_unique<CommentNode>(content, type, context);
}

std::unique_ptr<CommentNode> CommentFactory::createSingleLine(const std::string& content, CommentContext context) {
    return std::make_unique<CommentNode>(content, CommentType::SINGLE_LINE, context);
}

std::unique_ptr<CommentNode> CommentFactory::createMultiLine(const std::string& content, CommentContext context) {
    return std::make_unique<CommentNode>(content, CommentType::MULTI_LINE, context);
}

std::unique_ptr<CommentNode> CommentFactory::createGenerator(const std::string& content) {
    return std::make_unique<CommentNode>(content, CommentType::GENERATOR, CommentContext::CHTL);
}

std::unique_ptr<CommentNode> CommentFactory::createHTML(const std::string& content) {
    return std::make_unique<CommentNode>(content, CommentType::HTML, CommentContext::HTML);
}

std::unique_ptr<CommentNode> CommentFactory::createCSS(const std::string& content) {
    return std::make_unique<CommentNode>(content, CommentType::CSS, CommentContext::CSS);
}

std::unique_ptr<CommentNode> CommentFactory::parseComment(const std::string& comment_text) {
    CommentType type = detectCommentType(comment_text);
    std::string content = extractCommentContent(comment_text, type);
    CommentContext context = CommentContext::UNKNOWN;
    
    // 根据类型推断默认上下文
    switch (type) {
        case CommentType::HTML:
            context = CommentContext::HTML;
            break;
        case CommentType::CSS:
            context = CommentContext::CSS;
            break;
        case CommentType::GENERATOR:
            context = CommentContext::CHTL;
            break;
        default:
            context = CommentContext::CHTL_JS;
            break;
    }
    
    return std::make_unique<CommentNode>(content, type, context);
}

CommentType CommentFactory::detectCommentType(const std::string& text) {
    std::string trimmed = text;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    if (trimmed.length() >= 4 && trimmed.substr(0, 4) == "<!--") {
        return CommentType::HTML;
    }
    if (trimmed.length() >= 2 && trimmed.substr(0, 2) == "//") {
        return CommentType::SINGLE_LINE;
    }
    if (trimmed.length() >= 2 && trimmed.substr(0, 2) == "/*") {
        return CommentType::MULTI_LINE;
    }
    if (trimmed.length() >= 2 && trimmed.substr(0, 2) == "--") {
        return CommentType::GENERATOR;
    }
    
    return CommentType::SINGLE_LINE; // 默认类型
}

std::string CommentFactory::extractCommentContent(const std::string& text, CommentType type) {
    std::string trimmed = text;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    switch (type) {
        case CommentType::SINGLE_LINE:
            if (trimmed.length() >= 2 && trimmed.substr(0, 2) == "//") {
                return trimmed.substr(2);
            }
            break;
            
        case CommentType::MULTI_LINE:
            if (trimmed.length() >= 4 && trimmed.substr(0, 2) == "/*" && 
                trimmed.substr(trimmed.length() - 2) == "*/") {
                return trimmed.substr(2, trimmed.length() - 4);
            }
            break;
            
        case CommentType::GENERATOR:
            if (trimmed.length() >= 2 && trimmed.substr(0, 2) == "--") {
                return trimmed.substr(2);
            }
            break;
            
        case CommentType::HTML:
            if (trimmed.length() >= 7 && trimmed.substr(0, 4) == "<!--" && 
                trimmed.substr(trimmed.length() - 3) == "-->") {
                return trimmed.substr(4, trimmed.length() - 7);
            }
            break;
            
        case CommentType::CSS:
            if (trimmed.length() >= 4 && trimmed.substr(0, 2) == "/*" && 
                trimmed.substr(trimmed.length() - 2) == "*/") {
                return trimmed.substr(2, trimmed.length() - 4);
            }
            break;
    }
    
    return trimmed; // 如果无法提取，返回原始内容
}

// CommentManager 实现
CommentManager::CommentManager() : global_context_(CommentContext::CHTL) {}

CommentManager::~CommentManager() = default;

void CommentManager::addComment(std::unique_ptr<CommentNode> comment) {
    if (comment) {
        comments_.push_back(std::move(comment));
    }
}

void CommentManager::addComment(const std::string& content, CommentType type, CommentContext context) {
    auto comment = std::make_unique<CommentNode>(content, type, context);
    addComment(std::move(comment));
}

void CommentManager::setGlobalContext(CommentContext context) {
    global_context_ = context;
}

std::vector<std::string> CommentManager::generateAllComments() const {
    std::vector<std::string> result;
    for (const auto& comment : comments_) {
        result.push_back(comment->generateComment());
    }
    return result;
}

std::vector<std::string> CommentManager::generateCommentsForContext(CommentContext context) const {
    std::vector<std::string> result;
    for (const auto& comment : comments_) {
        result.push_back(comment->generateComment(context));
    }
    return result;
}

std::vector<CommentNode*> CommentManager::getCommentsByType(CommentType type) const {
    std::vector<CommentNode*> result;
    for (const auto& comment : comments_) {
        if (comment->getType() == type) {
            result.push_back(comment.get());
        }
    }
    return result;
}

std::vector<CommentNode*> CommentManager::getCommentsByContext(CommentContext context) const {
    std::vector<CommentNode*> result;
    for (const auto& comment : comments_) {
        if (comment->getContext() == context) {
            result.push_back(comment.get());
        }
    }
    return result;
}

std::vector<CommentNode*> CommentManager::getOutputComments() const {
    std::vector<CommentNode*> result;
    for (const auto& comment : comments_) {
        if (comment->shouldAppearInOutput()) {
            result.push_back(comment.get());
        }
    }
    return result;
}

void CommentManager::clear() {
    comments_.clear();
}

void CommentManager::removeCommentsByType(CommentType type) {
    comments_.erase(
        std::remove_if(comments_.begin(), comments_.end(),
            [type](const std::unique_ptr<CommentNode>& comment) {
                return comment->getType() == type;
            }),
        comments_.end());
}

void CommentManager::removeCommentsByContext(CommentContext context) {
    comments_.erase(
        std::remove_if(comments_.begin(), comments_.end(),
            [context](const std::unique_ptr<CommentNode>& comment) {
                return comment->getContext() == context;
            }),
        comments_.end());
}

std::string CommentManager::getStatistics() const {
    std::ostringstream oss;
    oss << "CommentManager Statistics:\n";
    oss << "  Total comments: " << comments_.size() << "\n";
    oss << "  Global context: " << CommentUtils::commentContextToString(global_context_) << "\n";
    
    // 按类型统计
    std::map<CommentType, int> type_counts;
    std::map<CommentContext, int> context_counts;
    
    for (const auto& comment : comments_) {
        type_counts[comment->getType()]++;
        context_counts[comment->getContext()]++;
    }
    
    oss << "  By type:\n";
    for (const auto& pair : type_counts) {
        oss << "    " << CommentUtils::commentTypeToString(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "  By context:\n";
    for (const auto& pair : context_counts) {
        oss << "    " << CommentUtils::commentContextToString(pair.first) << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

// CommentUtils 实现
namespace CommentUtils {

std::string commentTypeToString(CommentType type) {
    switch (type) {
        case CommentType::SINGLE_LINE: return "SINGLE_LINE";
        case CommentType::MULTI_LINE: return "MULTI_LINE";
        case CommentType::GENERATOR: return "GENERATOR";
        case CommentType::HTML: return "HTML";
        case CommentType::CSS: return "CSS";
        default: return "UNKNOWN";
    }
}

std::string commentContextToString(CommentContext context) {
    switch (context) {
        case CommentContext::CHTL: return "CHTL";
        case CommentContext::CHTL_JS: return "CHTL_JS";
        case CommentContext::CSS: return "CSS";
        case CommentContext::JAVASCRIPT: return "JAVASCRIPT";
        case CommentContext::HTML: return "HTML";
        case CommentContext::UNKNOWN: return "UNKNOWN";
        default: return "UNKNOWN";
    }
}

CommentType stringToCommentType(const std::string& type_str) {
    if (type_str == "SINGLE_LINE") return CommentType::SINGLE_LINE;
    if (type_str == "MULTI_LINE") return CommentType::MULTI_LINE;
    if (type_str == "GENERATOR") return CommentType::GENERATOR;
    if (type_str == "HTML") return CommentType::HTML;
    if (type_str == "CSS") return CommentType::CSS;
    return CommentType::SINGLE_LINE;
}

CommentContext stringToCommentContext(const std::string& context_str) {
    if (context_str == "CHTL") return CommentContext::CHTL;
    if (context_str == "CHTL_JS") return CommentContext::CHTL_JS;
    if (context_str == "CSS") return CommentContext::CSS;
    if (context_str == "JAVASCRIPT") return CommentContext::JAVASCRIPT;
    if (context_str == "HTML") return CommentContext::HTML;
    return CommentContext::UNKNOWN;
}

bool isValidCommentFormat(const std::string& text, CommentType type) {
    std::string trimmed = text;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    switch (type) {
        case CommentType::SINGLE_LINE:
            return trimmed.length() >= 2 && trimmed.substr(0, 2) == "//";
        case CommentType::MULTI_LINE:
            return trimmed.length() >= 4 && trimmed.substr(0, 2) == "/*" && 
                   trimmed.substr(trimmed.length() - 2) == "*/";
        case CommentType::GENERATOR:
            return trimmed.length() >= 2 && trimmed.substr(0, 2) == "--";
        case CommentType::HTML:
            return trimmed.length() >= 7 && trimmed.substr(0, 4) == "<!--" && 
                   trimmed.substr(trimmed.length() - 3) == "-->";
        case CommentType::CSS:
            return trimmed.length() >= 4 && trimmed.substr(0, 2) == "/*" && 
                   trimmed.substr(trimmed.length() - 2) == "*/";
        default:
            return false;
    }
}

bool isCommentLine(const std::string& line) {
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    
    return trimmed.length() >= 2 && 
           (trimmed.substr(0, 2) == "//" || 
            trimmed.substr(0, 2) == "/*" || 
            trimmed.substr(0, 2) == "--" ||
            (trimmed.length() >= 4 && trimmed.substr(0, 4) == "<!--"));
}

CommentType detectCommentTypeFromLine(const std::string& line) {
    return CommentFactory::detectCommentType(line);
}

std::string trimCommentWhitespace(const std::string& content) {
    std::string result = content;
    result.erase(0, result.find_first_not_of(" \t\n\r"));
    result.erase(result.find_last_not_of(" \t\n\r") + 1);
    return result;
}

std::string normalizeCommentContent(const std::string& content) {
    std::string result = trimCommentWhitespace(content);
    
    // 规范化换行符
    size_t pos = 0;
    while ((pos = result.find("\r\n", pos)) != std::string::npos) {
        result.replace(pos, 2, "\n");
        pos += 1;
    }
    
    return result;
}

std::vector<std::string> splitMultiLineComment(const std::string& content) {
    std::vector<std::string> lines;
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        lines.push_back(trimCommentWhitespace(line));
    }
    
    return lines;
}

CommentContext inferContextFromFileExtension(const std::string& filename) {
    size_t dot_pos = filename.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return CommentContext::UNKNOWN;
    }
    
    std::string ext = filename.substr(dot_pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "chtl") return CommentContext::CHTL;
    if (ext == "css") return CommentContext::CSS;
    if (ext == "js" || ext == "javascript") return CommentContext::JAVASCRIPT;
    if (ext == "html" || ext == "htm") return CommentContext::HTML;
    
    return CommentContext::UNKNOWN;
}

CommentContext inferContextFromSurroundingCode(const std::string& code) {
    return CommentNode::inferContextFromCode(code);
}

} // namespace CommentUtils

} // namespace ast
} // namespace chtl