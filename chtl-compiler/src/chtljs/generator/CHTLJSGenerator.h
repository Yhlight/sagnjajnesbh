#pragma once

#include "../ast/CHTLJSASTNodes.h"
#include <string>
#include <sstream>
#include <memory>

namespace chtljs {

/**
 * CHTL JS 代码生成器
 * 将CHTL JS AST转换为JavaScript代码
 */
class CHTLJSGenerator : public ast::CHTLJSVisitor {
public:
    CHTLJSGenerator();
    
    /**
     * 生成JavaScript代码
     */
    std::string Generate(std::shared_ptr<ast::ScriptBlockNode> root);
    
    // 访问者方法
    void VisitScriptBlock(ast::ScriptBlockNode* node) override;
    void VisitSelector(ast::SelectorNode* node) override;
    void VisitMethodCall(ast::MethodCallNode* node) override;
    void VisitListen(ast::ListenNode* node) override;
    void VisitDelegate(ast::DelegateNode* node) override;
    void VisitAnimate(ast::AnimateNode* node) override;
    void VisitVir(ast::VirNode* node) override;
    
private:
    // 辅助方法
    void Write(const std::string& text);
    void WriteLine(const std::string& text = "");
    void WriteIndent();
    void IncreaseIndent() { m_IndentLevel++; }
    void DecreaseIndent() { if (m_IndentLevel > 0) m_IndentLevel--; }
    
    // 生成选择器查询代码
    std::string GenerateSelectorQuery(ast::SelectorNode* selector);
    
    // 成员变量
    std::stringstream m_Output;
    int m_IndentLevel;
    bool m_PrettyPrint;
};

} // namespace chtljs