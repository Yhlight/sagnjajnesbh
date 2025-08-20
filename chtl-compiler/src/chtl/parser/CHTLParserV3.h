#pragma once

#include "../lexer/CHTLLexerV2.h"
#include "../ast/CHTLASTNodes.h"
#include <memory>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>

namespace chtl {

// 完整实现所有CHTL语法特征的解析器
class CHTLParserV3 {
public:
    CHTLParserV3();
    
    // 主要解析接口
    std::shared_ptr<ast::DocumentNode> Parse(const std::string& source, const std::string& filename = "");
    
    // 错误处理
    bool HasErrors() const { return !m_Errors.empty(); }
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    
    // 设置源代码（用于Origin块的原始内容提取）
    void SetSource(const std::string& source) { m_Source = source; }

private:
    // 解析上下文
    enum class Context {
        TOP_LEVEL,
        ELEMENT,
        STYLE_GLOBAL,
        STYLE_LOCAL,
        SCRIPT_GLOBAL,
        SCRIPT_LOCAL,
        TEMPLATE,
        CUSTOM,
        NAMESPACE,
        CONFIGURATION,
        ORIGIN,
        IMPORT,
        SPECIALIZATION
    };
    
    // Token管理
    std::vector<CHTLToken> m_Tokens;
    size_t m_Current;
    std::string m_Source;
    
    // 上下文栈
    std::stack<Context> m_ContextStack;
    
    // 错误收集
    std::vector<std::string> m_Errors;
    
    // 配置状态
    std::unordered_map<std::string, std::string> m_CurrentConfig;
    
    // 当前命名空间
    std::string m_CurrentNamespace;
    
    // 主要解析方法
    std::shared_ptr<ast::DocumentNode> ParseTokens(const std::vector<CHTLToken>& tokens);
    std::shared_ptr<ast::ASTNode> ParseTopLevel();
    
    // 元素解析
    std::shared_ptr<ast::ASTNode> ParseElement();
    void ParseElementContent(ast::ElementNode* element);
    void ParseAttributes(ast::ElementNode* element);
    
    // 文本解析
    std::shared_ptr<ast::ASTNode> ParseText();
    
    // 样式解析
    std::shared_ptr<ast::ASTNode> ParseStyle();
    void ParseStyleContent(ast::StyleNode* style);
    void ParseStyleRule(ast::StyleNode* style);
    void ParseLocalStyle(ast::ElementNode* element);
    
    // 脚本解析
    std::shared_ptr<ast::ASTNode> ParseScript();
    void ParseLocalScript(ast::ElementNode* element);
    
    // 注释解析
    std::shared_ptr<ast::ASTNode> ParseComment();
    
    // 模板解析
    std::shared_ptr<ast::ASTNode> ParseTemplate();
    void ParseTemplateParameters(ast::TemplateNode* tmpl);
    void ParseTemplateDefinition(ast::TemplateNode* tmpl);
    
    // 自定义解析
    std::shared_ptr<ast::ASTNode> ParseCustom();
    void ParseCustomDefinition(ast::CustomNode* custom);
    void ParseCustomStyleDefinition(ast::CustomNode* custom);
    void ParseCustomElementDefinition(ast::CustomNode* custom);
    void ParseCustomVarDefinition(ast::CustomNode* custom);
    void ParseNoValueStyleProperties(ast::CustomNode* custom);
    
    // 导入解析
    std::shared_ptr<ast::ASTNode> ParseImport();
    void ParseImportSpecifiers(ast::ImportNode* import);
    std::string ParseImportPath();
    
    // 命名空间解析
    std::shared_ptr<ast::ASTNode> ParseNamespace();
    
    // 配置解析
    std::shared_ptr<ast::ASTNode> ParseConfiguration();
    void ParseConfigurationContent(ast::ConfigurationNode* config);
    void ParseNameConfig(ast::ConfigurationNode* config);
    void ParseOriginTypeConfig(ast::ConfigurationNode* config);
    
    // 原始嵌入解析
    std::shared_ptr<ast::ASTNode> ParseOrigin();
    std::string ParseOriginTypeName();
    
    // 特殊化解析
    std::shared_ptr<ast::ASTNode> ParseSpecialization();
    std::shared_ptr<ast::ASTNode> ParseDelete();
    std::shared_ptr<ast::ASTNode> ParseInsert();
    
    // 继承解析
    std::shared_ptr<ast::ASTNode> ParseInherit();
    
    // 变量解析
    std::shared_ptr<ast::ASTNode> ParseVar();
    std::shared_ptr<ast::ASTNode> ParseVarCall();
    
    // 表达式解析
    std::shared_ptr<ast::ASTNode> ParseExpression();
    
    // 约束解析
    std::shared_ptr<ast::ASTNode> ParseExcept();
    std::vector<std::string> ParseExceptTargets();
    
    // 全缀名解析
    std::string ParseFullQualifiedName();
    
    // Token辅助方法
    bool Match(CHTLTokenType type);
    bool Check(CHTLTokenType type) const;
    bool CheckAny(std::initializer_list<CHTLTokenType> types) const;
    CHTLToken Advance();
    CHTLToken Peek() const;
    CHTLToken Previous() const;
    bool IsAtEnd() const;
    
    // 特殊匹配方法
    bool MatchColonOrEquals();
    std::string ParseStringOrUnquoted();
    std::string ParseIdentifierPath();
    
    // 错误报告
    void ReportError(const std::string& message);
    
    // 上下文管理
    Context CurrentContext() const;
    void PushContext(Context ctx);
    void PopContext();
    
    // 解析辅助方法
    bool IsKeyword(const std::string& identifier) const;
    bool IsHtmlElement(const std::string& identifier) const;
    std::string ExtractOriginContent(size_t startPos, size_t endPos);
    bool IsTemplateOrCustomUsage() const;
    bool IsSpecializationContext() const;
};

} // namespace chtl