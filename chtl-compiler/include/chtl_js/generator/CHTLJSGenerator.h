#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include "chtl_js/ast/CHTLJSASTNode.h"
#include "chtl_js/core/CHTLJSGlobalMap.h"

namespace chtl {
namespace chtljs {
namespace generator {

/**
 * CHTL JS代码生成结果
 */
struct CHTLJSGenerateResult {
    bool Success;
    std::string JavaScript;     // 生成的JavaScript代码
    std::vector<std::string> Errors;
    
    // 额外的元数据
    std::unordered_set<std::string> GeneratedFunctions;  // 生成的全局函数
    std::unordered_map<std::string, std::string> VirMappings;  // 虚对象映射
    std::unordered_set<std::string> UsedSelectors;  // 使用的选择器
};

/**
 * CHTL JS代码生成器
 * 负责将CHTL JS AST转换为JavaScript
 * 严格按照CHTL JS语法文档生成代码
 * 注意：完全独立于CHTL生成器
 */
class CHTLJSGenerator : public ast::CHTLJSVisitor {
public:
    CHTLJSGenerator();
    ~CHTLJSGenerator();
    
    /**
     * 生成代码
     * @param root AST根节点
     * @return 生成结果
     */
    CHTLJSGenerateResult Generate(ast::CHTLJSASTNode* root);
    
    /**
     * 设置全局映射表
     */
    void SetGlobalMap(std::shared_ptr<CHTLJSGlobalMap> globalMap) {
        m_GlobalMap = globalMap;
    }
    
    /**
     * 设置生成选项
     */
    void SetPrettyPrint(bool pretty) { m_PrettyPrint = pretty; }
    void SetIndentSize(int size) { m_IndentSize = size; }
    void SetGenerateSourceMap(bool generate) { m_GenerateSourceMap = generate; }
    
    // 实现访问者接口
    void VisitDocument(ast::CHTLJSDocumentNode* node) override;
    void VisitScriptBlock(ast::ScriptBlockNode* node) override;
    void VisitEnhancedSelector(ast::EnhancedSelectorNode* node) override;
    void VisitArrowOperator(ast::ArrowOperatorNode* node) override;
    void VisitListenCall(ast::ListenCallNode* node) override;
    void VisitDelegateCall(ast::DelegateCallNode* node) override;
    void VisitAnimateCall(ast::AnimateCallNode* node) override;
    void VisitINeverAwayCall(ast::INeverAwayCallNode* node) override;
    void VisitPrintMyLoveCall(ast::PrintMyLoveCallNode* node) override;
    void VisitVirDeclaration(ast::VirDeclarationNode* node) override;
    void VisitVirAccess(ast::VirAccessNode* node) override;
    void VisitStateTag(ast::StateTagNode* node) override;
    void VisitObjectLiteral(ast::ObjectLiteralNode* node) override;
    void VisitProperty(ast::PropertyNode* node) override;
    void VisitIdentifier(ast::IdentifierNode* node) override;
    void VisitLiteral(ast::LiteralNode* node) override;
    void VisitJSCodeFragment(ast::JSCodeFragmentNode* node) override;
    
private:
    // 输出流
    std::stringstream m_Output;
    std::stringstream* m_CurrentStream;
    
    // 全局映射表（独立的CHTL JS映射表）
    std::shared_ptr<CHTLJSGlobalMap> m_GlobalMap;
    
    // 生成状态
    int m_IndentLevel;
    bool m_InObjectLiteral;
    bool m_InFunctionBody;
    
    // 生成选项
    bool m_PrettyPrint;
    int m_IndentSize;
    bool m_GenerateSourceMap;
    
    // 生成的元数据
    std::unordered_set<std::string> m_GeneratedFunctions;
    std::unordered_map<std::string, std::string> m_VirMappings;
    std::unordered_set<std::string> m_UsedSelectors;
    std::unordered_map<std::string, int> m_FunctionCounter;
    
    // 错误收集
    std::vector<std::string> m_Errors;
    
    // 辅助方法
    void Write(const std::string& text);
    void WriteLine(const std::string& text = "");
    void WriteIndent();
    void IncreaseIndent() { m_IndentLevel++; }
    void DecreaseIndent() { if (m_IndentLevel > 0) m_IndentLevel--; }
    
    // 增强选择器生成
    std::string GenerateSelector(ast::EnhancedSelectorNode* node);
    std::string ConvertSelectorToJS(const std::string& selector, ast::EnhancedSelectorNode::SelectorType type);
    
    // CHTL JS函数生成
    void GenerateListenFunction(ast::ListenCallNode* node);
    void GenerateDelegateFunction(ast::DelegateCallNode* node);
    void GenerateAnimateFunction(ast::AnimateCallNode* node);
    void GenerateINeverAwayFunction(ast::INeverAwayCallNode* node);
    void GeneratePrintMyLoveFunction(ast::PrintMyLoveCallNode* node);
    
    // 虚对象处理
    void ProcessVirDeclaration(ast::VirDeclarationNode* node);
    std::string GenerateVirFunction(const std::string& virName, const std::string& functionKey);
    std::string GetVirFunctionName(const std::string& virName, const std::string& functionKey);
    
    // 事件委托管理
    void GenerateDelegateRegistry();
    std::string GetDelegateRegistryName() { return "__chtljs_delegate_registry"; }
    
    // 工具函数生成
    void GenerateUtilityFunctions();
    void GenerateSelectorFunction();
    void GenerateListenUtility();
    void GenerateDelegateUtility();
    void GenerateAnimateUtility();
    
    // 对象字面量生成
    void GenerateObjectLiteral(ast::ObjectLiteralNode* node);
    void GenerateProperty(ast::PropertyNode* node);
    
    // 状态标签处理
    std::string ProcessStateTag(ast::StateTagNode* node);
    std::string GenerateStateFunctionName(const std::string& baseName, const std::string& state);
    
    // 错误处理
    void ReportError(const std::string& message);
    
    // 字符串处理
    std::string EscapeJS(const std::string& text);
    std::string QuoteString(const std::string& text);
};

} // namespace generator
} // namespace chtljs
} // namespace chtl