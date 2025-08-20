#include "Lexer.h"
#include <cctype>
#include <sstream>

namespace chtl {
namespace simple {

Lexer::Lexer() : m_Current(0), m_Line(1), m_Column(1) {
}

void Lexer::SetInput(const std::string& input) {
    m_Input = input;
    m_Current = 0;
    m_Line = 1;
    m_Column = 1;
    m_Errors.clear();
}

std::vector<Token> Lexer::Tokenize() {
    std::vector<Token> tokens;
    
    while (!IsAtEnd()) {
        SkipWhitespace();
        
        if (IsAtEnd()) {
            break;
        }
        
        Token token = NextToken();
        if (token.type != TokenType::COMMENT && token.type != TokenType::ERROR) {
            tokens.push_back(token);
        }
    }
    
    tokens.push_back(Token(TokenType::EOF_TOKEN, "", m_Line, m_Column));
    return tokens;
}

Token Lexer::NextToken() {
    size_t startLine = m_Line;
    size_t startColumn = m_Column;
    
    char c = Peek();
    
    // 单字符标记
    switch (c) {
        case '{':
            Advance();
            return Token(TokenType::LBRACE, "{", startLine, startColumn);
        case '}':
            Advance();
            return Token(TokenType::RBRACE, "}", startLine, startColumn);
        case ':':
            Advance();
            return Token(TokenType::COLON, ":", startLine, startColumn);
        case ';':
            Advance();
            return Token(TokenType::SEMICOLON, ";", startLine, startColumn);
        case '"':
            return Token(TokenType::STRING, ReadString(), startLine, startColumn);
    }
    
    // 注释
    if (c == '/' && PeekNext() == '/') {
        SkipComment();
        return Token(TokenType::COMMENT, "", startLine, startColumn);
    } else if (c == '/' && PeekNext() == '*') {
        SkipComment();
        return Token(TokenType::COMMENT, "", startLine, startColumn);
    }
    
    // 标识符
    if (std::isalpha(c) || c == '_') {
        return Token(TokenType::IDENTIFIER, ReadIdentifier(), startLine, startColumn);
    }
    
    // 数字
    if (std::isdigit(c)) {
        return Token(TokenType::NUMBER, ReadNumber(), startLine, startColumn);
    }
    
    // 对于其他字符，作为单字符标识符处理
    std::string specialChar(1, c);
    Advance();
    return Token(TokenType::IDENTIFIER, specialChar, startLine, startColumn);
}

void Lexer::SkipWhitespace() {
    while (!IsAtEnd() && std::isspace(Peek())) {
        if (Peek() == '\n') {
            m_Line++;
            m_Column = 1;
        } else {
            m_Column++;
        }
        m_Current++;
    }
}

void Lexer::SkipComment() {
    if (Peek() == '/' && PeekNext() == '/') {
        // 单行注释
        Advance(); // /
        Advance(); // /
        while (!IsAtEnd() && Peek() != '\n') {
            Advance();
        }
    } else if (Peek() == '/' && PeekNext() == '*') {
        // 多行注释
        Advance(); // /
        Advance(); // *
        while (!IsAtEnd()) {
            if (Peek() == '*' && PeekNext() == '/') {
                Advance(); // *
                Advance(); // /
                break;
            }
            if (Peek() == '\n') {
                m_Line++;
                m_Column = 1;
            } else {
                m_Column++;
            }
            m_Current++;
        }
    }
}

std::string Lexer::ReadString() {
    std::string result;
    Advance(); // 跳过开始的引号
    
    while (!IsAtEnd() && Peek() != '"') {
        if (Peek() == '\\' && PeekNext() != '\0') {
            // 处理转义字符
            Advance();
            char escaped = Advance();
            switch (escaped) {
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                default: result += escaped; break;
            }
        } else {
            result += Advance();
        }
    }
    
    if (!IsAtEnd()) {
        Advance(); // 跳过结束的引号
    } else {
        ReportError("Unterminated string");
    }
    
    return result;
}

std::string Lexer::ReadIdentifier() {
    std::string result;
    while (!IsAtEnd() && 
           (std::isalnum(Peek()) || Peek() == '_' || Peek() == '-')) {
        result += Advance();
    }
    return result;
}

std::string Lexer::ReadNumber() {
    std::string result;
    while (!IsAtEnd() && 
           (std::isdigit(Peek()) || Peek() == '.')) {
        result += Advance();
    }
    return result;
}

char Lexer::Peek() const {
    if (IsAtEnd()) return '\0';
    return m_Input[m_Current];
}

char Lexer::PeekNext() const {
    if (m_Current + 1 >= m_Input.length()) return '\0';
    return m_Input[m_Current + 1];
}

char Lexer::Advance() {
    if (IsAtEnd()) return '\0';
    char c = m_Input[m_Current];
    m_Current++;
    if (c == '\n') {
        m_Line++;
        m_Column = 1;
    } else {
        m_Column++;
    }
    return c;
}

bool Lexer::IsAtEnd() const {
    return m_Current >= m_Input.length();
}

void Lexer::ReportError(const std::string& message) {
    std::stringstream error;
    error << "Lexer error at line " << m_Line << ", column " << m_Column << ": " << message;
    m_Errors.push_back(error.str());
}

} // namespace simple
} // namespace chtl