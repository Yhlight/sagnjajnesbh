#pragma once
#include "../Parser/CHTLContext.h"
#include "../State/CHTLState.h"
#include "../AST/CHTLNodes.h"
#include "../Lexer/CHTLToken.h"
#include "CustomManager.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace custom_system {

// 自定义解析状态
enum class CustomParseState {
    EXPECTING_CUSTOM_KEYWORD,      // 期望[Custom]关键字
    EXPECTING_TYPE_SPECIFIER,      // 期望@Style/@Element/@Var
    EXPECTING_CUSTOM_NAME,         // 期望自定义名称
    PARSING_CUSTOM_BODY,           // 解析自定义体
    PARSING_NO_VALUE_PROPERTIES,   // 解析无值属性
    PARSING_SPECIALIZATION,        // 解析特例化操作
    PARSING_INHERITANCE,           // 解析继承语句
    PARSING_USAGE_CONTEXT,         // 解析使用上下文
    CUSTOM_COMPLETE                // 自定义解析完成
};

// 自定义解析上下文
struct CustomParseContext {
    CustomParseState state;
    std::string customType;        // Style, Element, Var
    std::string customName;
    size_t braceDepth;
    bool isInSpecialization;
    bool isInUsageContext;
    std::vector<std::string> inheritedTemplates;
    std::vector<std::string> inheritedCustoms;
    std::unordered_map<std::string, std::string> properties;
    std::unordered_set<std::string> noValueProperties;
    std::vector<SpecializationOperation> specializations;
    std::vector<std::unique_ptr<ast::ASTNode>> elements;
    
    CustomParseContext() : state(CustomParseState::EXPECTING_CUSTOM_KEYWORD),
                          braceDepth(0), isInSpecialization(false), isInUsageContext(false) {}
};

// 自定义解析器
class CustomParser {
public:
    CustomParser();
    explicit CustomParser(CHTLContext& context, StateManager& stateManager);
    ~CustomParser();
    
    // 主解析接口
    std::unique_ptr<ast::CustomNode> parseCustom(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::CustomNode> parseCustomUsage(const std::vector<Token>& tokens, size_t& position);
    
    // 分类型解析
    std::unique_ptr<ast::CustomNode> parseCustomStyle(const std::vector<Token>& tokens, size_t& position, 
                                                      std::unique_ptr<ast::CustomNode> customNode);
    std::unique_ptr<ast::CustomNode> parseCustomElement(const std::vector<Token>& tokens, size_t& position, 
                                                       std::unique_ptr<ast::CustomNode> customNode);
    std::unique_ptr<ast::CustomNode> parseCustomVariable(const std::vector<Token>& tokens, size_t& position, 
                                                        std::unique_ptr<ast::CustomNode> customNode);
    
    // 特例化操作解析
    bool parseSpecializationOperation(const std::vector<Token>& tokens, size_t& position,
                                     SpecializationOperation& operation);
    bool parseDeleteOperation(const std::vector<Token>& tokens, size_t& position,
                             SpecializationOperation& operation);
    bool parseInsertOperation(const std::vector<Token>& tokens, size_t& position,
                             SpecializationOperation& operation);
    bool parseReplaceOperation(const std::vector<Token>& tokens, size_t& position,
                              SpecializationOperation& operation);
    bool parseInheritOperation(const std::vector<Token>& tokens, size_t& position,
                              SpecializationOperation& operation);
    
    // 无值样式组解析
    bool parseNoValueStyleGroup(const std::vector<Token>& tokens, size_t& position,
                               std::unordered_set<std::string>& noValueProperties);
    
    // 索引访问解析
    std::unique_ptr<ast::IndexAccessNode> parseIndexAccess(const std::vector<Token>& tokens, size_t& position);
    
    // 特例化解析
    std::unique_ptr<ast::SpecializationNode> parseSpecialization(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::SpecializationNode> parseInheritSpecialization(const std::vector<Token>& tokens, size_t& position,
                                                                        std::unique_ptr<ast::SpecializationNode> node);
    std::unique_ptr<ast::SpecializationNode> parseDeleteSpecialization(const std::vector<Token>& tokens, size_t& position,
                                                                       std::unique_ptr<ast::SpecializationNode> node);
    std::unique_ptr<ast::SpecializationNode> parseInsertSpecialization(const std::vector<Token>& tokens, size_t& position,
                                                                       std::unique_ptr<ast::SpecializationNode> node);
    std::unique_ptr<ast::SpecializationNode> parseReplaceSpecialization(const std::vector<Token>& tokens, size_t& position,
                                                                        std::unique_ptr<ast::SpecializationNode> node);
    
    // 继承解析 (重载版本以支持不同的自定义类型)
    bool parseCustomInheritance(const std::vector<Token>& tokens, size_t& position,
                               std::vector<std::string>& inheritedItems);
    bool parseCustomInheritance(const std::vector<Token>& tokens, size_t& position,
                               CustomStyle& customStyle);
    bool parseCustomInheritance(const std::vector<Token>& tokens, size_t& position,
                               CustomElement& customElement);
    bool parseCustomInheritance(const std::vector<Token>& tokens, size_t& position,
                               CustomVariable& customVariable);
    
    // 自定义使用解析
    bool parseCustomUsageWithSpecialization(const std::vector<Token>& tokens, size_t& position,
                                           std::string& customType, std::string& customName,
                                           std::vector<SpecializationOperation>& specializations);
    
    // 状态机集成
    bool updateParseState(CustomParseState newState, const std::string& reason = "");
    CustomParseState getCurrentParseState() const { return parseContext_.state; }
    
    // 上下文集成
    void enterCustomContext(const std::string& customType, const std::string& customName);
    void exitCustomContext();
    void enterSpecializationContext();
    void exitSpecializationContext();
    
    // 验证
    bool validateCustomDefinition(const std::string& customType, const std::string& customName);
    bool validateCustomUsage(const std::string& customType, const std::string& customName);
    bool validateSpecialization(const SpecializationOperation& operation);
    bool validateNoValueProperties(const std::unordered_set<std::string>& properties);
    
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
    
    CustomParseContext parseContext_;
    std::shared_ptr<CustomManager> customManager_;
    
    std::vector<std::string> errors_;
    bool debugMode_;
    
    // 内部解析方法
    bool parseCustomHeader(const std::vector<Token>& tokens, size_t& position);
    bool parseCustomBody(const std::vector<Token>& tokens, size_t& position);
    
    // 特例化操作解析辅助
    bool parsePositionSpecifier(const std::vector<Token>& tokens, size_t& position,
                               std::string& position_spec);
    bool parseTargetSelector(const std::vector<Token>& tokens, size_t& position,
                            std::string& selector);
    bool parseSpecializationContent(const std::vector<Token>& tokens, size_t& position,
                                   std::vector<std::unique_ptr<ast::ASTNode>>& content);
    
    // Token处理（复用TemplateParser的方法）
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
    bool isValidCustomState() const;
    bool canParseCustomType(const std::string& customType) const;
    
    // HTML元素解析
    std::unique_ptr<ast::ASTNode> parseHTMLElement(const std::vector<Token>& tokens, size_t& position);
    
    // 工具方法
    std::string extractStringLiteral(const std::string& tokenValue);
    bool isValidIdentifier(const std::string& identifier);
    bool isValidCustomType(const std::string& type);
    bool isValidPositionSpecifier(const std::string& position);
};

} // namespace custom_system
} // namespace chtl