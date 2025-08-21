#include "CHTL/State.h"
#include "Utils/Logger.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

namespace CHTL {

// 状态转换规则定义
const std::unordered_map<ParseState, std::vector<ParseState>> StateMachine::transitionRules_ = {
    {ParseState::TopLevel, {
        ParseState::InElement, ParseState::InTemplate, ParseState::InCustom,
        ParseState::InOrigin, ParseState::InImport, ParseState::InNamespace,
        ParseState::InConfiguration, ParseState::InComment
    }},
    {ParseState::InElement, {
        ParseState::InAttribute, ParseState::InElement, ParseState::InText,
        ParseState::InStyle, ParseState::InScript, ParseState::InComment,
        ParseState::InExceptBlock
    }},
    {ParseState::InAttribute, {
        ParseState::InString, ParseState::InElement, ParseState::InAttribute
    }},
    {ParseState::InText, {
        ParseState::InString, ParseState::InComment
    }},
    {ParseState::InStyle, {
        ParseState::InStyleSelector, ParseState::InComment, ParseState::InOrigin
    }},
    {ParseState::InScript, {
        ParseState::InString, ParseState::InComment, ParseState::InOrigin
    }},
    {ParseState::InTemplate, {
        ParseState::InElement, ParseState::InStyle, ParseState::InSpecialization
    }},
    {ParseState::InCustom, {
        ParseState::InElement, ParseState::InStyle, ParseState::InSpecialization
    }},
    {ParseState::InStyleSelector, {
        ParseState::InStyleProperty, ParseState::InComment
    }},
    {ParseState::InStyleProperty, {
        ParseState::InStyleValue, ParseState::InComment
    }},
    {ParseState::InSpecialization, {
        ParseState::InElement, ParseState::InStyle, ParseState::InDeleteBlock,
        ParseState::InInsertBlock
    }}
};

// StateMachine内部实现
class StateMachine::Impl {
public:
    std::stack<StateContext> stateStack_;
    std::vector<std::string> stateHistory_;
    StateContext currentContext_;
    
    Impl() {
        currentContext_ = StateContext(ParseState::TopLevel);
        stateHistory_.push_back("TopLevel");
    }
};

// StateMachine实现
StateMachine::StateMachine() : impl_(std::make_unique<Impl>()) {}
StateMachine::~StateMachine() = default;

bool StateMachine::TransitionTo(ParseState newState, const std::string& contextName) {
    if (!CanTransitionTo(newState)) {
        LOG_ERROR("无效的状态转换: " + GetCurrentStateName() + " -> " + StateUtils::GetStateName(newState));
        return false;
    }
    
    impl_->currentContext_.state = newState;
    impl_->currentContext_.contextName = contextName;
    impl_->stateHistory_.push_back(StateUtils::GetStateName(newState));
    
    LOG_DEBUG("状态转换: " + GetCurrentStateName() + " -> " + StateUtils::GetStateName(newState));
    return true;
}

bool StateMachine::CanTransitionTo(ParseState newState) const {
    auto it = transitionRules_.find(impl_->currentContext_.state);
    if (it == transitionRules_.end()) {
        return false;
    }
    
    const auto& allowedStates = it->second;
    return std::find(allowedStates.begin(), allowedStates.end(), newState) != allowedStates.end();
}

ParseState StateMachine::GetCurrentState() const {
    return impl_->currentContext_.state;
}

StateContext StateMachine::GetCurrentContext() const {
    return impl_->currentContext_;
}

std::string StateMachine::GetCurrentStateName() const {
    return StateUtils::GetStateName(impl_->currentContext_.state);
}

bool StateMachine::IsInState(ParseState state) const {
    return impl_->currentContext_.state == state;
}

void StateMachine::PushState(ParseState state, const std::string& contextName) {
    impl_->stateStack_.push(impl_->currentContext_);
    impl_->currentContext_ = StateContext(state);
    impl_->currentContext_.contextName = contextName;
    impl_->currentContext_.nestingLevel = impl_->stateStack_.size();
    
    LOG_DEBUG("推入状态: " + StateUtils::GetStateName(state) + " (深度: " + 
              std::to_string(impl_->currentContext_.nestingLevel) + ")");
}

bool StateMachine::PopState() {
    if (impl_->stateStack_.empty()) {
        LOG_WARNING("状态栈为空，无法弹出");
        return false;
    }
    
    StateContext previousState = impl_->currentContext_;
    impl_->currentContext_ = impl_->stateStack_.top();
    impl_->stateStack_.pop();
    
    LOG_DEBUG("弹出状态: " + StateUtils::GetStateName(previousState.state) + 
              " -> " + StateUtils::GetStateName(impl_->currentContext_.state));
    return true;
}

size_t StateMachine::GetStateStackDepth() const {
    return impl_->stateStack_.size();
}

void StateMachine::SetContextProperty(const std::string& key, const std::string& value) {
    impl_->currentContext_.properties[key] = value;
}

std::string StateMachine::GetContextProperty(const std::string& key) const {
    auto it = impl_->currentContext_.properties.find(key);
    return it != impl_->currentContext_.properties.end() ? it->second : "";
}

bool StateMachine::HasContextProperty(const std::string& key) const {
    return impl_->currentContext_.properties.find(key) != impl_->currentContext_.properties.end();
}

bool StateMachine::ValidateStateTransition(ParseState from, ParseState to) const {
    auto it = transitionRules_.find(from);
    if (it == transitionRules_.end()) {
        return false;
    }
    
    const auto& allowedStates = it->second;
    return std::find(allowedStates.begin(), allowedStates.end(), to) != allowedStates.end();
}

bool StateMachine::IsTerminalState() const {
    return impl_->currentContext_.state == ParseState::Error;
}

bool StateMachine::IsErrorState() const {
    return impl_->currentContext_.state == ParseState::Error;
}

void StateMachine::Reset() {
    while (!impl_->stateStack_.empty()) {
        impl_->stateStack_.pop();
    }
    impl_->currentContext_ = StateContext(ParseState::TopLevel);
    impl_->stateHistory_.clear();
    impl_->stateHistory_.push_back("TopLevel");
}

void StateMachine::DumpStateStack() const {
    std::cout << "=== 状态栈 ===" << std::endl;
    std::cout << "当前状态: " << GetCurrentStateName() << std::endl;
    
    std::stack<StateContext> temp = impl_->stateStack_;
    std::vector<StateContext> states;
    
    while (!temp.empty()) {
        states.push_back(temp.top());
        temp.pop();
    }
    
    for (auto it = states.rbegin(); it != states.rend(); ++it) {
        std::cout << "  " << StateUtils::GetStateName(it->state) 
                  << " (" << it->contextName << ")" << std::endl;
    }
}

std::string StateMachine::GetStateHistory() const {
    std::stringstream ss;
    for (size_t i = 0; i < impl_->stateHistory_.size(); ++i) {
        if (i > 0) ss << " -> ";
        ss << impl_->stateHistory_[i];
    }
    return ss.str();
}

bool StateMachine::IsInGlobalScope() const {
    return impl_->currentContext_.state == ParseState::TopLevel ||
           impl_->currentContext_.state == ParseState::InNamespace ||
           impl_->currentContext_.state == ParseState::InConfiguration;
}

bool StateMachine::IsInLocalScope() const {
    return impl_->currentContext_.state == ParseState::InElement ||
           impl_->currentContext_.state == ParseState::InTemplate ||
           impl_->currentContext_.state == ParseState::InCustom;
}

bool StateMachine::IsInStyleContext() const {
    return impl_->currentContext_.state == ParseState::InStyle ||
           impl_->currentContext_.state == ParseState::InStyleSelector ||
           impl_->currentContext_.state == ParseState::InStyleProperty ||
           impl_->currentContext_.state == ParseState::InStyleValue;
}

bool StateMachine::IsInScriptContext() const {
    return impl_->currentContext_.state == ParseState::InScript;
}

bool StateMachine::IsInTemplateContext() const {
    return impl_->currentContext_.state == ParseState::InTemplate;
}

bool StateMachine::IsInCustomContext() const {
    return impl_->currentContext_.state == ParseState::InCustom;
}

// StateUtils实现
std::string StateUtils::GetStateName(ParseState state) {
    switch (state) {
        case ParseState::TopLevel: return "TopLevel";
        case ParseState::InElement: return "InElement";
        case ParseState::InAttribute: return "InAttribute";
        case ParseState::InText: return "InText";
        case ParseState::InStyle: return "InStyle";
        case ParseState::InScript: return "InScript";
        case ParseState::InTemplate: return "InTemplate";
        case ParseState::InCustom: return "InCustom";
        case ParseState::InOrigin: return "InOrigin";
        case ParseState::InImport: return "InImport";
        case ParseState::InNamespace: return "InNamespace";
        case ParseState::InConfiguration: return "InConfiguration";
        case ParseState::InStyleSelector: return "InStyleSelector";
        case ParseState::InStyleProperty: return "InStyleProperty";
        case ParseState::InStyleValue: return "InStyleValue";
        case ParseState::InSpecialization: return "InSpecialization";
        case ParseState::InInsertBlock: return "InInsertBlock";
        case ParseState::InDeleteBlock: return "InDeleteBlock";
        case ParseState::InExceptBlock: return "InExceptBlock";
        case ParseState::InString: return "InString";
        case ParseState::InComment: return "InComment";
        case ParseState::Error: return "Error";
        default: return "Unknown";
    }
}

bool StateUtils::IsBlockState(ParseState state) {
    return state == ParseState::InElement ||
           state == ParseState::InText ||
           state == ParseState::InStyle ||
           state == ParseState::InScript ||
           IsSpecialBlockState(state);
}

bool StateUtils::IsSpecialBlockState(ParseState state) {
    return state == ParseState::InTemplate ||
           state == ParseState::InCustom ||
           state == ParseState::InOrigin ||
           state == ParseState::InImport ||
           state == ParseState::InNamespace ||
           state == ParseState::InConfiguration;
}

bool StateUtils::IsStyleRelatedState(ParseState state) {
    return state == ParseState::InStyle ||
           state == ParseState::InStyleSelector ||
           state == ParseState::InStyleProperty ||
           state == ParseState::InStyleValue;
}

bool StateUtils::CanContainElements(ParseState state) {
    return state == ParseState::TopLevel ||
           state == ParseState::InElement ||
           state == ParseState::InTemplate ||
           state == ParseState::InCustom ||
           state == ParseState::InNamespace;
}

bool StateUtils::CanContainText(ParseState state) {
    return state == ParseState::InText ||
           state == ParseState::InElement;
}

bool StateUtils::RequiresBraces(ParseState state) {
    return IsBlockState(state);
}

} // namespace CHTL