#include "CHTL/Lexer/CHTLLexer.h"
#include "Utils/StringUtils.h"
#include <algorithm>
#include <sstream>
#include <cctype>

namespace CHTL {
namespace Lexer {

CHTLLexer::CHTLLexer(const LexerConfig& config) 
    : config_(config), current_(0), start_(0), line_(1), column_(1), tokenCount_(0) {}

Core::TokenStream CHTLLexer::Tokenize(const std::string& source, const std::string& fileName) {
    source_ = source;
    fileName_ = fileName;
    current_ = 0;
    start_ = 0;
    line_ = 1;
    column_ = 1;
    tokenCount_ = 0;
    tokenStats_.clear();
    
    Core::TokenStream tokens;
    
    while (!IsAtEnd()) {
        start_ = current_;
        Core::CHTLToken token = ScanToken();
        
        if (token.GetType() != Core::TokenType::UNKNOWN) {
            tokens.AddToken(token);
            tokenCount_++;
            tokenStats_[token.GetType()]++;
            
            if (config_.enableDebug) {
                Utils::ErrorHandler::GetInstance().LogInfo(
                    "Token: " + token.ToString()
                );
            }
        }
    }
    
    // 添加EOF Token
    tokens.AddToken(MakeToken(Core::TokenType::END_OF_FILE, ""));
    
    return tokens;
}

Core::CHTLToken CHTLLexer::ScanToken() {
    // 跳过空白字符（除非配置保留）
    if (!config_.preserveWhitespace) {
        SkipWhitespace();
    }
    
    if (IsAtEnd()) {
        return MakeToken(Core::TokenType::END_OF_FILE, "");
    }
    
    char ch = Advance();
    
    // 扫描单字符Token
    switch (ch) {
        case '{': return MakeToken(Core::TokenType::LEFT_BRACE, "{");
        case '}': return MakeToken(Core::TokenType::RIGHT_BRACE, "}");
        case '[': 
            // 可能是标记关键字的开始
            if (IsAlpha(Peek())) {
                current_--; // 回退
                return ScanMarkerKeyword();
            }
            return MakeToken(Core::TokenType::LEFT_BRACKET, "[");
        case ']': return MakeToken(Core::TokenType::RIGHT_BRACKET, "]");
        case '(': return MakeToken(Core::TokenType::LEFT_PAREN, "(");
        case ')': return MakeToken(Core::TokenType::RIGHT_PAREN, ")");
        case ';': return MakeToken(Core::TokenType::SEMICOLON, ";");
        case ':': 
            // 检查是否为伪类或伪元素选择器
            if (Peek() == ':') {
                // 伪元素选择器 ::
                Advance();
                return ScanCSSSelector(':');
            } else if (IsAlpha(Peek())) {
                // 可能是伪类选择器
                return ScanCSSSelector(':');
            }
            return MakeToken(Core::TokenType::COLON, ":");
        case '=': return MakeToken(Core::TokenType::EQUAL, "=");
        case '.': 
            // 检查是否为CSS类选择器
            if (IsAlpha(Peek()) || Peek() == '-' || Peek() == '_') {
                return ScanCSSSelector('.');
            }
            return MakeToken(Core::TokenType::DOT, ".");
        case ',': return MakeToken(Core::TokenType::COMMA, ",");
        case '&': 
            // 上下文推导选择器
            return ScanCSSSelector('&');
        case '#': 
            // CSS ID选择器
            if (IsAlpha(Peek()) || Peek() == '-' || Peek() == '_') {
                return ScanCSSSelector('#');
            }
            return MakeToken(Core::TokenType::HASH, "#");
        case '@': 
            // 类型关键字
            if (IsAlpha(Peek())) {
                return ScanTypeKeyword();
            }
            return MakeToken(Core::TokenType::AT, "@");
        case '/':
            // 注释
            if (Match('/')) {
                return ScanSingleLineComment();
            } else if (Match('*')) {
                return ScanMultiLineComment();
            }
            return MakeErrorToken("意外的字符: /");
        case '-':
            // 生成器注释或数字
            if (Match('-')) {
                return ScanGeneratorComment();
            } else if (IsDigit(Peek())) {
                current_--; // 回退
                return ScanNumberLiteral();
            }
            return MakeErrorToken("意外的字符: -");
        case '"':
        case '\'':
            // 字符串字面量
            current_--; // 回退
            return ScanStringLiteral(ch);
        case ' ':
        case '\t':
        case '\r':
            // 空白字符
            if (config_.preserveWhitespace) {
                return MakeToken(Core::TokenType::WHITESPACE, std::string(1, ch));
            }
            break;
        case '\n':
            // 换行符
            line_++;
            column_ = 1;
            if (config_.preserveWhitespace) {
                return MakeToken(Core::TokenType::NEWLINE, "\n");
            }
            break;
        default:
            // 数字
            if (IsDigit(ch)) {
                current_--; // 回退
                return ScanNumberLiteral();
            }
            // 字母（标识符或关键字）
            if (IsAlpha(ch)) {
                current_--; // 回退
                return ScanIdentifierOrKeyword();
            }
            // 其他字符
            return MakeErrorToken("意外的字符: " + std::string(1, ch));
    }
    
    // 继续扫描下一个Token
    return ScanToken();
}

Core::CHTLToken CHTLLexer::ScanIdentifierOrKeyword() {
    while (IsValidIdentifierChar(Peek(), false)) {
        Advance();
    }
    
    std::string value = source_.substr(start_, current_ - start_);
    
    // 检查组合关键字（如"at top", "at bottom"）
    if (value == "at" && (Peek() == ' ' || Peek() == '\t')) {
        // 保存当前位置
        size_t savedCurrent = current_;
        int savedColumn = column_;
        
        // 跳过空白
        while (IsWhitespace(Peek())) {
            Advance();
        }
        
        // 检查是否为"top"或"bottom"
        if (source_.substr(current_, 3) == "top" && 
            !IsAlphaNumeric(Peek(3))) {
            current_ += 3;
            column_ += 3;
            return MakeToken(Core::TokenType::AT_TOP_KEYWORD, "at top");
        } else if (source_.substr(current_, 6) == "bottom" && 
                  !IsAlphaNumeric(Peek(6))) {
            current_ += 6;
            column_ += 6;
            return MakeToken(Core::TokenType::AT_BOTTOM_KEYWORD, "at bottom");
        } else {
            // 恢复位置
            current_ = savedCurrent;
            column_ = savedColumn;
        }
    }
    
    // 检查是否为关键字
    Core::TokenType tokenType = Core::CHTLToken::GetTokenTypeFromString(value);
    
    return MakeToken(tokenType, value);
}

Core::CHTLToken CHTLLexer::ScanStringLiteral(char quote) {
    Advance(); // 消费开始引号
    
    std::string value = "";
    
    while (!IsAtEnd() && Peek() != quote) {
        if (Peek() == '\\') {
            // 处理转义字符
            Advance(); // 跳过反斜杠
            if (!IsAtEnd()) {
                char escaped = Advance();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case '\'': value += '\''; break;
                    default: 
                        value += escaped;
                        ReportWarning("未知的转义字符: \\" + std::string(1, escaped));
                        break;
                }
            }
        } else {
            if (Peek() == '\n') {
                line_++;
                column_ = 1;
            }
            value += Advance();
        }
    }
    
    if (IsAtEnd()) {
        return MakeErrorToken("未闭合的字符串字面量");
    }
    
    // 消费结束引号
    Advance();
    
    return MakeToken(Core::TokenType::STRING_LITERAL, value);
}

Core::CHTLToken CHTLLexer::ScanNumberLiteral() {
    bool hasDecimal = false;
    
    // 处理负号
    if (Peek() == '-') {
        Advance();
    }
    
    // 扫描整数部分
    while (IsDigit(Peek())) {
        Advance();
    }
    
    // 检查小数点
    if (Peek() == '.' && IsDigit(Peek(1))) {
        hasDecimal = true;
        Advance(); // 消费小数点
        
        // 扫描小数部分
        while (IsDigit(Peek())) {
            Advance();
        }
    }
    
    // 检查单位（CSS单位）
    if (IsAlpha(Peek())) {
        while (IsAlpha(Peek())) {
            Advance();
        }
    }
    
    std::string value = source_.substr(start_, current_ - start_);
    return MakeToken(Core::TokenType::NUMBER, value);
}

Core::CHTLToken CHTLLexer::ScanSingleLineComment() {
    // 已经消费了 //
    std::string value = "//";
    
    while (!IsAtEnd() && Peek() != '\n') {
        value += Advance();
    }
    
    if (config_.preserveComments) {
        return MakeToken(Core::TokenType::SINGLE_LINE_COMMENT, value);
    } else {
        return ScanToken(); // 跳过注释，继续扫描
    }
}

Core::CHTLToken CHTLLexer::ScanMultiLineComment() {
    // 已经消费了 /*
    std::string value = "/*";
    
    while (!IsAtEnd()) {
        if (Peek() == '*' && Peek(1) == '/') {
            value += Advance(); // *
            value += Advance(); // /
            break;
        }
        
        if (Peek() == '\n') {
            line_++;
            column_ = 1;
        }
        value += Advance();
    }
    
    if (config_.preserveComments) {
        return MakeToken(Core::TokenType::MULTI_LINE_COMMENT, value);
    } else {
        return ScanToken(); // 跳过注释，继续扫描
    }
}

Core::CHTLToken CHTLLexer::ScanGeneratorComment() {
    // 已经消费了 --
    std::string value = "--";
    
    while (!IsAtEnd() && Peek() != '\n') {
        value += Advance();
    }
    
    // 生成器注释总是保留
    return MakeToken(Core::TokenType::GENERATOR_COMMENT, value);
}

Core::CHTLToken CHTLLexer::ScanMarkerKeyword() {
    if (!Match('[')) {
        return MakeErrorToken("期望 '['");
    }
    
    std::string value = "[";
    
    // 扫描标记名称
    while (!IsAtEnd() && Peek() != ']') {
        if (Peek() == '\n') {
            return MakeErrorToken("标记关键字不能跨行");
        }
        value += Advance();
    }
    
    if (!Match(']')) {
        return MakeErrorToken("期望 ']'");
    }
    
    value += "]";
    
    // 检查是否为有效的标记关键字
    Core::TokenType tokenType = Core::CHTLToken::GetTokenTypeFromString(value);
    if (tokenType == Core::TokenType::IDENTIFIER) {
        ReportWarning("未知的标记关键字: " + value);
    }
    
    return MakeToken(tokenType, value);
}

Core::CHTLToken CHTLLexer::ScanTypeKeyword() {
    if (!Match('@')) {
        return MakeErrorToken("期望 '@'");
    }
    
    std::string value = "@";
    
    // 扫描类型名称
    while (IsAlphaNumeric(Peek())) {
        value += Advance();
    }
    
    if (value.length() == 1) {
        return MakeErrorToken("类型关键字不能为空");
    }
    
    // 检查是否为有效的类型关键字
    Core::TokenType tokenType = Core::CHTLToken::GetTokenTypeFromString(value);
    if (tokenType == Core::TokenType::IDENTIFIER) {
        // 可能是自定义类型，在严格模式下报告警告
        if (config_.strictMode) {
            ReportWarning("未知的类型关键字: " + value);
        }
    }
    
    return MakeToken(tokenType, value);
}

Core::CHTLToken CHTLLexer::ScanCSSSelector(char prefix) {
    std::string value = std::string(1, prefix);
    
    if (prefix == ':' && Peek() == ':') {
        // 伪元素选择器 ::
        value += Advance();
    }
    
    // 扫描选择器名称
    while (IsAlphaNumeric(Peek()) || Peek() == '-' || Peek() == '_') {
        value += Advance();
    }
    
    // 对于&选择器，可能跟随伪类或伪元素
    if (prefix == '&') {
        if (Peek() == ':') {
            while (Peek() == ':' || IsAlphaNumeric(Peek()) || Peek() == '-') {
                value += Advance();
            }
        }
    }
    
    // 确定选择器类型
    Core::TokenType tokenType;
    switch (prefix) {
        case '.': tokenType = Core::TokenType::CLASS_SELECTOR; break;
        case '#': tokenType = Core::TokenType::ID_SELECTOR; break;
        case ':': 
            tokenType = (value.find("::") != std::string::npos) ? 
                       Core::TokenType::PSEUDO_ELEMENT : Core::TokenType::PSEUDO_CLASS;
            break;
        case '&': tokenType = Core::TokenType::AMPERSAND; break;
        default: tokenType = Core::TokenType::UNKNOWN; break;
    }
    
    return MakeToken(tokenType, value);
}

char CHTLLexer::Peek() const {
    return Peek(0);
}

char CHTLLexer::Peek(int offset) const {
    size_t pos = current_ + offset;
    if (pos >= source_.length()) {
        return '\0';
    }
    return source_[pos];
}

char CHTLLexer::Advance() {
    if (IsAtEnd()) {
        return '\0';
    }
    
    char ch = source_[current_++];
    if (ch == '\n') {
        line_++;
        column_ = 1;
    } else {
        column_++;
    }
    
    return ch;
}

bool CHTLLexer::Match(char expected) {
    if (IsAtEnd() || Peek() != expected) {
        return false;
    }
    
    Advance();
    return true;
}

bool CHTLLexer::IsAtEnd() const {
    return current_ >= source_.length();
}

void CHTLLexer::SkipWhitespace() {
    while (!IsAtEnd() && IsWhitespace(Peek())) {
        if (Peek() == '\n') {
            line_++;
            column_ = 1;
        }
        Advance();
    }
}

Core::CHTLToken CHTLLexer::MakeToken(Core::TokenType type, const std::string& value) {
    std::string tokenValue = value.empty() ? 
                            source_.substr(start_, current_ - start_) : value;
    
    return Core::CHTLToken(type, tokenValue, line_, column_ - tokenValue.length(), fileName_);
}

Core::CHTLToken CHTLLexer::MakeErrorToken(const std::string& message) {
    ReportError(message);
    return Core::CHTLToken(Core::TokenType::UNKNOWN, message, line_, column_, fileName_);
}

void CHTLLexer::ReportError(const std::string& message) {
    Utils::ErrorHandler::GetInstance().LogError(
        message, fileName_, line_, column_
    );
}

void CHTLLexer::ReportWarning(const std::string& message) {
    Utils::ErrorHandler::GetInstance().LogWarning(
        message, fileName_, line_, column_
    );
}

bool CHTLLexer::IsAlpha(char ch) {
    return std::isalpha(static_cast<unsigned char>(ch)) || ch == '_';
}

bool CHTLLexer::IsDigit(char ch) {
    return std::isdigit(static_cast<unsigned char>(ch));
}

bool CHTLLexer::IsAlphaNumeric(char ch) {
    return IsAlpha(ch) || IsDigit(ch);
}

bool CHTLLexer::IsWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r';
}

bool CHTLLexer::IsValidIdentifierChar(char ch, bool isFirst) {
    if (isFirst) {
        return IsAlpha(ch);
    }
    return IsAlphaNumeric(ch) || ch == '-';
}

void CHTLLexer::Reset() {
    source_.clear();
    fileName_.clear();
    current_ = 0;
    start_ = 0;
    line_ = 1;
    column_ = 1;
    tokenCount_ = 0;
    tokenStats_.clear();
}

size_t CHTLLexer::GetErrorCount() const {
    return Utils::ErrorHandler::GetInstance().GetErrorCount();
}

std::string CHTLLexer::GetStatistics() const {
    std::ostringstream oss;
    oss << "CHTL词法分析统计:\n";
    oss << "  文件名: " << fileName_ << "\n";
    oss << "  源代码长度: " << source_.length() << " 字符\n";
    oss << "  Token总数: " << tokenCount_ << "\n";
    oss << "  行数: " << line_ << "\n";
    oss << "  错误数量: " << GetErrorCount() << "\n";
    
    oss << "\nToken类型分布:\n";
    for (const auto& pair : tokenStats_) {
        oss << "  " << Core::CHTLToken::GetTokenTypeName(pair.first) 
            << ": " << pair.second << "\n";
    }
    
    return oss.str();
}

// LexerFactory 实现
std::unique_ptr<CHTLLexer> LexerFactory::CreateDefaultLexer() {
    return std::make_unique<CHTLLexer>();
}

std::unique_ptr<CHTLLexer> LexerFactory::CreateStrictLexer() {
    LexerConfig config;
    config.strictMode = true;
    config.preserveComments = true;
    config.preserveWhitespace = false;
    return std::make_unique<CHTLLexer>(config);
}

std::unique_ptr<CHTLLexer> LexerFactory::CreateDebugLexer() {
    LexerConfig config;
    config.enableDebug = true;
    config.preserveComments = true;
    config.preserveWhitespace = true;
    return std::make_unique<CHTLLexer>(config);
}

} // namespace Lexer
} // namespace CHTL
