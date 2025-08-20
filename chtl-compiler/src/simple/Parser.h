#pragma once

#include "Token.h"
#include "ASTNode.h"
#include <vector>
#include <memory>
#include <string>

namespace chtl {
namespace simple {

/**
 * 语法分析器
 * 将 Token 流转换为 AST
 */
class Parser {
public:
    Parser();
    
    /**
     * 解析 Token 流，生成 AST
     */
    std::shared_ptr<ASTNode> Parse(const std::vector<Token>& tokens);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 解析方法
    std::shared_ptr<ASTNode> ParseContent();
    std::shared_ptr<ASTNode> ParseElement();
    std::shared_ptr<ASTNode> ParseText();
    std::shared_ptr<ASTNode> ParseStyle();
    std::shared_ptr<ASTNode> ParseScript();
    std::shared_ptr<ASTNode> ParseTemplate();
    std::shared_ptr<ASTNode> ParseCustomElement(const std::string& name);
    std::pair<std::string, std::string> ParseAttribute();
    
    // 辅助方法
    bool Match(TokenType type);
    bool Check(TokenType type);
    Token Advance();
    Token Peek();
    Token Previous();
    bool IsAtEnd();
    void ReportError(const std::string& message);
    void Synchronize();
    
    // 成员变量
    std::vector<Token> m_Tokens;
    size_t m_Current;
    std::vector<std::string> m_Errors;
};

} // namespace simple
} // namespace chtl