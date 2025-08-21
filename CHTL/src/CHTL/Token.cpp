#include "CHTL/Token.h"
#include <set>
#include <sstream>

namespace CHTL {

// Token成员函数实现
bool Token::IsKeyword() const {
    return type >= TokenType::Text && type <= TokenType::Except;
}

bool Token::IsLiteral() const {
    return type == TokenType::Identifier ||
           type == TokenType::StringLiteral ||
           type == TokenType::SingleQuoteLiteral ||
           type == TokenType::UnquotedLiteral ||
           type == TokenType::Number;
}

bool Token::IsOperator() const {
    return type >= TokenType::LeftBrace && type <= TokenType::Ampersand;
}

bool Token::IsSpecialMarker() const {
    return type >= TokenType::Template && type <= TokenType::Export;
}

bool Token::IsAtMarker() const {
    return type >= TokenType::AtElement && type <= TokenType::AtCJmod;
}

std::string Token::ToString() const {
    std::stringstream ss;
    ss << "Token{type=" << TokenUtils::GetTokenTypeName(type)
       << ", value=\"" << value << "\""
       << ", line=" << line
       << ", column=" << column
       << "}";
    return ss.str();
}

// 静态成员定义
const std::unordered_map<std::string, TokenType> TokenUtils::keywordMap_ = {
    {"text", TokenType::Text},
    {"style", TokenType::Style},
    {"script", TokenType::Script},
    {"inherit", TokenType::Inherit},
    {"delete", TokenType::Delete},
    {"insert", TokenType::Insert},
    {"after", TokenType::After},
    {"before", TokenType::Before},
    {"replace", TokenType::Replace},
    {"at", TokenType::At},
    {"top", TokenType::Top},
    {"bottom", TokenType::Bottom},
    {"from", TokenType::From},
    {"as", TokenType::As},
    {"except", TokenType::Except}
};

const std::unordered_map<std::string, TokenType> TokenUtils::specialMarkerMap_ = {
    {"[Template]", TokenType::Template},
    {"[Custom]", TokenType::Custom},
    {"[Origin]", TokenType::Origin},
    {"[Import]", TokenType::Import},
    {"[Namespace]", TokenType::Namespace},
    {"[Configuration]", TokenType::Configuration},
    {"[Info]", TokenType::Info},
    {"[Export]", TokenType::Export}
};

const std::unordered_map<std::string, TokenType> TokenUtils::atMarkerMap_ = {
    {"@Element", TokenType::AtElement},
    {"@Style", TokenType::AtStyle},
    {"@Var", TokenType::AtVar},
    {"@Html", TokenType::AtHtml},
    {"@JavaScript", TokenType::AtJavaScript},
    {"@Javascript", TokenType::AtJavaScript},  // 支持两种拼写
    {"@Config", TokenType::AtConfig},
    {"@Chtl", TokenType::AtChtl},
    {"@CJmod", TokenType::AtCJmod}
};

const std::set<std::string> TokenUtils::htmlElements_ = {
    // 常见HTML元素
    "html", "head", "body", "title", "meta", "link", "style", "script",
    "div", "span", "p", "a", "img", "ul", "ol", "li", "dl", "dt", "dd",
    "table", "thead", "tbody", "tfoot", "tr", "td", "th", "caption",
    "form", "input", "button", "select", "option", "textarea", "label",
    "fieldset", "legend", "h1", "h2", "h3", "h4", "h5", "h6",
    "header", "footer", "nav", "section", "article", "aside", "main",
    "figure", "figcaption", "blockquote", "pre", "code", "em", "strong",
    "small", "mark", "del", "ins", "sub", "sup", "br", "hr", "wbr",
    "iframe", "object", "embed", "param", "video", "audio", "source",
    "track", "canvas", "svg", "math", "progress", "meter", "details",
    "summary", "dialog", "menu", "menuitem", "template"
};

// TokenUtils实现
std::string TokenUtils::GetTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::Identifier: return "Identifier";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::SingleQuoteLiteral: return "SingleQuoteLiteral";
        case TokenType::UnquotedLiteral: return "UnquotedLiteral";
        case TokenType::Number: return "Number";
        
        case TokenType::Text: return "Text";
        case TokenType::Style: return "Style";
        case TokenType::Script: return "Script";
        case TokenType::Inherit: return "Inherit";
        case TokenType::Delete: return "Delete";
        case TokenType::Insert: return "Insert";
        case TokenType::After: return "After";
        case TokenType::Before: return "Before";
        case TokenType::Replace: return "Replace";
        case TokenType::At: return "At";
        case TokenType::Top: return "Top";
        case TokenType::Bottom: return "Bottom";
        case TokenType::From: return "From";
        case TokenType::As: return "As";
        case TokenType::Except: return "Except";
        
        case TokenType::Template: return "Template";
        case TokenType::Custom: return "Custom";
        case TokenType::Origin: return "Origin";
        case TokenType::Import: return "Import";
        case TokenType::Namespace: return "Namespace";
        case TokenType::Configuration: return "Configuration";
        case TokenType::Info: return "Info";
        case TokenType::Export: return "Export";
        
        case TokenType::AtElement: return "@Element";
        case TokenType::AtStyle: return "@Style";
        case TokenType::AtVar: return "@Var";
        case TokenType::AtHtml: return "@Html";
        case TokenType::AtJavaScript: return "@JavaScript";
        case TokenType::AtConfig: return "@Config";
        case TokenType::AtChtl: return "@Chtl";
        case TokenType::AtCJmod: return "@CJmod";
        
        case TokenType::LeftBrace: return "LeftBrace";
        case TokenType::RightBrace: return "RightBrace";
        case TokenType::LeftParen: return "LeftParen";
        case TokenType::RightParen: return "RightParen";
        case TokenType::LeftBracket: return "LeftBracket";
        case TokenType::RightBracket: return "RightBracket";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::Colon: return "Colon";
        case TokenType::Comma: return "Comma";
        case TokenType::Dot: return "Dot";
        case TokenType::Equal: return "Equal";
        case TokenType::Ampersand: return "Ampersand";
        
        case TokenType::SingleLineComment: return "SingleLineComment";
        case TokenType::MultiLineComment: return "MultiLineComment";
        case TokenType::GeneratorComment: return "GeneratorComment";
        
        case TokenType::Eof: return "EOF";
        case TokenType::Unknown: return "Unknown";
        case TokenType::HtmlElement: return "HtmlElement";
        
        default: return "Unknown";
    }
}

bool TokenUtils::IsKeyword(const std::string& word) {
    return keywordMap_.find(word) != keywordMap_.end();
}

bool TokenUtils::IsHtmlElement(const std::string& word) {
    return htmlElements_.find(word) != htmlElements_.end();
}

bool TokenUtils::IsSpecialMarker(const std::string& word) {
    return specialMarkerMap_.find(word) != specialMarkerMap_.end();
}

bool TokenUtils::IsAtMarker(const std::string& word) {
    return atMarkerMap_.find(word) != atMarkerMap_.end();
}

TokenType TokenUtils::GetKeywordType(const std::string& word) {
    auto it = keywordMap_.find(word);
    return it != keywordMap_.end() ? it->second : TokenType::Unknown;
}

TokenType TokenUtils::GetSpecialMarkerType(const std::string& word) {
    auto it = specialMarkerMap_.find(word);
    return it != specialMarkerMap_.end() ? it->second : TokenType::Unknown;
}

TokenType TokenUtils::GetAtMarkerType(const std::string& word) {
    auto it = atMarkerMap_.find(word);
    return it != atMarkerMap_.end() ? it->second : TokenType::Unknown;
}

} // namespace CHTL