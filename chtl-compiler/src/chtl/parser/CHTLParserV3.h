#pragma once

#include "../lexer/CHTLLexerV2.h"
#include "../ast/CHTLASTNodesV3.h"
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
    std::shared_ptr<ast::v3::DocumentNode> Parse(const std::string& source, const std::string& filename = "");
    
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
    
    // 错误记录
    std::vector<std::string> m_Errors;
    std::string m_Filename;
    
    // 上下文栈
    std::stack<Context> m_ContextStack;
    
    // 当前命名空间栈
    std::vector<std::string> m_NamespaceStack;
    
    // 导入管理
    std::unordered_set<std::string> m_ImportedFiles;
    std::unordered_map<std::string, std::string> m_ImportAliases;
    
    // 约束管理
    std::unordered_set<std::string> m_GlobalExcepts;
    std::unordered_map<std::string, std::unordered_set<std::string>> m_LocalExcepts;
    
    // Token辅助方法
    bool IsAtEnd() const;
    const CHTLToken& Current() const;
    const CHTLToken& Previous() const;
    const CHTLToken& Peek(size_t offset = 0) const;
    void Advance();
    bool Match(CHTLTokenType type);
    bool Match(const std::vector<CHTLTokenType>& types);
    bool Check(CHTLTokenType type) const;
    bool CheckKeyword(const std::string& keyword) const;
    void Consume(CHTLTokenType type, const std::string& message);
    
    // 错误处理
    void AddError(const std::string& message);
    void AddError(const CHTLToken& token, const std::string& message);
    void Synchronize();
    
    // 上下文管理
    void PushContext(Context ctx);
    void PopContext();
    Context CurrentContext() const;
    bool IsInContext(Context ctx) const;
    
    // 命名空间管理
    std::string GetCurrentNamespace() const;
    std::string ResolveFullName(const std::string& name) const;
    
    // 主要解析方法
    void ParseTokens();
    
    // 顶层解析
    std::shared_ptr<ast::v3::ASTNode> ParseTopLevel();
    std::shared_ptr<ast::v3::ImportNode> ParseImport();
    std::shared_ptr<ast::v3::NamespaceNode> ParseNamespace();
    std::shared_ptr<ast::v3::ConfigurationNode> ParseConfiguration();
    std::shared_ptr<ast::v3::TemplateNode> ParseTemplate();
    std::shared_ptr<ast::v3::CustomNode> ParseCustom();
    std::shared_ptr<ast::v3::StyleNode> ParseGlobalStyle();
    std::shared_ptr<ast::v3::ScriptNode> ParseGlobalScript();
    std::shared_ptr<ast::v3::ElementNode> ParseElement();
    std::shared_ptr<ast::v3::OriginNode> ParseOrigin();
    std::shared_ptr<ast::v3::CommentNode> ParseComment();
    
    // 元素内容解析
    void ParseElementContent(ast::v3::ElementNode* element);
    std::shared_ptr<ast::v3::TextNode> ParseText();
    std::shared_ptr<ast::v3::StyleNode> ParseLocalStyle();
    std::shared_ptr<ast::v3::ScriptNode> ParseLocalScript();
    std::unordered_map<std::string, std::string> ParseAttributes();
    
    // 样式内容解析
    void ParseStyleContent(ast::v3::StyleNode* style);
    std::pair<std::string, std::string> ParseStyleProperty();
    ast::v3::StyleNode::Rule ParseStyleRule();
    
    // 脚本内容解析
    void ParseScriptContent(ast::v3::ScriptNode* script);
    
    // 模板内容解析
    void ParseTemplateDefinition(ast::v3::TemplateNode* tmpl);
    std::shared_ptr<ast::v3::InheritNode> ParseInherit();
    
    // 自定义内容解析
    void ParseCustomDefinition(ast::v3::CustomNode* custom);
    std::shared_ptr<ast::v3::DeleteNode> ParseDelete();
    std::shared_ptr<ast::v3::InsertNode> ParseInsert();
    
    // 变量解析
    std::shared_ptr<ast::v3::VarNode> ParseVar();
    std::shared_ptr<ast::v3::VarCallNode> ParseVarCall();
    
    // 配置内容解析
    void ParseConfigurationContent(ast::v3::ConfigurationNode* config);
    
    // 约束解析
    std::shared_ptr<ast::v3::ExceptNode> ParseExcept();
    void ProcessExcept(ast::v3::ExceptNode* except);
    
    // 原始内容解析
    void ParseOriginContent(ast::v3::OriginNode* origin);
    
    // 辅助方法
    std::string ParseStringOrUnquoted();
    std::string ParseQuotedString();
    std::string ParseIdentifier();
    std::string ParseFullQualifiedName();
    int ParseNumber();
    
    // 检查约束
    bool CheckConstraints(const std::string& element) const;
    
    // HTML元素验证
    bool IsValidHtmlElement(const std::string& tag) const;
};

} // namespace chtl