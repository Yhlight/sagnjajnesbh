#include "CHTL/Lexer/Lexer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <algorithm>

namespace CHTL {
namespace Compiler {

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
            // 跳过空白字符
            SkipWhitespace(state);
            if (state.IsEOF()) break;
            
            size_t startLine = state.line;
            size_t startColumn = state.column;
            
            // 尝试匹配各种token类型
            Token token(TokenType::ERROR_TOKEN, "", startLine, startColumn);
            
            if (TryMatchComment(state, token)) {
                if (!config.skipComments || token.type == TokenType::GENERATOR_COMMENT) {
                    tokens.push_back(token);
                }
            }
            else if (TryMatchKeyword(state, token) ||
                     TryMatchTypeMarker(state, token) ||
                     TryMatchOperator(state, token) ||
                     TryMatchString(state, token) ||
                     TryMatchNumber(state, token) ||
                     TryMatchIdentifier(state, token)) {
                tokens.push_back(token);
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
    
    bool TryMatchComment(LexState& state, Token& token) {
        size_t startLine = state.line;
        size_t startColumn = state.column;
        
        // 生成器注释 --
        if (state.Match("--")) {
            state.Skip(2);
            std::string content = "--";
            
            // 读取到行尾
            while (!state.IsEOF() && state.Peek() != '\n') {
                content += state.Advance();
            }
            
            token = Token(TokenType::GENERATOR_COMMENT, content, startLine, startColumn,
                         state.line, state.column);
            return true;
        }
        
        // 单行注释 //
        if (state.Match("//")) {
            state.Skip(2);
            std::string content = "//";
            
            while (!state.IsEOF() && state.Peek() != '\n') {
                content += state.Advance();
            }
            
            token = Token(TokenType::COMMENT, content, startLine, startColumn,
                         state.line, state.column);
            return true;
        }
        
        // 多行注释 /* */
        if (state.Match("/*")) {
            state.Skip(2);
            std::string content = "/*";
            
            while (!state.IsEOF()) {
                if (state.Match("*/")) {
                    content += "*/";
                    state.Skip(2);
                    break;
                }
                content += state.Advance();
            }
            
            token = Token(TokenType::MULTI_COMMENT, content, startLine, startColumn,
                         state.line, state.column);
            return true;
        }
        
        return false;
    }
    
    bool TryMatchKeyword(LexState& state, Token& token) {
        size_t startLine = state.line;
        size_t startColumn = state.column;
        
        // 匹配方括号关键字 [...]
        if (state.Peek() == '[') {
            size_t savePos = state.pos;
            state.Advance(); // skip [
            
            std::string keyword = "[";
            while (!state.IsEOF() && std::isalpha(state.Peek())) {
                keyword += state.Advance();
            }
            
            if (state.Peek() == ']') {
                keyword += state.Advance();
                
                TokenType type = TokenUtils::GetKeywordType(keyword);
                if (type != TokenType::IDENTIFIER) {
                    token = Token(type, keyword, startLine, startColumn, 
                                 state.line, state.column);
                    return true;
                }
            }
            
            // 回退
            state.pos = savePos;
            state.line = startLine;
            state.column = startColumn;
        }
        
        // 匹配普通关键字
        std::string word;
        size_t savePos = state.pos;
        
        // 特殊处理 "at top" 和 "at bottom"
        if (state.Match("at ")) {
            state.Skip(3);
            if (state.Match("top")) {
                state.Skip(3);
                token = Token(TokenType::KEYWORD_AT_TOP, "at top", 
                             startLine, startColumn, state.line, state.column);
                return true;
            }
            else if (state.Match("bottom")) {
                state.Skip(6);
                token = Token(TokenType::KEYWORD_AT_BOTTOM, "at bottom", 
                             startLine, startColumn, state.line, state.column);
                return true;
            }
            // 回退
            state.pos = savePos;
            state.line = startLine;
            state.column = startColumn;
        }
        
        // 普通标识符形式的关键字
        if (std::isalpha(state.Peek()) || state.Peek() == '_') {
            while (!state.IsEOF() && (std::isalnum(state.Peek()) || state.Peek() == '_')) {
                word += state.Advance();
            }
            
            TokenType type = TokenUtils::GetKeywordType(word);
            if (type != TokenType::IDENTIFIER) {
                token = Token(type, word, startLine, startColumn, 
                             state.line, state.column);
                return true;
            }
            
            // 回退，让标识符匹配器处理
            state.pos = savePos;
            state.line = startLine;
            state.column = startColumn;
        }
        
        return false;
    }
    
    bool TryMatchTypeMarker(LexState& state, Token& token) {
        if (state.Peek() != '@') return false;
        
        size_t startLine = state.line;
        size_t startColumn = state.column;
        
        state.Advance(); // skip @
        std::string marker = "@";
        
        // 读取类型名
        if (std::isalpha(state.Peek()) || state.Peek() == '_') {
            while (!state.IsEOF() && (std::isalnum(state.Peek()) || state.Peek() == '_')) {
                marker += state.Advance();
            }
            
            TokenType type = TokenUtils::GetKeywordType(marker);
            if (type != TokenType::IDENTIFIER) {
                token = Token(type, marker, startLine, startColumn, 
                             state.line, state.column);
                return true;
            }
        }
        
        // 不是已知的类型标记，作为自定义类型
        token = Token(TokenType::IDENTIFIER, marker, startLine, startColumn, 
                     state.line, state.column);
        return true;
    }
    
    bool TryMatchOperator(LexState& state, Token& token) {
        size_t startLine = state.line;
        size_t startColumn = state.column;
        char ch = state.Peek();
        
        TokenType type = TokenType::ERROR_TOKEN;
        std::string value;
        
        switch (ch) {
            case '{': type = TokenType::LEFT_BRACE; value = "{"; break;
            case '}': type = TokenType::RIGHT_BRACE; value = "}"; break;
            case '[': type = TokenType::LEFT_BRACKET; value = "["; break;
            case ']': type = TokenType::RIGHT_BRACKET; value = "]"; break;
            case '(': type = TokenType::LEFT_PAREN; value = "("; break;
            case ')': type = TokenType::RIGHT_PAREN; value = ")"; break;
            case ';': type = TokenType::SEMICOLON; value = ";"; break;
            case ',': type = TokenType::COMMA; value = ","; break;
            case ':': type = TokenType::COLON; value = ":"; break;
            case '=': type = TokenType::EQUALS; value = "="; break;
            case '.': type = TokenType::DOT; value = "."; break;
            case '/': type = TokenType::SLASH; value = "/"; break;
            case '*': type = TokenType::STAR; value = "*"; break;
            case '&': type = TokenType::AMPERSAND; value = "&"; break;
            default: return false;
        }
        
        state.Advance();
        token = Token(type, value, startLine, startColumn);
        return true;
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
        
        // 单位（如px, em, %等）
        if (std::isalpha(state.Peek()) || state.Peek() == '%') {
            while (!state.IsEOF() && (std::isalnum(state.Peek()) || state.Peek() == '%')) {
                value += state.Advance();
            }
        }
        
        token = Token(TokenType::NUMBER, value, startLine, startColumn,
                     state.line, state.column);
        return true;
    }
    
    bool TryMatchIdentifier(LexState& state, Token& token) {
        if (!std::isalpha(state.Peek()) && state.Peek() != '_') return false;
        
        size_t startLine = state.line;
        size_t startColumn = state.column;
        std::string value;
        
        // 标识符：字母或下划线开头，后跟字母、数字、下划线或连字符
        while (!state.IsEOF() && 
               (std::isalnum(state.Peek()) || state.Peek() == '_' || state.Peek() == '-')) {
            value += state.Advance();
        }
        
        // 检查是否是未引号的字面量（用于CSS属性值等）
        // 如果后面跟着冒号或分号，可能是属性值
        bool isUnquotedLiteral = false;
        if (!state.IsEOF()) {
            // 向前查看，跳过空白
            size_t lookAhead = 0;
            while (state.pos + lookAhead < state.source->length() && 
                   std::isspace(state.Peek(lookAhead))) {
                lookAhead++;
            }
            
            char nextChar = state.Peek(lookAhead);
            if (nextChar == ';' || nextChar == '}' || nextChar == ',') {
                // 可能是CSS属性值或其他无引号字面量
                // 继续读取直到遇到终止符
                while (!state.IsEOF()) {
                    char ch = state.Peek();
                    if (ch == ';' || ch == '}' || ch == ',' || ch == '\n') {
                        break;
                    }
                    value += state.Advance();
                }
                
                // 去除尾部空白
                while (!value.empty() && std::isspace(value.back())) {
                    value.pop_back();
                }
                
                isUnquotedLiteral = true;
            }
        }
        
        TokenType type = isUnquotedLiteral ? TokenType::UNQUOTED_LITERAL : TokenType::IDENTIFIER;
        token = Token(type, value, startLine, startColumn, state.line, state.column);
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
    std::cout << "=== CHTL Tokens ===" << std::endl;
    for (const auto& token : tokens) {
        std::cout << token.ToString() << std::endl;
    }
    std::cout << "总计: " << tokens.size() << " 个token" << std::endl;
    std::cout << "==================" << std::endl;
}

} // namespace Compiler
} // namespace CHTL