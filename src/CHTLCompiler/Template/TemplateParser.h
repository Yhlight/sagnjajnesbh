#pragma once
#include "../Parser/CHTLContext.h"
#include "../State/CHTLState.h"
#include "../AST/CHTLNodes.h"
#include "../Lexer/CHTLToken.h"
#include "TemplateManager.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace template_system {

// 模板解析状态
enum class TemplateParseState {
    EXPECTING_TEMPLATE_KEYWORD,    // 期望[Template]关键字
    EXPECTING_TYPE_SPECIFIER,      // 期望@Style/@Element/@Var
    EXPECTING_TEMPLATE_NAME,       // 期望模板名称
    PARSING_TEMPLATE_BODY,         // 解析模板体
    PARSING_STYLE_PROPERTIES,     // 解析样式属性
    PARSING_ELEMENT_CONTENT,       // 解析元素内容
    PARSING_VARIABLE_DEFINITIONS,  // 解析变量定义
    PARSING_INHERITANCE,           // 解析继承语句
    TEMPLATE_COMPLETE              // 模板解析完成
};

// 模板解析上下文
struct TemplateParseContext {
    TemplateParseState state;
    std::string templateType;      // Style, Element, Var
    std::string templateName;
    size_t braceDepth;
    bool isInInheritance;
    std::vector<std::string> inheritedTemplates;
    std::unordered_map<std::string, std::string> currentProperties;
    std::vector<std::unique_ptr<ast::ASTNode>> currentElements;
    
    TemplateParseContext() : state(TemplateParseState::EXPECTING_TEMPLATE_KEYWORD),
                            braceDepth(0), isInInheritance(false) {}
};

// 模板解析器
class TemplateParser {
public:
    TemplateParser();
    explicit TemplateParser(CHTLContext& context, StateManager& stateManager);
    ~TemplateParser();
    
    // 主解析接口
    std::unique_ptr<ast::TemplateNode> parseTemplate(const std::vector<Token>& tokens, size_t& position);
    
    // 分类型解析
    std::unique_ptr<ast::TemplateNode> parseStyleTemplate(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::TemplateNode> parseElementTemplate(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::TemplateNode> parseVariableTemplate(const std::vector<Token>& tokens, size_t& position);
    
    // 继承解析
    bool parseInheritanceStatement(const std::vector<Token>& tokens, size_t& position, 
                                  std::vector<std::string>& inheritedTemplates);
    bool parseExplicitInheritance(const std::vector<Token>& tokens, size_t& position,
                                 std::vector<std::string>& inheritedTemplates);
    bool parseCompositeInheritance(const std::vector<Token>& tokens, size_t& position,
                                  std::vector<std::string>& inheritedTemplates);
    
    // 模板使用解析
    bool parseTemplateUsage(const std::vector<Token>& tokens, size_t& position,
                           std::string& templateType, std::string& templateName);
    
    // 状态机集成
    bool updateParseState(TemplateParseState newState, const std::string& reason = "");
    TemplateParseState getCurrentParseState() const { return parseContext_.state; }
    
    // 上下文集成
    void enterTemplateContext(const std::string& templateType, const std::string& templateName);
    void exitTemplateContext();
    
    // 验证
    bool validateTemplateDefinition(const std::string& templateType, const std::string& templateName);
    bool validateTemplateUsage(const std::string& templateType, const std::string& templateName);
    bool validateInheritance(const std::string& templateType, const std::vector<std::string>& inherited);
    
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
    
    TemplateParseContext parseContext_;
    std::shared_ptr<TemplateManager> templateManager_;
    
    std::vector<std::string> errors_;
    bool debugMode_;
    
    // 内部解析方法
    bool parseTemplateHeader(const std::vector<Token>& tokens, size_t& position);
    bool parseTemplateBody(const std::vector<Token>& tokens, size_t& position);
    
    // 样式模板解析
    bool parseStyleProperties(const std::vector<Token>& tokens, size_t& position,
                             std::unordered_map<std::string, std::string>& properties);
    bool parseStyleProperty(const std::vector<Token>& tokens, size_t& position,
                           std::string& property, std::string& value);
    
    // 元素模板解析
    bool parseElementContent(const std::vector<Token>& tokens, size_t& position,
                            std::vector<std::unique_ptr<ast::ASTNode>>& elements);
    std::unique_ptr<ast::ElementNode> parseTemplateElement(const std::vector<Token>& tokens, size_t& position);
    
    // 变量模板解析
    bool parseVariableDefinitions(const std::vector<Token>& tokens, size_t& position,
                                 std::unordered_map<std::string, std::string>& variables);
    bool parseVariableDefinition(const std::vector<Token>& tokens, size_t& position,
                                 std::string& varName, std::string& varValue);
    
    // Token处理
    bool expectToken(const std::vector<Token>& tokens, size_t& position, TokenType expectedType,
                    const std::string& errorMessage = "");
    bool consumeToken(const std::vector<Token>& tokens, size_t& position, TokenType tokenType);
    Token getCurrentToken(const std::vector<Token>& tokens, size_t position) const;
    Token peekToken(const std::vector<Token>& tokens, size_t position, size_t offset = 1) const;
    bool isAtEnd(const std::vector<Token>& tokens, size_t position) const;
    
    // 错误处理
    void addError(const std::string& error);
    void addContextualError(const std::string& error, const Token& token);
    
    // 状态验证
    bool isValidTemplateState() const;
    bool canParseTemplateType(const std::string& templateType) const;
    
    // 工具方法
    std::string extractStringLiteral(const std::string& tokenValue);
    bool isValidIdentifier(const std::string& identifier);
    bool isValidTemplateType(const std::string& type);
};

} // namespace template_system
} // namespace chtl