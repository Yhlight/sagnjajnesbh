#!/bin/bash

echo "Starting compilation error fixes..."

# Fix 1: CommentNode.h - Fix the comment issue
echo "Fixing CommentNode.h..."
cat > /workspace/chtl-compiler/include/chtl/ast/CommentNode.h << 'EOF'
#pragma once

#include "ASTNode.h"
#include <string>

namespace chtl {
namespace ast {

/**
 * CommentNode - represents comment nodes in CHTL
 * Supports three comment types: single-line (//), multi-line (/* ... */), and HTML-style (--)
 */
class CommentNode : public ASTNode {
public:
    enum class CommentType {
        SingleLine,    // //
        MultiLine,     // /* */
        HTMLStyle      // <!--  -->
    };

    CommentNode(CommentType type, const std::string& content)
        : ASTNode(ASTNodeType::Comment), m_Type(type), m_Content(content) {}

    void Accept(ASTVisitor* visitor) override;

    CommentType GetCommentType() const { return m_Type; }
    const std::string& GetContent() const { return m_Content; }

private:
    CommentType m_Type;
    std::string m_Content;
};

} // namespace ast
} // namespace chtl
EOF

# Fix 2: ConstraintChecker.cpp - Make GetSyntaxElement public in ConstraintSystem.h
echo "Fixing ConstraintSystem.h..."
sed -i '0,/private:/s/private:/public:\n    static SyntaxElement GetSyntaxElement(const ASTNode* node);\n\nprivate:/' /workspace/chtl-compiler/include/chtl/constraint/ConstraintSystem.h 2>/dev/null || true

# Fix 3: FileUtils - Fix ReadFile signature
echo "Checking FileUtils.h for ReadFile signature..."
grep -n "ReadFile" /workspace/chtl-compiler/include/utils/FileUtils.h || true

# Fix 4: Add missing ParameterListNode to CHTL JS AST
echo "Adding ParameterListNode to CHTL JS AST..."
cat > /workspace/chtl-compiler/include/chtl_js/ast/ParameterListNode.h << 'EOF'
#pragma once

#include "CHTLJSASTNode.h"
#include <vector>
#include <memory>

namespace chtl {
namespace chtljs {
namespace ast {

/**
 * ParameterListNode - represents function parameter list
 */
class ParameterListNode : public CHTLJSASTNode {
public:
    ParameterListNode() : CHTLJSASTNode(CHTLJSASTNodeType::ParameterList) {}

    void Accept(CHTLJSVisitor* visitor) override {
        visitor->VisitParameterList(this);
    }

    void AddParameter(std::shared_ptr<CHTLJSASTNode> param) {
        AddChild(param);
    }

    const std::vector<std::shared_ptr<CHTLJSASTNode>>& GetParameters() const {
        return GetChildren();
    }
};

} // namespace ast
} // namespace chtljs
} // namespace chtl
EOF

# Fix 5: Add VisitParameterList to CHTLJSVisitor
echo "Updating CHTLJSASTNode.h to add VisitParameterList..."
sed -i '/VisitFunctionWithState/a\    virtual void VisitParameterList(class ParameterListNode* node) = 0;' /workspace/chtl-compiler/include/chtl_js/ast/CHTLJSASTNode.h 2>/dev/null || true

# Fix 6: Include ParameterListNode in FunctionNodes.h
echo "Adding include for ParameterListNode in FunctionNodes.h..."
sed -i '/#include "CHTLJSASTNode.h"/a\#include "ParameterListNode.h"' /workspace/chtl-compiler/include/chtl_js/ast/FunctionNodes.h 2>/dev/null || true

echo "Fixes applied. Now try compiling again."