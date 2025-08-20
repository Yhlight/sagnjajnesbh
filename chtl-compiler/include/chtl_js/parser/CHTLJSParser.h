#pragma once

#include <memory>
#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include "chtl_js/token/CHTLJSToken.h"
#include "chtl_js/ast/CHTLJSASTNode.h"
#include "chtl_js/ast/CHTLJSDocumentNode.h"
#include "chtl_js/core/CHTLJSGlobalMap.h"
#include "chtl_js/state/CHTLJSState.h"
#include "chtl_js/context/CHTLJSContext.h"

namespace chtl {
namespace chtljs {
namespace parser {

/**
 * CHTL JS解析器
 * 负责将Token流转换为CHTL JS AST
 * 严格按照CHTL JS语法文档实现
 * 注意：完全独立于CHTL解析器
 */
class CHTLJSParser {
public:
    CHTLJSParser();
    ~CHTLJSParser();
    
    /**
     * 解析Token流，生成AST
     * @param tokens Token列表
     * @param isLocal 是否是局部脚本
     * @return AST根节点
     */
    std::shared_ptr<ast::CHTLJSDocumentNode> Parse(
        const std::vector<CHTLJSToken>& tokens,
        bool isLocal = true
    );
    
    /**
     * 从脚本内容解析
     * @param scriptContent 脚本内容
     * @param isLocal 是否是局部脚本
     * @return AST根节点
     */
    std::shared_ptr<ast::CHTLJSDocumentNode> ParseScript(
        const std::string& scriptContent,
        bool isLocal = true
    );
    
    /**
     * 获取解析错误
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    void ClearErrors() { m_Errors.clear(); }
    
    /**
     * 设置全局映射表
     */
    void SetGlobalMap(std::shared_ptr<CHTLJSGlobalMap> globalMap) {
        m_GlobalMap = globalMap;
    }
    
    /**
     * 设置调试模式
     */
    void SetDebugMode(bool debug) { m_DebugMode = debug; }
    
private:
    // Token管理
    std::vector<CHTLJSToken> m_Tokens;
    size_t m_Current;
    
    // 状态管理（独立的CHTL JS状态机）
    std::unique_ptr<CHTLJSStateMachine> m_StateMachine;
    std::unique_ptr<CHTLJSContext> m_Context;
    std::shared_ptr<CHTLJSGlobalMap> m_GlobalMap;
    
    // 虚对象注册表
    std::unordered_map<std::string, std::shared_ptr<ast::VirDeclarationNode>> m_VirRegistry;
    
    // 错误处理
    std::vector<std::string> m_Errors;
    bool m_DebugMode;
    
    // Token访问方法
    bool IsAtEnd() const;
    const CHTLJSToken& Current() const;
    const CHTLJSToken& Previous() const;
    const CHTLJSToken& Peek(int offset = 0) const;
    CHTLJSToken Advance();
    bool Check(CHTLJSTokenType type) const;
    bool Match(CHTLJSTokenType type);
    bool Match(std::initializer_list<CHTLJSTokenType> types);
    
    // 主要解析方法
    std::shared_ptr<ast::CHTLJSASTNode> ParseStatement();
    std::shared_ptr<ast::CHTLJSASTNode> ParseExpression();
    
    // CHTL JS特有解析方法
    std::shared_ptr<ast::EnhancedSelectorNode> ParseEnhancedSelector();
    std::shared_ptr<ast::CHTLJSASTNode> ParseSelectorExpression();
    std::shared_ptr<ast::CHTLJSASTNode> ParseArrowExpression();
    std::shared_ptr<ast::VirDeclarationNode> ParseVirDeclaration();
    std::shared_ptr<ast::CHTLJSASTNode> ParseListenCall();
    std::shared_ptr<ast::CHTLJSASTNode> ParseDelegateCall();
    std::shared_ptr<ast::CHTLJSASTNode> ParseAnimateCall();
    std::shared_ptr<ast::CHTLJSASTNode> ParseINeverAwayCall();
    std::shared_ptr<ast::CHTLJSASTNode> ParsePrintMyLoveCall();
    
    // 对象字面量解析
    std::shared_ptr<ast::ObjectLiteralNode> ParseObjectLiteral();
    std::shared_ptr<ast::PropertyNode> ParseProperty();
    
    // 函数解析
    std::shared_ptr<ast::CHTLJSASTNode> ParseFunction();
    std::shared_ptr<ast::ArrowFunctionNode> ParseArrowFunction();
    std::shared_ptr<ast::ParameterListNode> ParseParameterList();
    
    // 状态标签解析
    std::shared_ptr<ast::StateTagNode> ParseStateTag();
    std::shared_ptr<ast::FunctionWithStateNode> ParseFunctionWithState();
    
    // JavaScript片段解析
    std::shared_ptr<ast::JSCodeFragmentNode> ParseJSFragment();
    
    // 辅助方法
    bool IsEnhancedSelector();
    bool IsCHTLJSFunction(const std::string& name);
    bool IsVirDeclaration();
    void RegisterVirObject(std::shared_ptr<ast::VirDeclarationNode> virNode);
    std::shared_ptr<ast::VirDeclarationNode> GetVirObject(const std::string& name);
    
    // 错误处理
    void Synchronize();
    void ReportError(const std::string& message);
    void ReportError(const CHTLJSToken& token, const std::string& message);
    
    // 表达式解析辅助
    std::shared_ptr<ast::CHTLJSASTNode> ParsePrimary();
    std::shared_ptr<ast::CHTLJSASTNode> ParseCall();
    std::shared_ptr<ast::CHTLJSASTNode> ParseMemberAccess();
    
    // 字面量解析
    std::shared_ptr<ast::CHTLJSASTNode> ParseLiteral();
    std::shared_ptr<ast::IdentifierNode> ParseIdentifier();
    std::shared_ptr<ast::ArrayLiteralNode> ParseArrayLiteral();
};

} // namespace parser
} // namespace chtljs
} // namespace chtl