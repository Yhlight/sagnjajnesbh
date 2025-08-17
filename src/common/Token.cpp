#include "Token.h"
#include <algorithm>

namespace chtl {

// 静态成员初始化
std::unordered_map<std::string, TokenType> TokenFactory::keywordMap;
bool TokenFactory::initialized = false;

bool Token::isKeyword() const {
    return type >= TokenType::TEXT && type <= TokenType::EXPORT;
}

bool Token::isOperator() const {
    return type >= TokenType::LBRACE && type <= TokenType::AMPERSAND;
}

bool Token::isLiteral() const {
    return type == TokenType::STRING_LITERAL || 
           type == TokenType::UNQUOTED_LITERAL || 
           type == TokenType::NUMBER;
}

std::string Token::getTypeString() const {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::UNQUOTED_LITERAL: return "UNQUOTED_LITERAL";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::ARROW: return "ARROW";
        case TokenType::AMPERSAND: return "AMPERSAND";
        case TokenType::TEXT: return "TEXT";
        case TokenType::STYLE: return "STYLE";
        case TokenType::SCRIPT: return "SCRIPT";
        case TokenType::TEMPLATE: return "TEMPLATE";
        case TokenType::CUSTOM: return "CUSTOM";
        case TokenType::ORIGIN: return "ORIGIN";
        case TokenType::IMPORT: return "IMPORT";
        case TokenType::NAMESPACE: return "NAMESPACE";
        case TokenType::CONFIGURATION: return "CONFIGURATION";
        case TokenType::INFO: return "INFO";
        case TokenType::EXPORT: return "EXPORT";
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
        case TokenType::DOUBLE_LBRACE: return "DOUBLE_LBRACE";
        case TokenType::DOUBLE_RBRACE: return "DOUBLE_RBRACE";
        case TokenType::LISTEN: return "LISTEN";
        case TokenType::DELEGATE: return "DELEGATE";
        case TokenType::ANIMATE: return "ANIMATE";
        case TokenType::VIR: return "VIR";
        case TokenType::SINGLE_COMMENT: return "SINGLE_COMMENT";
        case TokenType::MULTI_COMMENT: return "MULTI_COMMENT";
        case TokenType::GENERATOR_COMMENT: return "GENERATOR_COMMENT";
        case TokenType::NEWLINE: return "NEWLINE";
        case TokenType::WHITESPACE: return "WHITESPACE";
        case TokenType::EOF_TOKEN: return "EOF_TOKEN";
        case TokenType::INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

bool Token::operator==(const Token& other) const {
    return type == other.type && value == other.value;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

void TokenFactory::initKeywordMap() {
    if (initialized) return;
    
    // CHTL关键字
    keywordMap["text"] = TokenType::TEXT;
    keywordMap["style"] = TokenType::STYLE;
    keywordMap["script"] = TokenType::SCRIPT;
    keywordMap["[Template]"] = TokenType::TEMPLATE;
    keywordMap["[Custom]"] = TokenType::CUSTOM;
    keywordMap["[Origin]"] = TokenType::ORIGIN;
    keywordMap["[Import]"] = TokenType::IMPORT;
    keywordMap["[Namespace]"] = TokenType::NAMESPACE;
    keywordMap["[Configuration]"] = TokenType::CONFIGURATION;
    keywordMap["[Info]"] = TokenType::INFO;
    keywordMap["[Export]"] = TokenType::EXPORT;
    
    // 类型标识符
    keywordMap["@Style"] = TokenType::AT_STYLE;
    keywordMap["@Element"] = TokenType::AT_ELEMENT;
    keywordMap["@Var"] = TokenType::AT_VAR;
    keywordMap["@Html"] = TokenType::AT_HTML;
    keywordMap["@JavaScript"] = TokenType::AT_JAVASCRIPT;
    keywordMap["@Chtl"] = TokenType::AT_CHTL;
    keywordMap["@CJmod"] = TokenType::AT_CJMOD;
    
    // 操作关键字
    keywordMap["inherit"] = TokenType::INHERIT;
    keywordMap["delete"] = TokenType::DELETE;
    keywordMap["insert"] = TokenType::INSERT;
    keywordMap["after"] = TokenType::AFTER;
    keywordMap["before"] = TokenType::BEFORE;
    keywordMap["replace"] = TokenType::REPLACE;
    keywordMap["at top"] = TokenType::AT_TOP;
    keywordMap["at bottom"] = TokenType::AT_BOTTOM;
    keywordMap["from"] = TokenType::FROM;
    keywordMap["as"] = TokenType::AS;
    keywordMap["except"] = TokenType::EXCEPT;
    
    // CHTL JS关键字
    keywordMap["listen"] = TokenType::LISTEN;
    keywordMap["delegate"] = TokenType::DELEGATE;
    keywordMap["animate"] = TokenType::ANIMATE;
    keywordMap["vir"] = TokenType::VIR;
    
    initialized = true;
}

Token TokenFactory::createToken(const std::string& text, const TokenPosition& pos) {
    initKeywordMap();
    
    // 检查是否为关键字
    auto it = keywordMap.find(text);
    if (it != keywordMap.end()) {
        return Token(it->second, text, pos);
    }
    
    // 检查是否为数字
    if (!text.empty() && std::isdigit(text[0])) {
        bool isNumber = true;
        bool hasDot = false;
        for (size_t i = 0; i < text.length(); ++i) {
            if (std::isdigit(text[i])) continue;
            if (text[i] == '.' && !hasDot) {
                hasDot = true;
                continue;
            }
            isNumber = false;
            break;
        }
        if (isNumber) {
            return Token(TokenType::NUMBER, text, pos);
        }
    }
    
    // 检查是否为字符串字面量
    if (text.length() >= 2 && 
        ((text[0] == '"' && text[text.length()-1] == '"') ||
         (text[0] == '\'' && text[text.length()-1] == '\''))) {
        return Token(TokenType::STRING_LITERAL, text, pos);
    }
    
    // 默认为标识符
    return Token(TokenType::IDENTIFIER, text, pos);
}

bool TokenFactory::isKeyword(const std::string& text) {
    initKeywordMap();
    return keywordMap.find(text) != keywordMap.end();
}

TokenType TokenFactory::getKeywordType(const std::string& text) {
    initKeywordMap();
    auto it = keywordMap.find(text);
    return it != keywordMap.end() ? it->second : TokenType::INVALID;
}

} // namespace chtl