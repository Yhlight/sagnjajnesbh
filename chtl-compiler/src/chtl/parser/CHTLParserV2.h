#pragma once

#include "../lexer/CHTLLexerV2.h"
#include "../ast/CHTLASTNodes.h"
#include <memory>
#include <vector>
#include <string>
#include <stack>

namespace chtl {

/**
 * CHTL 解析器
 * 严格实现 CHTL 语法规范
 */
class CHTLParserV2 {
public:
    CHTLParserV2();
    
    /**
     * 解析 CHTL 源代码
     */
    std::shared_ptr<ast::DocumentNode> Parse(const std::string& source, const std::string& filename = "");
    
    /**
     * 解析 Token 流
     */
    std::shared_ptr<ast::DocumentNode> ParseTokens(const std::vector<CHTLToken>& tokens);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 解析方法
    std::shared_ptr<ast::ASTNode> ParseTopLevel();
    std::shared_ptr<ast::ASTNode> ParseElement();
    std::shared_ptr<ast::ASTNode> ParseText();
    std::shared_ptr<ast::ASTNode> ParseStyle();
    std::shared_ptr<ast::ASTNode> ParseScript();
    std::shared_ptr<ast::ASTNode> ParseTemplate();
    std::shared_ptr<ast::ASTNode> ParseCustom();
    std::shared_ptr<ast::ASTNode> ParseImport();
    std::shared_ptr<ast::ASTNode> ParseNamespace();
    std::shared_ptr<ast::ASTNode> ParseConfiguration();
    std::shared_ptr<ast::ASTNode> ParseOrigin();
    std::shared_ptr<ast::ASTNode> ParseComment();
    
    // 元素内容解析
    void ParseElementContent(ast::ElementNode* element);
    void ParseAttributes(ast::ElementNode* element);
    void ParseLocalStyle(ast::ElementNode* element);
    void ParseLocalScript(ast::ElementNode* element);
    
    // 样式解析
    void ParseStyleContent(ast::StyleNode* style);
    void ParseStyleRule(ast::StyleNode* style);
    void ParseInlineStyle(ast::StyleNode* style);
    
    // 模板解析
    void ParseTemplateDefinition(ast::TemplateNode* tmpl);
    void ParseTemplateParameters(ast::TemplateNode* tmpl);
    
    // 特殊化解析
    std::shared_ptr<ast::ASTNode> ParseSpecialization();
    std::shared_ptr<ast::ASTNode> ParseInherit();
    std::shared_ptr<ast::ASTNode> ParseDelete();
    std::shared_ptr<ast::ASTNode> ParseInsert();
    
    // 变量和表达式解析
    std::shared_ptr<ast::ASTNode> ParseVar();
    std::shared_ptr<ast::ASTNode> ParseExpression();
    
    // 辅助方法
    bool Match(CHTLTokenType type);
    bool Check(CHTLTokenType type) const;
    bool CheckAny(std::initializer_list<CHTLTokenType> types) const;
    CHTLToken Advance();
    CHTLToken Peek() const;
    CHTLToken Previous() const;
    bool IsAtEnd() const;
    
    // CE对等式处理
    bool MatchColonOrEquals();
    
    // 错误处理
    void ReportError(const std::string& message);
    void Synchronize();
    
    // 字面量处理
    std::string ParseLiteral();
    std::string ParseStringOrUnquoted();
    
    // 成员变量
    std::vector<CHTLToken> m_Tokens;
    size_t m_Current;
    std::vector<std::string> m_Errors;
    
    // 上下文状态
    enum class Context {
        TOP_LEVEL,
        ELEMENT,
        STYLE_GLOBAL,
        STYLE_LOCAL,
        SCRIPT_GLOBAL,
        SCRIPT_LOCAL,
        TEMPLATE,
        NAMESPACE,
        CONFIGURATION
    };
    
    std::stack<Context> m_ContextStack;
    Context CurrentContext() const;
    void PushContext(Context ctx);
    void PopContext();
};

} // namespace chtl