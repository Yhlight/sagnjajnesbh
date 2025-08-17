#pragma once
#include "../Lexer/CHTLToken.h"
#include "CHTLContext.h"
#include "../AST/CHTLNodes.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace parser {

/**
 * CHTL语法解析器
 * 专门处理CHTL语法的语法分析
 */
class CHTLParser {
public:
    CHTLParser();
    explicit CHTLParser(CHTLContext& context);
    ~CHTLParser() = default;
    
    // 设置输入
    void setTokens(const std::vector<Token>& tokens);
    void setInput(const std::string& input);
    
    // 解析方法
    std::unique_ptr<ast::ASTNode> parseProgram();
    std::unique_ptr<ast::ASTNode> parseStatement();
    std::unique_ptr<ast::ASTNode> parseTemplate();
    std::unique_ptr<ast::ASTNode> parseCustom();
    std::unique_ptr<ast::ASTNode> parseElement();
    
    // 状态查询
    bool hasMoreTokens() const;
    Token getCurrentToken() const;
    Token peekToken(size_t offset = 1) const;
    
    // 错误处理
    std::vector<std::string> getErrors() const;
    void clearErrors();
    
    // 配置
    void setDebugMode(bool debug) { debug_mode_ = debug; }
    bool isDebugMode() const { return debug_mode_; }
    
private:
    std::vector<Token> tokens_;
    size_t current_token_;
    CHTLContext* context_;
    std::unique_ptr<CHTLContext> owned_context_;
    bool debug_mode_;
    std::vector<std::string> errors_;
    
    // 内部解析方法
    void advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    
    // 错误处理
    void addError(const std::string& message);
    void synchronize();
    
    // 辅助方法
    bool isAtEnd() const;
    std::string tokenTypeToString(TokenType type) const;
};

} // namespace parser
} // namespace chtl