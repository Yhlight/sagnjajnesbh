#include "CHTL/Core/CHTLToken.h"
#include "Utils/StringUtils.h"
#include <sstream>

namespace CHTL {
namespace Core {

// 静态成员初始化
std::unordered_map<std::string, TokenType> CHTLToken::keywordMap_;
std::unordered_map<std::string, TokenType> CHTLToken::htmlTagMap_;
const CHTLToken TokenStream::eofToken_(TokenType::END_OF_FILE, "");

CHTLToken::CHTLToken(TokenType type, const std::string& value, int line, int column, 
                     const std::string& fileName)
    : type_(type), value_(value), line_(line), column_(column), fileName_(fileName) {
    EnsureMapsInitialized();
}

bool CHTLToken::IsKeyword() const {
    return type_ >= TokenType::TEXT && type_ <= TokenType::EXCEPT;
}

bool CHTLToken::IsMarkerKeyword() const {
    return type_ >= TokenType::TEMPLATE && type_ <= TokenType::ORIGIN_TYPE;
}

bool CHTLToken::IsTypeKeyword() const {
    return type_ >= TokenType::AT_STYLE && type_ <= TokenType::AT_CSS_LOWER;
}

bool CHTLToken::IsLiteral() const {
    return type_ == TokenType::STRING_LITERAL || 
           type_ == TokenType::UNQUOTED_LITERAL || 
           type_ == TokenType::NUMBER;
}

bool CHTLToken::IsOperator() const {
    return type_ == TokenType::COLON || 
           type_ == TokenType::EQUAL || 
           type_ == TokenType::DOT || 
           type_ == TokenType::AMPERSAND || 
           type_ == TokenType::HASH || 
           type_ == TokenType::AT;
}

bool CHTLToken::IsDelimiter() const {
    return type_ == TokenType::LEFT_BRACE || 
           type_ == TokenType::RIGHT_BRACE || 
           type_ == TokenType::LEFT_BRACKET || 
           type_ == TokenType::RIGHT_BRACKET || 
           type_ == TokenType::LEFT_PAREN || 
           type_ == TokenType::RIGHT_PAREN || 
           type_ == TokenType::SEMICOLON || 
           type_ == TokenType::COMMA;
}

bool CHTLToken::IsComment() const {
    return type_ == TokenType::SINGLE_LINE_COMMENT || 
           type_ == TokenType::MULTI_LINE_COMMENT || 
           type_ == TokenType::GENERATOR_COMMENT;
}

bool CHTLToken::IsHTMLTag() const {
    return type_ == TokenType::HTML_TAG;
}

bool CHTLToken::IsCSSSelector() const {
    return type_ == TokenType::CLASS_SELECTOR || 
           type_ == TokenType::ID_SELECTOR || 
           type_ == TokenType::PSEUDO_CLASS || 
           type_ == TokenType::PSEUDO_ELEMENT;
}

std::string CHTLToken::ToString() const {
    std::ostringstream oss;
    oss << "[" << GetTokenTypeName(type_) << ": \"" << value_ << "\" at " 
        << line_ << ":" << column_;
    if (!fileName_.empty()) {
        oss << " in " << fileName_;
    }
    oss << "]";
    return oss.str();
}

std::string CHTLToken::GetTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::DOT: return "DOT";
        case TokenType::COMMA: return "COMMA";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::HASH: return "HASH";
        case TokenType::AT: return "AT";
        case TokenType::SINGLE_LINE_COMMENT: return "SINGLE_LINE_COMMENT";
        case TokenType::MULTI_LINE_COMMENT: return "MULTI_LINE_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
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
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::INFO: return "INFO";
        case TokenType::EXPORT: return "EXPORT";
        case TokenType::NAME: return "NAME";
        case TokenType::ORIGIN_TYPE: return "ORIGIN_TYPE";
        case TokenType::AT_STYLE: return "AT_STYLE";
        case TokenType::AT_ELEMENT: return "AT_ELEMENT";
        case TokenType::AT_VAR: return "AT_VAR";
        case TokenType::AT_HTML: return "AT_HTML";
        case TokenType::AT_JAVASCRIPT: return "AT_JAVASCRIPT";
        case TokenType::AT_CHTL: return "AT_CHTL";
        case TokenType::AT_CJMOD: return "AT_CJMOD";
        case TokenType::AT_CONFIG: return "AT_CONFIG";
        case TokenType::AT_VUE: return "AT_VUE";
        case TokenType::AT_CSS: return "AT_CSS";
        case TokenType::AT_CSS_LOWER: return "AT_CSS_LOWER";
        case TokenType::HTML_TAG: return "HTML_TAG";
        case TokenType::CLASS_SELECTOR: return "CLASS_SELECTOR";
        case TokenType::ID_SELECTOR: return "ID_SELECTOR";
        case TokenType::PSEUDO_CLASS: return "PSEUDO_CLASS";
        case TokenType::PSEUDO_ELEMENT: return "PSEUDO_ELEMENT";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::UNKNOWN: return "UNKNOWN";
        case TokenType::CONFIG_KEY: return "CONFIG_KEY";
        case TokenType::CONFIG_VALUE: return "CONFIG_VALUE";
        case TokenType::INDEX_ACCESS: return "INDEX_ACCESS";
        case TokenType::AT_TOP_KEYWORD: return "AT_TOP_KEYWORD";
        case TokenType::AT_BOTTOM_KEYWORD: return "AT_BOTTOM_KEYWORD";
        default: return "UNKNOWN_TYPE";
    }
}

TokenType CHTLToken::GetTokenTypeFromString(const std::string& str) {
    EnsureMapsInitialized();
    
    auto it = keywordMap_.find(str);
    if (it != keywordMap_.end()) {
        return it->second;
    }
    
    auto htmlIt = htmlTagMap_.find(str);
    if (htmlIt != htmlTagMap_.end()) {
        return TokenType::HTML_TAG;
    }
    
    // 检查CSS选择器
    if (!str.empty()) {
        if (str[0] == '.') {
            return TokenType::CLASS_SELECTOR;
        } else if (str[0] == '#') {
            return TokenType::ID_SELECTOR;
        } else if (str.find("::") != std::string::npos) {
            return TokenType::PSEUDO_ELEMENT;
        } else if (str[0] == ':') {
            return TokenType::PSEUDO_CLASS;
        }
    }
    
    // 检查是否为数字
    if (!str.empty() && (std::isdigit(str[0]) || 
                        (str[0] == '-' && str.length() > 1 && std::isdigit(str[1])))) {
        return TokenType::NUMBER;
    }
    
    return TokenType::IDENTIFIER;
}

bool CHTLToken::IsKeywordString(const std::string& str) {
    EnsureMapsInitialized();
    return keywordMap_.find(str) != keywordMap_.end();
}

bool CHTLToken::IsHTMLTagString(const std::string& str) {
    EnsureMapsInitialized();
    return htmlTagMap_.find(str) != htmlTagMap_.end();
}

void CHTLToken::InitializeKeywordMaps() {
    // CHTL基础关键字
    keywordMap_["text"] = TokenType::TEXT;
    keywordMap_["style"] = TokenType::STYLE;
    keywordMap_["script"] = TokenType::SCRIPT;
    keywordMap_["inherit"] = TokenType::INHERIT;
    keywordMap_["delete"] = TokenType::DELETE;
    keywordMap_["insert"] = TokenType::INSERT;
    keywordMap_["after"] = TokenType::AFTER;
    keywordMap_["before"] = TokenType::BEFORE;
    keywordMap_["replace"] = TokenType::REPLACE;
    keywordMap_["from"] = TokenType::FROM;
    keywordMap_["as"] = TokenType::AS;
    keywordMap_["except"] = TokenType::EXCEPT;
    
    // 组合关键字
    keywordMap_["at top"] = TokenType::AT_TOP_KEYWORD;
    keywordMap_["at bottom"] = TokenType::AT_BOTTOM_KEYWORD;
    
    // CHTL标记关键字
    keywordMap_["[Template]"] = TokenType::TEMPLATE;
    keywordMap_["[Custom]"] = TokenType::CUSTOM;
    keywordMap_["[Origin]"] = TokenType::ORIGIN;
    keywordMap_["[Import]"] = TokenType::IMPORT;
    keywordMap_["[Namespace]"] = TokenType::NAMESPACE;
    keywordMap_["[Configuration]"] = TokenType::CONFIGURATION;
    keywordMap_["[Info]"] = TokenType::INFO;
    keywordMap_["[Export]"] = TokenType::EXPORT;
    keywordMap_["[Name]"] = TokenType::NAME;
    keywordMap_["[OriginType]"] = TokenType::ORIGIN_TYPE;
    
    // CHTL类型关键字
    keywordMap_["@Style"] = TokenType::AT_STYLE;
    keywordMap_["@Element"] = TokenType::AT_ELEMENT;
    keywordMap_["@Var"] = TokenType::AT_VAR;
    keywordMap_["@Html"] = TokenType::AT_HTML;
    keywordMap_["@JavaScript"] = TokenType::AT_JAVASCRIPT;
    keywordMap_["@Chtl"] = TokenType::AT_CHTL;
    keywordMap_["@CJmod"] = TokenType::AT_CJMOD;
    keywordMap_["@Config"] = TokenType::AT_CONFIG;
    keywordMap_["@Vue"] = TokenType::AT_VUE;
    keywordMap_["@CSS"] = TokenType::AT_CSS;
    keywordMap_["@css"] = TokenType::AT_CSS_LOWER;
    
    // HTML标签映射表（常用HTML标签）
    std::vector<std::string> htmlTags = {
        "html", "head", "body", "title", "meta", "link", "style", "script",
        "div", "span", "p", "a", "img", "h1", "h2", "h3", "h4", "h5", "h6",
        "ul", "ol", "li", "table", "tr", "td", "th", "thead", "tbody", "tfoot",
        "form", "input", "textarea", "button", "select", "option", "label",
        "header", "footer", "nav", "main", "section", "article", "aside",
        "br", "hr", "strong", "em", "b", "i", "u", "small", "sub", "sup",
        "code", "pre", "blockquote", "cite", "abbr", "address", "time",
        "canvas", "svg", "video", "audio", "source", "iframe", "embed", "object"
    };
    
    for (const auto& tag : htmlTags) {
        htmlTagMap_[tag] = TokenType::HTML_TAG;
    }
}

void CHTLToken::EnsureMapsInitialized() {
    static bool initialized = false;
    if (!initialized) {
        InitializeKeywordMaps();
        initialized = true;
    }
}

// TokenPosition 实现
std::string TokenPosition::ToString() const {
    std::ostringstream oss;
    oss << line << ":" << column;
    if (!fileName.empty()) {
        oss << " in " << fileName;
    }
    return oss.str();
}

// TokenStream 实现
void TokenStream::AddToken(const CHTLToken& token) {
    tokens_.push_back(token);
}

const CHTLToken& TokenStream::Current() const {
    if (position_ >= tokens_.size()) {
        return eofToken_;
    }
    return tokens_[position_];
}

void TokenStream::Advance() {
    if (position_ < tokens_.size()) {
        position_++;
    }
}

void TokenStream::Retreat() {
    if (position_ > 0) {
        position_--;
    }
}

const CHTLToken& TokenStream::Peek(int offset) const {
    size_t peekPos = position_ + offset;
    if (peekPos >= tokens_.size()) {
        return eofToken_;
    }
    return tokens_[peekPos];
}

bool TokenStream::IsAtEnd() const {
    return position_ >= tokens_.size();
}

void TokenStream::SetPosition(size_t pos) {
    position_ = std::min(pos, tokens_.size());
}

void TokenStream::Clear() {
    tokens_.clear();
    position_ = 0;
}

} // namespace Core
} // namespace CHTL