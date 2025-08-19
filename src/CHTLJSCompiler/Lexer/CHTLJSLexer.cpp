#include "CHTLJSLexer.h"
#include <cctype>

namespace chtl {
namespace chtljs {

CHTLJSLexer::CHTLJSLexer() : position_(0), line_(1), column_(1) {
}

void CHTLJSLexer::setInput(const std::string& input) {
    input_ = input;
    position_ = 0;
    line_ = 1;
    column_ = 1;
    clearErrors();
}

std::vector<Token> CHTLJSLexer::tokenize() {
    std::vector<Token> tokens;
    
    while (hasMoreTokens()) {
        Token token = nextToken();
        if (token.type != TokenType::INVALID) {
            tokens.push_back(token);
        }
    }
    
    // 添加EOF token
    tokens.emplace_back(TokenType::EOF_TOKEN, "", TokenPosition(line_, column_));
    
    return tokens;
}

Token CHTLJSLexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", TokenPosition(line_, column_));
    }
    
    char c = currentChar();
    TokenPosition pos(line_, column_);
    
    // 检查CHTL JS特殊语法 {{
    if (c == '{' && peekChar() == '{') {
        return readCHTLJSSelector();
    }
    
    // 字符串
    if (c == '"' || c == '\'') {
        return readString();
    }
    
    // 数字
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 标识符和关键字
    if (isAlpha(c) || c == '_') {
        return readIdentifier();
    }
    
    // 无修饰字面量（只在特定上下文中）
    if (isUndecoratedContext() && canBeUndecoratedLiteral()) {
        return readUndecoratedLiteral();
    }
    
    // 操作符和特殊字符
    return readOperator();
}

Token CHTLJSLexer::readCHTLJSSelector() {
    TokenPosition pos(line_, column_);
    std::string value;
    
    // 跳过 {{
    advance(); // {
    advance(); // {
    value += "{{";
    
    while (!isAtEnd() && !(currentChar() == '}' && peekChar() == '}')) {
        value += currentChar();
        advance();
    }
    
    if (isAtEnd()) {
        addError("未闭合的CHTL JS选择器");
        return Token(TokenType::INVALID, value, pos);
    }
    
    // 跳过 }}
    value += "}}";
    advance(); // }
    advance(); // }
    
    return Token(TokenType::CHTLJS_SELECTOR, value, pos);
}

bool CHTLJSLexer::hasMoreTokens() const {
    return !isAtEnd();
}

size_t CHTLJSLexer::getCurrentPosition() const {
    return position_;
}

size_t CHTLJSLexer::getCurrentLine() const {
    return line_;
}

size_t CHTLJSLexer::getCurrentColumn() const {
    return column_;
}

char CHTLJSLexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return input_[position_];
}

char CHTLJSLexer::peekChar(size_t offset) const {
    size_t pos = position_ + offset;
    if (pos >= input_.length()) return '\0';
    return input_[pos];
}

void CHTLJSLexer::advance() {
    if (isAtEnd()) return;
    
    if (input_[position_] == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    
    position_++;
}

void CHTLJSLexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(currentChar())) {
        advance();
    }
}

Token CHTLJSLexer::readString() {
    char quote = currentChar();
    TokenPosition pos(line_, column_);
    std::string value;
    
    advance(); // 跳过开始引号
    
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\') {
            advance();
            if (!isAtEnd()) {
                switch (currentChar()) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += currentChar(); break;
                }
                advance();
            }
        } else {
            value += currentChar();
            advance();
        }
    }
    
    if (isAtEnd()) {
        addError("未闭合的字符串");
        return Token(TokenType::INVALID, value, pos);
    }
    
    advance(); // 跳过结束引号
    return Token(TokenType::STRING, value, pos);
}

Token CHTLJSLexer::readNumber() {
    TokenPosition pos(line_, column_);
    std::string value;
    
    while (!isAtEnd() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::NUMBER, value, pos);
}

Token CHTLJSLexer::readIdentifier() {
    TokenPosition pos(line_, column_);
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }
    
    return readKeyword(value);
}

Token CHTLJSLexer::readOperator() {
    TokenPosition pos(line_, column_);
    char c = currentChar();
    
    switch (c) {
        case '{': advance(); return Token(TokenType::LEFT_BRACE, "{", pos);
        case '}': advance(); return Token(TokenType::RIGHT_BRACE, "}", pos);
        case '[': advance(); return Token(TokenType::LEFT_BRACKET, "[", pos);
        case ']': advance(); return Token(TokenType::RIGHT_BRACKET, "]", pos);
        case '(': advance(); return Token(TokenType::LEFT_PAREN, "(", pos);
        case ')': advance(); return Token(TokenType::RIGHT_PAREN, ")", pos);
        case ';': advance(); return Token(TokenType::SEMICOLON, ";", pos);
        case ':': advance(); return Token(TokenType::COLON, ":", pos);
        case ',': advance(); return Token(TokenType::COMMA, ",", pos);
        case '@': advance(); return Token(TokenType::AT, "@", pos);
        case '-':
            advance();
            if (currentChar() == '>') {
                advance();
                return Token(TokenType::ARROW, "->", pos);
            }
            return Token(TokenType::MINUS, "-", pos);
        case '=':
            advance();
            if (currentChar() == '>') {
                advance();
                return Token(TokenType::ARROW, "=>", pos);
            }
            return Token(TokenType::ASSIGN, "=", pos);
        default:
            advance();
            addError("未知字符: " + std::string(1, c));
            return Token(TokenType::INVALID, std::string(1, c), pos);
    }
}

Token CHTLJSLexer::readKeyword(const std::string& word) {
    TokenPosition pos(line_, column_ - word.length());
    
    // CHTL JS关键字
    if (word == "listen") return Token(TokenType::LISTEN, word, pos);
    if (word == "delegate") return Token(TokenType::DELEGATE, word, pos);
    if (word == "animate") return Token(TokenType::ANIMATE, word, pos);
    if (word == "vir") return Token(TokenType::VIR, word, pos);
    
    // CHTL关键字
    if (word == "Template") return Token(TokenType::TEMPLATE, word, pos);
    if (word == "Custom") return Token(TokenType::CUSTOM, word, pos);
    if (word == "Origin") return Token(TokenType::ORIGIN, word, pos);
    if (word == "Import") return Token(TokenType::IMPORT, word, pos);
    if (word == "Namespace") return Token(TokenType::NAMESPACE, word, pos);
    if (word == "text") return Token(TokenType::TEXT, word, pos);
    if (word == "style") return Token(TokenType::STYLE, word, pos);
    if (word == "script") return Token(TokenType::SCRIPT, word, pos);
    if (word == "from") return Token(TokenType::FROM, word, pos);
    if (word == "as") return Token(TokenType::AS, word, pos);
    
    // 默认为标识符
    return Token(TokenType::IDENTIFIER, word, pos);
}

bool CHTLJSLexer::isAtEnd() const {
    return position_ >= input_.length();
}

bool CHTLJSLexer::isAlpha(char c) const {
    return std::isalpha(c);
}

bool CHTLJSLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

Token CHTLJSLexer::readUndecoratedLiteral() {
    TokenPosition pos(line_, column_);
    std::string value;
    
    // 读取无修饰字面量，直到遇到分隔符
    while (!isAtEnd() && isValidUndecoratedChar(currentChar())) {
        value += currentChar();
        advance();
    }
    
    // 去除尾部空白
    while (!value.empty() && std::isspace(value.back())) {
        value.pop_back();
    }
    
    if (value.empty()) {
        addError("空的无修饰字面量");
        return Token(TokenType::INVALID, "", pos);
    }
    
    return Token(TokenType::UNDECORATED_LITERAL, value, pos);
}

bool CHTLJSLexer::canBeUndecoratedLiteral() const {
    char c = currentChar();
    
    // 无修饰字面量可以以字母、数字、中文或某些特殊字符开始
    return isAlpha(c) || isDigit(c) || 
           (c >= 0x4e00 && c <= 0x9fff) ||  // 中文字符范围
           c == '-' || c == '_' || c == '.' || c == '#';
}

bool CHTLJSLexer::isValidUndecoratedChar(char c) const {
    // 无修饰字面量中允许的字符
    if (isAlphaNumeric(c) || c == '_' || c == '-' || c == '.' || c == '#' || c == ' ') {
        return true;
    }
    
    // 中文字符
    if (c >= 0x4e00 && c <= 0x9fff) {
        return true;
    }
    
    // 停止字符（表示字面量结束）
    if (c == ';' || c == '}' || c == '{' || c == ')' || c == '(' || 
        c == '[' || c == ']' || c == ',' || c == ':' || c == '\n') {
        return false;
    }
    
    return true;
}

bool CHTLJSLexer::isUndecoratedContext() const {
    // 只在特定上下文中允许无修饰字面量
    return current_context_ == "text" || 
           current_context_ == "attribute" || 
           current_context_ == "css_value" ||
           current_context_ == "style_property";
}

void CHTLJSLexer::addError(const std::string& message) {
    errors_.push_back("行 " + std::to_string(line_) + ", 列 " + std::to_string(column_) + ": " + message);
}

} // namespace chtljs
} // namespace chtl