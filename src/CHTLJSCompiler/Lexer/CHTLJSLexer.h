#pragma once
#include "CHTLToken.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {

/**
 * CHTL词法分析器
 * 专门处理CHTL语法的词法分析
 */
class CHTLLexer {
public:
    CHTLLexer();
    ~CHTLLexer() = default;
    
    // 设置输入源
    void setInput(const std::string& input);
    
    // 词法分析
    std::vector<Token> tokenize();
    Token nextToken();
    
    // 状态查询
    bool hasMoreTokens() const;
    size_t getCurrentPosition() const;
    size_t getCurrentLine() const;
    size_t getCurrentColumn() const;
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }
    
private:
    std::string input_;
    size_t position_;
    size_t line_;
    size_t column_;
    std::vector<std::string> errors_;
    
    // 内部方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipComment();
    
    // Token识别
    Token readString();
    Token readNumber();
    Token readIdentifier();
    Token readOperator();
    Token readKeyword(const std::string& word);
    
    // 辅助方法
    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    void addError(const std::string& message);
};

} // namespace chtl