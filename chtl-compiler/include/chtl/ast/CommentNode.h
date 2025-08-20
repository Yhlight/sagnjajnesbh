#pragma once

#include "ASTNode.h"
#include <string>

namespace chtl {
namespace ast {

/**
 * CommentNode - represents comment nodes in CHTL
 * Supports three comment types: single-line (//), multi-line (slash-star), and HTML-style (--)
 */
class CommentNode : public ASTNode {
public:
    enum class CommentType {
        SingleLine,    // //
        MultiLine,     // /* */
        HTMLStyle      // <!--  -->
    };

    CommentNode(CommentType type, const std::string& content)
        : ASTNode(GetNodeType(type)), m_Type(type), m_Content(content) {}

    void Accept(ASTVisitor* visitor) override;

    CommentType GetCommentType() const { return m_Type; }
    const std::string& GetContent() const { return m_Content; }

private:
    static ASTNodeType GetNodeType(CommentType type) {
        switch (type) {
            case CommentType::SingleLine: return ASTNodeType::COMMENT_SINGLE;
            case CommentType::MultiLine: return ASTNodeType::COMMENT_MULTI;
            case CommentType::HTMLStyle: return ASTNodeType::COMMENT_GENERATOR;
            default: return ASTNodeType::COMMENT_SINGLE;
        }
    }
    
    CommentType m_Type;
    std::string m_Content;
};

} // namespace ast
} // namespace chtl
