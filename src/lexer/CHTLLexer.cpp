#include "../../include/CHTLLexer.h"
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer(const String& sourceCode, StateMachine& stateMachine, ContextManager& contextManager)
    : sourceCode_(sourceCode), currentPos_(0), currentLine_(1), currentColumn_(1),
      skipWhitespace_(true), skipComments_(true), generatePositionInfo_(true),
      stateMachine_(stateMachine), contextManager_(contextManager), 
      keywordMap_(KeywordMap::getInstance()) {
    stateStack_.push(LexerState::NORMAL);
}

std::vector<Token> CHTLLexer::tokenize() {
    std::vector<Token> tokens;
    while (hasMoreTokens()) {
        Token token = nextToken();
        if (token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

Token CHTLLexer::nextToken() {
    if (hasBufferedTokens()) {
        return getBufferedToken();
    }
    
    if (skipWhitespace_) {
        skipWhitespaceChars();
    }
    
    if (isAtEnd()) {
        return makeToken(TokenType::END_OF_FILE);
    }
    
    char ch = currentChar();
    
    // 基本token识别
    switch (ch) {
        case '{': nextChar(); return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': nextChar(); return makeToken(TokenType::RIGHT_BRACE, "}");
        case '(': nextChar(); return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': nextChar(); return makeToken(TokenType::RIGHT_PAREN, ")");
        case '[': nextChar(); return makeToken(TokenType::LEFT_BRACKET, "[");
        case ']': nextChar(); return makeToken(TokenType::RIGHT_BRACKET, "]");
        case ';': nextChar(); return makeToken(TokenType::SEMICOLON, ";");
        case ':': nextChar(); return makeToken(TokenType::COLON, ":");
        case '=': nextChar(); return makeToken(TokenType::EQUALS, "=");
        case ',': nextChar(); return makeToken(TokenType::COMMA, ",");
        case '.': nextChar(); return makeToken(TokenType::DOT, ".");
        case '/': nextChar(); return makeToken(TokenType::SLASH, "/");
        case '&': nextChar(); return makeToken(TokenType::AMPERSAND, "&");
        case '"': return scanString('"');
        case '\'': return scanString('\'');
        default:
            if (isAlpha(ch) || ch == '_') {
                return scanIdentifier();
            } else if (isDigit(ch)) {
                return scanNumber();
            } else {
                nextChar();
                return makeErrorToken("未知字符");
            }
    }
}

bool CHTLLexer::hasMoreTokens() const {
    return !isAtEnd() || hasBufferedTokens();
}

Token CHTLLexer::peekToken(size_t offset) {
    // 简化实现
    return nextToken();
}

std::vector<Token> CHTLLexer::peekTokens(size_t count) {
    std::vector<Token> tokens;
    for (size_t i = 0; i < count && hasMoreTokens(); ++i) {
        tokens.push_back(peekToken(i));
    }
    return tokens;
}

void CHTLLexer::pushState(LexerState state) {
    stateStack_.push(state);
}

void CHTLLexer::popState() {
    if (stateStack_.size() > 1) {
        stateStack_.pop();
    }
}

LexerState CHTLLexer::getCurrentState() const {
    return stateStack_.top();
}

char CHTLLexer::currentChar() const {
    if (isAtEnd()) return '\0';
    return sourceCode_[currentPos_];
}

char CHTLLexer::peekChar(size_t offset) const {
    size_t pos = currentPos_ + offset;
    if (pos >= sourceCode_.length()) return '\0';
    return sourceCode_[pos];
}

char CHTLLexer::nextChar() {
    if (isAtEnd()) return '\0';
    char ch = sourceCode_[currentPos_++];
    updatePosition(ch);
    return ch;
}

void CHTLLexer::skipWhitespaceChars() {
    while (!isAtEnd() && isWhitespace(currentChar())) {
        nextChar();
    }
}

bool CHTLLexer::isAtEnd() const {
    return currentPos_ >= sourceCode_.length();
}

Token CHTLLexer::makeToken(TokenType type, const String& value, size_t length) {
    if (length == 0 && !value.empty()) {
        length = value.length();
    }
    
    Token token(type, value, currentLine_, currentColumn_, currentPos_, length);
    statistics_.tokensGenerated++;
    
    return token;
}

Token CHTLLexer::makeErrorToken(const String& message) {
    reportError(message);
    return makeToken(TokenType::UNKNOWN, message);
}

Token CHTLLexer::scanString(char quote) {
    String value;
    nextChar(); // 跳过开始引号
    
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\' && peekChar(1) != '\0') {
            nextChar(); // 跳过反斜杠
            char escaped = nextChar();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: value += escaped; break;
            }
        } else {
            value += nextChar();
        }
    }
    
    if (isAtEnd()) {
        return makeErrorToken("未终止的字符串");
    }
    
    nextChar(); // 跳过结束引号
    
    TokenType type = (quote == '"') ? TokenType::STRING_LITERAL : TokenType::SINGLE_STRING;
    statistics_.literalsFound++;
    return makeToken(type, value);
}

Token CHTLLexer::scanUnquotedLiteral() {
    String value;
    while (!isAtEnd() && !isWhitespace(currentChar()) && 
           currentChar() != '{' && currentChar() != '}' && 
           currentChar() != ';' && currentChar() != ':') {
        value += nextChar();
    }
    
    statistics_.literalsFound++;
    return makeToken(TokenType::UNQUOTED_LITERAL, value);
}

Token CHTLLexer::scanNumber() {
    String value;
    bool hasDecimal = false;
    
    while (!isAtEnd() && (isDigit(currentChar()) || currentChar() == '.')) {
        if (currentChar() == '.') {
            if (hasDecimal) break;
            hasDecimal = true;
        }
        value += nextChar();
    }
    
    statistics_.literalsFound++;
    return makeToken(TokenType::NUMBER, value);
}

Token CHTLLexer::scanIdentifier() {
    String value;
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
        value += nextChar();
    }
    
    // 检查是否为关键字
    TokenType type = keywordMap_.getTokenType(value);
    if (type != TokenType::IDENTIFIER) {
        statistics_.keywordsFound++;
    } else {
        statistics_.identifiersFound++;
    }
    
    return makeToken(type, value);
}

void CHTLLexer::reportError(const String& message) {
    errors_.emplace_back(message, "", currentLine_, currentColumn_);
    statistics_.errorsEncountered++;
}

void CHTLLexer::updatePosition(char ch) {
    if (ch == '\n') {
        currentLine_++;
        currentColumn_ = 1;
    } else {
        currentColumn_++;
    }
}

bool CHTLLexer::isAlpha(char ch) const {
    return std::isalpha(ch) != 0;
}

bool CHTLLexer::isDigit(char ch) const {
    return std::isdigit(ch) != 0;
}

bool CHTLLexer::isAlphaNumeric(char ch) const {
    return std::isalnum(ch) != 0;
}

bool CHTLLexer::isWhitespace(char ch) const {
    return std::isspace(ch) != 0;
}

bool CHTLLexer::isValidIdentifierStart(char ch) const {
    return isAlpha(ch) || ch == '_';
}

bool CHTLLexer::isValidIdentifierChar(char ch) const {
    return isAlphaNumeric(ch) || ch == '_';
}

void CHTLLexer::bufferToken(const Token& token) {
    tokenBuffer_.push(token);
}

Token CHTLLexer::getBufferedToken() {
    if (tokenBuffer_.empty()) {
        return makeToken(TokenType::UNKNOWN);
    }
    Token token = tokenBuffer_.front();
    tokenBuffer_.pop();
    return token;
}

bool CHTLLexer::hasBufferedTokens() const {
    return !tokenBuffer_.empty();
}

void CHTLLexer::clearBuffer() {
    while (!tokenBuffer_.empty()) {
        tokenBuffer_.pop();
    }
}

// 其他方法的简化实现
Token CHTLLexer::scanSingleLineComment() { return makeToken(TokenType::COMMENT_SINGLE, "//"); }
Token CHTLLexer::scanMultiLineComment() { return makeToken(TokenType::COMMENT_MULTI, "/* */"); }
Token CHTLLexer::scanGeneratorComment() { return makeToken(TokenType::COMMENT_GENERATOR, "--"); }
Token CHTLLexer::scanTemplateDeclaration() { return makeToken(TokenType::TEMPLATE_DECL, "[Template]"); }
Token CHTLLexer::scanCustomDeclaration() { return makeToken(TokenType::CUSTOM_DECL, "[Custom]"); }
Token CHTLLexer::scanOriginDeclaration() { return makeToken(TokenType::ORIGIN_DECL, "[Origin]"); }
Token CHTLLexer::scanImportDeclaration() { return makeToken(TokenType::IMPORT_DECL, "[Import]"); }
Token CHTLLexer::scanNamespaceDeclaration() { return makeToken(TokenType::NAMESPACE_DECL, "[Namespace]"); }
Token CHTLLexer::scanConfigurationDeclaration() { return makeToken(TokenType::CONFIGURATION_DECL, "[Configuration]"); }
Token CHTLLexer::scanCHTLJSSelector() { return makeToken(TokenType::DOUBLE_LEFT_BRACE, "{{"); }
Token CHTLLexer::scanArrowOperator() { return makeToken(TokenType::ARROW, "->"); }
Token CHTLLexer::scanCHTLJSKeyword() { return makeToken(TokenType::VIR, "vir"); }
Token CHTLLexer::scanOperator() { return makeToken(TokenType::COLON, ":"); }
Token CHTLLexer::scanPunctuation() { return makeToken(TokenType::SEMICOLON, ";"); }

TokenType CHTLLexer::identifyKeyword(const String& word) {
    return keywordMap_.getTokenType(word);
}

bool CHTLLexer::isKeywordInContext(const String& word) {
    return keywordMap_.isKeyword(word);
}

bool CHTLLexer::shouldScanAsUnquotedLiteral() const { return false; }
bool CHTLLexer::shouldScanAsCHTLJS() const { return false; }
bool CHTLLexer::isInStyleContext() const { return false; }
bool CHTLLexer::isInScriptContext() const { return false; }
bool CHTLLexer::validateToken(const Token& token) { return true; }
bool CHTLLexer::validateStringLiteral(const String& value) { return true; }
bool CHTLLexer::validateIdentifier(const String& value) { return true; }
bool CHTLLexer::validateNumber(const String& value) { return true; }
void CHTLLexer::reportWarning(const String& message) {}
void CHTLLexer::recoverFromError() {}
void CHTLLexer::markPosition() {}
void CHTLLexer::resetPosition() {}
bool CHTLLexer::isNewline(char ch) const { return ch == '\n' || ch == '\r'; }
bool CHTLLexer::isValidNumberChar(char ch) const { return isDigit(ch) || ch == '.'; }
TokenType CHTLLexer::getCEEquivalentToken(TokenType type) const { return type; }
bool CHTLLexer::isCEContext() const { return false; }
bool CHTLLexer::matchSequence(const String& sequence) const { return false; }
bool CHTLLexer::matchKeywordBoundary(const String& keyword) const { return false; }
void CHTLLexer::dumpCurrentState() const {}
String CHTLLexer::getCurrentContext() const { return ""; }

} // namespace CHTL