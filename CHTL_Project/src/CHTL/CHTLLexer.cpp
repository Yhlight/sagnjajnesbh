#include "CHTL/CHTLLexer.hpp"
#include <iostream>
#include <cctype>

namespace CHTL {

CHTLLexer::CHTLLexer() 
    : position_(0), line_(1), column_(1), length_(0), 
      state_(LexerState::Normal), 
      skipWhitespace_(true), skipComments_(false), debugMode_(false),
      keywordMap_(std::make_unique<CHTLKeywordMap>()) {
    
    statistics_ = {};
    InitializeHtmlElements();
    
    // 初始化正则表达式模式
    identifierPattern_ = std::regex(R"([a-zA-Z_][a-zA-Z0-9_-]*)");
    numberPattern_ = std::regex(R"(\d+(\.\d+)?)");
    stringPattern_ = std::regex(R"(("[^"]*")|('[^']*'))");
    commentPattern_ = std::regex(R"((//[^\r\n]*)|(/\*.*?\*/)|(--[^\r\n]*))");
    chtljsSelectorPattern_ = std::regex(R"(\{\{[^}]+\}\})");
    htmlElementPattern_ = std::regex(R"([a-zA-Z][a-zA-Z0-9-]*)");
}

void CHTLLexer::SetSource(const String& source, const String& filename) {
    source_ = source;
    filename_ = filename;
    position_ = 0;
    line_ = 1;
    column_ = 1;
    length_ = source.length();
    state_ = LexerState::Normal;
    
    // 清空状态栈
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    
    statistics_ = {};
}

CHTLToken CHTLLexer::NextToken() {
    while (!IsAtEnd()) {
        if (skipWhitespace_ && IsWhitespace(CurrentChar())) {
            SkipWhitespace();
            continue;
        }
        
        CHTLToken token = ScanToken();
        
        if (!skipComments_ || !token.IsComment()) {
            UpdateStatistics(token.type);
            return token;
        }
    }
    
    return MakeToken(CHTLTokenType::EOF_TOKEN);
}

CHTLToken CHTLLexer::PeekToken(size_t lookahead) {
    // 保存当前状态
    size_t savedPosition = position_;
    size_t savedLine = line_;
    size_t savedColumn = column_;
    LexerState savedState = state_;
    auto savedStateStack = stateStack_;
    
    // 获取Token
    CHTLToken token;
    for (size_t i = 0; i < lookahead; ++i) {
        token = NextToken();
        if (token.type == CHTLTokenType::EOF_TOKEN) {
            break;
        }
    }
    
    // 恢复状态
    position_ = savedPosition;
    line_ = savedLine;
    column_ = savedColumn;
    state_ = savedState;
    stateStack_ = savedStateStack;
    
    return token;
}

bool CHTLLexer::IsAtEnd() const {
    return position_ >= length_;
}

SourceLocation CHTLLexer::GetCurrentLocation() const {
    return MakeLocation();
}

void CHTLLexer::SetOptions(bool skipWhitespace, bool skipComments, bool enableDebug) {
    skipWhitespace_ = skipWhitespace;
    skipComments_ = skipComments;
    debugMode_ = enableDebug;
}

void CHTLLexer::Reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
    state_ = LexerState::Normal;
    
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    
    statistics_ = {};
}

std::vector<CHTLToken> CHTLLexer::GetAllTokens() {
    std::vector<CHTLToken> tokens;
    Reset();
    
    while (!IsAtEnd()) {
        CHTLToken token = NextToken();
        tokens.push_back(token);
        
        if (token.type == CHTLTokenType::EOF_TOKEN) {
            break;
        }
    }
    
    return tokens;
}

CHTLToken CHTLLexer::ScanToken() {
    char ch = CurrentChar();
    
    // 处理注释
    if (ch == '/' && PeekChar() == '/') {
        return ScanComment();
    }
    if (ch == '-' && PeekChar() == '-') {
        return ScanComment();
    }
    if (ch == '/' && PeekChar() == '*') {
        return ScanComment();
    }
    
    // 处理字符串
    if (ch == '"' || ch == '\'') {
        return ScanString(ch);
    }
    
    // 处理CHTL JS选择器（暂时保留，但专注CHTL）
    if (ch == '{' && PeekChar() == '{') {
        return ScanCHTLJSSelector();
    }
    
    // 处理数字
    if (IsDigit(ch)) {
        return ScanNumber();
    }
    
    // 处理标识符和关键字
    if (IsAlpha(ch) || ch == '_') {
        return ScanIdentifierOrKeyword();
    }
    
    // 处理操作符和分隔符
    return ScanOperator();
}

CHTLToken CHTLLexer::ScanIdentifierOrKeyword() {
    SourceLocation startLocation = MakeLocation();
    String identifier;
    
    while (!IsAtEnd() && IsValidIdentifierChar(CurrentChar())) {
        identifier += CurrentChar();
        NextChar();
    }
    
    // 检查复合关键字（如 "at top", "at bottom"）
    if (identifier == "at" && !IsAtEnd() && IsWhitespace(CurrentChar())) {
        // 跳过空白
        while (!IsAtEnd() && IsWhitespace(CurrentChar())) {
            NextChar();
        }
        
        // 检查是否为 "top" 或 "bottom"
        String nextWord;
        size_t savedPos = position_;
        while (!IsAtEnd() && IsAlpha(CurrentChar())) {
            nextWord += CurrentChar();
            NextChar();
        }
        
        if (nextWord == "top") {
            return CHTLToken(CHTLTokenType::AT_TOP, "at top", startLocation);
        } else if (nextWord == "bottom") {
            return CHTLToken(CHTLTokenType::AT_BOTTOM, "at bottom", startLocation);
        } else {
            // 回退
            position_ = savedPos;
        }
    }
    
    // 检查是否为关键字
    CHTLTokenType tokenType = keywordMap_->GetKeywordType(identifier);
    if (tokenType != CHTLTokenType::IDENTIFIER) {
        return CHTLToken(tokenType, identifier, startLocation);
    }
    
    // 检查是否为HTML元素
    if (IsHtmlElement(identifier)) {
        return CHTLToken(CHTLTokenType::HTML_ELEMENT, identifier, startLocation);
    }
    
    return CHTLToken(CHTLTokenType::IDENTIFIER, identifier, startLocation);
}

CHTLToken CHTLLexer::ScanNumber() {
    SourceLocation startLocation = MakeLocation();
    String number;
    
    // 扫描整数部分
    while (!IsAtEnd() && IsDigit(CurrentChar())) {
        number += CurrentChar();
        NextChar();
    }
    
    // 检查小数点
    if (!IsAtEnd() && CurrentChar() == '.' && PeekChar() != '\0' && IsDigit(PeekChar())) {
        number += CurrentChar();  // 添加小数点
        NextChar();
        
        // 扫描小数部分
        while (!IsAtEnd() && IsDigit(CurrentChar())) {
            number += CurrentChar();
            NextChar();
        }
    }
    
    return CHTLToken(CHTLTokenType::NUMBER, number, startLocation);
}

CHTLToken CHTLLexer::ScanString(char quote) {
    SourceLocation startLocation = MakeLocation();
    String content;
    
    NextChar(); // 跳过开始引号
    
    while (!IsAtEnd() && CurrentChar() != quote) {
        if (CurrentChar() == '\\' && PeekChar() != '\0') {
            // 处理转义字符
            NextChar(); // 跳过反斜杠
            char escaped = CurrentChar();
            switch (escaped) {
                case 'n': content += '\n'; break;
                case 't': content += '\t'; break;
                case 'r': content += '\r'; break;
                case '\\': content += '\\'; break;
                case '"': content += '"'; break;
                case '\'': content += '\''; break;
                default: 
                    content += '\\';
                    content += escaped;
                    break;
            }
            NextChar();
        } else {
            content += CurrentChar();
            NextChar();
        }
    }
    
    if (IsAtEnd()) {
        return MakeErrorToken("未终止的字符串");
    }
    
    NextChar(); // 跳过结束引号
    
    return CHTLToken(CHTLTokenType::STRING_LITERAL, content, startLocation);
}

CHTLToken CHTLLexer::ScanUnquotedLiteral() {
    SourceLocation startLocation = MakeLocation();
    String literal;
    
    // 无修饰字面量可以包含字母、数字、连字符、下划线等
    while (!IsAtEnd() && IsValidUnquotedChar(CurrentChar())) {
        literal += CurrentChar();
        NextChar();
    }
    
    return CHTLToken(CHTLTokenType::UNQUOTED_LITERAL, literal, startLocation);
}

CHTLToken CHTLLexer::ScanComment() {
    SourceLocation startLocation = MakeLocation();
    String comment;
    
    char first = CurrentChar();
    char second = PeekChar();
    
    if (first == '/' && second == '/') {
        // 行注释
        while (!IsAtEnd() && CurrentChar() != '\n') {
            comment += CurrentChar();
            NextChar();
        }
        return CHTLToken(CHTLTokenType::LINE_COMMENT, comment, startLocation);
    }
    else if (first == '-' && second == '-') {
        // 生成器注释
        while (!IsAtEnd() && CurrentChar() != '\n') {
            comment += CurrentChar();
            NextChar();
        }
        return CHTLToken(CHTLTokenType::GENERATOR_COMMENT, comment, startLocation);
    }
    else if (first == '/' && second == '*') {
        // 块注释
        NextChar(); NextChar(); // 跳过 /*
        
        while (!IsAtEnd()) {
            if (CurrentChar() == '*' && PeekChar() == '/') {
                comment += "*/";
                NextChar(); NextChar(); // 跳过 */
                break;
            }
            comment += CurrentChar();
            NextChar();
        }
        
        return CHTLToken(CHTLTokenType::BLOCK_COMMENT, comment, startLocation);
    }
    
    return MakeErrorToken("无效的注释");
}

CHTLToken CHTLLexer::ScanCHTLJSSelector() {
    SourceLocation startLocation = MakeLocation();
    String selector;
    
    // 跳过开始的 {{
    NextChar(); NextChar();
    
    while (!IsAtEnd() && !(CurrentChar() == '}' && PeekChar() == '}')) {
        selector += CurrentChar();
        NextChar();
    }
    
    if (IsAtEnd() || !(CurrentChar() == '}' && PeekChar() == '}')) {
        return MakeErrorToken("未终止的CHTL JS选择器");
    }
    
    // 跳过结束的 }}
    NextChar(); NextChar();
    
    return CHTLToken(CHTLTokenType::CHTLJS_SELECTOR, selector, startLocation);
}

CHTLToken CHTLLexer::ScanOperator() {
    SourceLocation startLocation = MakeLocation();
    char ch = CurrentChar();
    
    switch (ch) {
        case '{': NextChar(); return CHTLToken(CHTLTokenType::LEFT_BRACE, "{", startLocation);
        case '}': NextChar(); return CHTLToken(CHTLTokenType::RIGHT_BRACE, "}", startLocation);
        case '[': NextChar(); return CHTLToken(CHTLTokenType::LEFT_BRACKET, "[", startLocation);
        case ']': NextChar(); return CHTLToken(CHTLTokenType::RIGHT_BRACKET, "]", startLocation);
        case '(': NextChar(); return CHTLToken(CHTLTokenType::LEFT_PAREN, "(", startLocation);
        case ')': NextChar(); return CHTLToken(CHTLTokenType::RIGHT_PAREN, ")", startLocation);
        case ';': NextChar(); return CHTLToken(CHTLTokenType::SEMICOLON, ";", startLocation);
        case ',': NextChar(); return CHTLToken(CHTLTokenType::COMMA, ",", startLocation);
        case '.': NextChar(); return CHTLToken(CHTLTokenType::DOT, ".", startLocation);
        case ':': NextChar(); return CHTLToken(CHTLTokenType::COLON, ":", startLocation);
        case '=': NextChar(); return CHTLToken(CHTLTokenType::EQUALS, "=", startLocation);
        case '@': NextChar(); return CHTLToken(CHTLTokenType::AT_SYMBOL, "@", startLocation);
        case '&': NextChar(); return CHTLToken(CHTLTokenType::AMPERSAND, "&", startLocation);
        case '\n': 
            NextChar(); 
            return CHTLToken(CHTLTokenType::NEWLINE, "\\n", startLocation);
        default:
            if (IsWhitespace(ch)) {
                String whitespace;
                while (!IsAtEnd() && IsWhitespace(CurrentChar()) && CurrentChar() != '\n') {
                    whitespace += CurrentChar();
                    NextChar();
                }
                return CHTLToken(CHTLTokenType::WHITESPACE, whitespace, startLocation);
            }
            
            // 尝试作为无修饰字面量处理
            if (IsValidUnquotedChar(ch)) {
                return ScanUnquotedLiteral();
            }
            
            NextChar();
            return MakeErrorToken("未知字符: " + String(1, ch));
    }
}

char CHTLLexer::CurrentChar() const {
    if (IsAtEnd()) return '\0';
    return source_[position_];
}

char CHTLLexer::NextChar() {
    if (IsAtEnd()) return '\0';
    
    char ch = source_[position_];
    AdvancePosition();
    return ch;
}

char CHTLLexer::PeekChar(size_t offset) const {
    size_t peekPos = position_ + offset;
    if (peekPos >= length_) return '\0';
    return source_[peekPos];
}

bool CHTLLexer::Match(char expected) {
    if (IsAtEnd() || CurrentChar() != expected) {
        return false;
    }
    NextChar();
    return true;
}

bool CHTLLexer::Match(const String& expected) {
    if (position_ + expected.length() > length_) {
        return false;
    }
    
    for (size_t i = 0; i < expected.length(); ++i) {
        if (source_[position_ + i] != expected[i]) {
            return false;
        }
    }
    
    position_ += expected.length();
    UpdateLocation();
    return true;
}

void CHTLLexer::SkipWhitespace() {
    while (!IsAtEnd() && IsWhitespace(CurrentChar()) && CurrentChar() != '\n') {
        NextChar();
    }
}

void CHTLLexer::SkipToEndOfLine() {
    while (!IsAtEnd() && CurrentChar() != '\n') {
        NextChar();
    }
}

void CHTLLexer::PushState(LexerState newState) {
    stateStack_.push(state_);
    state_ = newState;
}

void CHTLLexer::PopState() {
    if (!stateStack_.empty()) {
        state_ = stateStack_.top();
        stateStack_.pop();
    } else {
        state_ = LexerState::Normal;
    }
}

bool CHTLLexer::IsAtEnd(size_t position) const {
    return position >= length_;
}

bool CHTLLexer::IsAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLLexer::IsDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexer::IsAlphaNumeric(char c) const {
    return IsAlpha(c) || IsDigit(c);
}

bool CHTLLexer::IsWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

bool CHTLLexer::IsNewline(char c) const {
    return c == '\n';
}

bool CHTLLexer::IsHtmlElement(const String& identifier) const {
    return htmlElements_.find(identifier) != htmlElements_.end();
}

bool CHTLLexer::IsValidIdentifierStart(char c) const {
    return IsAlpha(c);
}

bool CHTLLexer::IsValidIdentifierChar(char c) const {
    return IsAlphaNumeric(c) || c == '-';
}

bool CHTLLexer::IsValidUnquotedChar(char c) const {
    return IsAlphaNumeric(c) || c == '-' || c == '_' || c == '.' || c == '/';
}

CHTLToken CHTLLexer::MakeErrorToken(const String& message) {
    return CHTLToken(CHTLTokenType::ERROR_TOKEN, message, MakeLocation());
}

void CHTLLexer::UpdateStatistics(CHTLTokenType type) {
    statistics_.totalTokens++;
    
    switch (type) {
        case CHTLTokenType::IDENTIFIER:
        case CHTLTokenType::HTML_ELEMENT:
            statistics_.identifiers++;
            break;
        case CHTLTokenType::STRING_LITERAL:
        case CHTLTokenType::UNQUOTED_LITERAL:
        case CHTLTokenType::NUMBER:
            statistics_.literals++;
            break;
        case CHTLTokenType::LINE_COMMENT:
        case CHTLTokenType::BLOCK_COMMENT:
        case CHTLTokenType::GENERATOR_COMMENT:
            statistics_.comments++;
            break;
        case CHTLTokenType::COLON:
        case CHTLTokenType::EQUALS:
        case CHTLTokenType::AT_SYMBOL:
        case CHTLTokenType::AMPERSAND:
            statistics_.operators++;
            break;
        case CHTLTokenType::ERROR_TOKEN:
            statistics_.errors++;
            break;
        default:
            if (CHTLToken(type, "", {}).IsKeyword()) {
                statistics_.keywords++;
            }
            break;
    }
}

void CHTLLexer::AdvancePosition() {
    if (IsAtEnd()) return;
    
    if (source_[position_] == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    position_++;
}

void CHTLLexer::UpdateLocation() {
    // 位置已经在AdvancePosition中更新
}

SourceLocation CHTLLexer::MakeLocation() const {
    return SourceLocation(filename_, line_, column_, position_);
}

CHTLToken CHTLLexer::MakeToken(CHTLTokenType type, const String& value) {
    return CHTLToken(type, value, MakeLocation());
}

CHTLToken CHTLLexer::MakeStringToken(const String& value, char quote) {
    String quotedValue = String(1, quote) + value + String(1, quote);
    return CHTLToken(CHTLTokenType::STRING_LITERAL, quotedValue, MakeLocation());
}

CHTLTokenType CHTLLexer::DetermineIdentifierType(const String& identifier) {
    // 首先检查是否为关键字
    CHTLTokenType keywordType = keywordMap_->GetKeywordType(identifier);
    if (keywordType != CHTLTokenType::IDENTIFIER) {
        return keywordType;
    }
    
    // 检查是否为HTML元素
    if (IsHtmlElement(identifier)) {
        return CHTLTokenType::HTML_ELEMENT;
    }
    
    return CHTLTokenType::IDENTIFIER;
}

void CHTLLexer::InitializeHtmlElements() {
    htmlElements_ = {
        "html", "head", "title", "meta", "link", "style", "script",
        "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "tbody", "thead", "tfoot",
        "form", "input", "button", "select", "option", "textarea", "label", "fieldset", "legend",
        "header", "footer", "nav", "section", "article", "aside", "main", "figure", "figcaption",
        "blockquote", "pre", "code", "strong", "em", "b", "i", "u", "br", "hr", "small", "mark",
        "del", "ins", "sub", "sup", "time", "progress", "meter", "details", "summary",
        "canvas", "svg", "video", "audio", "source", "track", "embed", "object", "param", "iframe"
    };
}

void CHTLLexer::LogDebugInfo(const String& message) const {
    if (debugMode_) {
        std::cout << "[CHTLLexer] " << message << std::endl;
    }
}

} // namespace CHTL