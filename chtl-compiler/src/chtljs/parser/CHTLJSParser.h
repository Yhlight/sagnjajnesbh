#pragma once

#include "../lexer/CHTLJSLexer.h"
#include "../ast/CHTLJSASTNodes.h"
#include <memory>
#include <vector>
#include <string>

namespace chtljs {

/**
 * CHTL JS 解析器
 * 完全独立的解析器，专门处理CHTL JS语法
 */
class CHTLJSParser {
public:
    CHTLJSParser();
    
    /**
     * 解析CHTL JS脚本块内容
     */
    std::shared_ptr<ast::ScriptBlockNode> ParseScript(const std::string& content);
    
    /**
     * 获取错误信息
     */
    const std::vector<std::string>& GetErrors() const { return m_Errors; }
    bool HasErrors() const { return !m_Errors.empty(); }
    
private:
    // 解析方法
    std::shared_ptr<ast::CHTLJSNode> ParseStatement();
    std::shared_ptr<ast::SelectorNode> ParseSelector();
    std::shared_ptr<ast::CHTLJSNode> ParseMethodChain(std::shared_ptr<ast::CHTLJSNode> target);
    std::shared_ptr<ast::ListenNode> ParseListen();
    std::shared_ptr<ast::DelegateNode> ParseDelegate();
    std::shared_ptr<ast::AnimateNode> ParseAnimate();
    std::shared_ptr<ast::VirNode> ParseVir();
    
    // 辅助方法
    bool Match(CHTLJSTokenType type);
    bool Check(CHTLJSTokenType type) const;
    CHTLJSToken Advance();
    CHTLJSToken Peek() const;
    bool IsAtEnd() const;
    
    void ReportError(const std::string& message);
    void Synchronize();
    
    // 成员变量
    std::vector<CHTLJSToken> m_Tokens;
    size_t m_Current;
    std::vector<std::string> m_Errors;
};

} // namespace chtljs