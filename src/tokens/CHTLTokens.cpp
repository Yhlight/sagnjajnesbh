#include "../../include/CHTLTokens.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// Token便利方法实现
bool Token::isKeyword() const {
    return type >= TokenType::TEXT && type <= TokenType::EXCEPT;
}

bool Token::isOperator() const {
    return type == TokenType::COLON || type == TokenType::EQUALS || 
           type == TokenType::ARROW || type == TokenType::AMPERSAND;
}

bool Token::isLiteral() const {
    return type == TokenType::STRING_LITERAL || type == TokenType::SINGLE_STRING ||
           type == TokenType::UNQUOTED_LITERAL || type == TokenType::NUMBER;
}

bool Token::isComment() const {
    return type == TokenType::COMMENT_SINGLE || type == TokenType::COMMENT_MULTI ||
           type == TokenType::COMMENT_GENERATOR;
}

bool Token::isCHTLJS() const {
    return type == TokenType::DOUBLE_LEFT_BRACE || type == TokenType::DOUBLE_RIGHT_BRACE ||
           type == TokenType::ARROW || type == TokenType::VIR || type == TokenType::LISTEN ||
           type == TokenType::DELEGATE || type == TokenType::ANIMATE;
}

std::string Token::toString() const {
    std::ostringstream oss;
    oss << "Token{" << TokenUtils::tokenTypeToString(type) 
        << ", \"" << value << "\", " << line << ":" << column << "}";
    return oss.str();
}

// KeywordMap单例实现
KeywordMap& KeywordMap::getInstance() {
    static KeywordMap instance;
    return instance;
}

void KeywordMap::initialize(const std::unordered_map<std::string, std::vector<std::string>>& nameConfig) {
    keywordMap_.clear();
    initializeDefaultKeywords();
    initializeHTMLTags();
    
    // 根据配置更新关键字映射
    for (const auto& [configKey, values] : nameConfig) {
        if (configKey == "CUSTOM_STYLE") {
            for (const auto& val : values) {
                keywordMap_[val] = TokenType::AT_STYLE;
            }
        }
        else if (configKey == "CUSTOM_ELEMENT") {
            for (const auto& val : values) {
                keywordMap_[val] = TokenType::AT_ELEMENT;
            }
        }
        else if (configKey == "CUSTOM_VAR") {
            for (const auto& val : values) {
                keywordMap_[val] = TokenType::AT_VAR;
            }
        }
        // 继续处理其他配置项...
    }
}

TokenType KeywordMap::getTokenType(const std::string& keyword) const {
    auto it = keywordMap_.find(keyword);
    if (it != keywordMap_.end()) {
        return it->second;
    }
    
    // 检查是否为HTML标签
    if (isHTMLTag(keyword)) {
        return TokenType::HTML_TAG;
    }
    
    return TokenType::IDENTIFIER;
}

bool KeywordMap::isKeyword(const std::string& word) const {
    return keywordMap_.find(word) != keywordMap_.end();
}

bool KeywordMap::isHTMLTag(const std::string& tag) const {
    return htmlTags_.find(tag) != htmlTags_.end();
}

void KeywordMap::addOriginType(const std::string& type) {
    originTypes_.insert(type);
    keywordMap_[type] = TokenType::AT_HTML; // 自定义原始嵌入类型
}

void KeywordMap::initializeDefaultKeywords() {
    // CHTL基本关键字
    keywordMap_["text"] = TokenType::TEXT;
    keywordMap_["style"] = TokenType::STYLE;
    keywordMap_["script"] = TokenType::SCRIPT;
    
    // 模板和自定义声明
    keywordMap_["[Template]"] = TokenType::TEMPLATE_DECL;
    keywordMap_["[Custom]"] = TokenType::CUSTOM_DECL;
    keywordMap_["[Origin]"] = TokenType::ORIGIN_DECL;
    keywordMap_["[Import]"] = TokenType::IMPORT_DECL;
    keywordMap_["[Namespace]"] = TokenType::NAMESPACE_DECL;
    keywordMap_["[Configuration]"] = TokenType::CONFIGURATION_DECL;
    
    // 配置块
    keywordMap_["[Name]"] = TokenType::NAME_BLOCK;
    keywordMap_["[OriginType]"] = TokenType::ORIGIN_TYPE_BLOCK;
    keywordMap_["[Info]"] = TokenType::INFO_BLOCK;
    keywordMap_["[Export]"] = TokenType::EXPORT_BLOCK;
    
    // 类型标识符
    keywordMap_["@Style"] = TokenType::AT_STYLE;
    keywordMap_["@Element"] = TokenType::AT_ELEMENT;
    keywordMap_["@Var"] = TokenType::AT_VAR;
    keywordMap_["@Html"] = TokenType::AT_HTML;
    keywordMap_["@JavaScript"] = TokenType::AT_JAVASCRIPT;
    keywordMap_["@Chtl"] = TokenType::AT_CHTL;
    keywordMap_["@CJmod"] = TokenType::AT_CJMOD;
    
    // 操作关键字
    keywordMap_["inherit"] = TokenType::INHERIT;
    keywordMap_["delete"] = TokenType::DELETE;
    keywordMap_["insert"] = TokenType::INSERT;
    keywordMap_["after"] = TokenType::AFTER;
    keywordMap_["before"] = TokenType::BEFORE;
    keywordMap_["replace"] = TokenType::REPLACE;
    keywordMap_["at top"] = TokenType::AT_TOP;
    keywordMap_["at bottom"] = TokenType::AT_BOTTOM;
    keywordMap_["from"] = TokenType::FROM;
    keywordMap_["as"] = TokenType::AS;
    keywordMap_["except"] = TokenType::EXCEPT;
    
    // CHTL JS关键字
    keywordMap_["vir"] = TokenType::VIR;
    keywordMap_["listen"] = TokenType::LISTEN;
    keywordMap_["delegate"] = TokenType::DELEGATE;
    keywordMap_["animate"] = TokenType::ANIMATE;
    
    // 动画相关关键字
    keywordMap_["target"] = TokenType::TARGET;
    keywordMap_["duration"] = TokenType::DURATION;
    keywordMap_["easing"] = TokenType::EASING;
    keywordMap_["begin"] = TokenType::BEGIN;
    keywordMap_["when"] = TokenType::WHEN;
    keywordMap_["end"] = TokenType::END;
    keywordMap_["loop"] = TokenType::LOOP;
    keywordMap_["direction"] = TokenType::DIRECTION;
    keywordMap_["delay"] = TokenType::DELAY;
    keywordMap_["callback"] = TokenType::CALLBACK;
    keywordMap_["at"] = TokenType::AT_KEYWORD;
}

void KeywordMap::initializeHTMLTags() {
    // HTML5标准标签
    std::vector<std::string> tags = {
        "html", "head", "body", "title", "meta", "link", "script", "style", "base",
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6", "br", "hr",
        "a", "img", "ul", "ol", "li", "dl", "dt", "dd",
        "table", "tr", "td", "th", "thead", "tbody", "tfoot",
        "form", "input", "button", "select", "option", "textarea", "label",
        "header", "footer", "main", "section", "article", "aside", "nav",
        "video", "audio", "canvas", "svg", "iframe",
        "strong", "em", "b", "i", "u", "s", "mark", "del", "ins", "sub", "sup",
        "pre", "code", "kbd", "samp", "var", "blockquote", "cite", "q",
        "abbr", "dfn", "time", "address", "small"
    };
    
    for (const auto& tag : tags) {
        htmlTags_.insert(tag);
    }
}

// TokenUtils命名空间实现
namespace TokenUtils {

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::SINGLE_STRING: return "SINGLE_STRING";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::COMMENT_SINGLE: return "COMMENT_SINGLE";
        case TokenType::COMMENT_MULTI: return "COMMENT_MULTI";
        case TokenType::COMMENT_GENERATOR: return "COMMENT_GENERATOR";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUALS: return "EQUALS";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::SLASH: return "SLASH";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::TEMPLATE_DECL: return "TEMPLATE_DECL";
        case TokenType::CUSTOM_DECL: return "CUSTOM_DECL";
        case TokenType::ORIGIN_DECL: return "ORIGIN_DECL";
        case TokenType::IMPORT_DECL: return "IMPORT_DECL";
        case TokenType::NAMESPACE_DECL: return "NAMESPACE_DECL";
        case TokenType::CONFIGURATION_DECL: return "CONFIGURATION_DECL";
        case TokenType::AT_STYLE: return "AT_STYLE";
        case TokenType::AT_ELEMENT: return "AT_ELEMENT";
        case TokenType::AT_VAR: return "AT_VAR";
        case TokenType::AT_HTML: return "AT_HTML";
        case TokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case TokenType::AT_CHTL: return "AT_CHTL";
        case TokenType::AT_CJMOD: return "AT_CJMOD";
        case TokenType::INHERIT: return "INHERIT";
        case TokenType::DELETE: return "DELETE";
        case TokenType::INSERT: return "INSERT";
        case TokenType::AFTER: return "AFTER";
        case TokenType::BEFORE: return "BEFORE";
        case TokenType::REPLACE: return "REPLACE";
        case TokenType::AT_TOP: return "AT_TOP";
        case TokenType::AT_BOTTOM: return "AT_BOTTOM";
        case TokenType::FROM: return "FROM";
        case TokenType::AS: return "AS";
        case TokenType::EXCEPT: return "EXCEPT";
        case TokenType::DOUBLE_LEFT_BRACE: return "DOUBLE_LEFT_BRACE";
        case TokenType::DOUBLE_RIGHT_BRACE: return "DOUBLE_RIGHT_BRACE";
        case TokenType::ARROW: return "ARROW";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::VIR: return "VIR";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::HTML_TAG: return "HTML_TAG";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        default: return "UNKNOWN";
    }
}

bool isBinaryOperator(TokenType type) {
    return type == TokenType::COLON || type == TokenType::EQUALS || 
           type == TokenType::ARROW || type == TokenType::DOT;
}

bool isUnaryOperator(TokenType type) {
    return type == TokenType::AMPERSAND;
}

int getOperatorPrecedence(TokenType type) {
    switch (type) {
        case TokenType::DOT:
        case TokenType::ARROW:
            return 10; // 最高优先级
        case TokenType::AMPERSAND:
            return 8;
        case TokenType::COLON:
        case TokenType::EQUALS:
            return 5;
        default:
            return 0;
    }
}

bool isRightAssociative(TokenType type) {
    return type == TokenType::EQUALS;
}

std::string parseStringLiteral(const std::string& literal) {
    if (literal.length() < 2) return literal;
    
    std::string result;
    char quote = literal[0];
    
    for (size_t i = 1; i < literal.length() - 1; ++i) {
        if (literal[i] == '\\' && i + 1 < literal.length() - 1) {
            char next = literal[i + 1];
            switch (next) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '\"': result += '\"'; break;
                case '\'': result += '\''; break;
                default: 
                    result += '\\';
                    result += next;
                    break;
            }
            ++i; // 跳过下一个字符
        } else {
            result += literal[i];
        }
    }
    
    return result;
}

TokenValue parseNumberLiteral(const std::string& literal) {
    try {
        if (literal.find('.') != std::string::npos) {
            return std::stod(literal);
        } else {
            return std::stoi(literal);
        }
    } catch (const std::exception&) {
        return literal; // 如果解析失败，返回原字符串
    }
}

} // namespace TokenUtils

} // namespace CHTL