#pragma once

#include "chtl_js/ast/CHTLJSASTNode.h"

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * Script块节点
 * 表示script { }块中的CHTL JS代码
 * 注意：这是CHTL JS的script块，不同于CHTL的ScriptBlockNode
 */
class ScriptBlockNode : public CHTLJSASTNode {
public:
    ScriptBlockNode() : CHTLJSASTNode(CHTLJSNodeType::SCRIPT_BLOCK) {}
    
    void Accept(CHTLJSVisitor* visitor) override;
    
    // 是否是局部脚本块
    void SetIsLocal(bool isLocal) { m_IsLocal = isLocal; }
    bool IsLocal() const { return m_IsLocal; }
    
    // 所属元素（如果是局部脚本）
    void SetOwnerElement(const std::string& element) { m_OwnerElement = element; }
    const std::string& GetOwnerElement() const { return m_OwnerElement; }
    
    // Script块包含的语句通过子节点管理
    
private:
    bool m_IsLocal = true;
    std::string m_OwnerElement;
};

} // namespace ast
} // namespace chtljs
} // namespace chtl