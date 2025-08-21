#include "CHTL/Lexer.h"
#include "Utils/Logger.h"
#include <cctype>
#include <sstream>
#include <iostream>
#include <deque>

namespace CHTL {

// Lexer内部实现
class Lexer::Impl {
public:
    std::string input_;
    std::string sourceName_;
    size_t position_ = 0;
    size_t line_ = 1;
    size_t column_ = 1;
    
    // Token缓存
    std::deque<Token> tokenCache_;
    
    char Current() const {
        return position_ < input_.length() ? input_[position_] : '\0';
    }
    
    char Peek(size_t ahead = 1) const {
        size_t pos = position_ + ahead;
        return pos < input_.length() ? input_[pos] : '\0';
    }
    
    void Advance() {
        if (position_ < input_.length()) {
            if (input_[position_] == '\n') {
                ++line_;
                column_ = 1;
            } else {
                ++column_;
            }
            ++position_;
        }
    }
    
    void SkipWhile(std::function<bool(char)> predicate) {
        while (position_ < input_.length() && predicate(Current())) {
            Advance();
        }
    }
    
    std::string ConsumeWhile(std::function<bool(char)> predicate) {
        std::string result;
        while (position_ < input_.length() && predicate(Current())) {
            result += Current();
            Advance();
        }
        return result;
    }
};

// Lexer实现
Lexer::Lexer() : impl_(std::make_unique<Impl>()), config_() {}
Lexer::Lexer(const LexerConfig& config) : impl_(std::make_unique<Impl>()), config_(config) {}
Lexer::~Lexer() = default;

void Lexer::SetInput(const std::string& input, const std::string& sourceName) {
    impl_->input_ = input;
    impl_->sourceName_ = sourceName;
    impl_->position_ = 0;
    impl_->line_ = 1;
    impl_->column_ = 1;
    impl_->tokenCache_.clear();
}

Token Lexer::NextToken() {
    // 如果缓存中有Token，直接返回
    if (!impl_->tokenCache_.empty()) {
        Token token = impl_->tokenCache_.front();
        impl_->tokenCache_.pop_front();
        return token;
    }
    
    // 跳过空白字符
    if (!config_.preserveWhitespace) {
        SkipWhitespace();
    }
    
    // 检查是否到达文件末尾
    if (impl_->position_ >= impl_->input_.length()) {
        return Token(TokenType::Eof, "", impl_->line_, impl_->column_, impl_->position_);
    }
    
    size_t startLine = impl_->line_;
    size_t startColumn = impl_->column_;
    size_t startPosition = impl_->position_;
    
    char ch = impl_->Current();
    char nextCh = impl_->Peek();
    
    // 注释
    if (ch == '/' && nextCh == '/') {
        Token token = ScanComment();
        if (!config_.skipComments) {
            return token;
        }
        return NextToken();  // 递归获取下一个Token
    }
    
    if (ch == '/' && nextCh == '*') {
        Token token = ScanComment();
        if (!config_.skipComments) {
            return token;
        }
        return NextToken();
    }
    
    if (ch == '-' && nextCh == '-') {
        // 生成器注释
        impl_->Advance();
        impl_->Advance();
        std::string comment = impl_->ConsumeWhile([](char c) { return c != '\n'; });
        return Token(TokenType::GeneratorComment, "--" + comment, 
                    startLine, startColumn, startPosition);
    }
    
    // 特殊标记 [...]
    if (ch == '[') {
        Token token = ScanSpecialMarker();
        if (token.type != TokenType::Unknown) {
            return token;
        }
        // 如果不是特殊标记，作为普通的左方括号
        impl_->Advance();
        return Token(TokenType::LeftBracket, "[", startLine, startColumn, startPosition);
    }
    
    // @标记
    if (ch == '@') {
        return ScanAtMarker();
    }
    
    // 字符串
    if (ch == '"' || ch == '\'') {
        return ScanString(ch);
    }
    
    // 数字
    if (IsDigit(ch)) {
        return ScanNumber();
    }
    
    // 标识符或关键字
    if (IsIdentifierStart(ch)) {
        return ScanIdentifier();
    }
    
    // 操作符和标点
    Token opToken = ScanOperator();
    if (opToken.type != TokenType::Unknown) {
        return opToken;
    }
    
    // 未知字符
    impl_->Advance();
    return Token(TokenType::Unknown, std::string(1, ch), 
                startLine, startColumn, startPosition);
}

Token Lexer::PeekToken() {
    return PeekToken(0);
}

Token Lexer::PeekToken(size_t ahead) {
    // 确保缓存中有足够的Token
    while (impl_->tokenCache_.size() <= ahead) {
        impl_->tokenCache_.push_back(NextToken());
    }
    return impl_->tokenCache_[ahead];
}

void Lexer::ConsumeToken() {
    NextToken();
}

bool Lexer::HasMoreTokens() const {
    return impl_->position_ < impl_->input_.length() || !impl_->tokenCache_.empty();
}

size_t Lexer::GetCurrentLine() const {
    return impl_->line_;
}

size_t Lexer::GetCurrentColumn() const {
    return impl_->column_;
}

size_t Lexer::GetCurrentPosition() const {
    return impl_->position_;
}

std::string Lexer::GetSourceName() const {
    return impl_->sourceName_;
}

std::vector<Token> Lexer::TokenizeAll() {
    std::vector<Token> tokens;
    Token token;
    
    do {
        token = NextToken();
        tokens.push_back(token);
    } while (token.type != TokenType::Eof);
    
    return tokens;
}

void Lexer::Reset() {
    impl_->position_ = 0;
    impl_->line_ = 1;
    impl_->column_ = 1;
    impl_->tokenCache_.clear();
}

void Lexer::DumpTokens() {
    size_t savedPos = impl_->position_;
    size_t savedLine = impl_->line_;
    size_t savedColumn = impl_->column_;
    
    Reset();
    std::cout << "=== Token列表 ===" << std::endl;
    
    Token token;
    do {
        token = NextToken();
        std::cout << token.ToString() << std::endl;
    } while (token.type != TokenType::Eof);
    
    impl_->position_ = savedPos;
    impl_->line_ = savedLine;
    impl_->column_ = savedColumn;
}

void Lexer::SkipWhitespace() {
    impl_->SkipWhile([this](char ch) { return IsWhitespace(ch); });
}

void Lexer::SkipComment() {
    if (impl_->Current() == '/' && impl_->Peek() == '/') {
        // 单行注释
        impl_->SkipWhile([](char ch) { return ch != '\n'; });
    } else if (impl_->Current() == '/' && impl_->Peek() == '*') {
        // 多行注释
        impl_->Advance();  // /
        impl_->Advance();  // *
        
        while (impl_->position_ < impl_->input_.length()) {
            if (impl_->Current() == '*' && impl_->Peek() == '/') {
                impl_->Advance();  // *
                impl_->Advance();  // /
                break;
            }
            impl_->Advance();
        }
    }
}

Token Lexer::ScanComment() {
    size_t startLine = impl_->line_;
    size_t startColumn = impl_->column_;
    size_t startPosition = impl_->position_;
    
    if (impl_->Current() == '/' && impl_->Peek() == '/') {
        // 单行注释
        impl_->Advance();  // /
        impl_->Advance();  // /
        std::string comment = "//" + impl_->ConsumeWhile([](char ch) { return ch != '\n'; });
        return Token(TokenType::SingleLineComment, comment, startLine, startColumn, startPosition);
    } else if (impl_->Current() == '/' && impl_->Peek() == '*') {
        // 多行注释
        std::string comment = "/*";
        impl_->Advance();  // /
        impl_->Advance();  // *
        
        while (impl_->position_ < impl_->input_.length()) {
            if (impl_->Current() == '*' && impl_->Peek() == '/') {
                comment += "*/";
                impl_->Advance();  // *
                impl_->Advance();  // /
                break;
            }
            comment += impl_->Current();
            impl_->Advance();
        }
        
        return Token(TokenType::MultiLineComment, comment, startLine, startColumn, startPosition);
    }
    
    return Token(TokenType::Unknown, "", startLine, startColumn, startPosition);
}

Token Lexer::ScanIdentifier() {
    size_t startLine = impl_->line_;
    size_t startColumn = impl_->column_;
    size_t startPosition = impl_->position_;
    
    std::string value = impl_->ConsumeWhile([this](char ch) {
        return IsIdentifierPart(ch);
    });
    
    // 检查是否为关键字
    TokenType type = TokenUtils::GetKeywordType(value);
    if (type != TokenType::Unknown) {
        return Token(type, value, startLine, startColumn, startPosition);
    }
    
    // 检查是否为HTML元素
    if (TokenUtils::IsHtmlElement(value)) {
        return Token(TokenType::HtmlElement, value, startLine, startColumn, startPosition);
    }
    
    // 普通标识符
    return Token(TokenType::Identifier, value, startLine, startColumn, startPosition);
}

Token Lexer::ScanNumber() {
    size_t startLine = impl_->line_;
    size_t startColumn = impl_->column_;
    size_t startPosition = impl_->position_;
    
    std::string value = impl_->ConsumeWhile([this](char ch) {
        return IsDigit(ch) || ch == '.';
    });
    
    return Token(TokenType::Number, value, startLine, startColumn, startPosition);
}

Token Lexer::ScanString(char quote) {
    size_t startLine = impl_->line_;
    size_t startColumn = impl_->column_;
    size_t startPosition = impl_->position_;
    
    impl_->Advance();  // 跳过开始引号
    
    std::string value;
    bool escaped = false;
    
    while (impl_->position_ < impl_->input_.length()) {
        char ch = impl_->Current();
        
        if (escaped) {
            value += ch;
            escaped = false;
        } else if (ch == '\\') {
            value += ch;
            escaped = true;
        } else if (ch == quote) {
            impl_->Advance();  // 跳过结束引号
            break;
        } else {
            value += ch;
        }
        
        impl_->Advance();
    }
    
    TokenType type = (quote == '"') ? TokenType::StringLiteral : TokenType::SingleQuoteLiteral;
    return Token(type, value, startLine, startColumn, startPosition);
}

Token Lexer::ScanUnquotedLiteral() {
    size_t startLine = impl_->line_;
    size_t startColumn = impl_->column_;
    size_t startPosition = impl_->position_;
    
    std::string value = impl_->ConsumeWhile([](char ch) {
        return ch != ';' && ch != ':' && ch != '{' && ch != '}' && 
               ch != '(' && ch != ')' && ch != '[' && ch != ']' &&
               ch != ',' && ch != '\n';
    });
    
    // 去除首尾空白
    size_t start = value.find_first_not_of(" \t");
    size_t end = value.find_last_not_of(" \t");
    if (start != std::string::npos && end != std::string::npos) {
        value = value.substr(start, end - start + 1);
    }
    
    return Token(TokenType::UnquotedLiteral, value, startLine, startColumn, startPosition);
}

Token Lexer::ScanOperator() {
    size_t startLine = impl_->line_;
    size_t startColumn = impl_->column_;
    size_t startPosition = impl_->position_;
    
    char ch = impl_->Current();
    TokenType type = TokenType::Unknown;
    std::string value(1, ch);
    
    switch (ch) {
        case '{': type = TokenType::LeftBrace; break;
        case '}': type = TokenType::RightBrace; break;
        case '(': type = TokenType::LeftParen; break;
        case ')': type = TokenType::RightParen; break;
        case '[': type = TokenType::LeftBracket; break;
        case ']': type = TokenType::RightBracket; break;
        case ';': type = TokenType::Semicolon; break;
        case ':': type = TokenType::Colon; break;
        case ',': type = TokenType::Comma; break;
        case '.': type = TokenType::Dot; break;
        case '=': type = TokenType::Equal; break;
        case '&': type = TokenType::Ampersand; break;
    }
    
    if (type != TokenType::Unknown) {
        impl_->Advance();
        return Token(type, value, startLine, startColumn, startPosition);
    }
    
    return Token(TokenType::Unknown, "", startLine, startColumn, startPosition);
}

Token Lexer::ScanSpecialMarker() {
    size_t startLine = impl_->line_;
    size_t startColumn = impl_->column_;
    size_t startPosition = impl_->position_;
    
    // 保存当前位置
    size_t savedPos = impl_->position_;
    size_t savedLine = impl_->line_;
    size_t savedColumn = impl_->column_;
    
    impl_->Advance();  // [
    
    std::string marker = "[";
    marker += impl_->ConsumeWhile([](char ch) {
        return std::isalpha(ch);
    });
    
    if (impl_->Current() == ']') {
        marker += ']';
        impl_->Advance();
        
        TokenType type = TokenUtils::GetSpecialMarkerType(marker);
        if (type != TokenType::Unknown) {
            return Token(type, marker, startLine, startColumn, startPosition);
        }
    }
    
    // 不是特殊标记，回退
    impl_->position_ = savedPos;
    impl_->line_ = savedLine;
    impl_->column_ = savedColumn;
    
    return Token(TokenType::Unknown, "", startLine, startColumn, startPosition);
}

Token Lexer::ScanAtMarker() {
    size_t startLine = impl_->line_;
    size_t startColumn = impl_->column_;
    size_t startPosition = impl_->position_;
    
    std::string marker = "@";
    impl_->Advance();  // @
    
    marker += impl_->ConsumeWhile([](char ch) {
        return std::isalpha(ch);
    });
    
    TokenType type = TokenUtils::GetAtMarkerType(marker);
    if (type != TokenType::Unknown) {
        return Token(type, marker, startLine, startColumn, startPosition);
    }
    
    // 未知的@标记，作为标识符
    return Token(TokenType::Identifier, marker, startLine, startColumn, startPosition);
}

bool Lexer::IsIdentifierStart(char ch) const {
    return std::isalpha(ch) || ch == '_';
}

bool Lexer::IsIdentifierPart(char ch) const {
    return std::isalnum(ch) || ch == '_';
}

bool Lexer::IsDigit(char ch) const {
    return std::isdigit(ch);
}

bool Lexer::IsWhitespace(char ch) const {
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

bool Lexer::IsOperator(char ch) const {
    return ch == '{' || ch == '}' || ch == '(' || ch == ')' ||
           ch == '[' || ch == ']' || ch == ';' || ch == ':' ||
           ch == ',' || ch == '.' || ch == '=' || ch == '&';
}

void Lexer::ReportError(const std::string& message) {
    if (errorCallback_) {
        errorCallback_(message, impl_->line_, impl_->column_);
    } else {
        LOG_ERROR(message + " at " + std::to_string(impl_->line_) + 
                 ":" + std::to_string(impl_->column_));
    }
}

void Lexer::ReportWarning(const std::string& message) {
    LOG_WARNING(message + " at " + std::to_string(impl_->line_) + 
               ":" + std::to_string(impl_->column_));
}

// LexerUtils实现
std::string LexerUtils::UnescapeString(const std::string& str) {
    std::string result;
    bool escaped = false;
    
    for (char ch : str) {
        if (escaped) {
            switch (ch) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                default: result += ch; break;
            }
            escaped = false;
        } else if (ch == '\\') {
            escaped = true;
        } else {
            result += ch;
        }
    }
    
    return result;
}

std::string LexerUtils::EscapeString(const std::string& str) {
    std::string result;
    
    for (char ch : str) {
        switch (ch) {
            case '\n': result += "\\n"; break;
            case '\t': result += "\\t"; break;
            case '\r': result += "\\r"; break;
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            default: result += ch; break;
        }
    }
    
    return result;
}

bool LexerUtils::IsValidIdentifier(const std::string& str) {
    if (str.empty()) return false;
    
    if (!std::isalpha(str[0]) && str[0] != '_') {
        return false;
    }
    
    for (size_t i = 1; i < str.length(); ++i) {
        if (!std::isalnum(str[i]) && str[i] != '_') {
            return false;
        }
    }
    
    return true;
}

bool LexerUtils::IsValidNumber(const std::string& str) {
    if (str.empty()) return false;
    
    bool hasDot = false;
    for (char ch : str) {
        if (ch == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (!std::isdigit(ch)) {
            return false;
        }
    }
    
    return true;
}

bool LexerUtils::IsValidString(const std::string& str) {
    // 简单检查，实际实现可能需要更复杂的验证
    return true;
}

std::vector<Token> LexerUtils::FilterComments(const std::vector<Token>& tokens) {
    std::vector<Token> result;
    
    for (const auto& token : tokens) {
        if (token.type != TokenType::SingleLineComment &&
            token.type != TokenType::MultiLineComment) {
            result.push_back(token);
        }
    }
    
    return result;
}

std::vector<Token> LexerUtils::FilterWhitespace(const std::vector<Token>& tokens) {
    // 这个实现假设空白字符不会生成Token
    return tokens;
}

std::string LexerUtils::TokensToString(const std::vector<Token>& tokens) {
    std::stringstream ss;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (i > 0) ss << "\n";
        ss << tokens[i].ToString();
    }
    
    return ss.str();
}

} // namespace CHTL