#include "CHTL/Lexer/Token.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

namespace CHTL {
namespace Compiler {

// Token成员函数实现
bool Token::IsKeyword() const {
    return type >= TokenType::KEYWORD_TEMPLATE && type <= TokenType::KEYWORD_SCRIPT;
}

bool Token::IsTypeMarker() const {
    return type >= TokenType::AT_STYLE && type <= TokenType::AT_CONFIG;
}

bool Token::IsOperator() const {
    return type >= TokenType::LEFT_BRACE && type <= TokenType::AMPERSAND;
}

std::string Token::ToString() const {
    return TokenUtils::TokenTypeToString(type) + ": " + value + 
           " (" + std::to_string(line) + ":" + std::to_string(column) + ")";
}

// TokenUtils静态成员实现
static const std::unordered_set<std::string> htmlElements = {
    "html", "head", "body", "div", "span", "p", "a", "img", "ul", "ol", "li",
    "h1", "h2", "h3", "h4", "h5", "h6", "table", "tr", "td", "th", "form",
    "input", "button", "select", "option", "textarea", "label", "header",
    "footer", "nav", "section", "article", "aside", "main", "figure",
    "figcaption", "video", "audio", "canvas", "svg", "iframe", "script",
    "style", "link", "meta", "title", "br", "hr", "code", "pre", "blockquote",
    "strong", "em", "b", "i", "u", "small", "sub", "sup", "del", "ins", "mark"
};

static const std::unordered_map<std::string, TokenType> keywordMap = {
    {"[Template]", TokenType::KEYWORD_TEMPLATE},
    {"[Custom]", TokenType::KEYWORD_CUSTOM},
    {"[Origin]", TokenType::KEYWORD_ORIGIN},
    {"[Import]", TokenType::KEYWORD_IMPORT},
    {"[Namespace]", TokenType::KEYWORD_NAMESPACE},
    {"[Configuration]", TokenType::KEYWORD_CONFIGURATION},
    {"[Info]", TokenType::KEYWORD_INFO},
    {"[Export]", TokenType::KEYWORD_EXPORT},
    {"[Name]", TokenType::KEYWORD_NAME},
    {"[OriginType]", TokenType::KEYWORD_ORIGINTYPE},
    {"@Style", TokenType::AT_STYLE},
    {"@Element", TokenType::AT_ELEMENT},
    {"@Var", TokenType::AT_VAR},
    {"@Html", TokenType::AT_HTML},
    {"@JavaScript", TokenType::AT_JAVASCRIPT},
    {"@Chtl", TokenType::AT_CHTL},
    {"@CJmod", TokenType::AT_CJMOD},
    {"@Config", TokenType::AT_CONFIG},
    {"inherit", TokenType::KEYWORD_INHERIT},
    {"delete", TokenType::KEYWORD_DELETE},
    {"insert", TokenType::KEYWORD_INSERT},
    {"after", TokenType::KEYWORD_AFTER},
    {"before", TokenType::KEYWORD_BEFORE},
    {"replace", TokenType::KEYWORD_REPLACE},
    {"at top", TokenType::KEYWORD_AT_TOP},
    {"at bottom", TokenType::KEYWORD_AT_BOTTOM},
    {"from", TokenType::KEYWORD_FROM},
    {"as", TokenType::KEYWORD_AS},
    {"except", TokenType::KEYWORD_EXCEPT},
    {"text", TokenType::KEYWORD_TEXT},
    {"style", TokenType::KEYWORD_STYLE},
    {"script", TokenType::KEYWORD_SCRIPT}
};

bool TokenUtils::IsHTMLElement(const std::string& name) {
    std::string lower = name;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return htmlElements.find(lower) != htmlElements.end();
}

bool TokenUtils::IsKeyword(const std::string& text) {
    return keywordMap.find(text) != keywordMap.end();
}

TokenType TokenUtils::GetKeywordType(const std::string& text) {
    auto it = keywordMap.find(text);
    if (it != keywordMap.end()) {
        return it->second;
    }
    return TokenType::IDENTIFIER;
}

std::string TokenUtils::TokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::KEYWORD_TEMPLATE: return "[Template]";
        case TokenType::KEYWORD_CUSTOM: return "[Custom]";
        case TokenType::KEYWORD_ORIGIN: return "[Origin]";
        case TokenType::KEYWORD_IMPORT: return "[Import]";
        case TokenType::KEYWORD_NAMESPACE: return "[Namespace]";
        case TokenType::KEYWORD_CONFIGURATION: return "[Configuration]";
        case TokenType::KEYWORD_INFO: return "[Info]";
        case TokenType::KEYWORD_EXPORT: return "[Export]";
        case TokenType::KEYWORD_NAME: return "[Name]";
        case TokenType::KEYWORD_ORIGINTYPE: return "[OriginType]";
        case TokenType::AT_STYLE: return "@Style";
        case TokenType::AT_ELEMENT: return "@Element";
        case TokenType::AT_VAR: return "@Var";
        case TokenType::AT_HTML: return "@Html";
        case TokenType::AT_JAVASCRIPT: return "@JavaScript";
        case TokenType::AT_CHTL: return "@Chtl";
        case TokenType::AT_CJMOD: return "@CJmod";
        case TokenType::AT_CONFIG: return "@Config";
        case TokenType::KEYWORD_INHERIT: return "inherit";
        case TokenType::KEYWORD_DELETE: return "delete";
        case TokenType::KEYWORD_INSERT: return "insert";
        case TokenType::KEYWORD_AFTER: return "after";
        case TokenType::KEYWORD_BEFORE: return "before";
        case TokenType::KEYWORD_REPLACE: return "replace";
        case TokenType::KEYWORD_AT_TOP: return "at top";
        case TokenType::KEYWORD_AT_BOTTOM: return "at bottom";
        case TokenType::KEYWORD_FROM: return "from";
        case TokenType::KEYWORD_AS: return "as";
        case TokenType::KEYWORD_EXCEPT: return "except";
        case TokenType::KEYWORD_TEXT: return "text";
        case TokenType::KEYWORD_STYLE: return "style";
        case TokenType::KEYWORD_SCRIPT: return "script";
        case TokenType::LEFT_BRACE: return "{";
        case TokenType::RIGHT_BRACE: return "}";
        case TokenType::LEFT_BRACKET: return "[";
        case TokenType::RIGHT_BRACKET: return "]";
        case TokenType::LEFT_PAREN: return "(";
        case TokenType::RIGHT_PAREN: return ")";
        case TokenType::SEMICOLON: return ";";
        case TokenType::COMMA: return ",";
        case TokenType::COLON: return ":";
        case TokenType::EQUALS: return "=";
        case TokenType::DOT: return ".";
        case TokenType::SLASH: return "/";
        case TokenType::STAR: return "*";
        case TokenType::AMPERSAND: return "&";
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::MULTI_COMMENT: return "MULTI_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::ERROR_TOKEN: return "ERROR";
        default: return "UNKNOWN";
    }
}

} // namespace Compiler
} // namespace CHTL