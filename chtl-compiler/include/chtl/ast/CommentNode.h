#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * CommentNode - represents comment nodes in CHTL
 * Supports three comment types: single-line (//), multi-line (/* ... */), and HTML-style (--)
 */
class CommentNode : public ASTNode {
public:
    enum CommentType {
        SINGLE_LINE,    // //
        MULTI_LINE,     // /* ... */
        HTML_STYLE      // --
    };
    
    CommentNode(const std::string& content, CommentType type,
                size_t line = 0, size_t column = 0)
        : ASTNode(ASTNodeType::COMMENT, line, column)
        , m_Content(content)
        , m_CommentType(type) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    const std::string& GetContent() const { return m_Content; }
    CommentType GetCommentType() const { return m_CommentType; }
    
    std::string GetCommentTypeString() const {
        switch (m_CommentType) {
            case SINGLE_LINE: return "//";
            case MULTI_LINE: return "/* ... */";
            case HTML_STYLE: return "--";
            default: return "unknown";
        }
    }
    
private:
    std::string m_Content;
    CommentType m_CommentType;
};

} // namespace ast
} // namespace chtl