#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 文档节点
 * AST的根节点，表示整个CHTL文档
 */
class DocumentNode : public ASTNode {
public:
    DocumentNode() : ASTNode(ASTNodeType::DOCUMENT) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 文档特有方法
    void SetFilename(const std::string& filename) { m_Filename = filename; }
    const std::string& GetFilename() const { return m_Filename; }
    
private:
    std::string m_Filename;
};

} // namespace ast
} // namespace chtl