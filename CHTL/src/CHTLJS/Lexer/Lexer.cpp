#include "CHTLJS/Lexer/Lexer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <algorithm>

namespace CHTL {
namespace JSCompiler {

class Lexer::Impl {
public:
    LexerConfig config;
    std::string lastError;
    
    // 词法分析状态
    struct LexState {
        const std::string* source;
        size_t pos = 0;
        size_t line = 1;
        size_t column = 1;
        bool inSelector = false;  // 是否在{{}}内
        
        bool IsEOF() const { return pos >= source->length(); }
        
        char Peek(size_t offset = 0) const {
            size_t p = pos + offset;
            if (p >= source->length()) return '\0';
            return (*source)[p];
        }
        
        char Advance() {
            if (IsEOF()) return '\0';
            char ch = (*source)[pos++];
            if (ch == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            return ch;
        }
        
        bool Match(const std::string& text) const {
            if (pos + text.length() > source->length()) return false;
            return source->substr(pos, text.length()) == text;
        }
        
        void Skip(size_t n) {
            for (size_t i = 0; i < n; ++i) {
                Advance();
            }
        }
    };
    
    explicit Impl(const LexerConfig& cfg) : config(cfg) {}
    
    std::vector<Token> Tokenize(const std::string& source) {
        std::vector<Token> tokens;
        LexState state;
        state.source = &source;
        
        while (!state.IsEOF()) {
            // 跳过空白字符（除非在选择器内）
            if (!state.inSelector) {
                SkipWhitespace(state);
                if (state.IsEOF()) break;
            }
            
            size_t startLine = state.line;
            size_t startColumn = state.column;
            
            Token token(TokenType::ERROR_TOKEN, "", startLine, startColumn);
            
            // 尝试匹配各种token类型
            if (TryMatchSelector(state, token) ||
                TryMatchOperator(state, token) ||
                TryMatchKeyword(state, token) ||
                TryMatchString(state, token) ||
                TryMatchNumber(state, token) ||
                TryMatchIdentifier(state, token)) {
                tokens.push_back(token);
            }
            else if (config.preserveJSCode) {
                // 收集JS代码片段
                std::string jsCode;
                while (!state.IsEOF() && !IsSpecialChar(state.Peek())) {
                    jsCode += state.Advance();
                }
                
                if (!jsCode.empty()) {
                    token = Token(TokenType::JS_CODE, jsCode, startLine, startColumn,
                                 state.line, state.column);
                    tokens.push_back(token);
                }
            }
            else {
                // 未知字符
                std::string value(1, state.Advance());
                token = Token(TokenType::ERROR_TOKEN, value, startLine, startColumn);
                tokens.push_back(token);
                
                lastError = "意外的字符: '" + value + "' 在 " + 
                           std::to_string(startLine) + ":" + std::to_string(startColumn);
            }
        }
        
        // 添加EOF token
        tokens.emplace_back(TokenType::EOF_TOKEN, "", state.line, state.column);
        
        return tokens;
    }
    
private:
    void SkipWhitespace(LexState& state) {
        while (!state.IsEOF() && std::isspace(state.Peek())) {
            state.Advance();
        }
    }
    
    bool IsSpecialChar(char ch) {
        return ch == '{' || ch == '}' || ch == '(' || ch == ')' ||
               ch == '[' || ch == ']' || ch == '.' || ch == '-' ||
               ch == ',' || ch == ':' || ch == ';' || ch == '=' ||
               ch == '"' || ch == '\'' || ch == '#';
    }
    
    bool TryMatchSelector(LexState& state, Token& token) {
        size_t startLine = state.line;
        size_t startColumn = state.column;
        
        // 开始选择器 {{
        if (state.Match("{{")) {
            state.Skip(2);
            state.inSelector = true;
            token = Token(TokenType::SELECTOR_START, "{{", startLine, startColumn);
            return true;
        }
        
        // 结束选择器 }}
        if (state.Match("}}")) {
            state.Skip(2);
            state.inSelector = false;
            token = Token(TokenType::SELECTOR_END, "}}", startLine, startColumn);
            return true;
        }
        
        // 在选择器内的特殊字符
        if (state.inSelector) {
            if (state.Peek() == '.' && !std::isdigit(state.Peek(1))) {
                state.Advance();
                token = Token(TokenType::CLASS_SELECTOR, ".", startLine, startColumn);
                return true;
            }
            if (state.Peek() == '#') {
                state.Advance();
                token = Token(TokenType::ID_SELECTOR, "#", startLine, startColumn);
                return true;
            }
        }
        
        return false;
    }
    
    bool TryMatchOperator(LexState& state, Token& token) {
        size_t startLine = state.line;
        size_t startColumn = state.column;
        
        // 箭头操作符 ->
        if (state.Match("->")) {
            state.Skip(2);
            token = Token(TokenType::ARROW, "->", startLine, startColumn);
            return true;
        }
        
        char ch = state.Peek();
        TokenType type = TokenType::ERROR_TOKEN;
        std::string value;
        
        switch (ch) {
            case '.': type = TokenType::DOT; value = "."; break;
            case '[': type = TokenType::LEFT_BRACKET; value = "["; break;
            case ']': type = TokenType::RIGHT_BRACKET; value = "]"; break;
            case '(': type = TokenType::LEFT_PAREN; value = "("; break;
            case ')': type = TokenType::RIGHT_PAREN; value = ")"; break;
            case '{': type = TokenType::LEFT_BRACE; value = "{"; break;
            case '}': type = TokenType::RIGHT_BRACE; value = "}"; break;
            case ',': type = TokenType::COMMA; value = ","; break;
            case ':': type = TokenType::COLON; value = ":"; break;
            case ';': type = TokenType::SEMICOLON; value = ";"; break;
            case '=': type = TokenType::EQUALS; value = "="; break;
            default: return false;
        }
        
        state.Advance();
        token = Token(type, value, startLine, startColumn);
        return true;
    }
    
    bool TryMatchKeyword(LexState& state, Token& token) {
        if (!std::isalpha(state.Peek()) && state.Peek() != '_') return false;
        
        size_t startLine = state.line;
        size_t startColumn = state.column;
        size_t savePos = state.pos;
        
        std::string word;
        while (!state.IsEOF() && (std::isalnum(state.Peek()) || state.Peek() == '_')) {
            word += state.Advance();
        }
        
        // 特殊处理iNeverAway（大小写敏感）
        if (word == "iNeverAway") {
            token = Token(TokenType::KEYWORD_INEVERAWAY, word, startLine, startColumn);
            return true;
        }
        
        TokenType type = TokenUtils::GetKeywordType(word);
        if (type != TokenType::IDENTIFIER) {
            token = Token(type, word, startLine, startColumn);
            return true;
        }
        
        // 回退，让标识符匹配器处理
        state.pos = savePos;
        state.line = startLine;
        state.column = startColumn;
        return false;
    }
    
    bool TryMatchString(LexState& state, Token& token) {
        char quote = state.Peek();
        if (quote != '"' && quote != '\'') return false;
        
        size_t startLine = state.line;
        size_t startColumn = state.column;
        
        state.Advance(); // skip quote
        std::string value;
        
        while (!state.IsEOF()) {
            char ch = state.Peek();
            
            if (ch == quote) {
                state.Advance();
                token = Token(TokenType::STRING_LITERAL, value, startLine, startColumn,
                             state.line, state.column);
                return true;
            }
            
            if (ch == '\\') {
                state.Advance();
                if (!state.IsEOF()) {
                    char escaped = state.Advance();
                    switch (escaped) {
                        case 'n': value += '\n'; break;
                        case 'r': value += '\r'; break;
                        case 't': value += '\t'; break;
                        case '\\': value += '\\'; break;
                        case '"': value += '"'; break;
                        case '\'': value += '\''; break;
                        default: value += escaped; break;
                    }
                }
            } else {
                value += state.Advance();
            }
        }
        
        // 未闭合的字符串
        lastError = "未闭合的字符串字面量";
        token = Token(TokenType::ERROR_TOKEN, value, startLine, startColumn,
                     state.line, state.column);
        return true;
    }
    
    bool TryMatchNumber(LexState& state, Token& token) {
        if (!std::isdigit(state.Peek())) return false;
        
        size_t startLine = state.line;
        size_t startColumn = state.column;
        std::string value;
        
        // 整数部分
        while (!state.IsEOF() && std::isdigit(state.Peek())) {
            value += state.Advance();
        }
        
        // 小数部分
        if (state.Peek() == '.' && std::isdigit(state.Peek(1))) {
            value += state.Advance(); // .
            while (!state.IsEOF() && std::isdigit(state.Peek())) {
                value += state.Advance();
            }
        }
        
        // 科学计数法
        if ((state.Peek() == 'e' || state.Peek() == 'E') &&
            (std::isdigit(state.Peek(1)) || 
             ((state.Peek(1) == '+' || state.Peek(1) == '-') && std::isdigit(state.Peek(2))))) {
            value += state.Advance(); // e/E
            if (state.Peek() == '+' || state.Peek() == '-') {
                value += state.Advance();
            }
            while (!state.IsEOF() && std::isdigit(state.Peek())) {
                value += state.Advance();
            }
        }
        
        token = Token(TokenType::NUMBER_LITERAL, value, startLine, startColumn,
                     state.line, state.column);
        return true;
    }
    
    bool TryMatchIdentifier(LexState& state, Token& token) {
        if (!std::isalpha(state.Peek()) && state.Peek() != '_' && state.Peek() != '$') {
            return false;
        }
        
        size_t startLine = state.line;
        size_t startColumn = state.column;
        std::string value;
        
        // JavaScript标识符规则
        while (!state.IsEOF() && 
               (std::isalnum(state.Peek()) || state.Peek() == '_' || state.Peek() == '$')) {
            value += state.Advance();
        }
        
        token = Token(TokenType::IDENTIFIER, value, startLine, startColumn,
                     state.line, state.column);
        return true;
    }
};

// Lexer 公共接口实现
Lexer::Lexer(const LexerConfig& config) : pImpl(std::make_unique<Impl>(config)) {}
Lexer::~Lexer() = default;

std::vector<Token> Lexer::Tokenize(const std::string& source) {
    return pImpl->Tokenize(source);
}

std::vector<Token> Lexer::TokenizeFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        pImpl->lastError = "无法打开文件: " + filePath;
        return {};
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return Tokenize(buffer.str());
}

std::string Lexer::GetLastError() const {
    return pImpl->lastError;
}

bool Lexer::HasError() const {
    return !pImpl->lastError.empty();
}

void Lexer::DumpTokens(const std::vector<Token>& tokens) const {
    std::cout << "=== CHTL JS Tokens ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << token.ToString() << std::endl;
    }
    std::cout << "总计: " << tokens.size() << " 个token" << std::endl;
    std::cout << "==================" << std::endl;
}

} // namespace JSCompiler
} // namespace CHTL