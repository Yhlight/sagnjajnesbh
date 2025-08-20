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
