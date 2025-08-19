#include "compilers/chtl_js/chtl_js_ast.h"
#include "compilers/chtl_js/chtl_js_code_generator.h"

namespace chtl {

void CHTLJSProgramNode::accept(CHTLJSASTVisitor* visitor) {
    visitor->visitProgram(this);
}

void EnhancedSelectorNode::accept(CHTLJSASTVisitor* visitor) {
    visitor->visitEnhancedSelector(this);
}

void VirtualObjectNode::accept(CHTLJSASTVisitor* visitor) {
    visitor->visitVirtualObject(this);
}

void ArrowCallNode::accept(CHTLJSASTVisitor* visitor) {
    visitor->visitArrowCall(this);
}

void ListenCallNode::accept(CHTLJSASTVisitor* visitor) {
    visitor->visitListenCall(this);
}

void DelegateCallNode::accept(CHTLJSASTVisitor* visitor) {
    visitor->visitDelegateCall(this);
}

void AnimateCallNode::accept(CHTLJSASTVisitor* visitor) {
    visitor->visitAnimateCall(this);
}

void RegularJSNode::accept(CHTLJSASTVisitor* visitor) {
    visitor->visitRegularJS(this);
}

} // namespace chtl