#include "Lexer.h"
#include <fstream>
#include <sstream>
#include <cctype>

namespace chtl {

Lexer::Lexer(CompilerContext& context)
    : context_(context), current_pos_(0), line_number_(1), column_number_(1) {
}

void Lexer::setInput(const std::string& input) {
    input_ = input;
    reset();
}

void Lexer::setInput(std::istream& input_stream) {
    std::ostringstream oss;
    oss << input_stream.rdbuf();
    setInput(oss.str());
}

void Lexer::setInputFile(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        reportError("无法打开文件: " + file_path);
        return;
    }
    
    context_.setCurrentFile(file_path);
    setInput(file);
}

Token Lexer::nextToken() {
    // 跳过空白字符
    skipWhitespace();
    
    // 更新上下文位置
    context_.setCurrentPosition(line_number_, column_number_);
    
    // 检查是否到达文件末尾
    if (isAtEnd()) {
        return Token(TokenType::EOF_TOKEN, "", getCurrentPosition());
    }
    
    // 根据当前上下文选择词法分析策略
    return nextTokenInContext();
}

Token Lexer::peekToken() {
    size_t saved_pos = current_pos_;
    size_t saved_line = line_number_;
    size_t saved_column = column_number_;
    
    Token token = nextToken();
    
    current_pos_ = saved_pos;
    line_number_ = saved_line;
    column_number_ = saved_column;
    
    return token;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd() && (isWhitespace(currentChar()) || isNewline(currentChar()))) {
        if (isNewline(currentChar())) {
            advanceLine();
        } else {
            advance();
        }
    }
}

void Lexer::skipComment() {
    if (currentChar() == '/' && peekChar() == '/') {
        // 单行注释
        while (!isAtEnd() && !isNewline(currentChar())) {
            advance();
        }
    } else if (currentChar() == '/' && peekChar() == '*') {
        // 多行注释
        advance(); // skip '/'
        advance(); // skip '*'
        
        while (!isAtEnd()) {
            if (currentChar() == '*' && peekChar() == '/') {
                advance(); // skip '*'
                advance(); // skip '/'
                break;
            }
            if (isNewline(currentChar())) {
                advanceLine();
            } else {
                advance();
            }
        }
    } else if (currentChar() == '-' && peekChar() == '-') {
        // 生成器注释
        while (!isAtEnd() && !isNewline(currentChar())) {
            advance();
        }
    }
}

TokenPosition Lexer::getCurrentPosition() const {
    return TokenPosition(line_number_, column_number_, current_pos_);
}

void Lexer::setPosition(size_t line, size_t column, size_t offset) {
    line_number_ = line;
    column_number_ = column;
    if (offset < input_.length()) {
        current_pos_ = offset;
    }
}

bool Lexer::hasError() const {
    return !last_error_.empty();
}

std::string Lexer::getLastError() const {
    return last_error_;
}

void Lexer::clearError() {
    last_error_.clear();
}

bool Lexer::isAtEnd() const {
    return current_pos_ >= input_.length();
}

size_t Lexer::getCurrentOffset() const {
    return current_pos_;
}

size_t Lexer::getInputLength() const {
    return input_.length();
}

std::string Lexer::getCurrentLine() const {
    size_t line_start = current_pos_;
    size_t line_end = current_pos_;
    
    // 找到行的开始
    while (line_start > 0 && !isNewline(input_[line_start - 1])) {
        line_start--;
    }
    
    // 找到行的结束
    while (line_end < input_.length() && !isNewline(input_[line_end])) {
        line_end++;
    }
    
    return input_.substr(line_start, line_end - line_start);
}

std::string Lexer::getContextAroundPosition(size_t radius) const {
    size_t start = current_pos_ > radius ? current_pos_ - radius : 0;
    size_t end = current_pos_ + radius < input_.length() ? current_pos_ + radius : input_.length();
    
    return input_.substr(start, end - start);
}

void Lexer::reset() {
    current_pos_ = 0;
    line_number_ = 1;
    column_number_ = 1;
    last_error_.clear();
}

char Lexer::currentChar() const {
    return isAtEnd() ? '\0' : input_[current_pos_];
}

char Lexer::peekChar(size_t offset) const {
    size_t pos = current_pos_ + offset;
    return pos >= input_.length() ? '\0' : input_[pos];
}

void Lexer::advance() {
    if (!isAtEnd()) {
        current_pos_++;
        column_number_++;
    }
}

void Lexer::advanceLine() {
    advance();
    line_number_++;
    column_number_ = 1;
}

Token Lexer::readIdentifier() {
    TokenPosition pos = getCurrentPosition();
    std::string value;
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        value += currentChar();
        advance();
    }
    
    return TokenFactory::createToken(value, pos);
}

Token Lexer::readNumber() {
    TokenPosition pos = getCurrentPosition();
    std::string value;
    bool has_dot = false;
    
    while (!isAtEnd() && (isDigit(currentChar()) || (currentChar() == '.' && !has_dot))) {
        if (currentChar() == '.') {
            has_dot = true;
        }
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::NUMBER, value, pos);
}

Token Lexer::readStringLiteral(char quote_char) {
    TokenPosition pos = getCurrentPosition();
    std::string value;
    value += quote_char;
    advance(); // skip opening quote
    
    while (!isAtEnd() && currentChar() != quote_char) {
        if (currentChar() == '\\') {
            value += currentChar();
            advance();
            if (!isAtEnd()) {
                value += currentChar();
                advance();
            }
        } else {
            if (isNewline(currentChar())) {
                advanceLine();
            } else {
                advance();
            }
            value += input_[current_pos_ - 1];
        }
    }
    
    if (!isAtEnd()) {
        value += currentChar();
        advance(); // skip closing quote
    } else {
        reportError("未闭合的字符串字面量");
    }
    
    return Token(TokenType::STRING_LITERAL, value, pos);
}

Token Lexer::readUnquotedLiteral() {
    TokenPosition pos = getCurrentPosition();
    std::string value;
    
    // 读取无引号字面量，直到遇到分隔符
    while (!isAtEnd() && !isWhitespace(currentChar()) && 
           currentChar() != ';' && currentChar() != ',' && 
           currentChar() != '{' && currentChar() != '}' &&
           currentChar() != '(' && currentChar() != ')') {
        value += currentChar();
        advance();
    }
    
    return Token(TokenType::UNQUOTED_LITERAL, value, pos);
}

Token Lexer::readOperator() {
    TokenPosition pos = getCurrentPosition();
    char c = currentChar();
    
    // 检查双字符操作符
    Token double_token = handleDoubleSymbol();
    if (double_token.type != TokenType::INVALID) {
        return double_token;
    }
    
    // 单字符操作符
    std::string value(1, c);
    advance();
    
    switch (c) {
        case '{': return Token(TokenType::LBRACE, value, pos);
        case '}': return Token(TokenType::RBRACE, value, pos);
        case '(': return Token(TokenType::LPAREN, value, pos);
        case ')': return Token(TokenType::RPAREN, value, pos);
        case '[': return Token(TokenType::LBRACKET, value, pos);
        case ']': return Token(TokenType::RBRACKET, value, pos);
        case ';': return Token(TokenType::SEMICOLON, value, pos);
        case ':': return Token(TokenType::COLON, value, pos);
        case ',': return Token(TokenType::COMMA, value, pos);
        case '.': return Token(TokenType::DOT, value, pos);
        case '=': return Token(TokenType::EQUAL, value, pos);
        case '&': return Token(TokenType::AMPERSAND, value, pos);
        default: return Token(TokenType::INVALID, value, pos);
    }
}

Token Lexer::readComment() {
    TokenPosition pos = getCurrentPosition();
    std::string value;
    
    if (currentChar() == '/' && peekChar() == '/') {
        // 单行注释
        while (!isAtEnd() && !isNewline(currentChar())) {
            value += currentChar();
            advance();
        }
        return Token(TokenType::SINGLE_COMMENT, value, pos);
    } else if (currentChar() == '/' && peekChar() == '*') {
        // 多行注释
        while (!isAtEnd()) {
            value += currentChar();
            if (currentChar() == '*' && peekChar() == '/') {
                advance();
                value += currentChar();
                advance();
                break;
            }
            if (isNewline(currentChar())) {
                advanceLine();
            } else {
                advance();
            }
        }
        return Token(TokenType::MULTI_COMMENT, value, pos);
    } else if (currentChar() == '-' && peekChar() == '-') {
        // 生成器注释
        while (!isAtEnd() && !isNewline(currentChar())) {
            value += currentChar();
            advance();
        }
        return Token(TokenType::GENERATOR_COMMENT, value, pos);
    }
    
    return Token(TokenType::INVALID, "", pos);
}

bool Lexer::isAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isDigit(char c) const {
    return std::isdigit(c);
}

bool Lexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool Lexer::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool Lexer::isNewline(char c) const {
    return c == '\n';
}

Token Lexer::handleDoubleSymbol() {
    TokenPosition pos = getCurrentPosition();
    char c1 = currentChar();
    char c2 = peekChar();
    
    if (c1 == '{' && c2 == '{') {
        advance();
        advance();
        return Token(TokenType::DOUBLE_LBRACE, "{{", pos);
    } else if (c1 == '}' && c2 == '}') {
        advance();
        advance();
        return Token(TokenType::DOUBLE_RBRACE, "}}", pos);
    } else if (c1 == '-' && c2 == '>') {
        advance();
        advance();
        return Token(TokenType::ARROW, "->", pos);
    }
    
    return Token(TokenType::INVALID, "", pos);
}

Token Lexer::handleAtSymbol() {
    TokenPosition pos = getCurrentPosition();
    std::string value = "@";
    advance(); // skip '@'
    
    // 读取@后面的标识符
    while (!isAtEnd() && isAlphaNumeric(currentChar())) {
        value += currentChar();
        advance();
    }
    
    return TokenFactory::createToken(value, pos);
}

Token Lexer::handleBracketKeyword() {
    TokenPosition pos = getCurrentPosition();
    std::string value;
    
    // 读取整个[...]结构
    while (!isAtEnd() && currentChar() != ']') {
        value += currentChar();
        advance();
    }
    
    if (!isAtEnd() && currentChar() == ']') {
        value += currentChar();
        advance();
    }
    
    return TokenFactory::createToken(value, pos);
}

void Lexer::reportError(const std::string& message) {
    last_error_ = message;
    context_.reportError(message);
}

bool Lexer::isCHTLJSStart() const {
    return currentChar() == '{' && peekChar() == '{';
}

bool Lexer::isCHTLKeywordStart() const {
    return currentChar() == '[';
}

Token Lexer::nextTokenInContext() {
    char c = currentChar();
    TokenPosition pos = getCurrentPosition();
    
    // 处理注释
    if ((c == '/' && (peekChar() == '/' || peekChar() == '*')) ||
        (c == '-' && peekChar() == '-')) {
        return readComment();
    }
    
    // 处理CHTL JS语法
    if (isCHTLJSStart()) {
        return handleDoubleSymbol();
    }
    
    // 处理CHTL关键字
    if (isCHTLKeywordStart()) {
        return handleBracketKeyword();
    }
    
    // 处理@符号
    if (c == '@') {
        return handleAtSymbol();
    }
    
    // 处理字符串字面量
    if (c == '"' || c == '\'') {
        return readStringLiteral(c);
    }
    
    // 处理数字
    if (isDigit(c)) {
        return readNumber();
    }
    
    // 处理标识符
    if (isAlpha(c)) {
        return readIdentifier();
    }
    
    // 处理操作符
    if (c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' ||
        c == ';' || c == ':' || c == ',' || c == '.' || c == '=' || c == '&' ||
        c == '-') {
        return readOperator();
    }
    
    // 处理无引号字面量
    if (!isWhitespace(c)) {
        return readUnquotedLiteral();
    }
    
    // 未知字符
    std::string error_msg = "未知字符: ";
    error_msg += c;
    reportError(error_msg);
    advance();
    return Token(TokenType::INVALID, std::string(1, c), pos);
}

Token Lexer::nextTokenInStyleBlock() {
    // 在样式块中的特殊处理
    return nextTokenInContext();
}

Token Lexer::nextTokenInScriptBlock() {
    // 在脚本块中的特殊处理
    return nextTokenInContext();
}

Token Lexer::nextTokenInTemplateBlock() {
    // 在模板块中的特殊处理
    return nextTokenInContext();
}

Token Lexer::nextTokenInCustomBlock() {
    // 在自定义块中的特殊处理
    return nextTokenInContext();
}

// LexerFactory实现
std::unique_ptr<Lexer> LexerFactory::createLexer(CompilerContext& context) {
    return std::make_unique<Lexer>(context);
}

std::unique_ptr<Lexer> LexerFactory::createLexerFromFile(CompilerContext& context, 
                                                        const std::string& file_path) {
    auto lexer = createLexer(context);
    lexer->setInputFile(file_path);
    return lexer;
}

std::unique_ptr<Lexer> LexerFactory::createLexerFromString(CompilerContext& context, 
                                                          const std::string& input) {
    auto lexer = createLexer(context);
    lexer->setInput(input);
    return lexer;
}

} // namespace chtl