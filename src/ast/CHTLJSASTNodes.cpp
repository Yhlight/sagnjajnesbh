#include "../../include/CHTLJSASTNodes.h"
#include <sstream>

namespace chtl {

// CHTLJSASTNode 基类实现
void CHTLJSASTNode::addChild(std::shared_ptr<CHTLJSASTNode> child) {
    if (child) {
        children_.push_back(child);
    }
}

void CHTLJSASTNode::setAttribute(const String& key, const String& value) {
    attributes_[key] = value;
}

std::optional<String> CHTLJSASTNode::getAttribute(const String& key) const {
    auto it = attributes_.find(key);
    if (it != attributes_.end()) {
        return it->second;
    }
    return std::nullopt;
}

String CHTLJSASTNode::toString() const {
    std::ostringstream oss;
    oss << "JSNode(type=" << static_cast<int>(nodeType_) 
        << ", line=" << line_ 
        << ", column=" << column_ 
        << ", children=" << children_.size() << ")";
    return oss.str();
}

// EnhancedSelectorNode 实现
void EnhancedSelectorNode::accept(CHTLJSASTVisitor& visitor) {
    visitor.visit(*this);
}

// EventListenerNode 实现
void EventListenerNode::addEventHandler(const String& eventType, const String& handlerCode) {
    eventHandlers_[eventType] = handlerCode;
}

void EventListenerNode::accept(CHTLJSASTVisitor& visitor) {
    visitor.visit(*this);
}

// EventDelegateNode 实现
void EventDelegateNode::addEventHandler(const String& eventType, const String& handlerCode) {
    eventHandlers_[eventType] = handlerCode;
}

void EventDelegateNode::accept(CHTLJSASTVisitor& visitor) {
    visitor.visit(*this);
}

// AnimationNode 实现
void AnimationNode::accept(CHTLJSASTVisitor& visitor) {
    visitor.visit(*this);
}

// VirtualObjectNode 实现
void VirtualObjectNode::accept(CHTLJSASTVisitor& visitor) {
    visitor.visit(*this);
}

// ChainCallNode 实现
void ChainCallNode::addCall(const String& methodName, const StringList& arguments) {
    MethodCall call;
    call.methodName = methodName;
    call.arguments = arguments;
    callChain_.push_back(call);
}

void ChainCallNode::accept(CHTLJSASTVisitor& visitor) {
    visitor.visit(*this);
}

// VariableUseNode 实现
void VariableUseNode::accept(CHTLJSASTVisitor& visitor) {
    visitor.visit(*this);
}

// JSOriginNode 实现
void JSOriginNode::accept(CHTLJSASTVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace chtl