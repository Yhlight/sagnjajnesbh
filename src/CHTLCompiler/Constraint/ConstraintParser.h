#pragma once
#include "../Parser/CHTLContext.h"
#include "../State/CHTLState.h"
#include "../AST/CHTLNodes.h"
#include "../Lexer/CHTLToken.h"
#include "ConstraintManager.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace constraint_system {

// 约束解析状态
enum class ConstraintParseState {
    EXPECTING_EXCEPT_KEYWORD,      // 期望except关键字
    PARSING_CONSTRAINT_TARGETS,    // 解析约束目标
    PARSING_PRECISE_CONSTRAINT,    // 解析精确约束
    PARSING_TYPE_CONSTRAINT,       // 解析类型约束
    PARSING_GLOBAL_CONSTRAINT,     // 解析全局约束
    CONSTRAINT_COMPLETE            // 约束解析完成
};

// 约束解析上下文
struct ConstraintParseContext {
    ConstraintParseState state;
    ConstraintType constraintType;
    std::string currentScope;
    std::vector<std::string> targets;
    std::vector<ConstraintTargetType> targetTypes;
    bool isGlobalConstraint;
    
    ConstraintParseContext() : state(ConstraintParseState::EXPECTING_EXCEPT_KEYWORD),
                              constraintType(ConstraintType::PRECISE), isGlobalConstraint(false) {}
};

// 约束解析器
class ConstraintParser {
public:
    ConstraintParser();
    explicit ConstraintParser(CHTLContext& context, StateManager& stateManager);
    ~ConstraintParser();
    
    // 主解析接口
    std::unique_ptr<ast::ConstraintNode> parseConstraint(const std::vector<Token>& tokens, size_t& position);
    
    // 分类型解析
    std::unique_ptr<ast::ConstraintNode> parsePreciseConstraint(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::ConstraintNode> parseTypeConstraint(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::ConstraintNode> parseGlobalConstraint(const std::vector<Token>& tokens, size_t& position);
    
    // 约束目标解析
    bool parseConstraintTargets(const std::vector<Token>& tokens, size_t& position,
                               std::vector<std::string>& targets);
    bool parseConstraintTarget(const std::vector<Token>& tokens, size_t& position,
                              std::string& target, ConstraintTargetType& targetType);
    std::string parseConstraintTarget(const std::vector<Token>& tokens, size_t& position);
    
    // 约束类型识别
    ConstraintTargetType identifyTargetType(const std::string& target);
    ConstraintType determineConstraintType(const std::vector<std::string>& targets);
    
    // 目标解析辅助方法
    std::string parseTypedConstraintTarget(const std::vector<Token>& tokens, size_t& position);
    std::string parseTypeIdentifierTarget(const std::vector<Token>& tokens, size_t& position);
    std::string parseElementTarget(const std::vector<Token>& tokens, size_t& position);
    ConstraintTargetType parseTargetType(const std::string& target);
    bool isHTMLElement(const std::string& element);
    bool validateConstraintSyntax(const std::string& constraintStatement);
    std::vector<std::string> extractConstraintTargets(const std::string& constraintStatement);
    std::string getCurrentScope();
    void enterConstraintScope(const std::string& scope);
    void exitConstraintScope(const std::string& scope);
    bool applyConstraintToContext(const std::string& constraintStatement, const std::string& contextScope);
    
    // 约束验证
    bool validateConstraintInScope(const std::string& scope, const std::vector<std::string>& targets);
    bool validateGlobalConstraint(const std::string& namespace_, ConstraintTargetType targetType);
    
    // 约束应用
    bool applyConstraint(const ast::ConstraintNode& constraintNode);
    bool enforceConstraint(const std::string& scope, const std::string& element, const std::string& elementType);
    
    // 状态机集成
    bool updateParseState(ConstraintParseState newState, const std::string& reason = "");
    ConstraintParseState getCurrentParseState() const { return parseContext_.state; }
    
    // 上下文集成
    void enterConstraintContext(const std::string& scope);
    void exitConstraintContext();
    
    // 错误处理
    std::vector<std::string> getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }
    
    // 调试
    void setDebugMode(bool debug) { debugMode_ = debug; }
    bool isDebugMode() const { return debugMode_; }

private:
    CHTLContext* context_;
    StateManager* stateManager_;
    std::unique_ptr<CHTLContext> ownedContext_;
    std::unique_ptr<StateManager> ownedStateManager_;
    
    ConstraintParseContext parseContext_;
    std::shared_ptr<ConstraintManager> constraintManager_;
    
    std::vector<std::string> errors_;
    bool debugMode_;
    
    // 内部解析方法
    bool parseConstraintHeader(const std::vector<Token>& tokens, size_t& position);
    bool parseConstraintBody(const std::vector<Token>& tokens, size_t& position);
    
    // 目标类型解析
    bool parseFullQualifiedTarget(const std::vector<Token>& tokens, size_t& position,
                                 std::string& target, ConstraintTargetType& targetType);
    bool parseSimpleTarget(const std::vector<Token>& tokens, size_t& position,
                          std::string& target);
    
    // Token处理
    bool expectToken(const std::vector<Token>& tokens, size_t& position, TokenType expectedType,
                    const std::string& errorMessage = "");
    bool expectKeyword(const std::vector<Token>& tokens, size_t& position, const std::string& keyword);
    bool consumeToken(const std::vector<Token>& tokens, size_t& position, TokenType tokenType);
    Token getCurrentToken(const std::vector<Token>& tokens, size_t position) const;
    Token peekToken(const std::vector<Token>& tokens, size_t position, size_t offset = 1) const;
    bool isAtEnd(const std::vector<Token>& tokens, size_t position) const;
    
    // 错误处理
    void addError(const std::string& error);
    void addContextualError(const std::string& error, const Token& token);
    
    // 状态验证
    bool isValidConstraintState() const;
    bool canParseConstraintType(ConstraintType type) const;
    
    // 工具方法
    std::string extractStringLiteral(const std::string& tokenValue);
    bool isValidIdentifier(const std::string& identifier);
    bool isCustomType(const std::string& type);
    bool isTemplateType(const std::string& type);

};

} // namespace constraint_system
} // namespace chtl