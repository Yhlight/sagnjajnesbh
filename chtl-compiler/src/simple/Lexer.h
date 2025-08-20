#pragma once

#include "Token.h"
#include <vector>
#include <string>

namespace chtl {
namespace simple {

/**
 * 词法分析器
 * 将源代码转换为 Token 流
 */
class Lexer {
public:
    Lexer();
    
    /**
     * 设置输入源代码
     */
    void SetInput(const std::string& input);
    
    /**
     * 执行词法分析
     */
    std::vector<Token> Tokenize();
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 词法分析方法
    Token NextToken();
    void SkipWhitespace();
    void SkipComment();
    std::string ReadString();
    std::string ReadIdentifier();
    std::string ReadNumber();
    
    // 辅助方法
    char Peek() const;
    char PeekNext() const;
    char Advance();
    bool IsAtEnd() const;
    void ReportError(const std::string& message);
    
    // 成员变量
    std::string m_Input;
    size_t m_Current;
    size_t m_Line;
    size_t m_Column;
    std::vector<std::string> m_Errors;
};

} // namespace simple
} // namespace chtl