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
        if (peekChar(1) == '/') {
            return scanSingleLineComment();
        } else if (peekChar(1) == '*') {
            return scanMultiLineComment();
        } else {
            nextChar();
            return makeToken(TokenType::SLASH, "/");
        }
    }
    
    // 处理生成器注释 --
    if (ch == '-' && peekChar(1) == '-') {
        return scanGeneratorComment();
    }
    
    // 处理CHTL JS增强选择器 {{
    if (ch == '{' && peekChar(1) == '{') {
        nextChar(); nextChar();
        return makeToken(TokenType::DOUBLE_LEFT_BRACE, "{{");
    }
    
    // 处理CHTL JS结束选择器 }}
    if (ch == '}' && peekChar(1) == '}') {
        nextChar(); nextChar();
        return makeToken(TokenType::DOUBLE_RIGHT_BRACE, "}}");
    }
    
    // 处理CHTL JS箭头操作符 ->
    if (ch == '-' && peekChar(1) == '>') {
        nextChar(); nextChar();
        return makeToken(TokenType::ARROW, "->");
    }
    
    // 处理声明块 [Template], [Custom], [Origin], [Import], [Namespace], [Configuration]
    if (ch == '[') {
        size_t savePos = currentPos_;
        nextChar(); // skip [
        
        String content;
        while (!isAtEnd() && currentChar() != ']') {
            content += nextChar();
        }
        
        if (isAtEnd()) {
            currentPos_ = savePos;
            nextChar();
            return makeToken(TokenType::LEFT_BRACKET, "[");
        }
        
        nextChar(); // skip ]
        
        if (content == "Template") {
            return makeToken(TokenType::TEMPLATE_DECL, "[Template]");
        } else if (content == "Custom") {
            return makeToken(TokenType::CUSTOM_DECL, "[Custom]");
        } else if (content == "Origin") {
            return makeToken(TokenType::ORIGIN_DECL, "[Origin]");
        } else if (content == "Import") {
            return makeToken(TokenType::IMPORT_DECL, "[Import]");
        } else if (content == "Namespace") {
            return makeToken(TokenType::NAMESPACE_DECL, "[Namespace]");
        } else if (content == "Configuration") {
            return makeToken(TokenType::CONFIGURATION_DECL, "[Configuration]");
        } else if (content == "Name") {
            return makeToken(TokenType::NAME_BLOCK, "[Name]");
        } else if (content == "OriginType") {
            return makeToken(TokenType::ORIGIN_TYPE_BLOCK, "[OriginType]");
        } else if (content == "Info") {
            return makeToken(TokenType::INFO_BLOCK, "[Info]");
        } else if (content == "Export") {
            return makeToken(TokenType::EXPORT_BLOCK, "[Export]");
        } else {
            // 不是特殊声明，回退
            currentPos_ = savePos;
            nextChar();
            return makeToken(TokenType::LEFT_BRACKET, "[");
        }
    }
    
    // 处理@开头的类型标识符
    if (ch == '@') {
        nextChar();
        String typeContent;
        while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
            typeContent += nextChar();
        }
        
        if (typeContent == "Style") {
            return makeToken(TokenType::AT_STYLE, "@Style");
        } else if (typeContent == "Element") {
            return makeToken(TokenType::AT_ELEMENT, "@Element");
        } else if (typeContent == "Var") {
            return makeToken(TokenType::AT_VAR, "@Var");
        } else if (typeContent == "Html") {
            return makeToken(TokenType::AT_HTML, "@Html");
        } else if (typeContent == "JavaScript") {
            return makeToken(TokenType::AT_JAVASCRIPT, "@JavaScript");
        } else if (typeContent == "Chtl") {
            return makeToken(TokenType::AT_CHTL, "@Chtl");
        } else if (typeContent == "CJmod") {
            return makeToken(TokenType::AT_CJMOD, "@CJmod");
        } else {
            // 可能是自定义原始嵌入类型
            return makeToken(TokenType::IDENTIFIER, "@" + typeContent);
        }
    }
    
    // 基本符号处理
    switch (ch) {
        case '{': nextChar(); return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': nextChar(); return makeToken(TokenType::RIGHT_BRACE, "}");
        case '(': nextChar(); return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': nextChar(); return makeToken(TokenType::RIGHT_PAREN, ")");
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
            } else if (!isWhitespace(ch) && shouldScanAsUnquotedLiteral()) {
                return scanUnquotedLiteral();
            } else {
                nextChar();
                return makeErrorToken("未知字符: " + String(1, ch));
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
    
    // 无引号字面量可以包含字母、数字、连字符、下划线等，但不能包含特殊符号
    while (!isAtEnd() && !isWhitespace(currentChar()) && 
           currentChar() != '{' && currentChar() != '}' && 
           currentChar() != ';' && currentChar() != ':' && 
           currentChar() != '=' && currentChar() != ',' &&
           currentChar() != '(' && currentChar() != ')' &&
           currentChar() != '[' && currentChar() != ']') {
        value += nextChar();
    }
    
    if (value.empty()) {
        return makeErrorToken("空的无引号字面量");
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
    while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        value += nextChar();
    }
    
    // 检查是否为CHTL关键字
    TokenType type = identifyKeyword(value);
    
    // 如果不是关键字，检查是否为HTML标签
    if (type == TokenType::IDENTIFIER && keywordMap_.isHTMLTag(value)) {
        type = TokenType::HTML_TAG;
    }
    
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
Token CHTLLexer::scanSingleLineComment() { 
    nextChar(); // skip first /
    nextChar(); // skip second /
    
    String content = "//";
    while (!isAtEnd() && !isNewline(currentChar())) {
        content += nextChar();
    }
    
    if (skipComments_) {
        statistics_.commentsSkipped++;
        return nextToken(); // 递归调用获取下一个token
    }
    
    return makeToken(TokenType::COMMENT_SINGLE, content);
}

Token CHTLLexer::scanMultiLineComment() { 
    nextChar(); // skip /
    nextChar(); // skip *
    
    String content = "/*";
    while (!isAtEnd()) {
        if (currentChar() == '*' && peekChar(1) == '/') {
            content += nextChar(); // *
            content += nextChar(); // /
            break;
        }
        content += nextChar();
    }
    
    if (skipComments_) {
        statistics_.commentsSkipped++;
        return nextToken(); // 递归调用获取下一个token
    }
    
    return makeToken(TokenType::COMMENT_MULTI, content);
}

Token CHTLLexer::scanGeneratorComment() { 
    nextChar(); // skip first -
    nextChar(); // skip second -
    
    String content = "--";
    while (!isAtEnd() && !isNewline(currentChar())) {
        content += nextChar();
    }
    
    // 生成器注释不应该被跳过，因为它们有语义意义
    return makeToken(TokenType::COMMENT_GENERATOR, content);
}

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
    // 基础CHTL关键字
    if (word == "text") return TokenType::TEXT;
    if (word == "style") return TokenType::STYLE;
    if (word == "script") return TokenType::SCRIPT;
    if (word == "inherit") return TokenType::INHERIT;
    if (word == "delete") return TokenType::DELETE;
    if (word == "insert") return TokenType::INSERT;
    if (word == "after") return TokenType::AFTER;
    if (word == "before") return TokenType::BEFORE;
    if (word == "replace") return TokenType::REPLACE;
    if (word == "from") return TokenType::FROM;
    if (word == "as") return TokenType::AS;
    if (word == "except") return TokenType::EXCEPT;
    
    // 处理复合关键字 "at top", "at bottom"
    if (word == "at") {
        // 需要预读下一个token来确定是否为复合关键字
        size_t savePos = currentPos_;
        skipWhitespaceChars();
        
        if (!isAtEnd()) {
            String nextWord;
            while (!isAtEnd() && (isAlphaNumeric(currentChar()) || currentChar() == '_')) {
                nextWord += currentChar();
                currentPos_++;
            }
            
            if (nextWord == "top") {
                return TokenType::AT_TOP;
            } else if (nextWord == "bottom") {
                return TokenType::AT_BOTTOM;
            } else {
                // 回退
                currentPos_ = savePos;
            }
        } else {
            currentPos_ = savePos;
        }
        
        return TokenType::AT_KEYWORD;
    }
    
    // CHTL JS关键字
    if (word == "vir") return TokenType::VIR;
    if (word == "listen") return TokenType::LISTEN;
    if (word == "delegate") return TokenType::DELEGATE;
    if (word == "animate") return TokenType::ANIMATE;
    if (word == "target") return TokenType::TARGET;
    if (word == "duration") return TokenType::DURATION;
    if (word == "easing") return TokenType::EASING;
    if (word == "begin") return TokenType::BEGIN;
    if (word == "when") return TokenType::WHEN;
    if (word == "end") return TokenType::END;
    if (word == "loop") return TokenType::LOOP;
    if (word == "direction") return TokenType::DIRECTION;
    if (word == "delay") return TokenType::DELAY;
    if (word == "callback") return TokenType::CALLBACK;
    
    // 使用KeywordMap进行其他关键字查找
    return keywordMap_.getTokenType(word);
}

bool CHTLLexer::isKeywordInContext(const String& word) {
    return keywordMap_.isKeyword(word);
}

bool CHTLLexer::shouldScanAsUnquotedLiteral() const {
    // 在样式上下文中，CSS属性值可以是无引号的
    // 在属性值上下文中，HTML属性值可以是无引号的
    return isInStyleContext() || (contextManager_.getCurrentContext().type == ContextType::ATTRIBUTE);
}

bool CHTLLexer::shouldScanAsCHTLJS() const {
    return isInScriptContext();
}

bool CHTLLexer::isInStyleContext() const {
    return contextManager_.getCurrentContext().type == ContextType::STYLE_BLOCK;
}

bool CHTLLexer::isInScriptContext() const {
    return contextManager_.getCurrentContext().type == ContextType::SCRIPT_BLOCK;
}

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
TokenType CHTLLexer::getCEEquivalentToken(TokenType type) const {
    // CE对等式：冒号(:)与等号(=)完全等价
    if (type == TokenType::COLON) {
        return TokenType::EQUALS;
    } else if (type == TokenType::EQUALS) {
        return TokenType::COLON;
    }
    return type;
}

bool CHTLLexer::isCEContext() const {
    // 在属性定义或CSS属性定义中，支持CE对等式
    ContextType contextType = contextManager_.getCurrentContext().type;
    return contextType == ContextType::ATTRIBUTE || 
           contextType == ContextType::STYLE_BLOCK || 
           contextType == ContextType::TEMPLATE || 
           contextType == ContextType::CUSTOM;
}

bool CHTLLexer::matchSequence(const String& sequence) const { return false; }
bool CHTLLexer::matchKeywordBoundary(const String& keyword) const { return false; }
void CHTLLexer::dumpCurrentState() const {}
String CHTLLexer::getCurrentContext() const { return ""; }

} // namespace chtl