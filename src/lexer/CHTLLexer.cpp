#include "../../include/CHTLLexer.h"
#include <cctype>

namespace chtl {

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
    
    // 处理注释
    if (ch == '/') {
        char next = peekChar();
        if (next == '/') {
            return scanSingleLineComment();
        } else if (next == '*') {
            return scanMultiLineComment();
        } else {
            nextChar();
            return makeToken(TokenType::SLASH, "/");
        }
    }
    
    // 处理生成器注释 --
    if (ch == '-' && peekChar() == '-') {
        return scanGeneratorComment();
    }
    
    // 处理CHTL JS双花括号 {{
    if (ch == '{' && peekChar() == '{') {
        nextChar(); nextChar();
        return makeToken(TokenType::DOUBLE_LEFT_BRACE, "{{");
    }
    
    // 处理CHTL JS箭头操作符 ->
    if (ch == '-' && peekChar() == '>') {
        nextChar(); nextChar();
        return makeToken(TokenType::ARROW, "->");
    }
    
    // 基本token识别
    switch (ch) {
        case '{': nextChar(); return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': 
            // 检查是否为 }}
            if (peekChar() == '}') {
                nextChar(); nextChar();
                return makeToken(TokenType::DOUBLE_RIGHT_BRACE, "}}");
            } else {
                nextChar(); 
                return makeToken(TokenType::RIGHT_BRACE, "}");
            }
        case '(': nextChar(); return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': nextChar(); return makeToken(TokenType::RIGHT_PAREN, ")");
        case '[': 
            // 检查是否为声明块开始
            return scanBracketDeclaration();
        case ']': nextChar(); return makeToken(TokenType::RIGHT_BRACKET, "]");
        case ';': nextChar(); return makeToken(TokenType::SEMICOLON, ";");
        case ':': nextChar(); return makeToken(TokenType::COLON, ":");
        case '=': nextChar(); return makeToken(TokenType::EQUALS, "=");
        case ',': nextChar(); return makeToken(TokenType::COMMA, ",");
        case '.': nextChar(); return makeToken(TokenType::DOT, ".");
        case '&': nextChar(); return makeToken(TokenType::AMPERSAND, "&");
        case '#': nextChar(); return makeToken(TokenType::HASH, "#");
        case '"': return scanString('"');
        case '\'': return scanString('\'');
        default:
            if (isAlpha(ch) || ch == '_') {
                return scanIdentifier();
            } else if (isDigit(ch)) {
                return scanNumber();
            } else if (isWhitespace(ch)) {
                skipWhitespaceChars();
                return nextToken(); // 递归调用处理下一个字符
            } else {
                nextChar();
                return makeErrorToken("未知字符: " + std::string(1, ch));
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

// 注释处理
Token CHTLLexer::scanSingleLineComment() {
    size_t start = currentPos_;
    nextChar(); // skip first /
    nextChar(); // skip second /
    
    std::string content;
    while (!isAtEnd() && !isNewline(currentChar())) {
        content += currentChar();
        nextChar();
    }
    
    if (skipComments_) {
        return nextToken(); // 跳过注释，继续下一个token
    }
    
    return makeToken(TokenType::COMMENT_SINGLE, content, currentPos_ - start);
}

Token CHTLLexer::scanMultiLineComment() {
    size_t start = currentPos_;
    nextChar(); // skip /
    nextChar(); // skip *
    
    std::string content;
    while (!isAtEnd()) {
        if (currentChar() == '*' && peekChar() == '/') {
            nextChar(); // skip *
            nextChar(); // skip /
            break;
        }
        content += currentChar();
        nextChar();
    }
    
    if (skipComments_) {
        return nextToken(); // 跳过注释，继续下一个token
    }
    
    return makeToken(TokenType::COMMENT_MULTI, content, currentPos_ - start);
}

Token CHTLLexer::scanGeneratorComment() {
    size_t start = currentPos_;
    nextChar(); // skip first -
    nextChar(); // skip second -
    
    std::string content;
    while (!isAtEnd() && !isNewline(currentChar())) {
        content += currentChar();
        nextChar();
    }
    
    return makeToken(TokenType::COMMENT_GENERATOR, content, currentPos_ - start);
}

// 字符串处理
Token CHTLLexer::scanString(char quote) {
    size_t start = currentPos_;
    nextChar(); // skip opening quote
    
    std::string content;
    while (!isAtEnd() && currentChar() != quote) {
        if (currentChar() == '\\') {
            nextChar(); // skip escape character
            if (!isAtEnd()) {
                // 处理转义字符
                char escaped = currentChar();
                switch (escaped) {
                    case 'n': content += '\n'; break;
                    case 't': content += '\t'; break;
                    case 'r': content += '\r'; break;
                    case '\\': content += '\\'; break;
                    case '"': content += '"'; break;
                    case '\'': content += '\''; break;
                    default: content += escaped; break;
                }
                nextChar();
            }
        } else {
            content += currentChar();
            nextChar();
        }
    }
    
    if (isAtEnd()) {
        return makeErrorToken("未终止的字符串");
    }
    
    nextChar(); // skip closing quote
    
    TokenType type = (quote == '"') ? TokenType::STRING_LITERAL : TokenType::SINGLE_STRING;
    return makeToken(type, content, currentPos_ - start);
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

// 数字处理
Token CHTLLexer::scanNumber() {
    size_t start = currentPos_;
    std::string content;
    
    while (!isAtEnd() && (isDigit(currentChar()) || currentChar() == '.')) {
        content += currentChar();
        nextChar();
    }
    
    return makeToken(TokenType::NUMBER, content, currentPos_ - start);
}

// 标识符和关键字处理
Token CHTLLexer::scanIdentifier() {
    size_t start = currentPos_;
    std::string content;
    
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        content += currentChar();
        nextChar();
    }
    
    // 检查是否为关键字
    TokenType type = identifyKeyword(content);
    if (type != TokenType::UNKNOWN) {
        return makeToken(type, content, currentPos_ - start);
    }
    
    // 检查是否为HTML标签
    if (keywordMap_.isHTMLTag(content)) {
        return makeToken(TokenType::HTML_TAG, content, currentPos_ - start);
    }
    
    return makeToken(TokenType::IDENTIFIER, content, currentPos_ - start);
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

// 辅助方法实现
bool CHTLLexer::isAlpha(char ch) const {
    return std::isalpha(ch) || ch == '_';
}

bool CHTLLexer::isDigit(char ch) const {
    return std::isdigit(ch);
}

bool CHTLLexer::isAlphaNumeric(char ch) const {
    return std::isalnum(ch) || ch == '_';
}

bool CHTLLexer::isWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

bool CHTLLexer::isNewline(char ch) const {
    return ch == '\n' || ch == '\r';
}

// 缓冲区管理
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

// 声明块处理
Token CHTLLexer::scanBracketDeclaration() {
    size_t start = currentPos_;
    nextChar(); // skip [
    
    std::string content;
    while (!isAtEnd() && currentChar() != ']') {
        content += currentChar();
        nextChar();
    }
    
    if (isAtEnd()) {
        return makeErrorToken("未终止的声明块");
    }
    
    nextChar(); // skip ]
    
    // 识别声明类型
    if (content == "Template") {
        return makeToken(TokenType::TEMPLATE_DECL, content, currentPos_ - start);
    } else if (content == "Custom") {
        return makeToken(TokenType::CUSTOM_DECL, content, currentPos_ - start);
    } else if (content == "Origin") {
        return makeToken(TokenType::ORIGIN_DECL, content, currentPos_ - start);
    } else if (content == "Import") {
        return makeToken(TokenType::IMPORT_DECL, content, currentPos_ - start);
    } else if (content == "Namespace") {
        return makeToken(TokenType::NAMESPACE_DECL, content, currentPos_ - start);
    } else if (content == "Configuration") {
        return makeToken(TokenType::CONFIGURATION_DECL, content, currentPos_ - start);
    } else if (content == "Name") {
        return makeToken(TokenType::NAME_BLOCK, content, currentPos_ - start);
    } else if (content == "OriginType") {
        return makeToken(TokenType::ORIGIN_TYPE_BLOCK, content, currentPos_ - start);
    } else if (content == "Info") {
        return makeToken(TokenType::INFO_BLOCK, content, currentPos_ - start);
    } else if (content == "Export") {
        return makeToken(TokenType::EXPORT_BLOCK, content, currentPos_ - start);
    } else {
        return makeToken(TokenType::LEFT_BRACKET, "[", 1);
    }
}

TokenType CHTLLexer::identifyKeyword(const String& word) {
    // 基础关键字映射
    static std::unordered_map<std::string, TokenType> keywords = {
        // CHTL基础关键字
        {"text", TokenType::TEXT},
        {"style", TokenType::STYLE},
        {"script", TokenType::SCRIPT},
        
        // 类型标识符
        {"@Style", TokenType::AT_STYLE},
        {"@Element", TokenType::AT_ELEMENT},
        {"@Var", TokenType::AT_VAR},
        {"@Html", TokenType::AT_HTML},
        {"@JavaScript", TokenType::AT_JAVASCRIPT},
        {"@Chtl", TokenType::AT_CHTL},
        {"@CJmod", TokenType::AT_CJMOD},
        
        // 操作关键字
        {"inherit", TokenType::INHERIT},
        {"delete", TokenType::DELETE},
        {"insert", TokenType::INSERT},
        {"after", TokenType::AFTER},
        {"before", TokenType::BEFORE},
        {"replace", TokenType::REPLACE},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"except", TokenType::EXCEPT},
        
        // CHTL JS关键字
        {"vir", TokenType::VIR},
        {"listen", TokenType::LISTEN},
        {"delegate", TokenType::DELEGATE},
        {"animate", TokenType::ANIMATE},
        {"target", TokenType::TARGET},
        {"duration", TokenType::DURATION},
        {"easing", TokenType::EASING},
        {"begin", TokenType::BEGIN},
        {"when", TokenType::WHEN},
        {"end", TokenType::END},
        {"loop", TokenType::LOOP},
        {"direction", TokenType::DIRECTION},
        {"delay", TokenType::DELAY},
        {"callback", TokenType::CALLBACK},
        {"at", TokenType::AT_KEYWORD}
    };
    
    // 处理复合关键字
    if (word == "at") {
        // 检查下一个token是否为top或bottom
        size_t savedPos = currentPos_;
        skipWhitespaceChars();
        if (!isAtEnd()) {
            std::string next;
            while (!isAtEnd() && isAlpha(currentChar())) {
                next += currentChar();
                nextChar();
            }
            if (next == "top") {
                return TokenType::AT_TOP;
            } else if (next == "bottom") {
                return TokenType::AT_BOTTOM;
            }
        }
        // 恢复位置
        currentPos_ = savedPos;
    }
    
    auto it = keywords.find(word);
    return (it != keywords.end()) ? it->second : TokenType::UNKNOWN;
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
bool CHTLLexer::isValidNumberChar(char ch) const { 
    return isDigit(ch) || ch == '.'; 
}

TokenType CHTLLexer::getCEEquivalentToken(TokenType type) const { 
    // CE对等式：冒号和等号等价
    if (type == TokenType::COLON) return TokenType::EQUALS;
    if (type == TokenType::EQUALS) return TokenType::COLON;
    return type; 
}

bool CHTLLexer::isCEContext() const { 
    return getCurrentState() == LexerState::NORMAL; 
}

bool CHTLLexer::matchSequence(const String& sequence) const { return false; }
bool CHTLLexer::matchKeywordBoundary(const String& keyword) const { return false; }
void CHTLLexer::dumpCurrentState() const {}
String CHTLLexer::getCurrentContext() const { return ""; }

} // namespace chtl