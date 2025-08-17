#pragma once

#include "CHTLCommon.h"
#include "CHTLTokens.h"
#include "CHTLASTNodes.h"
#include "CHTLLexer.h"
#include "StateMachine.h"
#include "ContextManager.h"
#include <memory>
#include <vector>
#include <stack>

namespace chtl {

// 解析器错误类型
enum class ParseErrorType {
    SYNTAX_ERROR,           // 语法错误
    UNEXPECTED_TOKEN,       // 意外的token
    MISSING_TOKEN,          // 缺失的token
    INVALID_STRUCTURE,      // 无效的结构
    TEMPLATE_ERROR,         // 模板错误
    IMPORT_ERROR,           // 导入错误
    NAMESPACE_ERROR,        // 命名空间错误
    CONSTRAINT_VIOLATION    // 约束违反
};

// 解析器错误信息
struct ParseError {
    ParseErrorType type;
    String message;
    size_t line;
    size_t column;
    String context;
    
    ParseError(ParseErrorType t, const String& msg, size_t l = 0, size_t c = 0, const String& ctx = "")
        : type(t), message(msg), line(l), column(c), context(ctx) {}
};

// 解析结果
struct ParseResult {
    bool success = false;
    std::shared_ptr<CHTLASTNode> ast = nullptr;
    std::vector<ParseError> errors;
    std::vector<String> warnings;
    
    void addError(ParseErrorType type, const String& message, size_t line = 0, size_t column = 0, const String& context = "") {
        errors.emplace_back(type, message, line, column, context);
        success = false;
    }
    
    void addWarning(const String& message) {
        warnings.push_back(message);
    }
};

// CHTL解析器主类
class CHTLParser {
public:
    explicit CHTLParser(const CompilerOptions& options = CompilerOptions{});
    ~CHTLParser() = default;
    
    // 主要解析接口
    ParseResult parse(const String& source, const String& filename = "");
    ParseResult parseFromTokens(const std::vector<Token>& tokens, const String& filename = "");
    
    // 解析特定结构
    std::shared_ptr<CHTLASTNode> parseElement();
    std::shared_ptr<CHTLASTNode> parseText();
    std::shared_ptr<CHTLASTNode> parseStyle();
    std::shared_ptr<CHTLASTNode> parseScript();
    std::shared_ptr<CHTLASTNode> parseTemplate();
    std::shared_ptr<CHTLASTNode> parseCustom();
    std::shared_ptr<CHTLASTNode> parseImport();
    std::shared_ptr<CHTLASTNode> parseNamespace();
    std::shared_ptr<CHTLASTNode> parseConfiguration();
    std::shared_ptr<CHTLASTNode> parseOrigin();
    
    // 属性和表达式解析
    std::shared_ptr<AttributeNode> parseAttribute();
    std::shared_ptr<CHTLASTNode> parseExpression();
    std::shared_ptr<CHTLASTNode> parseVariableReference();
    std::shared_ptr<CHTLASTNode> parseTemplateUsage();
    
    // 样式相关解析
    std::shared_ptr<CHTLASTNode> parseStyleRule();
    std::shared_ptr<CHTLASTNode> parseStyleProperty();
    std::shared_ptr<CHTLASTNode> parseSelector();
    std::shared_ptr<CHTLASTNode> parseInlineStyle();
    std::shared_ptr<CHTLASTNode> parseClassSelector();
    std::shared_ptr<CHTLASTNode> parseIdSelector();
    std::shared_ptr<CHTLASTNode> parsePseudoSelector();
    std::shared_ptr<CHTLASTNode> parseStyleContent();
    std::shared_ptr<CHTLASTNode> parseElementContent();
    
    // 模板和自定义解析
    std::shared_ptr<TemplateStyleNode> parseTemplateStyle();
    std::shared_ptr<TemplateElementNode> parseTemplateElement();
    std::shared_ptr<TemplateVarNode> parseTemplateVar();
    
    // 模板解析实现方法
    std::shared_ptr<TemplateStyleNode> parseTemplateStyleImpl(const String& templateName, size_t line, size_t column);
    std::shared_ptr<TemplateElementNode> parseTemplateElementImpl(const String& templateName, size_t line, size_t column);
    std::shared_ptr<TemplateVarNode> parseTemplateVarImpl(const String& templateName, size_t line, size_t column);
    
    // 自定义解析实现方法
    std::shared_ptr<CustomStyleNode> parseCustomStyle();
    std::shared_ptr<CustomElementNode> parseCustomElement();
    std::shared_ptr<CustomVarNode> parseCustomVar();
    std::shared_ptr<CustomStyleNode> parseCustomStyleImpl(const String& customName, size_t line, size_t column);
    std::shared_ptr<CustomElementNode> parseCustomElementImpl(const String& customName, size_t line, size_t column);
    std::shared_ptr<CustomVarNode> parseCustomVarImpl(const String& customName, size_t line, size_t column);
    
    // 特例化和插入操作解析
    std::shared_ptr<CHTLASTNode> parseStyleSpecialization(const String& templateName);
    
    // 特殊语法解析
    std::shared_ptr<CHTLASTNode> parseInherit();
    std::shared_ptr<CHTLASTNode> parseDelete();
    std::shared_ptr<CHTLASTNode> parseInsert();
    std::shared_ptr<CHTLASTNode> parseExcept();
    
    // 导入和命名空间
    std::shared_ptr<ImportNode> parseImportStatement();
    std::shared_ptr<NamespaceNode> parseNamespaceDeclaration();
    
    // 字面量和标识符解析
    String parseLiteral();
    String parseIdentifier();
    String parseStringLiteral();
    String parseUnquotedLiteral();
    String parseElementSelector(int& index);
    
    // 工具方法
    void setDebugMode(bool debug) { debugMode_ = debug; }
    bool isDebugMode() const { return debugMode_; }
    
    // 错误处理
    void reportError(ParseErrorType type, const String& message, const String& context = "");
    void reportWarning(const String& message);
    
    // 状态管理
    void pushContext(const String& contextName);
    void popContext();
    String getCurrentContext() const;

private:
    // 核心解析状态
    std::vector<Token> tokens_;
    size_t currentPos_;
    String currentFilename_;
    ParseResult result_;
    CompilerOptions options_;
    bool debugMode_;
    
    // 状态管理
    std::unique_ptr<StateMachine> stateMachine_;
    std::unique_ptr<ContextManager> contextManager_;
    std::stack<String> contextStack_;
    
    // 解析上下文
    struct ParseContext {
        StringSet allowedElements;
        StringSet forbiddenElements;
        StringSet templateNames;
        StringSet customNames;
        StringSet namespaces;
        StringList classNames;          // 自动生成的类名
        String currentElementName;      // 当前元素名称
        String currentElementId;        // 自动生成的ID
        bool inLocalStyle = false;
        bool inLocalScript = false;
        bool inTemplate = false;
        bool inCustom = false;
        bool inNamespace = false;
        int nestingLevel = 0;
    };
    
    ParseContext currentContext_;
    std::stack<ParseContext> contextStack_backup_;
    
    // Token操作
    const Token& currentToken() const;
    const Token& peekToken(size_t offset = 1) const;
    Token consumeToken();
    Token consumeToken(TokenType expectedType);
    bool matchToken(TokenType type) const;
    bool matchAnyToken(const std::vector<TokenType>& types) const;
    void skipToken();
    void skipToToken(TokenType type);
    void skipToMatchingBrace();
    
    // 语法验证
    bool isValidElementName(const String& name) const;
    bool isValidAttributeName(const String& name) const;
    bool isValidTemplateType(const String& type) const;
    bool isValidCustomType(const String& type) const;
    bool isValidOriginType(const String& type) const;
    
    // 约束检查
    bool checkElementConstraints(const String& elementName) const;
    bool checkTemplateConstraints(const String& templateName) const;
    bool checkCustomConstraints(const String& customName) const;
    bool checkNamespaceConstraints(const String& namespaceName) const;
    
    // 上下文相关解析
    void enterElementContext(const String& elementName);
    void exitElementContext();
    void enterStyleContext();
    void exitStyleContext();
    void enterScriptContext();
    void exitScriptContext();
    void enterTemplateContext(const String& templateName);
    void exitTemplateContext();
    void enterCustomContext(const String& customName);
    void exitCustomContext();
    void enterNamespaceContext(const String& namespaceName);
    void exitNamespaceContext();
    
    // 辅助方法
    bool isAtEnd() const;
    void synchronize();
    void skipWhitespace();
    void skipComments();
    
    // 调试和日志
    void debugLog(const String& message) const;
    void logParseStep(const String& step, const String& details = "") const;
};

// CHTL解析器工厂
class CHTLParserFactory {
public:
    static std::unique_ptr<CHTLParser> createParser(const CompilerOptions& options = CompilerOptions{});
    static std::unique_ptr<CHTLParser> createDebugParser();
};

} // namespace chtl