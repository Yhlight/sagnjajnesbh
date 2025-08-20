#include "chtl_js/lexer/CHTLJSLexer.h"
#include "utils/Logger.h"
#include <algorithm>
#include <cctype>

namespace chtl {
namespace chtljs {

// 静态成员定义
const std::unordered_map<std::string, CHTLJSTokenType> CHTLJSLexer::s_Keywords = {
    {"vir", CHTLJSTokenType::KEYWORD_VIR}
};

const std::unordered_map<std::string, CHTLJSTokenType> CHTLJSLexer::s_Functions = {
    {"listen", CHTLJSTokenType::FUNCTION_LISTEN},
    {"delegate", CHTLJSTokenType::FUNCTION_DELEGATE},
    {"animate", CHTLJSTokenType::FUNCTION_ANIMATE},
    {"iNeverAway", CHTLJSTokenType::FUNCTION_INEVERAWAY},
    {"printMylove", CHTLJSTokenType::FUNCTION_PRINTMYLOVE}
};

bool CHTLJSLexer::s_Initialized = false;

CHTLJSLexer::CHTLJSLexer()
    : m_Position(0)
    , m_Line(1)
    , m_Column(1)
    , m_InEnhancedSelector(false)
    , m_BraceDepth(0) {
    if (!s_Initialized) {
        InitializeKeywords();
        s_Initialized = true;
    }
}

CHTLJSLexer::~CHTLJSLexer() = default;

void CHTLJSLexer::Initialize(const std::string& source, const std::string& filename) {
    m_Source = source;
    m_Filename = filename;
    m_Position = 0;
    m_Line = 1;
    m_Column = 1;
    m_InEnhancedSelector = false;
    m_BraceDepth = 0;
    m_TokenCache.clear();
    m_Errors.clear();
}

CHTLJSToken CHTLJSLexer::NextToken() {
    if (!m_TokenCache.empty()) {
        CHTLJSToken token = m_TokenCache.front();
        m_TokenCache.erase(m_TokenCache.begin());
        return token;
    }
    
    return ScanToken();
}

CHTLJSToken CHTLJSLexer::PeekToken(size_t lookahead) {
    while (m_TokenCache.size() < lookahead && !IsAtEnd()) {
        m_TokenCache.push_back(ScanToken());
    }
    
    if (lookahead <= m_TokenCache.size()) {
        return m_TokenCache[lookahead - 1];
    }
    
    return MakeToken(CHTLJSTokenType::END_OF_FILE);
}

std::vector<CHTLJSToken> CHTLJSLexer::GetAllTokens() {
    std::vector<CHTLJSToken> tokens;
    
    while (!IsAtEnd()) {
        CHTLJSToken token = NextToken();
        tokens.push_back(token);
        
        if (token.GetType() == CHTLJSTokenType::END_OF_FILE) {
            break;
        }
    }
    
    return tokens;
}

void CHTLJSLexer::Reset() {
    m_Position = 0;
    m_Line = 1;
    m_Column = 1;
    m_InEnhancedSelector = false;
    m_BraceDepth = 0;
    m_TokenCache.clear();
    m_Errors.clear();
}

CHTLJSToken CHTLJSLexer::ScanToken() {
    SkipWhitespace();
    
    if (IsAtEnd()) {
        return MakeToken(CHTLJSTokenType::END_OF_FILE);
    }
    
    char ch = CurrentChar();
    
    // 如果在增强选择器内，使用特殊的扫描逻辑
    if (m_InEnhancedSelector) {
        return ScanSelectorContent();
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
    
    // 处理增强选择器开始
    if (ch == '{' && PeekChar() == '{') {
        Advance(); // 跳过第一个 {
        Advance(); // 跳过第二个 {
        m_InEnhancedSelector = true;
        return MakeToken(CHTLJSTokenType::ENHANCED_SELECTOR_START, "{{");
    }
    
    // 处理增强选择器结束
    if (ch == '}' && PeekChar() == '}') {
        Advance(); // 跳过第一个 }
        Advance(); // 跳过第二个 }
        m_InEnhancedSelector = false;
        return MakeToken(CHTLJSTokenType::ENHANCED_SELECTOR_END, "}}");
    }
    
    // 处理箭头操作符
    if (ch == '-' && PeekChar() == '>') {
        return ScanArrow();
    }
    
    // 处理状态标记
    if (ch == '<' && IsAlpha(PeekChar())) {
        return ScanStateMarker();
    }
    
    // 处理单字符Token
    Advance();
    switch (ch) {
        case '.': return MakeToken(CHTLJSTokenType::DOT, ".");
        case '=': return MakeToken(CHTLJSTokenType::EQUALS, "=");
        case ':': return MakeToken(CHTLJSTokenType::COLON, ":");
        case ';': return MakeToken(CHTLJSTokenType::SEMICOLON, ";");
        case ',': return MakeToken(CHTLJSTokenType::COMMA, ",");
        case '{': 
            m_BraceDepth++;
            return MakeToken(CHTLJSTokenType::LEFT_BRACE, "{");
        case '}': 
            m_BraceDepth--;
            return MakeToken(CHTLJSTokenType::RIGHT_BRACE, "}");
        case '[': return MakeToken(CHTLJSTokenType::LEFT_BRACKET, "[");
        case ']': return MakeToken(CHTLJSTokenType::RIGHT_BRACKET, "]");
        case '(': return MakeToken(CHTLJSTokenType::LEFT_PAREN, "(");
        case ')': return MakeToken(CHTLJSTokenType::RIGHT_PAREN, ")");
        case '<': return MakeToken(CHTLJSTokenType::LEFT_ANGLE, "<");
        case '>': return MakeToken(CHTLJSTokenType::RIGHT_ANGLE, ">");
        default:
            return MakeErrorToken("CHTL JS意外的字符: " + std::string(1, ch));
    }
}

CHTLJSToken CHTLJSLexer::ScanIdentifierOrKeyword() {
    size_t start = m_Position;
    
    while (!IsAtEnd() && IsIdentifierChar(CurrentChar())) {
        Advance();
    }
    
    std::string identifier = m_Source.substr(start, m_Position - start);
    
    // 检查是否是CHTL JS关键字
    auto keywordIt = s_Keywords.find(identifier);
    if (keywordIt != s_Keywords.end()) {
        return MakeToken(keywordIt->second, identifier);
    }
    
    // 检查是否是CHTL JS函数
    auto functionIt = s_Functions.find(identifier);
    if (functionIt != s_Functions.end()) {
        return MakeToken(functionIt->second, identifier);
    }
    
    // 检查是否是函数类型（如Void）
    if (identifier == "Void" || identifier == "Function") {
        return MakeToken(CHTLJSTokenType::FUNCTION_TYPE, identifier);
    }
    
    // 普通标识符
    return MakeToken(CHTLJSTokenType::IDENTIFIER, identifier);
}

CHTLJSToken CHTLJSLexer::ScanEnhancedSelector() {
    // 此方法已被ScanSelectorContent替代
    return ScanSelectorContent();
}

CHTLJSToken CHTLJSLexer::ScanSelectorContent() {
    SkipWhitespace();
    
    if (IsAtEnd()) {
        return MakeErrorToken("未闭合的增强选择器");
    }
    
    char ch = CurrentChar();
    
    // 检查选择器结束
    if (ch == '}' && PeekChar() == '}') {
        Advance(); // 跳过第一个 }
        Advance(); // 跳过第二个 }
        m_InEnhancedSelector = false;
        return MakeToken(CHTLJSTokenType::ENHANCED_SELECTOR_END, "}}");
    }
    
    // 处理选择器特殊字符
    switch (ch) {
        case '.':
            Advance();
            return MakeToken(CHTLJSTokenType::SELECTOR_CLASS, ".");
            
        case '#':
            Advance();
            return MakeToken(CHTLJSTokenType::SELECTOR_ID, "#");
            
        case '&':
            Advance();
            return MakeToken(CHTLJSTokenType::SELECTOR_AMPERSAND, "&");
            
        case '[': {
            // 检查是否是索引选择器
            size_t start = m_Position;
            Advance(); // 跳过 [
            
            if (IsDigit(CurrentChar())) {
                while (!IsAtEnd() && IsDigit(CurrentChar())) {
                    Advance();
                }
                
                if (CurrentChar() == ']') {
                    Advance(); // 跳过 ]
                    std::string index = m_Source.substr(start, m_Position - start);
                    return MakeToken(CHTLJSTokenType::SELECTOR_INDEX, index);
                }
            }
            
            // 不是索引选择器，回退
            m_Position = start;
            Advance();
            return MakeToken(CHTLJSTokenType::LEFT_BRACKET, "[");
        }
    }
    
    // 扫描选择器名称
    if (IsIdentifierStart(ch)) {
        size_t start = m_Position;
        while (!IsAtEnd() && (IsIdentifierChar(CurrentChar()) || 
               CurrentChar() == '-' || CurrentChar() == '_')) {
            Advance();
        }
        
        std::string selector = m_Source.substr(start, m_Position - start);
        return MakeToken(CHTLJSTokenType::IDENTIFIER, selector);
    }
    
    // 处理空格（在选择器内也是有意义的）
    if (ch == ' ') {
        Advance();
        return ScanSelectorContent(); // 跳过空格继续扫描
    }
    
    return MakeErrorToken("无效的选择器内容: " + std::string(1, ch));
}

CHTLJSToken CHTLJSLexer::ScanString(char delimiter) {
    size_t start = m_Position;
    Advance(); // 跳过开始的引号
    
    std::string value;
    bool escaped = false;
    
    while (!IsAtEnd() && (CurrentChar() != delimiter || escaped)) {
        if (escaped) {
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
    return MakeToken(CHTLJSTokenType::STRING_LITERAL, lexeme, value);
}

CHTLJSToken CHTLJSLexer::ScanNumber() {
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
    
    return MakeToken(CHTLJSTokenType::NUMBER_LITERAL, numberStr, value);
}

CHTLJSToken CHTLJSLexer::ScanArrow() {
    Advance(); // 跳过 -
    Advance(); // 跳过 >
    return MakeToken(CHTLJSTokenType::ARROW, "->");
}

CHTLJSToken CHTLJSLexer::ScanStateMarker() {
    size_t start = m_Position;
    Advance(); // 跳过 <
    
    while (!IsAtEnd() && IsAlpha(CurrentChar())) {
        Advance();
    }
    
    if (CurrentChar() == '>') {
        Advance(); // 跳过 >
        std::string marker = m_Source.substr(start, m_Position - start);
        return MakeToken(CHTLJSTokenType::STATE_MARKER, marker);
    }
    
    // 不是状态标记，回退
    m_Position = start;
    Advance();
    return MakeToken(CHTLJSTokenType::LEFT_ANGLE, "<");
}

char CHTLJSLexer::CurrentChar() const {
    return IsAtEnd() ? '\0' : m_Source[m_Position];
}

char CHTLJSLexer::NextChar() {
    if (IsAtEnd()) return '\0';
    
    char ch = m_Source[m_Position];
    Advance();
    return ch;
}

char CHTLJSLexer::PeekChar(size_t offset) const {
    size_t pos = m_Position + offset;
    return (pos >= m_Source.length()) ? '\0' : m_Source[pos];
}

bool CHTLJSLexer::IsAtEnd() const {
    return m_Position >= m_Source.length();
}

void CHTLJSLexer::Advance() {
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

void CHTLJSLexer::SkipWhitespace() {
    while (!IsAtEnd()) {
        char ch = CurrentChar();
        
        if (IsWhitespace(ch)) {
            Advance();
        } else if (ch == '/' && PeekChar() == '/') {
            // 跳过单行注释
            while (!IsAtEnd() && CurrentChar() != '\n') {
                Advance();
            }
        } else if (ch == '/' && PeekChar() == '*') {
            // 跳过多行注释
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
        } else {
            break;
        }
    }
}

bool CHTLJSLexer::Match(char expected) {
    if (IsAtEnd() || CurrentChar() != expected) {
        return false;
    }
    Advance();
    return true;
}

bool CHTLJSLexer::MatchSequence(const std::string& sequence) {
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

bool CHTLJSLexer::IsAlpha(char ch) const {
    return std::isalpha(static_cast<unsigned char>(ch));
}

bool CHTLJSLexer::IsDigit(char ch) const {
    return std::isdigit(static_cast<unsigned char>(ch));
}

bool CHTLJSLexer::IsAlphaNumeric(char ch) const {
    return IsAlpha(ch) || IsDigit(ch);
}

bool CHTLJSLexer::IsIdentifierStart(char ch) const {
    return IsAlpha(ch) || ch == '_' || ch == '$';
}

bool CHTLJSLexer::IsIdentifierChar(char ch) const {
    return IsAlphaNumeric(ch) || ch == '_' || ch == '$';
}

bool CHTLJSLexer::IsWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

bool CHTLJSLexer::IsSelectorSpecialChar(char ch) const {
    return ch == '.' || ch == '#' || ch == '[' || ch == ']' || ch == '&';
}

CHTLJSToken CHTLJSLexer::MakeToken(CHTLJSTokenType type) const {
    return CHTLJSToken(type, "", GetCurrentLocation());
}

CHTLJSToken CHTLJSLexer::MakeToken(CHTLJSTokenType type, const std::string& lexeme) const {
    return CHTLJSToken(type, lexeme, GetCurrentLocation(lexeme.length()));
}

CHTLJSToken CHTLJSLexer::MakeToken(CHTLJSTokenType type, const std::string& lexeme,
                                  const CHTLJSToken::ValueType& value) const {
    return CHTLJSToken(type, lexeme, value, GetCurrentLocation(lexeme.length()));
}

CHTLJSToken CHTLJSLexer::MakeErrorToken(const std::string& message) const {
    ReportError(message);
    return CHTLJSToken(CHTLJSTokenType::INVALID, message, GetCurrentLocation());
}

void CHTLJSLexer::ReportError(const std::string& message) const {
    std::string error = m_Filename + ":" + std::to_string(m_Line) + ":" + 
                       std::to_string(m_Column) + " - " + message;
    m_Errors.push_back(error);
    utils::Logger::GetInstance().Error("CHTL JS词法错误: " + message, m_Filename, m_Line);
}

CHTLJSTokenLocation CHTLJSLexer::GetCurrentLocation(size_t length) const {
    return CHTLJSTokenLocation(m_Line, m_Column, m_Position, length);
}

void CHTLJSLexer::UpdatePosition(char ch) {
    if (ch == '\n') {
        m_Line++;
        m_Column = 1;
    } else {
        m_Column++;
    }
}

void CHTLJSLexer::InitializeKeywords() {
    // 关键字映射表已经在静态初始化中完成
}

} // namespace chtljs
} // namespace chtl