#include "CHTLJS/Lexer/CHTLJSLexer.h"
#include "Utils/ErrorHandler.h"
#include <cctype>

namespace CHTL {
namespace CHTLJS {
namespace Lexer {

CHTLJSLexer::CHTLJSLexer(const LexerConfig& config) 
    : config_(config), start_(0), current_(0), line_(1), column_(1) {}

Core::CHTLJSTokenStream CHTLJSLexer::Tokenize(const std::string& source, const std::string& fileName) {
    source_ = source;
    fileName_ = fileName;
    start_ = 0;
    current_ = 0;
    line_ = 1;
    column_ = 1;
    
    Core::CHTLJSTokenStream tokenStream;
    
    while (!IsAtEnd()) {
        start_ = current_;
        
        Core::CHTLJSToken token = ScanToken();
        
        // 根据配置决定是否添加空白和注释Token
        if (token.GetType() == Core::TokenType::WHITESPACE && !config_.preserveWhitespace) {
            continue;
        }
        
        if (token.GetType() == Core::TokenType::COMMENT && !config_.preserveComments) {
            continue;
        }
        
        tokenStream.AddToken(token);
        
        if (config_.enableDebug) {
            Utils::ErrorHandler::GetInstance().LogInfo(
                "CHTL JS Token: " + token.ToString()
            );
        }
    }
    
    // 添加EOF Token
    tokenStream.AddToken(Core::CHTLJSToken(Core::TokenType::END_OF_FILE, "", GetCurrentPosition()));
    
    return tokenStream;
}

Core::CHTLJSToken CHTLJSLexer::ScanToken() {
    char c = Advance();
    
    switch (c) {
        case ' ':
        case '\r':
        case '\t':
            return MakeToken(Core::TokenType::WHITESPACE, " ");
            
        case '\n':
            line_++;
            column_ = 1;
            return MakeToken(Core::TokenType::NEWLINE, "\n");
            
        case '(':
            return MakeToken(Core::TokenType::LEFT_PAREN, "(");
        case ')':
            return MakeToken(Core::TokenType::RIGHT_PAREN, ")");
        case '{':
            // 检查是否为增强选择器开始 {{
            if (Peek() == '{') {
                current_--; // 回退
                return ScanEnhancedSelector();
            }
            return MakeToken(Core::TokenType::LEFT_BRACE, "{");
        case '}':
            return MakeToken(Core::TokenType::RIGHT_BRACE, "}");
        case '[':
            return MakeToken(Core::TokenType::LEFT_BRACKET, "[");
        case ']':
            return MakeToken(Core::TokenType::RIGHT_BRACKET, "]");
        case ',':
            return MakeToken(Core::TokenType::COMMA, ",");
        case ';':
            return MakeToken(Core::TokenType::SEMICOLON, ";");
        case ':':
            return MakeToken(Core::TokenType::COLON, ":");
        case '=':
            return MakeToken(Core::TokenType::EQUAL, "=");
        case '.':
            return MakeToken(Core::TokenType::DOT, ".");
        case '-':
            // 检查是否为箭头操作符 ->
            if (Peek() == '>') {
                return ScanArrowOperator();
            }
            return MakeToken(Core::TokenType::IDENTIFIER, "-");
            
        case '/':
            // 检查注释
            if (Peek() == '/' || Peek() == '*') {
                return ScanComment();
            }
            return MakeToken(Core::TokenType::IDENTIFIER, "/");
            
        case '"':
        case '\'':
            return ScanString(c);
            
        default:
            if (IsDigit(c)) {
                current_--; // 回退，让ScanNumber处理
                return ScanNumber();
            } else if (IsAlpha(c) || c == '_') {
                current_--; // 回退，让ScanIdentifierOrKeyword处理
                return ScanIdentifierOrKeyword();
            } else {
                return MakeErrorToken("意外的字符: " + std::string(1, c));
            }
    }
}

Core::CHTLJSToken CHTLJSLexer::ScanEnhancedSelector() {
    // 扫描增强选择器 {{selector}}
    if (!Match('{') || !Match('{')) {
        return MakeErrorToken("期望 '{{'");
    }
    
    std::string selectorContent = "";
    
    // 扫描选择器内容直到 }}
    while (!IsAtEnd() && !(Peek() == '}' && PeekNext() == '}')) {
        char c = Advance();
        selectorContent += c;
        
        if (c == '\n') {
            line_++;
            column_ = 1;
        }
    }
    
    // 消费结束的 }}
    if (!Match('}') || !Match('}')) {
        return MakeErrorToken("期望 '}}'");
    }
    
    // 创建增强选择器Token
    std::string fullSelector = "{{" + selectorContent + "}}";
    return MakeToken(Core::TokenType::ENHANCED_SELECTOR, fullSelector);
}

Core::CHTLJSToken CHTLJSLexer::ScanArrowOperator() {
    // 扫描箭头操作符 ->
    if (!Match('-') || !Match('>')) {
        return MakeErrorToken("期望 '->'");
    }
    
    return MakeToken(Core::TokenType::ARROW, "->");
}

Core::CHTLJSToken CHTLJSLexer::ScanIdentifierOrKeyword() {
    while (IsAlphaNumeric(Peek()) || Peek() == '_') {
        Advance();
    }
    
    std::string value = source_.substr(start_, current_ - start_);
    
    // 检查是否为关键字
    if (Core::CHTLJSToken::IsKeywordString(value)) {
        Core::TokenType keywordType = Core::CHTLJSToken::GetKeywordTokenType(value);
        return MakeToken(keywordType, value);
    }
    
    return MakeToken(Core::TokenType::IDENTIFIER, value);
}

Core::CHTLJSToken CHTLJSLexer::ScanString(char quote) {
    std::string value = "";
    
    while (!IsAtEnd() && Peek() != quote) {
        char c = Advance();
        
        if (c == '\n') {
            line_++;
            column_ = 1;
        }
        
        // 处理转义字符
        if (c == '\\' && !IsAtEnd()) {
            char escaped = Advance();
            switch (escaped) {
                case 'n': value += '\n'; break;
                case 't': value += '\t'; break;
                case 'r': value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"': value += '"'; break;
                case '\'': value += '\''; break;
                default: 
                    value += '\\';
                    value += escaped;
                    break;
            }
        } else {
            value += c;
        }
    }
    
    if (IsAtEnd()) {
        return MakeErrorToken("未结束的字符串");
    }
    
    // 消费结束引号
    Advance();
    
    return MakeToken(Core::TokenType::STRING, value);
}

Core::CHTLJSToken CHTLJSLexer::ScanNumber() {
    while (IsDigit(Peek())) {
        Advance();
    }
    
    // 检查小数点
    if (Peek() == '.' && IsDigit(PeekNext())) {
        Advance(); // 消费小数点
        while (IsDigit(Peek())) {
            Advance();
        }
    }
    
    std::string value = source_.substr(start_, current_ - start_);
    return MakeToken(Core::TokenType::NUMBER, value);
}

Core::CHTLJSToken CHTLJSLexer::ScanComment() {
    if (Peek() == '/') {
        return ScanSingleLineComment();
    } else if (Peek() == '*') {
        return ScanMultiLineComment();
    }
    
    return MakeErrorToken("无效的注释");
}

Core::CHTLJSToken CHTLJSLexer::ScanSingleLineComment() {
    // 消费第二个 /
    Advance();
    
    std::string content = "";
    
    while (!IsAtEnd() && Peek() != '\n') {
        content += Advance();
    }
    
    return MakeToken(Core::TokenType::COMMENT, "//" + content);
}

Core::CHTLJSToken CHTLJSLexer::ScanMultiLineComment() {
    // 消费 *
    Advance();
    
    std::string content = "";
    
    while (!IsAtEnd()) {
        if (Peek() == '*' && PeekNext() == '/') {
            Advance(); // 消费 *
            Advance(); // 消费 /
            break;
        }
        
        char c = Advance();
        content += c;
        
        if (c == '\n') {
            line_++;
            column_ = 1;
        }
    }
    
    return MakeToken(Core::TokenType::COMMENT, "/*" + content + "*/");
}

void CHTLJSLexer::SkipWhitespace() {
    while (!IsAtEnd() && IsWhitespace(Peek())) {
        if (Peek() == '\n') {
            line_++;
            column_ = 1;
        }
        Advance();
    }
}

char CHTLJSLexer::Advance() {
    if (IsAtEnd()) return '\0';
    
    column_++;
    return source_[current_++];
}

char CHTLJSLexer::Peek() const {
    if (IsAtEnd()) return '\0';
    return source_[current_];
}

char CHTLJSLexer::PeekNext(int offset) const {
    size_t position = current_ + offset;
    if (position >= source_.length()) return '\0';
    return source_[position];
}

bool CHTLJSLexer::Match(char expected) {
    if (IsAtEnd() || source_[current_] != expected) {
        return false;
    }
    
    current_++;
    column_++;
    return true;
}

bool CHTLJSLexer::IsAtEnd() const {
    return current_ >= source_.length();
}

bool CHTLJSLexer::IsAlpha(char c) const {
    return std::isalpha(c) || c == '_';
}

bool CHTLJSLexer::IsDigit(char c) const {
    return std::isdigit(c);
}

bool CHTLJSLexer::IsAlphaNumeric(char c) const {
    return IsAlpha(c) || IsDigit(c);
}

bool CHTLJSLexer::IsWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\r';
}

Core::CHTLJSToken CHTLJSLexer::MakeToken(Core::TokenType type, const std::string& value) {
    std::string tokenValue = value.empty() ? source_.substr(start_, current_ - start_) : value;
    return Core::CHTLJSToken(type, tokenValue, GetCurrentPosition());
}

Core::CHTLJSToken CHTLJSLexer::MakeErrorToken(const std::string& message) {
    Utils::ErrorHandler::GetInstance().LogError(
        "CHTL JS词法错误: " + message + " 在 " + fileName_ + ":" + 
        std::to_string(line_) + ":" + std::to_string(column_)
    );
    
    return Core::CHTLJSToken(Core::TokenType::ERROR_TOKEN, message, GetCurrentPosition());
}

Core::TokenPosition CHTLJSLexer::GetCurrentPosition() const {
    return Core::TokenPosition(line_, column_ - (current_ - start_), start_);
}

} // namespace Lexer
} // namespace CHTLJS
} // namespace CHTL
