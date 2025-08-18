#include "CHTLJSToken.h"
#include <sstream>

namespace chtl {

std::unordered_map<TokenType, std::string> Token::type_names_;

std::string Token::toString() const {
    std::ostringstream oss;
    oss << typeToString(type) << "(" << value << ") at " 
        << position.line << ":" << position.column;
    return oss.str();
}

bool Token::isKeyword() const {
    return isKeywordType(type);
}

bool Token::isOperator() const {
    return isOperatorType(type);
}

bool Token::isLiteral() const {
    return type == TokenType::STRING || type == TokenType::NUMBER || type == TokenType::UNDECORATED_LITERAL;
}

bool Token::isCHTLJSKeyword() const {
    return isCHTLJSKeywordType(type);
}

bool Token::operator==(const Token& other) const {
    return type == other.type && value == other.value;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

std::string Token::typeToString(TokenType type) {
    if (type_names_.empty()) {
        initializeTypeNames();
    }
    
    auto it = type_names_.find(type);
    if (it != type_names_.end()) {
        return it->second;
    }
    
    return "UNKNOWN";
}

bool Token::isKeywordType(TokenType type) {
    switch (type) {
        case TokenType::TEMPLATE:
        case TokenType::CUSTOM:
        case TokenType::ORIGIN:
        case TokenType::IMPORT:
        case TokenType::NAMESPACE:
        case TokenType::CONFIGURATION:
        case TokenType::TEXT:
        case TokenType::STYLE:
        case TokenType::SCRIPT:
        case TokenType::INHERIT:
        case TokenType::DELETE:
        case TokenType::FROM:
        case TokenType::AS:
        case TokenType::LISTEN:
        case TokenType::DELEGATE:
        case TokenType::ANIMATE:
        case TokenType::VIR:
            return true;
        default:
            return false;
    }
}

bool Token::isCHTLJSKeywordType(TokenType type) {
    switch (type) {
        case TokenType::LISTEN:
        case TokenType::DELEGATE:
        case TokenType::ANIMATE:
        case TokenType::VIR:
            return true;
        default:
            return false;
    }
}

bool Token::isOperatorType(TokenType type) {
    switch (type) {
        case TokenType::LEFT_BRACE:
        case TokenType::RIGHT_BRACE:
        case TokenType::LEFT_BRACKET:
        case TokenType::RIGHT_BRACKET:
        case TokenType::LEFT_PAREN:
        case TokenType::RIGHT_PAREN:
        case TokenType::SEMICOLON:
        case TokenType::COLON:
        case TokenType::COMMA:
        case TokenType::AT:
        case TokenType::ARROW:
        case TokenType::ASSIGN:
        case TokenType::MINUS:
        case TokenType::SELECTOR_START:
        case TokenType::SELECTOR_END:
            return true;
        default:
            return false;
    }
}

void Token::initializeTypeNames() {
    type_names_[TokenType::INVALID] = "INVALID";
    type_names_[TokenType::EOF_TOKEN] = "EOF";
    type_names_[TokenType::IDENTIFIER] = "IDENTIFIER";
    type_names_[TokenType::STRING] = "STRING";
    type_names_[TokenType::NUMBER] = "NUMBER";
    type_names_[TokenType::UNDECORATED_LITERAL] = "UNDECORATED_LITERAL";
    
    // CHTL关键字
    type_names_[TokenType::TEMPLATE] = "TEMPLATE";
    type_names_[TokenType::CUSTOM] = "CUSTOM";
    type_names_[TokenType::ORIGIN] = "ORIGIN";
    type_names_[TokenType::IMPORT] = "IMPORT";
    type_names_[TokenType::NAMESPACE] = "NAMESPACE";
    type_names_[TokenType::CONFIGURATION] = "CONFIGURATION";
    type_names_[TokenType::TEXT] = "TEXT";
    type_names_[TokenType::STYLE] = "STYLE";
    type_names_[TokenType::SCRIPT] = "SCRIPT";
    type_names_[TokenType::INHERIT] = "INHERIT";
    type_names_[TokenType::DELETE] = "DELETE";
    type_names_[TokenType::FROM] = "FROM";
    type_names_[TokenType::AS] = "AS";
    
    // CHTL JS关键字
    type_names_[TokenType::LISTEN] = "LISTEN";
    type_names_[TokenType::DELEGATE] = "DELEGATE";
    type_names_[TokenType::ANIMATE] = "ANIMATE";
    type_names_[TokenType::VIR] = "VIR";
    
    // 操作符
    type_names_[TokenType::LEFT_BRACE] = "LEFT_BRACE";
    type_names_[TokenType::RIGHT_BRACE] = "RIGHT_BRACE";
    type_names_[TokenType::LEFT_BRACKET] = "LEFT_BRACKET";
    type_names_[TokenType::RIGHT_BRACKET] = "RIGHT_BRACKET";
    type_names_[TokenType::LEFT_PAREN] = "LEFT_PAREN";
    type_names_[TokenType::RIGHT_PAREN] = "RIGHT_PAREN";
    type_names_[TokenType::SEMICOLON] = "SEMICOLON";
    type_names_[TokenType::COLON] = "COLON";
    type_names_[TokenType::COMMA] = "COMMA";
    type_names_[TokenType::AT] = "AT";
    type_names_[TokenType::ARROW] = "ARROW";
    type_names_[TokenType::ASSIGN] = "ASSIGN";
    type_names_[TokenType::MINUS] = "MINUS";
    
    // CHTL JS特殊
    type_names_[TokenType::SELECTOR_START] = "SELECTOR_START";
    type_names_[TokenType::SELECTOR_END] = "SELECTOR_END";
}

} // namespace chtl