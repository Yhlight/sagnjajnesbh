#pragma once

#include <memory>
#include <stack>
#include <vector>
#include <string>
#include "chtl_js/ast/CHTLJSASTNode.h"
#include "chtl_js/token/CHTLJSToken.h"

namespace chtl {
namespace chtljs {
namespace ast {

// 前向声明
class CHTLJSDocumentNode;
class ScriptBlockNode;
class EnhancedSelectorNode;
class ObjectLiteralNode;
class VirDeclarationNode;

/**
 * CHTL JS AST构建器
 * 负责从Token流构建CHTL JS AST
 * 注意：完全独立于CHTL AST构建器
 */
class CHTLJSASTBuilder {
public:
    CHTLJSASTBuilder();
    ~CHTLJSASTBuilder();
    
    /**
     * 从Token流构建AST
     * @param tokens Token列表
     * @return AST根节点
     */
    std::shared_ptr<CHTLJSDocumentNode> Build(const std::vector<compiler::CHTLJSToken>& tokens);
    
    /**
     * 从script块内容构建AST
     * @param scriptContent script块的内容
     * @param isLocal 是否是局部script
     * @return AST根节点
     */
    std::shared_ptr<CHTLJSDocumentNode> BuildFromScript(const std::string& scriptContent, bool isLocal = true);
    
    /**
     * 获取构建错误
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 当前处理的Token索引
    size_t m_CurrentIndex;
    std::vector<compiler::CHTLJSToken> m_Tokens;
    
    // 错误信息
    std::vector<std::string> m_Errors;
    
    // 虚对象注册表（记录所有vir声明）
    std::unordered_map<std::string, std::shared_ptr<VirDeclarationNode>> m_VirRegistry;
    
    // Token访问方法
    const compiler::CHTLJSToken& Current() const;
    const compiler::CHTLJSToken& Peek(size_t offset = 1) const;
    bool IsAtEnd() const;
    void Advance();
    bool Match(compiler::CHTLJSTokenType type);
    bool Check(compiler::CHTLJSTokenType type) const;
    
    // 构建方法
    std::shared_ptr<CHTLJSASTNode> BuildStatement();
    std::shared_ptr<CHTLJSASTNode> BuildExpression();
    
    // CHTL JS特有构建方法
    std::shared_ptr<EnhancedSelectorNode> BuildEnhancedSelector();
    std::shared_ptr<CHTLJSASTNode> BuildArrowOperation();
    std::shared_ptr<CHTLJSASTNode> BuildListenCall();
    std::shared_ptr<CHTLJSASTNode> BuildDelegateCall();
    std::shared_ptr<CHTLJSASTNode> BuildAnimateCall();
    std::shared_ptr<CHTLJSASTNode> BuildINeverAwayCall();
    std::shared_ptr<CHTLJSASTNode> BuildPrintMyLoveCall();
    std::shared_ptr<VirDeclarationNode> BuildVirDeclaration();
    std::shared_ptr<CHTLJSASTNode> BuildVirAccess();
    std::shared_ptr<ObjectLiteralNode> BuildObjectLiteral();
    std::shared_ptr<CHTLJSASTNode> BuildStateTag();
    std::shared_ptr<CHTLJSASTNode> BuildFunctionWithState();
    
    // JavaScript片段处理
    std::shared_ptr<CHTLJSASTNode> BuildJSFragment();
    
    // 辅助方法
    bool IsEnhancedSelector();
    bool IsCHTLJSFunction(const std::string& name);
    void RegisterVirObject(std::shared_ptr<VirDeclarationNode> virDecl);
    
    // 错误处理
    void ReportError(const std::string& message);
    void Synchronize();
};

/**
 * CHTL JS AST打印访问者
 * 用于调试输出AST结构
 */
class CHTLJSASTPrinter : public CHTLJSVisitor {
public:
    CHTLJSASTPrinter();
    
    void Print(CHTLJSASTNode* root);
    
    // 实现访问者接口
    void VisitDocument(CHTLJSDocumentNode* node) override;
    void VisitScriptBlock(ScriptBlockNode* node) override;
    void VisitEnhancedSelector(EnhancedSelectorNode* node) override;
    void VisitArrowOperator(ArrowOperatorNode* node) override;
    void VisitListenCall(ListenCallNode* node) override;
    void VisitDelegateCall(DelegateCallNode* node) override;
    void VisitAnimateCall(AnimateCallNode* node) override;
    void VisitINeverAwayCall(INeverAwayCallNode* node) override;
    void VisitPrintMyLoveCall(PrintMyLoveCallNode* node) override;
    void VisitVirDeclaration(VirDeclarationNode* node) override;
    void VisitVirAccess(VirAccessNode* node) override;
    void VisitStateTag(StateTagNode* node) override;
    void VisitObjectLiteral(ObjectLiteralNode* node) override;
    void VisitProperty(PropertyNode* node) override;
    void VisitIdentifier(IdentifierNode* node) override;
    void VisitLiteral(LiteralNode* node) override;
    void VisitJSCodeFragment(JSCodeFragmentNode* node) override;
    
private:
    int m_Indent;
    
    void PrintIndent();
    void IncreaseIndent() { m_Indent += 2; }
    void DecreaseIndent() { m_Indent -= 2; }
};

} // namespace ast
} // namespace chtljs
} // namespace chtl