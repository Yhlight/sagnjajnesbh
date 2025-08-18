#pragma once
#include "../Parser/CHTLContext.h"
#include "../State/CHTLState.h"
#include "../AST/CHTLNodes.h"
#include "../Lexer/CHTLToken.h"
#include "OriginManager.h"
#include "../../Shared/AST/OriginEmbedDetector.h"
#include <string>
#include <vector>
#include <memory>

namespace chtl {
namespace origin_system {

// 原始嵌入解析状态
enum class OriginParseState {
    EXPECTING_ORIGIN_KEYWORD,      // 期望[Origin]关键字
    EXPECTING_TYPE_SPECIFIER,      // 期望@Html/@Style/@JavaScript等
    EXPECTING_ORIGIN_NAME,         // 期望原始嵌入名称（可选）
    PARSING_ORIGIN_BODY,           // 解析原始嵌入体
    PARSING_ORIGIN_CONTENT,        // 解析原始内容
    PARSING_ORIGIN_REFERENCE,      // 解析原始嵌入引用
    ORIGIN_COMPLETE                // 原始嵌入解析完成
};

// 原始嵌入解析上下文
struct OriginParseContext {
    OriginParseState state;
    std::string originType;        // Html, Style, JavaScript, Vue等
    std::string originName;        // 带名原始嵌入的名称
    std::string originContent;     // 原始内容
    bool isNamed;                  // 是否为带名原始嵌入
    bool isReference;              // 是否为引用
    bool isCustomType;             // 是否为自定义类型
    size_t contentStart;           // 内容开始位置
    size_t contentEnd;             // 内容结束位置
    
    OriginParseContext() : state(OriginParseState::EXPECTING_ORIGIN_KEYWORD),
                          isNamed(false), isReference(false), isCustomType(false),
                          contentStart(0), contentEnd(0) {}
};

// 原始嵌入解析器
class OriginParser {
public:
    OriginParser();
    explicit OriginParser(CHTLContext& context, StateManager& stateManager);
    ~OriginParser();
    
    // 主解析接口
    std::unique_ptr<ast::OriginNode> parseOrigin(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::OriginNode> parseOriginReference(const std::vector<Token>& tokens, size_t& position);
    
    // 分类型解析
    std::unique_ptr<ast::OriginNode> parseHTMLOrigin(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::OriginNode> parseStyleOrigin(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::OriginNode> parseJavaScriptOrigin(const std::vector<Token>& tokens, size_t& position);
    std::unique_ptr<ast::OriginNode> parseCustomTypeOrigin(const std::vector<Token>& tokens, size_t& position);
    
    // 带名原始嵌入解析
    std::unique_ptr<ast::OriginNode> parseNamedOrigin(const std::vector<Token>& tokens, size_t& position);
    bool parseOriginName(const std::vector<Token>& tokens, size_t& position, std::string& name);
    
    // 原始内容解析
    bool parseOriginContent(const std::vector<Token>& tokens, size_t& position, std::string& content);
    bool extractRawContent(const std::vector<Token>& tokens, size_t startPos, size_t endPos, std::string& content);
    
    // 自定义类型处理
    bool registerCustomOriginType(const std::string& typeName);
    bool isRegisteredCustomType(const std::string& typeName);
    std::vector<std::string> getRegisteredCustomTypes();
    
    // 原始嵌入使用
    bool resolveOriginReference(const std::string& typeName, const std::string& name, std::string& content);
    
    // 与OriginEmbedDetector集成
    bool integrateWithDetector(const std::string& sourceCode);
    std::vector<ast::OriginEmbedBlock> detectOriginBlocks(const std::string& sourceCode);
    
    // 状态机集成
    bool updateParseState(OriginParseState newState, const std::string& reason = "");
    OriginParseState getCurrentParseState() const { return parseContext_.state; }
    
    // 上下文集成
    void enterOriginContext(const std::string& originType, const std::string& originName = "");
    void exitOriginContext();
    
    // 验证
    bool validateOriginDefinition(const std::string& originType, const std::string& originName = "");
    bool validateOriginUsage(const std::string& originType, const std::string& originName = "");
    bool validateOriginContent(const std::string& content, const std::string& originType);
    bool validateCustomOriginType(const std::string& typeName);
    
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
    
    OriginParseContext parseContext_;
    std::shared_ptr<OriginManager> originManager_;
    std::unique_ptr<ast::OriginEmbedDetector> originDetector_;
    
    std::vector<std::string> errors_;
    bool debugMode_;
    
    // 内部解析方法
    bool parseOriginHeader(const std::vector<Token>& tokens, size_t& position);
    bool parseOriginBody(const std::vector<Token>& tokens, size_t& position);
    
    // 原始类型处理
    OriginType parseOriginTypeFromString(const std::string& typeString);
    std::string formatOriginType(OriginType type);
    bool isStandardOriginType(const std::string& typeName);
    
    // 内容验证
    bool validateHTMLContent(const std::string& content);
    bool validateStyleContent(const std::string& content);
    bool validateJavaScriptContent(const std::string& content);
    bool validateCustomContent(const std::string& content, const std::string& typeName);
    
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
    bool isValidOriginState() const;
    bool canParseOriginType(const std::string& originType) const;
    
    // 工具方法
    std::string extractStringLiteral(const std::string& tokenValue);
    bool isValidIdentifier(const std::string& identifier);
    bool isValidOriginType(const std::string& type);
    std::string normalizeOriginTypeName(const std::string& typeName);
};

} // namespace origin_system
} // namespace chtl