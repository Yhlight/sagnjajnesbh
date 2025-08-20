#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 脚本块节点
 * 表示 script { } 块
 * 注意：脚本内容由CHTL JS和JavaScript编译器处理
 */
class ScriptBlockNode : public ASTNode {
public:
    ScriptBlockNode() : ASTNode(ASTNodeType::SCRIPT_BLOCK) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 是否是局部脚本块（在元素内部）
    void SetIsLocal(bool isLocal) { m_IsLocal = isLocal; }
    bool IsLocal() const { return m_IsLocal; }
    
    // 脚本内容（原始内容，需要进一步解析）
    void SetContent(const std::string& content) { m_Content = content; }
    const std::string& GetContent() const { return m_Content; }
    
    // 脚本片段（由扫描器分割后的片段）
    struct ScriptFragment {
        enum Type {
            CHTL_JS,      // CHTL JS代码
            JAVASCRIPT,   // 纯JavaScript代码
            MIXED         // 混合内容
        };
        
        Type type;
        std::string content;
        size_t line;
        size_t column;
        
        ScriptFragment(Type t, const std::string& c, size_t l = 0, size_t col = 0)
            : type(t), content(c), line(l), column(col) {}
    };
    
    void AddFragment(const ScriptFragment& fragment) { m_Fragments.push_back(fragment); }
    const std::vector<ScriptFragment>& GetFragments() const { return m_Fragments; }
    
private:
    bool m_IsLocal = true;
    std::string m_Content;
    std::vector<ScriptFragment> m_Fragments;
};

} // namespace ast
} // namespace chtl