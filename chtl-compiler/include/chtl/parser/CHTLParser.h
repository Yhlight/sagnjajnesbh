#pragma once

#include <memory>
#include <vector>
#include <string>
#include <stack>
#include "chtl/token/CHTLToken.h"
#include "chtl/ast/ASTNode.h"
#include "chtl/ast/DocumentNode.h"
#include "chtl/core/CHTLGlobalMap.h"
#include "chtl/ast/TemplateNode.h"
#include "chtl/ast/CustomNode.h"
#include "chtl/state/CHTLState.h"
#include "chtl/context/CHTLContext.h"

namespace chtl {
namespace parser {

/**
 * CHTL解析器
 * 负责将Token流转换为AST
 * 严格按照CHTL语法文档实现
 */
class CHTLParser {
public:
    CHTLParser();
    ~CHTLParser();
    
    /**
     * 解析Token流，生成AST
     * @param tokens Token列表
     * @param filename 源文件名
     * @return AST根节点
     */
    std::shared_ptr<ast::DocumentNode> Parse(
        const std::vector<compiler::CHTLToken>& tokens,
        const std::string& filename = ""
    );
    
    /**
     * 获取解析错误
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    void ClearErrors() { m_Errors.clear(); }
    
    /**
     * 设置全局映射表（用于解析时的符号查找）
     */
    void SetGlobalMap(std::shared_ptr<compiler::CHTLGlobalMap> globalMap) {
        m_GlobalMap = globalMap;
    }
    
    /**
     * 设置调试模式
     */
    void SetDebugMode(bool debug) { m_DebugMode = debug; }
    
private:
    // Token管理
    std::vector<compiler::CHTLToken> m_Tokens;
    size_t m_Current;
    
    // 状态管理
    std::unique_ptr<compiler::CHTLStateMachine> m_StateMachine;
    std::unique_ptr<compiler::CHTLContext> m_Context;
    std::shared_ptr<compiler::CHTLGlobalMap> m_GlobalMap;
    
    // 错误处理
    std::vector<std::string> m_Errors;
    bool m_DebugMode;
    
    // 解析栈（用于处理嵌套结构）
    std::stack<std::shared_ptr<ast::ASTNode>> m_ParseStack;
    
    // Token访问方法
    bool IsAtEnd() const;
    const compiler::CHTLToken& Current() const;
    const compiler::CHTLToken& Previous() const;
    const compiler::CHTLToken& Peek(int offset = 0) const;
    compiler::CHTLToken Advance();
    bool Check(compiler::CHTLTokenType type) const;
    bool Match(compiler::CHTLTokenType type);
    bool Match(std::initializer_list<compiler::CHTLTokenType> types);
    
    // 主要解析方法
    std::shared_ptr<ast::ASTNode> ParseTopLevel();
    std::shared_ptr<ast::ASTNode> ParseStatement();
    
    // 元素解析
    std::shared_ptr<ast::ElementNode> ParseElement();
    std::shared_ptr<ast::AttributeNode> ParseAttribute();
    std::shared_ptr<ast::TextNode> ParseTextNode();
    std::shared_ptr<ast::StyleBlockNode> ParseStyleBlock();
    std::shared_ptr<ast::ScriptBlockNode> ParseScriptBlock();
    
    // 模板解析
    std::shared_ptr<ast::TemplateNode> ParseTemplate();
    std::shared_ptr<ast::TemplateStyleNode> ParseTemplateStyle();
    std::shared_ptr<ast::TemplateElementNode> ParseTemplateElement();
    std::shared_ptr<ast::TemplateVarNode> ParseTemplateVar();
    
    // 自定义解析
    std::shared_ptr<ast::CustomNode> ParseCustom();
    std::shared_ptr<ast::CustomStyleNode> ParseCustomStyle();
    std::shared_ptr<ast::CustomElementNode> ParseCustomElement();
    std::shared_ptr<ast::CustomVarNode> ParseCustomVar();
    
    // 特化操作解析
    std::shared_ptr<ast::InheritNode> ParseInherit();
    std::shared_ptr<ast::DeleteNode> ParseDelete();
    std::shared_ptr<ast::InsertNode> ParseInsert();
    
    // 导入解析
    std::shared_ptr<ast::ImportNode> ParseImport();
    
    // 命名空间解析
    std::shared_ptr<ast::NamespaceNode> ParseNamespace();
    
    // 配置解析
    std::shared_ptr<ast::ConfigurationNode> ParseConfiguration();
    std::shared_ptr<ast::ConfigNameBlockNode> ParseConfigNameBlock();
    std::shared_ptr<ast::ConfigOriginTypeNode> ParseConfigOriginType();
    
    // 原始嵌入解析
    std::shared_ptr<ast::OriginNode> ParseOrigin();
    
    // 约束解析
    std::shared_ptr<ast::ExceptNode> ParseExcept();
    
    // 表达式和字面量解析
    std::shared_ptr<ast::ASTNode> ParseExpression();
    std::shared_ptr<ast::ASTNode> ParseLiteral();
    std::shared_ptr<ast::StringLiteralNode> ParseStringLiteral();
    std::shared_ptr<ast::UnquotedLiteralNode> ParseUnquotedLiteral();
    std::shared_ptr<ast::IdentifierNode> ParseIdentifier();
    
    // 引用解析
    std::shared_ptr<ast::StyleReferenceNode> ParseStyleReference();
    std::shared_ptr<ast::ElementReferenceNode> ParseElementReference();
    std::shared_ptr<ast::VarReferenceNode> ParseVarReference();
    
    // 辅助方法
    bool ExpectToken(compiler::CHTLTokenType type, const std::string& message);
    bool ConsumeToken(compiler::CHTLTokenType type, const std::string& message);
    void Synchronize();
    void ReportError(const std::string& message);
    void ReportError(const compiler::CHTLToken& token, const std::string& message);
    
    // 块解析辅助
    std::vector<std::shared_ptr<ast::ASTNode>> ParseBlock();
    std::pair<std::string, std::string> ParseKeyValuePair();
    
    // 状态管理辅助
    void EnterState(CHTLStateType state);
    void ExitState();
    
    // 上下文管理辅助
    void PushScope(const std::string& name);
    void PopScope();
};

} // namespace parser
} // namespace chtl