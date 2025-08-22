#include "CHTLJS/AST/CHTLJSASTNodes.h"
#include "CHTLJS/AST/CHTLJSASTVisitor.h"
#include <sstream>
#include <regex>

namespace CHTL {
namespace CHTLJS {
namespace AST {

// ASTNode基类实现
ASTNode::ASTNode(NodeType type, const Core::CHTLJSToken& token)
    : type_(type), token_(token), parent_(nullptr) {}

void ASTNode::AddChild(ASTNodePtr child) {
    if (child) {
        child->SetParent(this);
        children_.push_back(child);
    }
}

// RootNode实现
RootNode::RootNode() : ASTNode(NodeType::ROOT) {}

void RootNode::Accept(ASTVisitor& visitor) {
    visitor.VisitRootNode(*this);
}

ASTNodePtr RootNode::Clone() const {
    auto clone = std::make_shared<RootNode>();
    for (const auto& child : children_) {
        clone->AddChild(child->Clone());
    }
    return clone;
}

std::string RootNode::ToString() const {
    return "ROOT(" + std::to_string(children_.size()) + " children)";
}

// IdentifierNode实现
IdentifierNode::IdentifierNode(const std::string& name, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::IDENTIFIER, token), name_(name) {}

void IdentifierNode::Accept(ASTVisitor& visitor) {
    visitor.VisitIdentifierNode(*this);
}

ASTNodePtr IdentifierNode::Clone() const {
    return std::make_shared<IdentifierNode>(name_, token_);
}

std::string IdentifierNode::ToString() const {
    return "IDENTIFIER(" + name_ + ")";
}

// LiteralNode实现
LiteralNode::LiteralNode(LiteralType literalType, const std::string& value, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::LITERAL, token), literalType_(literalType), value_(value) {}

void LiteralNode::Accept(ASTVisitor& visitor) {
    visitor.VisitLiteralNode(*this);
}

ASTNodePtr LiteralNode::Clone() const {
    return std::make_shared<LiteralNode>(literalType_, value_, token_);
}

std::string LiteralNode::ToString() const {
    std::string typeStr;
    switch (literalType_) {
        case LiteralType::STRING: typeStr = "STRING"; break;
        case LiteralType::NUMBER: typeStr = "NUMBER"; break;
        case LiteralType::BOOLEAN: typeStr = "BOOLEAN"; break;
        case LiteralType::NULL_LITERAL: typeStr = "NULL"; break;
    }
    return "LITERAL(" + typeStr + ": " + value_ + ")";
}

// EnhancedSelectorNode实现
EnhancedSelectorNode::EnhancedSelectorNode(const std::string& selector, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::ENHANCED_SELECTOR, token), selector_(selector), index_(-1) {
    ParseSelector();
}

void EnhancedSelectorNode::Accept(ASTVisitor& visitor) {
    visitor.VisitEnhancedSelectorNode(*this);
}

ASTNodePtr EnhancedSelectorNode::Clone() const {
    return std::make_shared<EnhancedSelectorNode>(selector_, token_);
}

std::string EnhancedSelectorNode::ToString() const {
    return "ENHANCED_SELECTOR(" + selector_ + ")";
}

void EnhancedSelectorNode::ParseSelector() {
    // 移除{{和}}
    std::string content = selector_;
    if (content.length() >= 4 && content.substr(0, 2) == "{{" && content.substr(content.length() - 2) == "}}") {
        content = content.substr(2, content.length() - 4);
    }
    
    // 检查索引访问 button[0]
    std::regex indexRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\[(\d+)\])");
    std::smatch match;
    if (std::regex_match(content, match, indexRegex)) {
        selectorType_ = SelectorType::INDEXED;
        parsedSelector_ = match[1].str();
        index_ = std::stoi(match[2].str());
        return;
    }
    
    // 检查类选择器 .box
    if (content.length() > 1 && content[0] == '.') {
        selectorType_ = SelectorType::CLASS;
        parsedSelector_ = content.substr(1);
        return;
    }
    
    // 检查ID选择器 #box
    if (content.length() > 1 && content[0] == '#') {
        selectorType_ = SelectorType::ID;
        parsedSelector_ = content.substr(1);
        return;
    }
    
    // 检查复合选择器 .box button
    if (content.find(' ') != std::string::npos) {
        selectorType_ = SelectorType::COMPLEX;
        parsedSelector_ = content;
        return;
    }
    
    // 默认为标签选择器
    selectorType_ = SelectorType::TAG;
    parsedSelector_ = content;
}

// VirtualObjectNode实现
VirtualObjectNode::VirtualObjectNode(const std::string& name, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::VIRTUAL_OBJECT, token), name_(name) {}

void VirtualObjectNode::Accept(ASTVisitor& visitor) {
    visitor.VisitVirtualObjectNode(*this);
}

ASTNodePtr VirtualObjectNode::Clone() const {
    auto clone = std::make_shared<VirtualObjectNode>(name_, token_);
    if (assignment_) {
        clone->SetAssignment(assignment_->Clone());
    }
    return clone;
}

std::string VirtualObjectNode::ToString() const {
    return "VIRTUAL_OBJECT(" + name_ + ")";
}

// ListenBlockNode实现
ListenBlockNode::ListenBlockNode(const Core::CHTLJSToken& token)
    : ASTNode(NodeType::LISTEN_BLOCK, token) {}

void ListenBlockNode::Accept(ASTVisitor& visitor) {
    visitor.VisitListenBlockNode(*this);
}

ASTNodePtr ListenBlockNode::Clone() const {
    auto clone = std::make_shared<ListenBlockNode>(token_);
    for (const auto& handler : eventHandlers_) {
        clone->AddEventHandler(handler.first, handler.second->Clone());
    }
    return clone;
}

std::string ListenBlockNode::ToString() const {
    return "LISTEN_BLOCK(" + std::to_string(eventHandlers_.size()) + " handlers)";
}

void ListenBlockNode::AddEventHandler(const std::string& eventType, ASTNodePtr handler) {
    eventHandlers_[eventType] = handler;
}

// DelegateBlockNode实现
DelegateBlockNode::DelegateBlockNode(const Core::CHTLJSToken& token)
    : ASTNode(NodeType::DELEGATE_BLOCK, token) {}

void DelegateBlockNode::Accept(ASTVisitor& visitor) {
    visitor.VisitDelegateBlockNode(*this);
}

ASTNodePtr DelegateBlockNode::Clone() const {
    auto clone = std::make_shared<DelegateBlockNode>(token_);
    if (target_) {
        clone->SetTarget(target_->Clone());
    }
    for (const auto& handler : eventHandlers_) {
        clone->AddEventHandler(handler.first, handler.second->Clone());
    }
    return clone;
}

std::string DelegateBlockNode::ToString() const {
    return "DELEGATE_BLOCK(" + std::to_string(eventHandlers_.size()) + " handlers)";
}

void DelegateBlockNode::AddEventHandler(const std::string& eventType, ASTNodePtr handler) {
    eventHandlers_[eventType] = handler;
}

// AnimateBlockNode实现
AnimateBlockNode::AnimateBlockNode(const Core::CHTLJSToken& token)
    : ASTNode(NodeType::ANIMATE_BLOCK, token), duration_(0), loop_(1), delay_(0) {}

void AnimateBlockNode::Accept(ASTVisitor& visitor) {
    visitor.VisitAnimateBlockNode(*this);
}

ASTNodePtr AnimateBlockNode::Clone() const {
    auto clone = std::make_shared<AnimateBlockNode>(token_);
    if (target_) clone->SetTarget(target_->Clone());
    clone->SetDuration(duration_);
    clone->SetEasing(easing_);
    if (begin_) clone->SetBegin(begin_->Clone());
    if (end_) clone->SetEnd(end_->Clone());
    for (const auto& keyframe : keyframes_) {
        clone->AddKeyframe(keyframe->Clone());
    }
    clone->SetLoop(loop_);
    clone->SetDelay(delay_);
    if (callback_) clone->SetCallback(callback_->Clone());
    return clone;
}

std::string AnimateBlockNode::ToString() const {
    return "ANIMATE_BLOCK(duration: " + std::to_string(duration_) + "ms, " + 
           std::to_string(keyframes_.size()) + " keyframes)";
}

// INeverAwayBlockNode实现
INeverAwayBlockNode::INeverAwayBlockNode(const Core::CHTLJSToken& token)
    : ASTNode(NodeType::I_NEVER_AWAY_BLOCK, token) {}

void INeverAwayBlockNode::Accept(ASTVisitor& visitor) {
    visitor.VisitINeverAwayBlockNode(*this);
}

ASTNodePtr INeverAwayBlockNode::Clone() const {
    auto clone = std::make_shared<INeverAwayBlockNode>(token_);
    for (const auto& voidMethod : voidMethods_) {
        clone->AddVoidMethod(voidMethod.first, voidMethod.second->Clone());
    }
    for (const auto& customMethod : customMethods_) {
        clone->AddCustomMethod(customMethod.first, customMethod.second->Clone());
    }
    return clone;
}

std::string INeverAwayBlockNode::ToString() const {
    return "I_NEVER_AWAY_BLOCK(" + std::to_string(voidMethods_.size()) + " void methods, " +
           std::to_string(customMethods_.size()) + " custom methods)";
}

void INeverAwayBlockNode::AddVoidMethod(const std::string& state, ASTNodePtr method) {
    voidMethods_[state] = method;
}

void INeverAwayBlockNode::AddCustomMethod(const std::string& name, ASTNodePtr method) {
    customMethods_[name] = method;
}

// FunctionDefinitionNode实现
FunctionDefinitionNode::FunctionDefinitionNode(const std::string& name, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::FUNCTION_DEFINITION, token), name_(name) {}

void FunctionDefinitionNode::Accept(ASTVisitor& visitor) {
    visitor.VisitFunctionDefinitionNode(*this);
}

ASTNodePtr FunctionDefinitionNode::Clone() const {
    auto clone = std::make_shared<FunctionDefinitionNode>(name_, token_);
    for (const auto& param : parameters_) {
        clone->AddParameter(param);
    }
    if (body_) {
        clone->SetBody(body_->Clone());
    }
    return clone;
}

std::string FunctionDefinitionNode::ToString() const {
    return "FUNCTION_DEFINITION(" + name_ + ", " + std::to_string(parameters_.size()) + " params)";
}

// ArrowFunctionNode实现
ArrowFunctionNode::ArrowFunctionNode(const Core::CHTLJSToken& token)
    : ASTNode(NodeType::ARROW_FUNCTION, token) {}

void ArrowFunctionNode::Accept(ASTVisitor& visitor) {
    visitor.VisitArrowFunctionNode(*this);
}

ASTNodePtr ArrowFunctionNode::Clone() const {
    auto clone = std::make_shared<ArrowFunctionNode>(token_);
    for (const auto& param : parameters_) {
        clone->AddParameter(param);
    }
    if (body_) {
        clone->SetBody(body_->Clone());
    }
    return clone;
}

std::string ArrowFunctionNode::ToString() const {
    return "ARROW_FUNCTION(" + std::to_string(parameters_.size()) + " params)";
}

// ObjectLiteralNode实现
ObjectLiteralNode::ObjectLiteralNode(const Core::CHTLJSToken& token)
    : ASTNode(NodeType::OBJECT_LITERAL, token) {}

void ObjectLiteralNode::Accept(ASTVisitor& visitor) {
    visitor.VisitObjectLiteralNode(*this);
}

ASTNodePtr ObjectLiteralNode::Clone() const {
    auto clone = std::make_shared<ObjectLiteralNode>(token_);
    for (const auto& prop : properties_) {
        clone->AddProperty(prop.first, prop.second->Clone());
    }
    return clone;
}

std::string ObjectLiteralNode::ToString() const {
    return "OBJECT_LITERAL(" + std::to_string(properties_.size()) + " properties)";
}

void ObjectLiteralNode::AddProperty(const std::string& key, ASTNodePtr value) {
    properties_[key] = value;
}

// ArrayLiteralNode实现
ArrayLiteralNode::ArrayLiteralNode(const Core::CHTLJSToken& token)
    : ASTNode(NodeType::ARRAY_LITERAL, token) {}

void ArrayLiteralNode::Accept(ASTVisitor& visitor) {
    visitor.VisitArrayLiteralNode(*this);
}

ASTNodePtr ArrayLiteralNode::Clone() const {
    auto clone = std::make_shared<ArrayLiteralNode>(token_);
    for (const auto& element : elements_) {
        clone->AddElement(element->Clone());
    }
    return clone;
}

std::string ArrayLiteralNode::ToString() const {
    return "ARRAY_LITERAL(" + std::to_string(elements_.size()) + " elements)";
}

// MethodCallNode实现
MethodCallNode::MethodCallNode(ASTNodePtr object, const std::string& method, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::METHOD_CALL, token), object_(object), method_(method), isArrowCall_(false) {}

void MethodCallNode::Accept(ASTVisitor& visitor) {
    visitor.VisitMethodCallNode(*this);
}

ASTNodePtr MethodCallNode::Clone() const {
    auto clone = std::make_shared<MethodCallNode>(object_->Clone(), method_, token_);
    for (const auto& arg : arguments_) {
        clone->AddArgument(arg->Clone());
    }
    clone->SetIsArrowCall(isArrowCall_);
    return clone;
}

std::string MethodCallNode::ToString() const {
    std::string op = isArrowCall_ ? "->" : ".";
    return "METHOD_CALL(" + object_->ToString() + op + method_ + ", " + 
           std::to_string(arguments_.size()) + " args)";
}

// VirtualMethodCallNode实现
VirtualMethodCallNode::VirtualMethodCallNode(const std::string& objectName, const std::string& method, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::VIRTUAL_METHOD_CALL, token), objectName_(objectName), method_(method), isVoidStateCall_(false) {}

void VirtualMethodCallNode::Accept(ASTVisitor& visitor) {
    visitor.VisitVirtualMethodCallNode(*this);
}

ASTNodePtr VirtualMethodCallNode::Clone() const {
    auto clone = std::make_shared<VirtualMethodCallNode>(objectName_, method_, token_);
    for (const auto& arg : arguments_) {
        clone->AddArgument(arg->Clone());
    }
    clone->SetIsVoidStateCall(isVoidStateCall_);
    clone->SetVoidState(voidState_);
    return clone;
}

std::string VirtualMethodCallNode::ToString() const {
    std::string methodCall = isVoidStateCall_ ? 
                           "Void<" + voidState_ + ">" : method_;
    return "VIRTUAL_METHOD_CALL(" + objectName_ + "->" + methodCall + ")";
}

// AnimationKeyframeNode实现
AnimationKeyframeNode::AnimationKeyframeNode(double time, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::ANIMATION_KEYFRAME, token), time_(time) {}

void AnimationKeyframeNode::Accept(ASTVisitor& visitor) {
    visitor.VisitAnimationKeyframeNode(*this);
}

ASTNodePtr AnimationKeyframeNode::Clone() const {
    auto clone = std::make_shared<AnimationKeyframeNode>(time_, token_);
    for (const auto& prop : cssProperties_) {
        clone->AddCSSProperty(prop.first, prop.second);
    }
    return clone;
}

std::string AnimationKeyframeNode::ToString() const {
    return "ANIMATION_KEYFRAME(at: " + std::to_string(time_) + ", " + 
           std::to_string(cssProperties_.size()) + " properties)";
}

void AnimationKeyframeNode::AddCSSProperty(const std::string& property, const std::string& value) {
    cssProperties_[property] = value;
}

// VariableDeclarationNode实现
VariableDeclarationNode::VariableDeclarationNode(DeclarationType type, const std::string& name, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::VARIABLE_DECLARATION, token), declarationType_(type), name_(name) {}

void VariableDeclarationNode::Accept(ASTVisitor& visitor) {
    visitor.VisitVariableDeclarationNode(*this);
}

ASTNodePtr VariableDeclarationNode::Clone() const {
    auto clone = std::make_shared<VariableDeclarationNode>(declarationType_, name_, token_);
    if (initializer_) {
        clone->SetInitializer(initializer_->Clone());
    }
    return clone;
}

std::string VariableDeclarationNode::ToString() const {
    std::string typeStr;
    switch (declarationType_) {
        case DeclarationType::CONST: typeStr = "const"; break;
        case DeclarationType::LET: typeStr = "let"; break;
        case DeclarationType::VAR: typeStr = "var"; break;
    }
    return "VARIABLE_DECLARATION(" + typeStr + " " + name_ + ")";
}

// AssignmentExpressionNode实现
AssignmentExpressionNode::AssignmentExpressionNode(ASTNodePtr left, ASTNodePtr right, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::ASSIGNMENT_EXPRESSION, token), left_(left), right_(right) {}

void AssignmentExpressionNode::Accept(ASTVisitor& visitor) {
    visitor.VisitAssignmentExpressionNode(*this);
}

ASTNodePtr AssignmentExpressionNode::Clone() const {
    return std::make_shared<AssignmentExpressionNode>(left_->Clone(), right_->Clone(), token_);
}

std::string AssignmentExpressionNode::ToString() const {
    return "ASSIGNMENT_EXPRESSION(" + left_->ToString() + " = " + right_->ToString() + ")";
}

// PropertyAccessNode实现
PropertyAccessNode::PropertyAccessNode(ASTNodePtr object, const std::string& property, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::PROPERTY_ACCESS, token), object_(object), property_(property) {}

void PropertyAccessNode::Accept(ASTVisitor& visitor) {
    visitor.VisitPropertyAccessNode(*this);
}

ASTNodePtr PropertyAccessNode::Clone() const {
    return std::make_shared<PropertyAccessNode>(object_->Clone(), property_, token_);
}

std::string PropertyAccessNode::ToString() const {
    return "PROPERTY_ACCESS(" + object_->ToString() + "." + property_ + ")";
}

// CommentNode实现
CommentNode::CommentNode(const std::string& content, const Core::CHTLJSToken& token)
    : ASTNode(NodeType::COMMENT, token), content_(content) {}

void CommentNode::Accept(ASTVisitor& visitor) {
    visitor.VisitCommentNode(*this);
}

ASTNodePtr CommentNode::Clone() const {
    return std::make_shared<CommentNode>(content_, token_);
}

std::string CommentNode::ToString() const {
    return "COMMENT(" + content_ + ")";
}

} // namespace AST
} // namespace CHTLJS
} // namespace CHTL
