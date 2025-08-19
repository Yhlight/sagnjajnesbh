#include "CHTLLexer.h"
#include <cctype>

namespace chtl {

CHTLLexer::CHTLLexer() : position_(0), line_(1), column_(1) {
}

void CHTLLexer::setInput(const std::string& input) {
    input_ = input;
    position_ = 0;
    line_ = 1;
    column_ = 1;
    clearErrors();
}

std::vector<Token> CHTLLexer::tokenize() {
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

Token CHTLLexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", TokenPosition(line_, column_));
    }
    
    char c = currentChar();
    TokenPosition pos(line_, column_);
    
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
    
    // 操作符和特殊字符
    return readOperator();
}

bool CHTLLexer::hasMoreTokens() const {
    return !isAtEnd();
}

size_t CHTLLexer::getCurrentPosition() const {
    return position_;
}

size_t CHTLLexer::getCurrentLine() const {
    return line_;
}

size_t CHTLLexer::getCurrentColumn() const {
    return column_;
}

char CHTLLexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return input_[position_];
}

char CHTLLexer::peekChar(size_t offset) const {
    size_t pos = position_ + offset;
    if (pos >= input_.length()) return '\0';
    return input_[pos];
}

void CHTLLexer::advance() {
    if (isAtEnd()) return;
    
    if (input_[position_] == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    
    position_++;
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd() && std::isspace(currentChar())) {
        advance();
    }
}

Token CHTLLexer::readString() {
    char quote = currentChar();
    TokenPosition pos(line_, column_);
    std::string value;
    
    advance(); // 跳过开始引号
    
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\') {
            advance();
            if (!isAtEnd()) {
                // 处理转义字符
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

Token CHTLLexer::readNumber() {
    TokenPosition pos(line_, column_);
    std::string value;
    
    while (!isAtEnd() && (isDigit(currentChar()) || currentChar() == '.')) {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::NUMBER, value, pos);
}

Token CHTLLexer::readIdentifier() {
    TokenPosition pos(line_, column_);
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }
    
    // 检查是否为关键字
    return readKeyword(value);
}

Token CHTLLexer::readOperator() {
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
        case '=': advance(); return Token(TokenType::ASSIGN, "=", pos);
        case '.': advance(); return Token(TokenType::DOT, ".", pos);
        case '#': advance(); return Token(TokenType::HASH, "#", pos);
        case '%': advance(); return Token(TokenType::PERCENT, "%", pos);
        case '/':
            advance();
            if (currentChar() == '/') {
                advance();
                return Token(TokenType::COMMENT_LINE, "//", pos);
            }
            return Token(TokenType::SLASH, "/", pos);
        case '*': advance(); return Token(TokenType::ASTERISK, "*", pos);
        case '+': advance(); return Token(TokenType::PLUS, "+", pos);
        case '<': advance(); return Token(TokenType::LESS_THAN, "<", pos);
        case '>': advance(); return Token(TokenType::GREATER_THAN, ">", pos);
        case '!': advance(); return Token(TokenType::EXCLAMATION, "!", pos);
        case '|': advance(); return Token(TokenType::PIPE, "|", pos);
        case '&': advance(); return Token(TokenType::AMPERSAND, "&", pos);
        case '^': advance(); return Token(TokenType::CARET, "^", pos);
        case '$': advance(); return Token(TokenType::DOLLAR, "$", pos);
        case '\\': advance(); return Token(TokenType::BACKSLASH, "\\", pos);
        default:
            advance();
            addError("未知字符: " + std::string(1, c));
            return Token(TokenType::INVALID, std::string(1, c), pos);
    }
}

Token CHTLLexer::readKeyword(const std::string& word) {
    TokenPosition pos(line_, column_ - word.length());
    
    // CHTL关键字
    if (word == "Template") return Token(TokenType::TEMPLATE, word, pos);
    if (word == "Custom") return Token(TokenType::CUSTOM, word, pos);
    if (word == "Origin") return Token(TokenType::ORIGIN, word, pos);
    if (word == "Import") return Token(TokenType::IMPORT, word, pos);
    if (word == "Namespace") return Token(TokenType::NAMESPACE, word, pos);
    if (word == "Configuration") return Token(TokenType::CONFIGURATION, word, pos);
    if (word == "text") return Token(TokenType::TEXT, word, pos);
    if (word == "style") return Token(TokenType::STYLE, word, pos);
    if (word == "script") return Token(TokenType::SCRIPT, word, pos);
    if (word == "inherit") return Token(TokenType::INHERIT, word, pos);
    if (word == "delete") return Token(TokenType::DELETE, word, pos);
    if (word == "insert") return Token(TokenType::INSERT, word, pos);
    if (word == "replace") return Token(TokenType::REPLACE, word, pos);
    if (word == "after") return Token(TokenType::AFTER, word, pos);
    if (word == "before") return Token(TokenType::BEFORE, word, pos);
    if (word == "except") return Token(TokenType::EXCEPT, word, pos);
    if (word == "from") return Token(TokenType::FROM, word, pos);
    if (word == "as") return Token(TokenType::AS, word, pos);
    
    // 复合关键字
    if (word == "at") {
        // 检查是否为 "at top" 或 "at bottom"
        std::string nextWord = peekNextWord();
        if (nextWord == "top") {
            advance(); // 跳过当前 "at"
            advance(); // 跳过 "top"
            return Token(TokenType::ATTOP, "at top", pos);
        } else if (nextWord == "bottom") {
            advance(); // 跳过当前 "at"
            advance(); // 跳过 "bottom"
            return Token(TokenType::ATBOTTOM, "at bottom", pos);
        }
    }
    
    // 默认为标识符
    return Token(TokenType::IDENTIFIER, word, pos);
}

bool CHTLLexer::isAtEnd() const {
    return position_ >= input_.length();
}

bool CHTLLexer::isAlpha(char c) const {
    // 支持ASCII字母和中文字符
    if (std::isalpha(c)) {
        return true;
    }
    
    // 简单的中文字符检测（UTF-8）
    unsigned char uc = static_cast<unsigned char>(c);
    return uc >= 0x80; // 非ASCII字符，可能是中文
}

bool CHTLLexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

std::string CHTLLexer::peekNextWord() {
    // 保存当前位置
    size_t saved_pos = position_;
    size_t saved_line = line_;
    size_t saved_column = column_;
    
    // 跳过空白字符
    while (!isAtEnd() && std::isspace(currentChar())) {
        advance();
    }
    
    // 读取下一个单词
    std::string word;
    while (!isAtEnd() && isAlphaNumeric(currentChar())) {
        word += currentChar();
        advance();
    }
    
    // 恢复位置
    position_ = saved_pos;
    line_ = saved_line;
    column_ = saved_column;
    
    return word;
}

void CHTLLexer::addError(const std::string& message) {
    errors_.push_back("行 " + std::to_string(line_) + ", 列 " + std::to_string(column_) + ": " + message);
}

} // namespace chtl