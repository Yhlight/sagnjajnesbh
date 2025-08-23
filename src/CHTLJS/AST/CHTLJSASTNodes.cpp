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

// INeverAwayBlockNode实现已移除 - iNeverAway是CJMOD扩展功能，不属于CHTL JS核心语法
// 语法文档第1485行明确说明iNeverAway属于CJMOD扩展

// FunctionDefinitionNode实现已移除 - function是JavaScript语法，不属于CHTL JS
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

// FunctionDefinitionNode的所有方法实现已移除 - function是JavaScript语法

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

// ObjectLiteralNode实现已移除 - 对象字面量是JavaScript语法，不属于CHTL JS
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

// 所有JavaScript语法节点实现已移除：
// - ArrayLiteralNode - 数组字面量是JavaScript语法
// - MethodCallNode - 方法调用是JavaScript语法
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

// 所有JavaScript语法和CJMOD扩展节点实现已移除：
// - MethodCallNode - 方法调用是JavaScript语法
// - VirtualMethodCallNode - 虚方法调用属于CJMOD扩展
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"
// 语法文档第1485行明确说明虚对象调用属于CJMOD扩展

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

// 所有JavaScript语法节点实现已移除：
// - VariableDeclarationNode - 变量声明是JavaScript语法
// - AssignmentExpressionNode - 赋值表达式是JavaScript语法  
// - PropertyAccessNode - 属性访问是JavaScript语法
// 语法文档第1100行明确说明"CHTL JS不支持JS的语法"

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
