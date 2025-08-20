#include "../../../include/compiler/chtl/Lexer.h"
#include "../../../include/common/GlobalMap.h"
#include <cctype>
#include <iostream>
#include <unordered_set>

namespace chtl {
namespace compiler {

// 静态成员初始化已在头文件中完成

CHTLLexer::CHTLLexer(const std::string& filename, 
                     const std::string& content,
                     DiagnosticCollector* diag)
    : filename_(filename), content_(content), diagnostics_(diag) {
    stateMachine_ = std::make_unique<StateMachine>();
}

char CHTLLexer::peek(size_t offset) const {
    size_t pos = position_ + offset;
    return pos < content_.size() ? content_[pos] : '\0';
}

char CHTLLexer::advance() {
    if (position_ >= content_.size()) {
        return '\0';
    }
    
    char ch = content_[position_++];
    updatePosition(ch);
    return ch;
}

void CHTLLexer::skipWhitespace() {
    while (position_ < content_.size()) {
        char ch = peek();
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            advance();
        } else {
            break;
        }
    }
}

void CHTLLexer::skipToEndOfLine() {
    while (position_ < content_.size() && peek() != '\n') {
        advance();
    }
}

bool CHTLLexer::match(const std::string& expected) {
    if (position_ + expected.size() > content_.size()) {
        return false;
    }
    
    if (content_.substr(position_, expected.size()) == expected) {
        for (size_t i = 0; i < expected.size(); ++i) {
            advance();
        }
        return true;
    }
    
    return false;
}

TokenLocation CHTLLexer::getCurrentLocation() const {
    return TokenLocation(filename_, line_, column_, position_);
}

void CHTLLexer::updatePosition(char ch) {
    if (ch == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
}

TokenPtr CHTLLexer::makeToken(TokenType type) {
    std::string value = content_.substr(tokenStart_, position_ - tokenStart_);
    return std::make_shared<Token>(type, value, TokenLocation(filename_, line_, column_ - value.size(), tokenStart_));
}

TokenPtr CHTLLexer::makeToken(TokenType type, const std::string& value) {
    return std::make_shared<Token>(type, value, TokenLocation(filename_, line_, column_ - value.size(), tokenStart_));
}

TokenPtr CHTLLexer::makeToken(TokenType type, const TokenValue& value) {
    std::string raw = content_.substr(tokenStart_, position_ - tokenStart_);
    return std::make_shared<Token>(type, value, TokenLocation(filename_, line_, column_ - raw.size(), tokenStart_), raw);
}

std::vector<TokenPtr> CHTLLexer::tokenize() {
    reset();
    std::vector<TokenPtr> tokens;
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        auto token = scanToken();
        if (token) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(makeToken(TokenType::EOF_TOKEN, ""));
    return tokens;
}

TokenPtr CHTLLexer::nextToken() {
    skipWhitespace();
    
    if (isAtEnd()) {
        return makeToken(TokenType::EOF_TOKEN, "");
    }
    
    return scanToken();
}

TokenPtr CHTLLexer::scanToken() {
    markTokenStart();
    
    char c = peek();
    
    // 检查双字符符号 {{
    if (c == '{' && peek(1) == '{') {
        return handleDoubleLeftBrace();
    }
    
    // 检查双字符符号 }}
    if (c == '}' && peek(1) == '}') {
        return handleDoubleRightBrace();
    }
    
    // 注释
    if (c == '/') {
        if (peek(1) == '/') {
            return scanComment();
        } else if (peek(1) == '*') {
            return scanComment();
        }
    }
    
    // 生成器注释
    if (c == '-' && peek(1) == '-') {
        return scanGeneratorComment();
    }
    
    // 箭头 ->
    if (c == '-' && peek(1) == '>') {
        return handleArrow();
    }
    
    // 符号
    switch (c) {
        case '{': advance(); return makeToken(TokenType::LEFT_BRACE);
        case '}': advance(); return makeToken(TokenType::RIGHT_BRACE);
        case '(': advance(); return makeToken(TokenType::LEFT_PAREN);
        case ')': advance(); return makeToken(TokenType::RIGHT_PAREN);
        case '[': 
            // 检查是否是方括号关键字
            if (peek(1) >= 'A' && peek(1) <= 'Z') {
                return scanBracketKeyword();
            }
            advance();
            return makeToken(TokenType::LEFT_BRACKET);
        case ']': advance(); return makeToken(TokenType::RIGHT_BRACKET);
        case ';': advance(); return makeToken(TokenType::SEMICOLON);
        case ':': advance(); return makeToken(TokenType::COLON);
        case '=': advance(); return makeToken(TokenType::EQUAL);
        case ',': advance(); return makeToken(TokenType::COMMA);
        case '.': advance(); return makeToken(TokenType::DOT);
        case '@':
            // 检查是否是类型标识符
            if (LexerUtils::isAlpha(peek(1))) {
                return scanTypeIdentifier();
            }
            advance();
            return makeToken(TokenType::AT);
        case '"':
        case '\'':
            return scanString(c);
    }
    
    // 标识符或关键字
    if (LexerUtils::isIdentifierStart(c)) {
        return scanIdentifier();
    }
    
    // 数字
    if (LexerUtils::isDigit(c)) {
        return scanNumber();
    }
    
    // 未知字符
    advance();
    reportError("未知字符: " + std::string(1, c));
    return makeToken(TokenType::UNKNOWN, std::string(1, c));
}

TokenPtr CHTLLexer::scanIdentifier() {
    // 扫描标识符
    while (LexerUtils::isIdentifierPart(peek())) {
        advance();
    }
    
    std::string value = content_.substr(tokenStart_, position_ - tokenStart_);
    
    // 检查是否是关键字
    auto it = keywords_.find(value);
    if (it != keywords_.end()) {
        // 特殊处理 "at top" 和 "at bottom"
        if (value == "at" && peek() == ' ') {
            size_t savedPos = position_;
            skipWhitespace();
            if (match("top")) {
                return makeToken(TokenType::KEYWORD_AT_TOP, "at top");
            } else if (match("bottom")) {
                return makeToken(TokenType::KEYWORD_AT_BOTTOM, "at bottom");
            }
            // 回退
            position_ = savedPos;
        }
        return makeToken(it->second, value);
    }
    
    // 检查是否是HTML元素
    if (GlobalMap::getInstance().isHTMLElement(value)) {
        return makeToken(TokenType::HTML_ELEMENT, value);
    }
    
    // 检查是否是CSS属性（在style上下文中）
    auto currentState = stateMachine_->getContext().getCurrentState();
    if (currentState == StateType::CHTL_STYLE ||
        currentState == StateType::CSS_DECLARATION) {
        if (GlobalMap::getInstance().isCSSProperty(value)) {
            return makeToken(TokenType::CSS_PROPERTY, value);
        }
    }
    
    // 普通标识符
    return makeToken(TokenType::IDENTIFIER, value);
}

TokenPtr CHTLLexer::scanString(char quote) {
    advance(); // 跳过引号
    
    std::string value;
    while (!isAtEnd() && peek() != quote) {
        if (peek() == '\\' && peek(1) == quote) {
            advance(); // 跳过反斜杠
            value += quote;
            advance();
        } else if (peek() == '\\') {
            advance();
            char escaped = advance();
            value += LexerUtils::unescapeChar(escaped);
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        reportError("未结束的字符串");
        return makeToken(TokenType::UNKNOWN);
    }
    
    advance(); // 跳过结束引号
    return makeToken(TokenType::STRING_LITERAL, value);
}

TokenPtr CHTLLexer::scanUnquotedLiteral() {
    // 扫描无引号字符串（用于CSS值等）
    while (!isAtEnd()) {
        char c = peek();
        if (c == ';' || c == '}' || c == '\n' || c == ',' || c == ')') {
            break;
        }
        advance();
    }
    
    std::string value = content_.substr(tokenStart_, position_ - tokenStart_);
    // 去除尾部空白
    value.erase(value.find_last_not_of(" \t\r") + 1);
    
    return makeToken(TokenType::UNQUOTED_LITERAL, value);
}

TokenPtr CHTLLexer::scanNumber() {
    // 扫描整数部分
    while (LexerUtils::isDigit(peek())) {
        advance();
    }
    
    // 检查小数点
    if (peek() == '.' && LexerUtils::isDigit(peek(1))) {
        advance(); // 跳过小数点
        while (LexerUtils::isDigit(peek())) {
            advance();
        }
    }
    
    // 检查单位（如px, em, %等）
    if (LexerUtils::isAlpha(peek()) || peek() == '%') {
        while (LexerUtils::isAlpha(peek()) || peek() == '%') {
            advance();
        }
        // 带单位的数字作为字符串处理
        std::string value = content_.substr(tokenStart_, position_ - tokenStart_);
        return makeToken(TokenType::STRING_LITERAL, value);
    }
    
    std::string value = content_.substr(tokenStart_, position_ - tokenStart_);
    double num = std::stod(value);
    return makeToken(TokenType::NUMBER_LITERAL, num);
}

TokenPtr CHTLLexer::scanComment() {
    if (match("//")) {
        // 单行注释
        std::string value;
        while (!isAtEnd() && peek() != '\n') {
            value += advance();
        }
        return makeToken(TokenType::SINGLE_LINE_COMMENT, value);
    } else if (match("/*")) {
        // 多行注释
        std::string value;
        while (!isAtEnd()) {
            if (peek() == '*' && peek(1) == '/') {
                advance(); advance(); // 跳过 */
                break;
            }
            value += advance();
        }
        return makeToken(TokenType::MULTI_LINE_COMMENT, value);
    }
    
    return nullptr;
}

TokenPtr CHTLLexer::scanGeneratorComment() {
    advance(); advance(); // 跳过 --
    
    std::string value;
    while (!isAtEnd() && peek() != '\n') {
        value += advance();
    }
    
    return makeToken(TokenType::GENERATOR_COMMENT, value);
}

TokenPtr CHTLLexer::scanBracketKeyword() {
    // 扫描如 [Template], [Custom] 等
    size_t start = position_;
    advance(); // 跳过 [
    
    std::string keyword;
    while (LexerUtils::isAlpha(peek())) {
        keyword += advance();
    }
    
    if (peek() != ']') {
        // 不是有效的方括号关键字，回退
        position_ = start;
        advance();
        return makeToken(TokenType::LEFT_BRACKET);
    }
    
    advance(); // 跳过 ]
    
    std::string value = "[" + keyword + "]";
    
    // 映射到对应的token类型
    if (keyword == "Template") return makeToken(TokenType::KEYWORD_TEMPLATE, value);
    if (keyword == "Custom") return makeToken(TokenType::KEYWORD_CUSTOM, value);
    if (keyword == "Origin") return makeToken(TokenType::KEYWORD_ORIGIN, value);
    if (keyword == "Import") return makeToken(TokenType::KEYWORD_IMPORT, value);
    if (keyword == "Namespace") return makeToken(TokenType::KEYWORD_NAMESPACE, value);
    if (keyword == "Configuration") return makeToken(TokenType::KEYWORD_CONFIGURATION, value);
    if (keyword == "Info") return makeToken(TokenType::KEYWORD_INFO, value);
    if (keyword == "Export") return makeToken(TokenType::KEYWORD_EXPORT, value);
    
    // 未知的方括号关键字
    return makeToken(TokenType::IDENTIFIER, value);
}

TokenPtr CHTLLexer::scanTypeIdentifier() {
    // 扫描如 @Style, @Element 等
    advance(); // 跳过 @
    
    std::string name;
    while (LexerUtils::isAlpha(peek())) {
        name += advance();
    }
    
    std::string value = "@" + name;
    
    // 检查类型标识符映射
    auto it = typeIdentifiers_.find(value);
    if (it != typeIdentifiers_.end()) {
        return makeToken(it->second, value);
    }
    
    // 未知的类型标识符，作为普通标识符
    return makeToken(TokenType::IDENTIFIER, value);
}

TokenPtr CHTLLexer::scanHTMLElement() {
    return scanIdentifier();
}

TokenPtr CHTLLexer::scanCSSProperty() {
    return scanIdentifier();
}

TokenPtr CHTLLexer::handleDoubleLeftBrace() {
    advance(); advance(); // 跳过 {{
    return makeToken(TokenType::DOUBLE_LEFT_BRACE, "{{");
}

TokenPtr CHTLLexer::handleDoubleRightBrace() {
    advance(); advance(); // 跳过 }}
    return makeToken(TokenType::DOUBLE_RIGHT_BRACE, "}}");
}

TokenPtr CHTLLexer::handleArrow() {
    advance(); advance(); // 跳过 ->
    return makeToken(TokenType::ARROW, "->");
}

bool CHTLLexer::isValidIdentifier(const std::string& id) const {
    if (id.empty()) return false;
    
    if (!LexerUtils::isIdentifierStart(id[0])) return false;
    
    for (size_t i = 1; i < id.size(); ++i) {
        if (!LexerUtils::isIdentifierPart(id[i])) return false;
    }
    
    return true;
}

bool CHTLLexer::isHTMLElement(const std::string& name) const {
    return GlobalMap::getInstance().isHTMLElement(name);
}

bool CHTLLexer::isCSSProperty(const std::string& name) const {
    return GlobalMap::getInstance().isCSSProperty(name);
}

void CHTLLexer::reportError(const std::string& message) {
    if (diagnostics_) {
        diagnostics_->lexicalError(message, getCurrentLocation());
    }
}

void CHTLLexer::reportWarning(const std::string& message) {
    if (diagnostics_) {
        diagnostics_->warning(message, getCurrentLocation());
    }
}

TokenPtr CHTLLexer::peekToken(size_t offset) {
    // 确保缓冲区中有足够的token
    while (tokenBuffer_.size() <= bufferPosition_ + offset) {
        auto token = nextToken();
        if (!token || token->type == TokenType::EOF_TOKEN) {
            return token;
        }
        tokenBuffer_.push_back(token);
    }
    
    return tokenBuffer_[bufferPosition_ + offset];
}

bool CHTLLexer::hasMoreTokens() const {
    return !isAtEnd() || bufferPosition_ < tokenBuffer_.size();
}

void CHTLLexer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    tokenStart_ = 0;
    tokenBuffer_.clear();
    bufferPosition_ = 0;
    stateMachine_->reset();
}

void CHTLLexer::dumpTokens(const std::vector<TokenPtr>& tokens) const {
    std::cout << "=== CHTL Tokens ===\n";
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        std::cout << i << ": " << Token::getTypeName(token->type);
        if (!token->getStringValue().empty()) {
            std::cout << " [" << token->getStringValue() << "]";
        }
        std::cout << " at " << token->location.toString() << "\n";
    }
}

// LexerUtils实现
namespace LexerUtils {

bool isAlpha(char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool isDigit(char ch) {
    return ch >= '0' && ch <= '9';
}

bool isAlphaNumeric(char ch) {
    return isAlpha(ch) || isDigit(ch);
}

bool isIdentifierStart(char ch) {
    return isAlpha(ch) || ch == '_' || ch == '$';
}

bool isIdentifierPart(char ch) {
    return isAlphaNumeric(ch) || ch == '_' || ch == '$' || ch == '-';
}

bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

bool isNewline(char ch) {
    return ch == '\n' || ch == '\r';
}

std::string unescapeString(const std::string& str) {
    std::string result;
    bool escaped = false;
    
    for (char ch : str) {
        if (escaped) {
            result += unescapeChar(ch);
            escaped = false;
        } else {
            if (ch == '\\') {
                escaped = true;
            } else {
                result += ch;
            }
        }
    }
    
    return result;
}

char unescapeChar(char ch) {
    switch (ch) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case '\\': return '\\';
        case '"': return '"';
        case '\'': return '\'';
        default: return ch;
    }
}

bool isKeyword(const std::string& str) {
    static const std::unordered_set<std::string> keywords = {
        "text", "style", "script", "inherit", "delete", "insert",
        "after", "before", "replace", "at", "from", "as", "except",
        "vir", "animate", "listen", "delegate"
    };
    
    return keywords.find(str) != keywords.end();
}

bool isTypeIdentifier(const std::string& str) {
    return str.size() > 1 && str[0] == '@' && isAlpha(str[1]);
}

bool isBracketKeyword(const std::string& str) {
    return str.size() > 2 && str[0] == '[' && str[str.size()-1] == ']';
}

} // namespace LexerUtils

} // namespace compiler
} // namespace chtl