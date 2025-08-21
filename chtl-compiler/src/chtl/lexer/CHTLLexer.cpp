#include "chtl/lexer/CHTLLexer.h"
#include "utils/Logger.h"
#include <algorithm>
#include <cctype>

namespace chtl {
namespace compiler {

// 静态成员定义
const std::unordered_map<std::string, CHTLTokenType> CHTLLexer::s_Keywords = {
    {"text", CHTLTokenType::KEYWORD_TEXT},
    {"style", CHTLTokenType::KEYWORD_STYLE},
    {"script", CHTLTokenType::KEYWORD_SCRIPT},
    {"inherit", CHTLTokenType::KEYWORD_INHERIT},
    {"delete", CHTLTokenType::KEYWORD_DELETE},
    {"insert", CHTLTokenType::KEYWORD_INSERT},
    {"after", CHTLTokenType::KEYWORD_AFTER},
    {"before", CHTLTokenType::KEYWORD_BEFORE},
    {"replace", CHTLTokenType::KEYWORD_REPLACE},
    {"at", CHTLTokenType::KEYWORD_AT_TOP},  // 特殊处理 "at top" 和 "at bottom"
    {"from", CHTLTokenType::KEYWORD_FROM},
    {"as", CHTLTokenType::KEYWORD_AS},
    {"except", CHTLTokenType::KEYWORD_EXCEPT},
    {"vir", CHTLTokenType::KEYWORD_VIR}
};

const std::unordered_map<std::string, CHTLTokenType> CHTLLexer::s_AtKeywords = {
    {"@Style", CHTLTokenType::AT_STYLE},
    {"@Element", CHTLTokenType::AT_ELEMENT},
    {"@Var", CHTLTokenType::AT_VAR},
    {"@Html", CHTLTokenType::AT_HTML},
    {"@JavaScript", CHTLTokenType::AT_JAVASCRIPT},
    {"@Chtl", CHTLTokenType::AT_CHTL},
    {"@CJmod", CHTLTokenType::AT_CJMOD},
    {"@Config", CHTLTokenType::AT_CONFIG},
    {"@Vue", CHTLTokenType::AT_VUE}
};

const std::unordered_map<std::string, CHTLTokenType> CHTLLexer::s_BracketKeywords = {
    {"[Template]", CHTLTokenType::KEYWORD_TEMPLATE},
    {"[Custom]", CHTLTokenType::KEYWORD_CUSTOM},
    {"[Configuration]", CHTLTokenType::KEYWORD_CONFIGURATION},
    {"[Namespace]", CHTLTokenType::KEYWORD_NAMESPACE},
    {"[Import]", CHTLTokenType::KEYWORD_IMPORT},
    {"[Origin]", CHTLTokenType::KEYWORD_ORIGIN},
    {"[Info]", CHTLTokenType::KEYWORD_INFO},
    {"[Export]", CHTLTokenType::KEYWORD_EXPORT}
};

bool CHTLLexer::s_Initialized = false;

CHTLLexer::CHTLLexer()
    : m_Position(0)
    , m_Line(1)
    , m_Column(1) {
    if (!s_Initialized) {
        InitializeKeywords();
        s_Initialized = true;
    }
}

CHTLLexer::~CHTLLexer() = default;

void CHTLLexer::Initialize(const std::string& source, const std::string& filename) {
    m_Source = source;
    m_Filename = filename;
    m_Position = 0;
    m_Line = 1;
    m_Column = 1;
    m_TokenCache.clear();
    m_Errors.clear();
}

CHTLToken CHTLLexer::NextToken() {
    // 如果有缓存的Token，先返回缓存的
    if (!m_TokenCache.empty()) {
        CHTLToken token = m_TokenCache.front();
        m_TokenCache.erase(m_TokenCache.begin());
        return token;
    }
    
    return ScanToken();
}

CHTLToken CHTLLexer::PeekToken(size_t lookahead) {
    // 确保缓存中有足够的Token
    while (m_TokenCache.size() < lookahead && !IsAtEnd()) {
        m_TokenCache.push_back(ScanToken());
    }
    
    if (lookahead <= m_TokenCache.size()) {
        return m_TokenCache[lookahead - 1];
    }
    
    return MakeToken(CHTLTokenType::END_OF_FILE);
}

std::vector<CHTLToken> CHTLLexer::GetAllTokens() {
    std::vector<CHTLToken> tokens;
    
    while (!IsAtEnd()) {
        CHTLToken token = NextToken();
        tokens.push_back(token);
        
        if (token.GetType() == CHTLTokenType::END_OF_FILE) {
            break;
        }
    }
    
    return tokens;
}

void CHTLLexer::Reset() {
    m_Position = 0;
    m_Line = 1;
    m_Column = 1;
    m_TokenCache.clear();
    m_Errors.clear();
}

CHTLToken CHTLLexer::ScanToken() {
    SkipWhitespace();
    
    if (IsAtEnd()) {
        return MakeToken(CHTLTokenType::END_OF_FILE);
    }
    
    size_t startPos = m_Position;
    size_t startLine = m_Line;
    size_t startColumn = m_Column;
    
    char ch = CurrentChar();
    
    // 处理注释
    if (ch == '/') {
        if (PeekChar() == '/') {
            return ScanComment();
        } else if (PeekChar() == '*') {
            return ScanComment();
        }
    }
    
    // 处理生成器注释
    if (ch == '-' && PeekChar() == '-') {
        return ScanGeneratorComment();
    }
    
    // 处理字符串字面量
    if (ch == '"' || ch == '\'') {
        return ScanString(ch);
    }
    
    // 处理数字
    if (IsDigit(ch)) {
        return ScanNumber();
    }
    
    // 处理标识符和关键字
    if (IsIdentifierStart(ch)) {
        return ScanIdentifierOrKeyword();
    }
    
    // 处理方括号关键字
    if (ch == '[') {
        if (IsAlpha(PeekChar())) {
            return ScanBracketKeyword();
        }
    }
    
    // 处理@关键字
    if (ch == '@') {
        if (IsAlpha(PeekChar())) {
            return ScanAtKeyword();
        }
    }
    
    // 处理增强选择器（CHTL JS）
    if (ch == '{' && PeekChar() == '{') {
        Advance(); // 跳过第一个 {
        Advance(); // 跳过第二个 {
        return MakeToken(CHTLTokenType::ENHANCED_SELECTOR_START, "{{");
    }
    
    if (ch == '}' && PeekChar() == '}') {
        Advance(); // 跳过第一个 }
        Advance(); // 跳过第二个 }
        return MakeToken(CHTLTokenType::ENHANCED_SELECTOR_END, "}}");
    }
    
    // 处理箭头操作符（CHTL JS）
    if (ch == '-' && PeekChar() == '>') {
        Advance(); // 跳过 -
        Advance(); // 跳过 >
        return MakeToken(CHTLTokenType::ARROW, "->");
    }
    
    // 处理单字符Token
    Advance();
    switch (ch) {
        case ':': return MakeToken(CHTLTokenType::COLON, ":");
        case '=': return MakeToken(CHTLTokenType::EQUALS, "=");
        case ';': return MakeToken(CHTLTokenType::SEMICOLON, ";");
        case '.': return MakeToken(CHTLTokenType::DOT, ".");
        case ',': return MakeToken(CHTLTokenType::COMMA, ",");
        case '&': return MakeToken(CHTLTokenType::AMPERSAND, "&");
        case '/': return MakeToken(CHTLTokenType::SLASH, "/");
        case '*': return MakeToken(CHTLTokenType::ASTERISK, "*");
        case '{': return MakeToken(CHTLTokenType::LEFT_BRACE, "{");
        case '}': return MakeToken(CHTLTokenType::RIGHT_BRACE, "}");
        case '[': return MakeToken(CHTLTokenType::LEFT_BRACKET, "[");
        case ']': return MakeToken(CHTLTokenType::RIGHT_BRACKET, "]");
        case '(': return MakeToken(CHTLTokenType::LEFT_PAREN, "(");
        case ')': return MakeToken(CHTLTokenType::RIGHT_PAREN, ")");
        default:
            return MakeErrorToken("意外的字符: " + std::string(1, ch));
    }
}

CHTLToken CHTLLexer::ScanIdentifierOrKeyword() {
    size_t start = m_Position;
    
    while (!IsAtEnd() && IsIdentifierChar(CurrentChar())) {
        Advance();
    }
    
    std::string identifier = m_Source.substr(start, m_Position - start);
    
    // 检查是否是关键字
    auto it = s_Keywords.find(identifier);
    if (it != s_Keywords.end()) {
        // 特殊处理 "at top" 和 "at bottom"
        if (identifier == "at") {
            size_t savedPos = m_Position;
            size_t savedLine = m_Line;
            size_t savedColumn = m_Column;
            
            SkipWhitespace();
            size_t nextStart = m_Position;
            
            while (!IsAtEnd() && IsAlpha(CurrentChar())) {
                Advance();
            }
            
            std::string nextWord = m_Source.substr(nextStart, m_Position - nextStart);
            
            if (nextWord == "top") {
                return MakeToken(CHTLTokenType::KEYWORD_AT_TOP, "at top");
            } else if (nextWord == "bottom") {
                return MakeToken(CHTLTokenType::KEYWORD_AT_BOTTOM, "at bottom");
            } else {
                // 不是 "at top" 或 "at bottom"，恢复位置
                m_Position = savedPos;
                m_Line = savedLine;
                m_Column = savedColumn;
            }
        }
        
        return MakeToken(it->second, identifier);
    }
    
    // 不是关键字，返回标识符
    return MakeToken(CHTLTokenType::IDENTIFIER, identifier);
}

CHTLToken CHTLLexer::ScanString(char delimiter) {
    size_t start = m_Position;
    Advance(); // 跳过开始的引号
    
    std::string value;
    bool escaped = false;
    
    while (!IsAtEnd() && (CurrentChar() != delimiter || escaped)) {
        if (escaped) {
            // 处理转义字符
            switch (CurrentChar()) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: value += CurrentChar(); break;
            }
            escaped = false;
        } else {
            if (CurrentChar() == '\\') {
                escaped = true;
            } else {
                value += CurrentChar();
            }
        }
        
        Advance();
    }
    
    if (IsAtEnd()) {
        return MakeErrorToken("未闭合的字符串");
    }
    
    Advance(); // 跳过结束的引号
    
    std::string lexeme = m_Source.substr(start, m_Position - start);
    return MakeToken(CHTLTokenType::STRING_LITERAL, lexeme, value);
}

CHTLToken CHTLLexer::ScanNumber() {
    size_t start = m_Position;
    
    while (!IsAtEnd() && IsDigit(CurrentChar())) {
        Advance();
    }
    
    // 处理小数部分
    if (!IsAtEnd() && CurrentChar() == '.' && IsDigit(PeekChar())) {
        Advance(); // 跳过小数点
        
        while (!IsAtEnd() && IsDigit(CurrentChar())) {
            Advance();
        }
    }
    
    std::string numberStr = m_Source.substr(start, m_Position - start);
    double value = std::stod(numberStr);
    
    return MakeToken(CHTLTokenType::NUMBER_LITERAL, numberStr, value);
}

CHTLToken CHTLLexer::ScanComment() {
    size_t start = m_Position;
    
    if (CurrentChar() == '/' && PeekChar() == '/') {
        // 单行注释
        Advance(); // 跳过第一个 /
        Advance(); // 跳过第二个 /
        
        while (!IsAtEnd() && CurrentChar() != '\n') {
            Advance();
        }
        
        std::string comment = m_Source.substr(start, m_Position - start);
        return MakeToken(CHTLTokenType::COMMENT_SINGLE, comment);
    } else if (CurrentChar() == '/' && PeekChar() == '*') {
        // 多行注释
        Advance(); // 跳过 /
        Advance(); // 跳过 *
        
        while (!IsAtEnd()) {
            if (CurrentChar() == '*' && PeekChar() == '/') {
                Advance(); // 跳过 *
                Advance(); // 跳过 /
                break;
            }
            Advance();
        }
        
        std::string comment = m_Source.substr(start, m_Position - start);
        return MakeToken(CHTLTokenType::COMMENT_MULTI, comment);
    }
    
    return MakeErrorToken("无效的注释");
}

CHTLToken CHTLLexer::ScanGeneratorComment() {
    size_t start = m_Position;
    Advance(); // 跳过第一个 -
    Advance(); // 跳过第二个 -
    
    while (!IsAtEnd() && CurrentChar() != '\n') {
        Advance();
    }
    
    std::string comment = m_Source.substr(start, m_Position - start);
    return MakeToken(CHTLTokenType::COMMENT_GENERATOR, comment);
}

CHTLToken CHTLLexer::ScanBracketKeyword() {
    size_t start = m_Position;
    std::string keyword = "[";
    Advance(); // 跳过 [
    
    while (!IsAtEnd() && IsAlpha(CurrentChar())) {
        keyword += CurrentChar();
        Advance();
    }
    
    if (!IsAtEnd() && CurrentChar() == ']') {
        keyword += ']';
        Advance();
        
        auto it = s_BracketKeywords.find(keyword);
        if (it != s_BracketKeywords.end()) {
            return MakeToken(it->second, keyword);
        }
    }
    
    return MakeErrorToken("无效的方括号关键字: " + keyword);
}

CHTLToken CHTLLexer::ScanAtKeyword() {
    size_t start = m_Position;
    std::string keyword = "@";
    Advance(); // 跳过 @
    
    while (!IsAtEnd() && (IsAlpha(CurrentChar()) || (keyword.length() > 1 && IsDigit(CurrentChar())))) {
        keyword += CurrentChar();
        Advance();
    }
    
    auto it = s_AtKeywords.find(keyword);
    if (it != s_AtKeywords.end()) {
        return MakeToken(it->second, keyword);
    }
    
    // 可能是自定义类型
    if (keyword.length() > 1) {
        return MakeToken(CHTLTokenType::AT_CUSTOM_TYPE, keyword);
    }
    
    return MakeErrorToken("无效的@关键字: " + keyword);
}

CHTLToken CHTLLexer::ScanUnquotedLiteral() {
    size_t start = m_Position;
    
    // 无引号字面量可以包含字母、数字、某些特殊字符
    while (!IsAtEnd()) {
        char ch = CurrentChar();
        if (IsWhitespace(ch) || ch == ';' || ch == ':' || ch == '=' ||
            ch == '{' || ch == '}' || ch == ',' || ch == '(' || ch == ')') {
            break;
        }
        Advance();
    }
    
    std::string literal = m_Source.substr(start, m_Position - start);
    return MakeToken(CHTLTokenType::UNQUOTED_LITERAL, literal);
}

char CHTLLexer::CurrentChar() const {
    return IsAtEnd() ? '\0' : m_Source[m_Position];
}

char CHTLLexer::NextChar() {
    if (IsAtEnd()) return '\0';
    
    char ch = m_Source[m_Position];
    Advance();
    return ch;
}

char CHTLLexer::PeekChar(size_t offset) const {
    size_t pos = m_Position + offset;
    return (pos >= m_Source.length()) ? '\0' : m_Source[pos];
}

bool CHTLLexer::IsAtEnd() const {
    return m_Position >= m_Source.length();
}

void CHTLLexer::Advance() {
    if (!IsAtEnd()) {
        if (m_Source[m_Position] == '\n') {
            m_Line++;
            m_Column = 1;
        } else {
            m_Column++;
        }
        m_Position++;
    }
}

void CHTLLexer::SkipWhitespace() {
    while (!IsAtEnd() && IsWhitespace(CurrentChar())) {
        Advance();
    }
}

bool CHTLLexer::Match(char expected) {
    if (IsAtEnd() || CurrentChar() != expected) {
        return false;
    }
    Advance();
    return true;
}

bool CHTLLexer::MatchSequence(const std::string& sequence) {
    for (size_t i = 0; i < sequence.length(); ++i) {
        if (PeekChar(i) != sequence[i]) {
            return false;
        }
    }
    
    for (size_t i = 0; i < sequence.length(); ++i) {
        Advance();
    }
    
    return true;
}

bool CHTLLexer::IsAlpha(char ch) const {
    return std::isalpha(static_cast<unsigned char>(ch));
}

bool CHTLLexer::IsDigit(char ch) const {
    return std::isdigit(static_cast<unsigned char>(ch));
}

bool CHTLLexer::IsAlphaNumeric(char ch) const {
    return IsAlpha(ch) || IsDigit(ch);
}

bool CHTLLexer::IsIdentifierStart(char ch) const {
    return IsAlpha(ch) || ch == '_';
}

bool CHTLLexer::IsIdentifierChar(char ch) const {
    return IsAlphaNumeric(ch) || ch == '_' || ch == '-';
}

bool CHTLLexer::IsWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

CHTLToken CHTLLexer::MakeToken(CHTLTokenType type) const {
    return CHTLToken(type, "", GetCurrentLocation());
}

CHTLToken CHTLLexer::MakeToken(CHTLTokenType type, const std::string& lexeme) const {
    return CHTLToken(type, lexeme, GetCurrentLocation(lexeme.length()));
}

CHTLToken CHTLLexer::MakeToken(CHTLTokenType type, const std::string& lexeme,
                              const CHTLToken::ValueType& value) const {
    return CHTLToken(type, lexeme, value, GetCurrentLocation(lexeme.length()));
}

CHTLToken CHTLLexer::MakeErrorToken(const std::string& message) const {
    ReportError(message);
    return CHTLToken(CHTLTokenType::INVALID, message, GetCurrentLocation());
}

void CHTLLexer::ReportError(const std::string& message) const {
    std::string error = m_Filename + ":" + std::to_string(m_Line) + ":" + 
                       std::to_string(m_Column) + " - " + message;
    m_Errors.push_back(error);
    utils::Logger::GetInstance().Error(message, m_Filename, m_Line);
}

TokenLocation CHTLLexer::GetCurrentLocation(size_t length) const {
    return TokenLocation(m_Line, m_Column, m_Position, length);
}

void CHTLLexer::UpdatePosition(char ch) {
    if (ch == '\n') {
        m_Line++;
        m_Column = 1;
    } else {
        m_Column++;
    }
}

void CHTLLexer::InitializeKeywords() {
    // 关键字映射表已经在静态初始化中完成
}

} // namespace compiler
} // namespace chtl