#pragma once
#include "CHTLJSToken.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace chtljs {

/**
 * CHTL JS词法分析器
 * 专门处理CHTL JS语法的词法分析
 */
class CHTLJSLexer {
public:
    CHTLJSLexer();
    ~CHTLJSLexer() = default;
    
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
    
    // 上下文管理（用于无修饰字面量识别）
    void setContext(const std::string& context) { current_context_ = context; }
    std::string getContext() const { return current_context_; }
    
private:
    std::string input_;
    size_t position_;
    size_t line_;
    size_t column_;
    std::string current_context_;  // 当前上下文（text, attribute, script等）
    std::vector<std::string> errors_;
    
    // 内部方法
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    
    // Token识别
    Token readString();
    Token readNumber();
    Token readIdentifier();
    Token readOperator();
    Token readCHTLJSSelector(); // {{...}}
    Token readUndecoratedLiteral(); // 无修饰字面量
    Token readKeyword(const std::string& word);
    
    // 辅助方法
    bool isAtEnd() const;
    bool isAlpha(char c) const;
    bool isDigit(char c) const;
    bool isAlphaNumeric(char c) const;
    void addError(const std::string& message);
    
    // 无修饰字面量识别辅助
    bool canBeUndecoratedLiteral() const;
    bool isValidUndecoratedChar(char c) const;
    bool isUndecoratedContext() const;
};

} // namespace chtljs
} // namespace chtl