#include "compilers/chtl/chtl_lexer.h"
#include <cctype>
#include <sstream>

namespace chtl {

// 静态成员初始化
const std::unordered_map<std::string, CHTLTokenType> CHTLLexer::keywords_ = CHTLLexer::initKeywords();
const std::unordered_map<std::string, CHTLTokenType> CHTLLexer::atTypes_ = CHTLLexer::initAtTypes();
const std::unordered_set<std::string> CHTLLexer::htmlTags_ = CHTLLexer::initHTMLTags();

CHTLLexer::CHTLLexer(CHTLContext& context) 
    : context_(context), start_(0), current_(0), line_(1), column_(1),
      skipWhitespace_(true), skipComments_(true) {
}

CHTLLexer::~CHTLLexer() = default;

std::vector<CHTLToken> CHTLLexer::tokenize(const std::string& source) {
    source_ = source;
    reset();
    
    std::vector<CHTLToken> tokens;
    
    while (!isAtEnd()) {
        start_ = current_;
        CHTLToken token = scanToken();
        
        // 根据选项决定是否添加空白和注释Token
        if (token.type == CHTLTokenType::WHITESPACE && skipWhitespace_) {
            continue;
        }
        if ((token.type == CHTLTokenType::SINGLE_LINE_COMMENT ||
             token.type == CHTLTokenType::MULTI_LINE_COMMENT) && skipComments_) {
            continue;
        }
        
        tokens.push_back(token);
        
        if (token.type == CHTLTokenType::EOF_TOKEN) {
            break;
        }
    }
    
    return tokens;
}

CHTLToken CHTLLexer::scanToken() {
    if (skipWhitespace_) {
        skipWhitespace();
    }
    
    if (isAtEnd()) {
        return makeToken(CHTLTokenType::EOF_TOKEN);
    }
    
    start_ = current_;
    char c = advance();
    
    // 标识符和关键字
    if (isIdentifierStart(c)) {
        return identifier();
    }
    
    // 数字
    if (isDigit(c)) {
        return number();
    }
    
    // 根据字符扫描
    switch (c) {
        // 字符串
        case '"': return string('"');
        case '\'': return string('\'');
        
        // 操作符和分隔符
        case ':': return makeToken(CHTLTokenType::COLON);
        case '=': return makeToken(CHTLTokenType::EQUALS);
        case ';': return makeToken(CHTLTokenType::SEMICOLON);
        case ',': return makeToken(CHTLTokenType::COMMA);
        case '.': return makeToken(CHTLTokenType::DOT);
        case '&': return makeToken(CHTLTokenType::AMPERSAND);
        case '@': 
            // 检查是否是@类型标识
            if (isAlpha(peek())) {
                return atType();
            }
            return makeToken(CHTLTokenType::AT);
            
        // 括号
        case '{': return makeToken(CHTLTokenType::LEFT_BRACE);
        case '}': return makeToken(CHTLTokenType::RIGHT_BRACE);
        case '(': return makeToken(CHTLTokenType::LEFT_PAREN);
        case ')': return makeToken(CHTLTokenType::RIGHT_PAREN);
        case '[': return makeToken(CHTLTokenType::LEFT_BRACKET);
        case ']': return makeToken(CHTLTokenType::RIGHT_BRACKET);
        
        // 注释
        case '/':
            if (match('/')) {
                return singleLineComment();
            } else if (match('*')) {
                return multiLineComment();
            }
            // 如果不是注释，可能是除法（但CHTL中没有除法运算符）
            return errorToken("Unexpected character '/'");
            
        case '-':
            if (match('-')) {
                return generatorComment();
            }
            // CHTL中'-'可能是标识符的一部分
            if (isIdentifierPart(peek())) {
                current_--; // 退回
                return identifier();
            }
            return errorToken("Unexpected character '-'");
            
        // 空白字符
        case ' ':
        case '\r':
        case '\t':
        case '\n':
            current_--; // 退回
            skipWhitespace();
            if (!skipWhitespace_) {
                return makeToken(CHTLTokenType::WHITESPACE);
            }
            return scanToken(); // 递归扫描下一个
            
        default:
            // 检查是否应该作为无引号字面量
            if (shouldScanAsUnquotedLiteral()) {
                current_--; // 退回
                return unquotedLiteral();
            }
            return errorToken("Unexpected character");
    }
}

void CHTLLexer::reset() {
    start_ = 0;
    current_ = 0;
    line_ = 1;
    column_ = 1;
}

CHTLToken CHTLLexer::identifier() {
    while (isIdentifierPart(peek())) {
        advance();
    }
    
    std::string text = source_.substr(start_, current_ - start_);
    
    // 检查是否是关键字
    auto it = keywords_.find(text);
    if (it != keywords_.end()) {
        return makeToken(it->second);
    }
    
    // 检查是否是HTML标签（作为元素标识符）
    if (isHTMLTag(text)) {
        return makeToken(CHTLTokenType::IDENTIFIER);
    }
    
    return makeToken(CHTLTokenType::IDENTIFIER);
}

CHTLToken CHTLLexer::number() {
    while (isDigit(peek())) {
        advance();
    }
    
    // 检查小数点
    if (peek() == '.' && isDigit(peek(1))) {
        advance(); // 消费 '.'
        while (isDigit(peek())) {
            advance();
        }
    }
    
    std::string numStr = source_.substr(start_, current_ - start_);
    double value = std::stod(numStr);
    
    return makeToken(CHTLTokenType::NUMBER, value);
}

CHTLToken CHTLLexer::string(char quote) {
    std::string value;
    
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\n') {
            line_++;
            column_ = 0;
        }
        
        // 处理转义字符
        if (peek() == '\\') {
            advance();
            if (!isAtEnd()) {
                char escaped = advance();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: value += escaped; break;
                }
            }
        } else {
            value += advance();
        }
    }
    
    if (isAtEnd()) {
        return errorToken("Unterminated string");
    }
    
    advance(); // 消费结束引号
    
    CHTLTokenType type = (quote == '"') ? 
        CHTLTokenType::STRING_LITERAL : 
        CHTLTokenType::SINGLE_STRING_LITERAL;
    
    return makeToken(type, value);
}

CHTLToken CHTLLexer::unquotedLiteral() {
    // 无引号字面量：读取到分号、逗号、花括号或换行
    while (!isAtEnd()) {
        char c = peek();
        if (c == ';' || c == ',' || c == '{' || c == '}' || 
            c == '\n' || c == ':' || c == '=') {
            break;
        }
        advance();
    }
    
    std::string value = source_.substr(start_, current_ - start_);
    
    // 去除尾部空白
    size_t end = value.find_last_not_of(" \t\r");
    if (end != std::string::npos) {
        value = value.substr(0, end + 1);
    }
    
    return makeToken(CHTLTokenType::UNQUOTED_LITERAL, value);
}

CHTLToken CHTLLexer::singleLineComment() {
    // 已经消费了 //
    std::string comment;
    while (peek() != '\n' && !isAtEnd()) {
        comment += advance();
    }
    
    return makeToken(CHTLTokenType::SINGLE_LINE_COMMENT, comment);
}

CHTLToken CHTLLexer::multiLineComment() {
    // 已经消费了 /*
    std::string comment;
    
    while (!isAtEnd()) {
        if (peek() == '*' && peek(1) == '/') {
            advance(); // *
            advance(); // /
            break;
        }
        
        if (peek() == '\n') {
            line_++;
            column_ = 0;
        }
        
        comment += advance();
    }
    
    return makeToken(CHTLTokenType::MULTI_LINE_COMMENT, comment);
}

CHTLToken CHTLLexer::generatorComment() {
    // 已经消费了 --
    std::string comment;
    while (peek() != '\n' && !isAtEnd()) {
        comment += advance();
    }
    
    return makeToken(CHTLTokenType::GENERATOR_COMMENT, comment);
}

CHTLToken CHTLLexer::atType() {
    // 已经在 @ 后面
    while (isAlpha(peek()) || peek() == '_') {
        advance();
    }
    
    std::string typeName = source_.substr(start_ + 1, current_ - start_ - 1); // 不包括@
    
    auto it = atTypes_.find(typeName);
    if (it != atTypes_.end()) {
        return makeToken(it->second);
    }
    
    // 未知的@类型，可能是自定义类型
    return makeToken(CHTLTokenType::IDENTIFIER);
}

bool CHTLLexer::isAtEnd() const {
    return current_ >= source_.length();
}

char CHTLLexer::peek(int offset) const {
    size_t pos = current_ + offset;
    if (pos >= source_.length()) {
        return '\0';
    }
    return source_[pos];
}

char CHTLLexer::advance() {
    char ch = source_[current_++];
    updatePosition(ch);
    return ch;
}

bool CHTLLexer::match(char expected) {
    if (isAtEnd()) return false;
    if (source_[current_] != expected) return false;
    
    advance();
    return true;
}

bool CHTLLexer::matchString(const std::string& expected) {
    for (size_t i = 0; i < expected.length(); ++i) {
        if (peek(i) != expected[i]) {
            return false;
        }
    }
    
    for (size_t i = 0; i < expected.length(); ++i) {
        advance();
    }
    
    return true;
}

void CHTLLexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (isWhitespace(c)) {
            advance();
        } else {
            break;
        }
    }
}

bool CHTLLexer::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool CHTLLexer::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

bool CHTLLexer::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

bool CHTLLexer::isIdentifierStart(char c) const {
    return isAlpha(c) || c == '_';
}

bool CHTLLexer::isIdentifierPart(char c) const {
    return isAlphaNumeric(c) || c == '_' || c == '-';
}

bool CHTLLexer::isWhitespace(char c) const {
    return c == ' ' || c == '\r' || c == '\t' || c == '\n';
}

CHTLToken CHTLLexer::makeToken(CHTLTokenType type) {
    std::string lexeme = source_.substr(start_, current_ - start_);
    return CHTLToken(type, lexeme, line_, column_ - (current_ - start_), start_, current_);
}

CHTLToken CHTLLexer::makeToken(CHTLTokenType type, const std::string& value) {
    std::string lexeme = source_.substr(start_, current_ - start_);
    return CHTLToken(type, lexeme, value, line_, column_ - (current_ - start_), start_, current_);
}

template<typename T>
CHTLToken CHTLLexer::makeToken(CHTLTokenType type, T value) {
    std::string lexeme = source_.substr(start_, current_ - start_);
    return CHTLToken(type, lexeme, value, line_, column_ - (current_ - start_), start_, current_);
}

CHTLToken CHTLLexer::errorToken(const std::string& message) {
    context_.addError(message, line_, column_);
    return makeToken(CHTLTokenType::ERROR, message);
}

void CHTLLexer::updatePosition(char ch) {
    if (ch == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
}

bool CHTLLexer::isHTMLTag(const std::string& word) const {
    return htmlTags_.find(word) != htmlTags_.end();
}

bool CHTLLexer::isUnquotedLiteralContext() const {
    // 检查是否在属性值上下文中
    auto state = context_.getStateManager().getCurrentState();
    return state == CHTLCompilerState::IN_ATTRIBUTE_VALUE ||
           state == CHTLCompilerState::IN_CSS_DECLARATION;
}

bool CHTLLexer::shouldScanAsUnquotedLiteral() const {
    // 在某些上下文中，非标识符字符应该作为无引号字面量
    return isUnquotedLiteralContext() && !isWhitespace(peek());
}

std::unordered_map<std::string, CHTLTokenType> CHTLLexer::initKeywords() {
    return {
        // 基本关键字
        {"text", CHTLTokenType::KEYWORD_TEXT},
        {"style", CHTLTokenType::KEYWORD_STYLE},
        {"script", CHTLTokenType::KEYWORD_SCRIPT},
        
        // 方括号内关键字
        {"Template", CHTLTokenType::KEYWORD_TEMPLATE},
        {"Custom", CHTLTokenType::KEYWORD_CUSTOM},
        {"Origin", CHTLTokenType::KEYWORD_ORIGIN},
        {"Import", CHTLTokenType::KEYWORD_IMPORT},
        {"Namespace", CHTLTokenType::KEYWORD_NAMESPACE},
        {"Configuration", CHTLTokenType::KEYWORD_CONFIGURATION},
        {"Info", CHTLTokenType::KEYWORD_INFO},
        {"Export", CHTLTokenType::KEYWORD_EXPORT},
        
        // 操作关键字
        {"inherit", CHTLTokenType::KEYWORD_INHERIT},
        {"delete", CHTLTokenType::KEYWORD_DELETE},
        {"insert", CHTLTokenType::KEYWORD_INSERT},
        {"except", CHTLTokenType::KEYWORD_EXCEPT},
        {"from", CHTLTokenType::KEYWORD_FROM},
        {"as", CHTLTokenType::KEYWORD_AS},
        
        // 插入位置关键字
        {"after", CHTLTokenType::KEYWORD_AFTER},
        {"before", CHTLTokenType::KEYWORD_BEFORE},
        {"replace", CHTLTokenType::KEYWORD_REPLACE},
        {"at", CHTLTokenType::KEYWORD_AT},
        {"top", CHTLTokenType::KEYWORD_TOP},
        {"bottom", CHTLTokenType::KEYWORD_BOTTOM}
    };
}

std::unordered_map<std::string, CHTLTokenType> CHTLLexer::initAtTypes() {
    return {
        {"Style", CHTLTokenType::AT_STYLE},
        {"Element", CHTLTokenType::AT_ELEMENT},
        {"Var", CHTLTokenType::AT_VAR},
        {"Html", CHTLTokenType::AT_HTML},
        {"JavaScript", CHTLTokenType::AT_JAVASCRIPT},
        {"Chtl", CHTLTokenType::AT_CHTL},
        {"CJmod", CHTLTokenType::AT_CJMOD}
    };
}

std::unordered_set<std::string> CHTLLexer::initHTMLTags() {
    return {
        // 常用HTML标签
        "html", "head", "body", "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th", "thead", "tbody", "tfoot",
        "form", "input", "button", "select", "option", "textarea", "label",
        "header", "footer", "nav", "main", "section", "article", "aside",
        "strong", "em", "b", "i", "u", "code", "pre", "blockquote",
        "br", "hr", "meta", "link", "title", "style", "script"
    };
}

// 显式实例化模板函数
template CHTLToken CHTLLexer::makeToken<double>(CHTLTokenType type, double value);
template CHTLToken CHTLLexer::makeToken<int>(CHTLTokenType type, int value);

} // namespace chtl