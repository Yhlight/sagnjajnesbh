#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * JavaScript代码片段节点
 * 表示纯JavaScript代码，不包含CHTL JS语法
 * 这些代码将直接传递给JavaScript编译器处理
 */
class JSCodeFragmentNode : public CHTLJSASTNode {
public:
    JSCodeFragmentNode(const std::string& code)
        : CHTLJSASTNode(CHTLJSNodeType::JS_CODE_FRAGMENT, code) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // JavaScript代码内容
    const std::string& GetCode() const { return m_Value; }
    
    // 代码片段的上下文（在什么情况下出现）
    enum Context {
        GENERAL,        // 一般JavaScript代码
        FUNCTION_BODY,  // 函数体内的JavaScript
        EVENT_HANDLER,  // 事件处理器中的JavaScript
        EXPRESSION      // 表达式上下文
    };
    
    void SetContext(Context ctx) { m_Context = ctx; }
    Context GetContext() const { return m_Context; }
    
    // 是否需要分号
    void SetNeedsSemicolon(bool needs) { m_NeedsSemicolon = needs; }
    bool NeedsSemicolon() const { return m_NeedsSemicolon; }
    
private:
    Context m_Context = GENERAL;
    bool m_NeedsSemicolon = true;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl