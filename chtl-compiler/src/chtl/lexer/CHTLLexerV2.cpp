#include "CHTLLexerV2.h"
#include <sstream>
#include <cctype>
#include <unordered_map>

namespace chtl {

// 关键字映射表
static std::unordered_map<std::string, CHTLTokenType> s_Keywords = {
    {"text", CHTLTokenType::TEXT},
    {"style", CHTLTokenType::STYLE},
    {"script", CHTLTokenType::SCRIPT},
    {"except", CHTLTokenType::EXCEPT},
    {"delete", CHTLTokenType::DELETE},
    {"insert", CHTLTokenType::INSERT},
    {"from", CHTLTokenType::FROM}
};

// 方括号关键字映射表
static std::unordered_map<std::string, CHTLTokenType> s_BracketKeywords = {
    {"Template", CHTLTokenType::TEMPLATE},
    {"Custom", CHTLTokenType::CUSTOM},
    {"Import", CHTLTokenType::IMPORT},
    {"Namespace", CHTLTokenType::NAMESPACE},
    {"Configuration", CHTLTokenType::CONFIGURATION},
    {"Origin", CHTLTokenType::ORIGIN}
};

// @ 关键字映射表
static std::unordered_map<std::string, CHTLTokenType> s_AtKeywords = {
    {"Style", CHTLTokenType::AT_STYLE},
    {"Element", CHTLTokenType::AT_ELEMENT},
    {"Var", CHTLTokenType::AT_VAR},
    {"Html", CHTLTokenType::AT_HTML},
    {"JavaScript", CHTLTokenType::AT_JAVASCRIPT},
    {"Chtl", CHTLTokenType::AT_CHTL},
    {"CJmod", CHTLTokenType::AT_CJMOD}
};

CHTLLexerV2::CHTLLexerV2() : m_Current(0), m_Line(1), m_Column(1), 
                           m_InOriginBlock(false), m_OriginBraceDepth(0) {
}

void CHTLLexerV2::SetInput(const std::string& input, const std::string& filename) {
    m_Input = input;
    m_Filename = filename;
    m_Current = 0;
    m_Line = 1;
    m_Column = 1;
    m_Errors.clear();
}

std::vector<CHTLToken> CHTLLexerV2::Tokenize() {
    std::vector<CHTLToken> tokens;
    
    while (!IsAtEnd()) {
        SkipWhitespace();
        if (IsAtEnd()) break;
        
        CHTLToken token = NextToken();
        // 注释根据类型决定是否保留
        if (token.type == CHTLTokenType::COMMENT_HTML || 
            token.type != CHTLTokenType::COMMENT_SINGLE && 
            token.type != CHTLTokenType::COMMENT_MULTI) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(CHTLToken(CHTLTokenType::EOF_TOKEN, "", m_Line, m_Column));
    return tokens;
}

CHTLToken CHTLLexerV2::NextToken() {
    size_t startPos = m_Current;
    size_t startLine = m_Line;
    size_t startColumn = m_Column;
    
    char c = CurrentChar();
    
    // 注释
    if (c == '/') {
        char next = NextChar();
        if (next == '/') {
            return ScanComment();
        } else if (next == '*') {
            return ScanComment();
        }
    }
    
    if (c == '-' && NextChar() == '-') {
        return ScanComment();
    }
    
    // 字符串
    if (c == '"' || c == '\'') {
        return ScanString(c);
    }
    
    // 数字
    if (IsDigit(c)) {
        return ScanNumber();
    }
    
    // 方括号关键字
    if (c == '[') {
        char next = NextChar();
        if (IsAlpha(next) && std::isupper(next)) {
            return ScanBracketKeyword();
        }
    }
    
    // @ 关键字
    if (c == '@') {
        return ScanAtKeyword();
    }
    
    // 标识符或关键字
    if (IsIdentifierStart(c)) {
        return ScanIdentifierOrKeyword();
    }
    
    // 单字符符号
    switch (c) {
        case '{':
            Advance();
            return MakeToken(CHTLTokenType::LBRACE, startPos);
        case '}':
            Advance();
            return MakeToken(CHTLTokenType::RBRACE, startPos);
        case '(':
            Advance();
            return MakeToken(CHTLTokenType::LPAREN, startPos);
        case ')':
            Advance();
            return MakeToken(CHTLTokenType::RPAREN, startPos);
        case '[':
            Advance();
            return MakeToken(CHTLTokenType::LBRACKET, startPos);
        case ']':
            Advance();
            return MakeToken(CHTLTokenType::RBRACKET, startPos);
        case ';':
            Advance();
            return MakeToken(CHTLTokenType::SEMICOLON, startPos);
        case ':':
            Advance();
            return MakeToken(CHTLTokenType::COLON, startPos);
        case '=':
            Advance();
            return MakeToken(CHTLTokenType::EQUALS, startPos);
        case '.':
            Advance();
            return MakeToken(CHTLTokenType::DOT, startPos);
        case ',':
            Advance();
            return MakeToken(CHTLTokenType::COMMA, startPos);
        case '/':
            Advance();
            return MakeToken(CHTLTokenType::SLASH, startPos);
        case '$':
            Advance();
            return MakeToken(CHTLTokenType::DOLLAR, startPos);
        case '#':
            Advance();
            return MakeToken(CHTLTokenType::HASH, startPos);
        case '&':
            Advance();
            return MakeToken(CHTLTokenType::AMPERSAND, startPos);
        case '-':
            if (NextChar() == '>') {
                Advance();
                Advance();
                return MakeToken(CHTLTokenType::ARROW, "->", startPos);
            }
            break;
        case '<':
        case '>':
            // 在Origin块中这些字符是允许的
            Advance();
            return MakeToken(CHTLTokenType::LITERAL_UNQUOTED, std::string(1, c), startPos);
    }
    
    // 无引号字面量（在特定上下文中）
    if (IsUnquotedLiteralChar(c)) {
        return ScanUnquotedLiteral();
    }
    
    // 错误
    ReportError("Unexpected character: " + std::string(1, c));
    Advance();
    return MakeToken(CHTLTokenType::ERROR, std::string(1, c), startPos);
}

// 字符处理方法实现

char CHTLLexerV2::CurrentChar() const {
    if (IsAtEnd()) return '\0';
    return m_Input[m_Current];
}

char CHTLLexerV2::NextChar() const {
    if (m_Current + 1 >= m_Input.length()) return '\0';
    return m_Input[m_Current + 1];
}

char CHTLLexerV2::PeekChar(size_t offset) const {
    if (m_Current + offset >= m_Input.length()) return '\0';
    return m_Input[m_Current + offset];
}

void CHTLLexerV2::Advance() {
    if (IsAtEnd()) return;
    
    if (m_Input[m_Current] == '\n') {
        m_Line++;
        m_Column = 1;
    } else {
        m_Column++;
    }
    m_Current++;
}

void CHTLLexerV2::AdvanceN(size_t n) {
    for (size_t i = 0; i < n; i++) {
        Advance();
    }
}

bool CHTLLexerV2::IsAtEnd() const {
    return m_Current >= m_Input.length();
}

// 字符判断方法实现

bool CHTLLexerV2::IsAlpha(char c) const {
    return std::isalpha(c);
}

bool CHTLLexerV2::IsDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLLexerV2::IsAlphaNumeric(char c) const {
    return std::isalnum(c);
}

bool CHTLLexerV2::IsWhitespace(char c) const {
    return std::isspace(c);
}

bool CHTLLexerV2::IsIdentifierStart(char c) const {
    return IsAlpha(c) || c == '_';
}

bool CHTLLexerV2::IsIdentifierPart(char c) const {
    return IsAlphaNumeric(c) || c == '_' || c == '-';
}

bool CHTLLexerV2::IsUnquotedLiteralChar(char c) const {
    // 无引号字面量允许的字符
    return IsAlphaNumeric(c) || c == '_' || c == '-' || c == '+' || 
           c == '*' || c == '%' || c == '!' || c == '?' || c == '@' ||
           c == '#' || c == '&' || c == '~' || c == '^' || c == '|' ||
           (unsigned char)c >= 128; // 支持UTF-8字符
}

// Token 识别方法实现

void CHTLLexerV2::SkipWhitespace() {
    while (!IsAtEnd() && IsWhitespace(CurrentChar())) {
        Advance();
    }
}

CHTLToken CHTLLexerV2::ScanComment() {
    size_t startPos = m_Current;
    
    if (CurrentChar() == '/' && NextChar() == '/') {
        // 单行注释
        AdvanceN(2);
        while (!IsAtEnd() && CurrentChar() != '\n') {
            Advance();
        }
        return MakeToken(CHTLTokenType::COMMENT_SINGLE, startPos);
    } else if (CurrentChar() == '/' && NextChar() == '*') {
        // 多行注释
        AdvanceN(2);
        while (!IsAtEnd()) {
            if (CurrentChar() == '*' && NextChar() == '/') {
                AdvanceN(2);
                break;
            }
            Advance();
        }
        return MakeToken(CHTLTokenType::COMMENT_MULTI, startPos);
    } else if (CurrentChar() == '-' && NextChar() == '-') {
        // HTML 注释
        AdvanceN(2);
        while (!IsAtEnd() && CurrentChar() != '\n') {
            Advance();
        }
        return MakeToken(CHTLTokenType::COMMENT_HTML, startPos);
    }
    
    return MakeToken(CHTLTokenType::ERROR, startPos);
}

CHTLToken CHTLLexerV2::ScanString(char quote) {
    size_t startPos = m_Current;
    Advance(); // 跳过开始引号
    
    std::string value;
    while (!IsAtEnd() && CurrentChar() != quote) {
        if (CurrentChar() == '\\') {
            Advance();
            if (!IsAtEnd()) {
                // 处理转义字符
                char escaped = CurrentChar();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '\'': value += '\''; break;
                    case '"': value += '"'; break;
                    default: value += escaped; break;
                }
                Advance();
            }
        } else {
            value += CurrentChar();
            Advance();
        }
    }
    
    if (!IsAtEnd()) {
        Advance(); // 跳过结束引号
    } else {
        ReportError("Unterminated string");
    }
    
    CHTLTokenType type = (quote == '"') ? CHTLTokenType::STRING_DOUBLE : CHTLTokenType::STRING_SINGLE;
    return MakeToken(type, value, startPos);
}

CHTLToken CHTLLexerV2::ScanNumber() {
    size_t startPos = m_Current;
    
    while (!IsAtEnd() && IsDigit(CurrentChar())) {
        Advance();
    }
    
    // 小数部分
    if (CurrentChar() == '.' && IsDigit(NextChar())) {
        Advance(); // 跳过 .
        while (!IsAtEnd() && IsDigit(CurrentChar())) {
            Advance();
        }
    }
    
    return MakeToken(CHTLTokenType::NUMBER, startPos);
}

CHTLToken CHTLLexerV2::ScanIdentifierOrKeyword() {
    size_t startPos = m_Current;
    size_t startLine = m_Line;
    size_t startColumn = m_Column;
    
    while (!IsAtEnd() && IsIdentifierPart(CurrentChar())) {
        Advance();
    }
    
    std::string value = m_Input.substr(startPos, m_Current - startPos);
    
    // 检查是否是关键字
    auto it = s_Keywords.find(value);
    if (it != s_Keywords.end()) {
        return CHTLToken(it->second, value, startLine, startColumn, value.length());
    }
    
    return CHTLToken(CHTLTokenType::IDENTIFIER, value, startLine, startColumn, value.length());
}

CHTLToken CHTLLexerV2::ScanUnquotedLiteral() {
    size_t startPos = m_Current;
    
    while (!IsAtEnd() && IsUnquotedLiteralChar(CurrentChar())) {
        Advance();
    }
    
    return MakeToken(CHTLTokenType::LITERAL_UNQUOTED, startPos);
}

CHTLToken CHTLLexerV2::ScanBracketKeyword() {
    size_t startPos = m_Current;
    Advance(); // 跳过 [
    
    std::string keyword;
    while (!IsAtEnd() && IsAlpha(CurrentChar())) {
        keyword += CurrentChar();
        Advance();
    }
    
    if (CurrentChar() == ']') {
        Advance();
        
        auto it = s_BracketKeywords.find(keyword);
        if (it != s_BracketKeywords.end()) {
            return CHTLToken(it->second, "[" + keyword + "]", 
                           m_Line, m_Column - keyword.length() - 2, keyword.length() + 2);
        }
    }
    
    ReportError("Invalid bracket keyword: [" + keyword);
    return MakeToken(CHTLTokenType::ERROR, "[" + keyword, startPos);
}

CHTLToken CHTLLexerV2::ScanAtKeyword() {
    size_t startPos = m_Current;
    Advance(); // 跳过 @
    
    std::string keyword;
    while (!IsAtEnd() && IsAlpha(CurrentChar())) {
        keyword += CurrentChar();
        Advance();
    }
    
    auto it = s_AtKeywords.find(keyword);
    if (it != s_AtKeywords.end()) {
        return CHTLToken(it->second, "@" + keyword, 
                       m_Line, m_Column - keyword.length() - 1, keyword.length() + 1);
    }
    
    ReportError("Invalid @ keyword: @" + keyword);
    return MakeToken(CHTLTokenType::ERROR, "@" + keyword, startPos);
}

// 辅助方法实现

void CHTLLexerV2::ReportError(const std::string& message) {
    std::stringstream error;
    error << m_Filename << ":" << m_Line << ":" << m_Column << ": " << message;
    m_Errors.push_back(error.str());
}

CHTLToken CHTLLexerV2::MakeToken(CHTLTokenType type, size_t startPos) {
    std::string value = m_Input.substr(startPos, m_Current - startPos);
    return CHTLToken(type, value, m_Line, m_Column - value.length(), value.length());
}

CHTLToken CHTLLexerV2::MakeToken(CHTLTokenType type, const std::string& value, size_t startPos) {
    return CHTLToken(type, value, m_Line, m_Column - (m_Current - startPos), m_Current - startPos);
}

} // namespace chtl