#include "CHTLJS/Lexer/Token.h"
#include <unordered_map>
#include <unordered_set>

namespace CHTL {
namespace JSCompiler {

// Token成员函数实现
bool Token::IsKeyword() const {
    return type >= TokenType::KEYWORD_VIR && type <= TokenType::KEYWORD_CALLBACK;
}

bool Token::IsOperator() const {
    return type >= TokenType::ARROW && type <= TokenType::EQUALS;
}

std::string Token::ToString() const {
    return TokenUtils::TokenTypeToString(type) + ": " + value + 
           " (" + std::to_string(line) + ":" + std::to_string(column) + ")";
}

// TokenUtils静态成员实现
static const std::unordered_map<std::string, TokenType> keywordMap = {
    {"vir", TokenType::KEYWORD_VIR},
    {"listen", TokenType::KEYWORD_LISTEN},
    {"delegate", TokenType::KEYWORD_DELEGATE},
    {"animate", TokenType::KEYWORD_ANIMATE},
    {"iNeverAway", TokenType::KEYWORD_INEVERAWAY},
    {"target", TokenType::KEYWORD_TARGET},
    {"duration", TokenType::KEYWORD_DURATION},
    {"easing", TokenType::KEYWORD_EASING},
    {"begin", TokenType::KEYWORD_BEGIN},
    {"when", TokenType::KEYWORD_WHEN},
    {"end", TokenType::KEYWORD_END},
    {"at", TokenType::KEYWORD_AT},
    {"loop", TokenType::KEYWORD_LOOP},
    {"direction", TokenType::KEYWORD_DIRECTION},
    {"delay", TokenType::KEYWORD_DELAY},
    {"callback", TokenType::KEYWORD_CALLBACK}
};

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
        case TokenType::SELECTOR_START: return "{{";
        case TokenType::SELECTOR_END: return "}}";
        case TokenType::CLASS_SELECTOR: return ".";
        case TokenType::ID_SELECTOR: return "#";
        case TokenType::ARROW: return "->";
        case TokenType::DOT: return ".";
        case TokenType::LEFT_BRACKET: return "[";
        case TokenType::RIGHT_BRACKET: return "]";
        case TokenType::LEFT_PAREN: return "(";
        case TokenType::RIGHT_PAREN: return ")";
        case TokenType::LEFT_BRACE: return "{";
        case TokenType::RIGHT_BRACE: return "}";
        case TokenType::COMMA: return ",";
        case TokenType::COLON: return ":";
        case TokenType::SEMICOLON: return ";";
        case TokenType::EQUALS: return "=";
        case TokenType::KEYWORD_VIR: return "vir";
        case TokenType::KEYWORD_LISTEN: return "listen";
        case TokenType::KEYWORD_DELEGATE: return "delegate";
        case TokenType::KEYWORD_ANIMATE: return "animate";
        case TokenType::KEYWORD_INEVERAWAY: return "iNeverAway";
        case TokenType::KEYWORD_TARGET: return "target";
        case TokenType::KEYWORD_DURATION: return "duration";
        case TokenType::KEYWORD_EASING: return "easing";
        case TokenType::KEYWORD_BEGIN: return "begin";
        case TokenType::KEYWORD_WHEN: return "when";
        case TokenType::KEYWORD_END: return "end";
        case TokenType::KEYWORD_AT: return "at";
        case TokenType::KEYWORD_LOOP: return "loop";
        case TokenType::KEYWORD_DIRECTION: return "direction";
        case TokenType::KEYWORD_DELAY: return "delay";
        case TokenType::KEYWORD_CALLBACK: return "callback";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::NUMBER_LITERAL: return "NUMBER_LITERAL";
        case TokenType::JS_CODE: return "JS_CODE";
        case TokenType::EOF_TOKEN: return "EOF";
        case TokenType::ERROR_TOKEN: return "ERROR";
        default: return "UNKNOWN";
    }
}

} // namespace JSCompiler
} // namespace CHTL