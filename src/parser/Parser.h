#pragma once
#include "../common/Token.h"
#include "../common/Lexer.h"
#include "../common/Context.h"
#include "../ast/ASTNode.h"
#include "../ast/CHTLNodes.h"
#include "../ast/SpecializedNodes.h"
#include <memory>
#include <vector>

namespace chtl {
namespace parser {

// 解析器错误类型
enum class ParseError {
    UNEXPECTED_TOKEN,       // 意外的Token
    MISSING_TOKEN,          // 缺少Token
    INVALID_SYNTAX,         // 无效语法
    UNEXPECTED_EOF,         // 意外的文件结束
    INVALID_ELEMENT,        // 无效元素
    INVALID_ATTRIBUTE,      // 无效属性
    INVALID_TEMPLATE,       // 无效模板
    INVALID_CUSTOM,         // 无效自定义
    INVALID_IMPORT,         // 无效导入
    INVALID_NAMESPACE,      // 无效命名空间
    INVALID_CHTL_JS,        // 无效CHTL JS语法
    CONSTRAINT_VIOLATION    // 约束违反
};

// 解析器异常
class ParseException : public std::exception {
public:
    ParseError error_type;
    std::string message;
    TokenPosition position;
    
    ParseException(ParseError type, const std::string& msg, const TokenPosition& pos)
        : error_type(type), message(msg), position(pos) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// CHTL解析器
class CHTLParser {
public:
    explicit CHTLParser(CompilerContext& context);
    ~CHTLParser() = default;
    
    // 设置输入
    void setInput(const std::string& input);
    void setInputFile(const std::string& file_path);
    void setLexer(std::unique_ptr<Lexer> lexer);
    
    // 解析方法
    std::unique_ptr<ast::ProgramNode> parseProgram();
    std::unique_ptr<ast::ElementNode> parseElement();
    std::unique_ptr<ast::TextNode> parseText();
    std::unique_ptr<ast::AttributeNode> parseAttribute();
    std::unique_ptr<ast::StyleBlockNode> parseStyleBlock();
    std::unique_ptr<ast::ScriptBlockNode> parseScriptBlock();
    std::unique_ptr<ast::TemplateDefinitionNode> parseTemplateDefinition();
    std::unique_ptr<ast::CustomDefinitionNode> parseCustomDefinition();
    std::unique_ptr<ast::OriginBlockNode> parseOriginBlock();
    std::unique_ptr<ast::ImportStatementNode> parseImportStatement();
    std::unique_ptr<ast::NamespaceDefinitionNode> parseNamespaceDefinition();
    std::unique_ptr<ast::CommentNode> parseComment();
    
    // CHTL JS解析方法
    std::unique_ptr<ast::CHTLJSSelectorNode> parseCHTLJSSelector();
    std::unique_ptr<ast::CHTLJSListenNode> parseCHTLJSListen();
    std::unique_ptr<ast::CHTLJSDelegateNode> parseCHTLJSDelegate();
    std::unique_ptr<ast::CHTLJSVirNode> parseCHTLJSVir();
    
    // 样式相关解析
    std::unique_ptr<ast::CSSPropertyNode> parseCSSProperty();
    std::unique_ptr<ast::CSSSelectorNode> parseCSSSelector();
    std::unique_ptr<ast::StyleInheritanceNode> parseStyleInheritance();
    std::unique_ptr<ast::StyleDeleteNode> parseStyleDelete();
    
    // 变量相关解析
    std::unique_ptr<ast::VariableReferenceNode> parseVariableReference();
    std::unique_ptr<ast::VariableSpecializationNode> parseVariableSpecialization();
    
    // 元素操作解析
    std::unique_ptr<ast::ElementInsertNode> parseElementInsert();
    std::unique_ptr<ast::ElementDeleteNode> parseElementDelete();
    std::unique_ptr<ast::ElementIndexNode> parseElementIndex();
    
    // 约束解析
    std::unique_ptr<ast::ExceptConstraintNode> parseExceptConstraint();
    
    // 字面量解析
    std::unique_ptr<ast::LiteralNode> parseLiteral();
    std::unique_ptr<ast::IdentifierNode> parseIdentifier();
    
    // 错误处理
    bool hasError() const;
    std::vector<ParseException> getErrors() const;
    void clearErrors();
    
    // 调试和诊断
    std::string getCurrentTokenInfo() const;
    void printParseTree(const ast::ASTNode* node, int indent = 0) const;
    
private:
    CompilerContext& context_;              // 编译上下文
    std::unique_ptr<Lexer> lexer_;         // 词法分析器
    Token current_token_;                   // 当前Token
    Token peek_token_;                      // 预读Token
    std::vector<ParseException> errors_;    // 解析错误列表
    
    // Token管理
    void advance();                         // 前进到下一个Token
    Token peek();                           // 预读下一个Token
    Token peek() const;                     // const版本的预读
    bool match(TokenType type);             // 匹配Token类型
    bool expect(TokenType type);            // 期望Token类型
    void consume(TokenType type, const std::string& error_msg = "");
    
    // 错误处理
    void reportError(ParseError error_type, const std::string& message);
    void synchronize();                     // 错误同步恢复
    
    // 辅助解析方法
    bool isElementStart() const;            // 判断是否为元素开始
    bool isAttributeStart() const;          // 判断是否为属性开始
    bool isTemplateStart() const;           // 判断是否为模板开始
    bool isCustomStart() const;             // 判断是否为自定义开始
    bool isOriginStart() const;             // 判断是否为原始嵌入开始
    bool isImportStart() const;             // 判断是否为导入开始
    bool isNamespaceStart() const;          // 判断是否为命名空间开始
    bool isCHTLJSStart() const;             // 判断是否为CHTL JS开始
    bool isStylePropertyStart() const;      // 判断是否为样式属性开始
    bool isVariableReferenceStart() const;  // 判断是否为变量引用开始
    
    // 语法约束检查
    void checkSyntaxConstraints(const ast::ASTNode* node);
    bool isValidInCurrentContext(ast::NodeType node_type) const;
    
    // 解析块内容
    std::vector<std::unique_ptr<ast::ASTNode>> parseBlockContent();
    std::vector<std::unique_ptr<ast::ASTNode>> parseStyleBlockContent();
    std::vector<std::unique_ptr<ast::ASTNode>> parseScriptBlockContent();
    std::vector<std::unique_ptr<ast::ASTNode>> parseTemplateContent();
    std::vector<std::unique_ptr<ast::ASTNode>> parseCustomContent();
    
    // 解析表达式
    std::string parseStringValue();         // 解析字符串值
    std::string parseIdentifierValue();     // 解析标识符值
    std::vector<std::string> parseIdentifierList(); // 解析标识符列表
    
    // 特殊语法处理
    void handleCEEquivalence();             // 处理CE对等式（: 与 = 等价）
    std::string resolveVariableReference(const std::string& group, const std::string& name);
    void validateElementName(const std::string& name);
    void validateAttributeName(const std::string& name);
    
    // 上下文管理
    void enterParsingContext(ast::NodeType context_type);
    void exitParsingContext();
};

} // namespace parser
} // namespace chtl