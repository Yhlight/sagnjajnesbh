#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 注释节点
 * 支持三种注释类型：//, /* */, --
 */
class CommentNode : public ASTNode {
public:
    enum CommentType {
        SINGLE_LINE,    // //
        MULTI_LINE,     // /* */
        GENERATOR       // --
    };
    
    CommentNode(CommentType commentType, const std::string& content)
        : ASTNode(GetNodeType(commentType), content)
        , m_CommentType(commentType) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    CommentType GetCommentType() const { return m_CommentType; }
    
    // 是否是生成器注释（会被包含在生成的HTML中）
    bool IsGeneratorComment() const { return m_CommentType == GENERATOR; }
    
private:
    CommentType m_CommentType;
    
    static ASTNodeType GetNodeType(CommentType type) {
        switch (type) {
            case SINGLE_LINE: return ASTNodeType::COMMENT_SINGLE;
            case MULTI_LINE: return ASTNodeType::COMMENT_MULTI;
            case GENERATOR: return ASTNodeType::COMMENT_GENERATOR;
            default: return ASTNodeType::COMMENT_SINGLE;
        }
    }
};

} // namespace ast
} // namespace chtl