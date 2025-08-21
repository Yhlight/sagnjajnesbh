#ifndef CHTL_PARSER_PARSER_H
#define CHTL_PARSER_PARSER_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "CHTL/Token.h"
#include "CHTL/Lexer.h"
#include "CHTL/GlobalMap.h"
#include "CHTL/State.h"
#include "CHTL/Context.h"
#include "CHTL/AST/ASTNode.h"
#include "CHTL/AST/TemplateNodes.h"

namespace CHTL {

// 解析器配置
struct ParserConfig {
    bool strictMode = false;           // 严格模式
    bool debugMode = false;            // 调试模式
    bool recoverFromErrors = true;     // 错误恢复
    bool validateSemantics = true;     // 语义验证
};

// 解析错误信息
struct ParseError {
    std::string message;
    size_t line;
    size_t column;
    std::string filename;
    TokenType expectedToken;
    TokenType actualToken;
};

// CHTL解析器
class Parser {
public:
    Parser();
    explicit Parser(const ParserConfig& config);
    ~Parser();
    
    // 解析接口
    std::shared_ptr<AST::ProgramNode> Parse(const std::string& input, const std::string& filename = "<input>");
    std::shared_ptr<AST::ProgramNode> ParseFile(const std::string& filename);
    
    // 配置
    void SetConfig(const ParserConfig& config) { config_ = config; }
    const ParserConfig& GetConfig() const { return config_; }
    
    // 错误处理
    using ErrorCallback = std::function<void(const ParseError& error)>;
    void SetErrorCallback(ErrorCallback callback) { errorCallback_ = callback; }
    const std::vector<ParseError>& GetErrors() const { return errors_; }
    bool HasErrors() const { return !errors_.empty(); }
    void ClearErrors() { errors_.clear(); }
    
    // 符号表访问
    std::shared_ptr<GlobalMap> GetGlobalMap() const { return globalMap_; }
    
private:
    ParserConfig config_;
    ErrorCallback errorCallback_;
    std::vector<ParseError> errors_;
    
    // 核心组件
    std::unique_ptr<Lexer> lexer_;
    std::shared_ptr<GlobalMap> globalMap_;
    std::shared_ptr<StateMachine> stateMachine_;
    std::shared_ptr<ContextManager> contextManager_;
    
    // 当前解析状态
    Token currentToken_;
    Token previousToken_;
    std::string currentFilename_;
    
    // 解析方法
    std::shared_ptr<AST::ProgramNode> ParseProgram();
    
    // 顶层解析
    std::shared_ptr<AST::ASTNode> ParseTopLevelStatement();
    std::shared_ptr<AST::ASTNode> ParseElement();
    std::shared_ptr<AST::ASTNode> ParseTemplateDeclaration();
    std::shared_ptr<AST::ASTNode> ParseCustomDeclaration();
    std::shared_ptr<AST::ASTNode> ParseOriginBlock();
    std::shared_ptr<AST::ASTNode> ParseImportStatement();
    std::shared_ptr<AST::ASTNode> ParseNamespaceDeclaration();
    std::shared_ptr<AST::ASTNode> ParseConfigurationBlock();
    
    // 元素解析
    std::shared_ptr<AST::ElementNode> ParseElementNode(const std::string& tagName);
    void ParseElementContent(std::shared_ptr<AST::ElementNode> element);
    void ParseAttributes(std::shared_ptr<AST::ElementNode> element);
    std::shared_ptr<AST::AttributeNode> ParseAttribute();
    
    // 文本解析
    std::shared_ptr<AST::TextBlockNode> ParseTextBlock();
    
    // 样式解析
    std::shared_ptr<AST::StyleBlockNode> ParseStyleBlock(bool isLocal = false);
    std::shared_ptr<AST::ASTNode> ParseStyleRule();
    std::shared_ptr<AST::ASTNode> ParseStyleSelector();
    std::shared_ptr<AST::ASTNode> ParseStyleProperty();
    
    // 脚本解析
    std::shared_ptr<AST::ScriptBlockNode> ParseScriptBlock(bool isLocal = false);
    
    // 模板解析
    std::shared_ptr<AST::ASTNode> ParseTemplateType();
    void ParseTemplateContent(AST::TemplateType type);
    std::shared_ptr<AST::ASTNode> ParseTemplateUse();
    
    // 特例化解析
    std::shared_ptr<AST::ASTNode> ParseSpecialization();
    std::shared_ptr<AST::ASTNode> ParseInsertBlock();
    std::shared_ptr<AST::ASTNode> ParseDeleteBlock();
    
    // 约束解析
    std::shared_ptr<AST::ASTNode> ParseExceptBlock();
    
    // 表达式解析
    std::shared_ptr<AST::ASTNode> ParseExpression();
    std::shared_ptr<AST::ASTNode> ParseIdentifier();
    std::shared_ptr<AST::ASTNode> ParseLiteral();
    std::shared_ptr<AST::ASTNode> ParseVariableUse();
    
    // 辅助方法
    bool Match(TokenType type);
    bool Check(TokenType type) const;
    Token Advance();
    Token Consume(TokenType type, const std::string& message);
    Token Peek() const;
    Token PeekNext() const;
    
    // 错误处理
    void ReportError(const std::string& message);
    void ReportError(const std::string& message, const Token& token);
    ParseError CreateError(const std::string& message, const Token& token);
    void Synchronize();
    
    // 工具方法
    bool IsAtEnd() const;
    bool IsHTMLElement(const std::string& name) const;
    bool IsKeyword(const std::string& name) const;
    std::string ParseStringLiteral();
    
    // 验证方法
    bool ValidateElement(const std::shared_ptr<AST::ElementNode>& element);
    bool ValidateAttribute(const std::string& elementName, const std::string& attrName);
    bool ValidateNesting(const std::string& parent, const std::string& child);
};

// 解析器工具类
class ParserUtils {
public:
    // HTML元素分类
    static bool IsVoidElement(const std::string& tagName);
    static bool IsBlockElement(const std::string& tagName);
    static bool IsInlineElement(const std::string& tagName);
    
    // 属性验证
    static bool IsGlobalAttribute(const std::string& attrName);
    static bool IsValidAttributeForElement(const std::string& element, const std::string& attr);
    
    // 嵌套规则
    static bool CanContain(const std::string& parent, const std::string& child);
};

} // namespace CHTL

#endif // CHTL_PARSER_PARSER_H