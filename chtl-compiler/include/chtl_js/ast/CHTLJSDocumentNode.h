#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * CHTL JS文档节点
 * CHTL JS AST的根节点
 */
class CHTLJSDocumentNode : public CHTLJSASTNode {
public:
    CHTLJSDocumentNode() : CHTLJSASTNode(CHTLJSNodeType::DOCUMENT) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 文档来源（通常是从script块提取）
    void SetSource(const std::string& source) { m_Source = source; }
    const std::string& GetSource() const { return m_Source; }
    
    // 是否是局部脚本（在元素内部）
    void SetIsLocal(bool isLocal) { m_IsLocal = isLocal; }
    bool IsLocal() const { return m_IsLocal; }
    
private:
    std::string m_Source;
    bool m_IsLocal = true;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl