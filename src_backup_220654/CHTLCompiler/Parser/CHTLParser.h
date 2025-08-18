#pragma once
#include "../Lexer/CHTLToken.h"
#include "CHTLContext.h"
#include "../AST/CHTLNodes.h"
#include "../State/CHTLState.h"
#include "../Template/TemplateParser.h"
#include "../Custom/CustomParser.h"
#include "../Origin/OriginParser.h"
#include "../Constraint/ConstraintParser.h"
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
    std::unique_ptr<ast::ASTNode> parseOrigin();
    std::unique_ptr<ast::ASTNode> parseConfiguration();
    std::unique_ptr<ast::ASTNode> parseNamespace();
    std::unique_ptr<ast::ASTNode> parseImport();
    std::unique_ptr<ast::ASTNode> parseConstraint();
    std::unique_ptr<ast::ASTNode> parseElement();
    std::unique_ptr<ast::ASTNode> parseSpecialization();
    std::unique_ptr<ast::ASTNode> parseIndexAccess();
    
    // 模板和自定义使用解析
    std::unique_ptr<ast::ASTNode> parseTemplateUsage();
    std::unique_ptr<ast::ASTNode> parseCustomUsage();
    std::unique_ptr<ast::ASTNode> parseOriginUsage();
    
    // 变量组解析
    std::unique_ptr<ast::ASTNode> parseVariableUsage();
    bool parseVariableCall(const std::vector<Token>& tokens, size_t& position,
                          std::string& templateName, std::string& variableName, std::string& value);
    
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
    
    // 状态管理
    std::unique_ptr<StateManager> state_manager_;
    
    // 专门解析器
    std::unique_ptr<template_system::TemplateParser> template_parser_;
    std::unique_ptr<custom_system::CustomParser> custom_parser_;
    std::unique_ptr<origin_system::OriginParser> origin_parser_;
    std::unique_ptr<constraint_system::ConstraintParser> constraint_parser_;
    
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