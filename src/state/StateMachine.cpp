#include "../../include/StateMachine.h"
#include <algorithm>

namespace chtl {

// StateGuard实现
StateGuard::StateGuard(StateMachine& sm, const StateInfo& newState) 
    : stateMachine_(sm), previousState_(sm.getCurrentState()) {
    stateMachine_.pushState(newState);
}

StateGuard::~StateGuard() {
    stateMachine_.popState();
}

void StateGuard::updateState(const StateInfo& newState) {
    // 更新当前状态而不改变栈结构
    if (!stateMachine_.stateStack_.empty()) {
        stateMachine_.stateStack_.top() = newState;
        stateMachine_.updateStatistics();
    }
}

const StateInfo& StateGuard::getCurrentState() const {
    return stateMachine_.getCurrentState();
}

// StateMachine实现
StateMachine::StateMachine() : inErrorState_(false) {
    // 初始化默认状态
    StateInfo initialState(CompilerState::IDLE, ParseState::ROOT, ScopeType::GLOBAL, "初始状态");
    stateStack_.push(initialState);
    stateHistory_.push_back(initialState);
}

void StateMachine::pushState(const StateInfo& state) {
    stateStack_.push(state);
    stateHistory_.push_back(state);
    updateStatistics();
    statistics_.stateChanges++;
}

void StateMachine::popState() {
    if (stateStack_.size() > 1) { // 保留至少一个状态
        stateStack_.pop();
        updateStatistics();
    }
}

const StateInfo& StateMachine::getCurrentState() const {
    return stateStack_.top();
}

const StateInfo& StateMachine::getPreviousState() const {
    if (stateHistory_.size() >= 2) {
        return stateHistory_[stateHistory_.size() - 2];
    }
    return getCurrentState();
}

bool StateMachine::isInState(CompilerState state) const {
    return getCurrentState().compilerState == state;
}

bool StateMachine::isInParseState(ParseState state) const {
    return getCurrentState().parseState == state;
}

bool StateMachine::isInScope(ScopeType scope) const {
    return getCurrentState().scopeType == scope;
}

void StateMachine::setCompilerState(CompilerState state) {
    if (!stateStack_.empty()) {
        stateStack_.top().compilerState = state;
        updateStatistics();
    }
}

void StateMachine::setParseState(ParseState state) {
    if (!stateStack_.empty()) {
        stateStack_.top().parseState = state;
        updateStatistics();
    }
}

void StateMachine::setScopeType(ScopeType scope) {
    if (!stateStack_.empty()) {
        stateStack_.top().scopeType = scope;
        updateStatistics();
    }
}

void StateMachine::setContext(const String& context) {
    if (!stateStack_.empty()) {
        stateStack_.top().context = context;
    }
}

void StateMachine::setPosition(size_t line, size_t column) {
    if (!stateStack_.empty()) {
        stateStack_.top().line = line;
        stateStack_.top().column = column;
    }
}

void StateMachine::setFilename(const String& filename) {
    if (!stateStack_.empty()) {
        stateStack_.top().filename = filename;
    }
}

void StateMachine::enterScope(ScopeType scope, const String& name) {
    scopeStack_.push(name);
    setScopeType(scope);
    statistics_.scopeEnters++;
}

void StateMachine::exitScope() {
    if (!scopeStack_.empty()) {
        scopeStack_.pop();
        statistics_.scopeExits++;
    }
}

String StateMachine::getCurrentScopeName() const {
    return scopeStack_.empty() ? "" : scopeStack_.top();
}

std::vector<String> StateMachine::getScopeHierarchy() const {
    std::vector<String> hierarchy;
    std::stack<String> temp = scopeStack_;
    
    while (!temp.empty()) {
        hierarchy.push_back(temp.top());
        temp.pop();
    }
    
    std::reverse(hierarchy.begin(), hierarchy.end());
    return hierarchy;
}

void StateMachine::enterErrorState(const String& errorMessage) {
    lastError_ = errorMessage;
    inErrorState_ = true;
    setCompilerState(CompilerState::ERROR_RECOVERY);
    statistics_.errorStates++;
}

void StateMachine::exitErrorState() {
    inErrorState_ = false;
    lastError_.clear();
}

bool StateMachine::isInErrorState() const {
    return inErrorState_;
}

String StateMachine::getLastError() const {
    return lastError_;
}

std::vector<StateInfo> StateMachine::getStateHistory() const {
    return stateHistory_;
}

void StateMachine::clearHistory() {
    stateHistory_.clear();
    if (!stateStack_.empty()) {
        stateHistory_.push_back(stateStack_.top());
    }
}

size_t StateMachine::getHistorySize() const {
    return stateHistory_.size();
}

String StateMachine::getCurrentStateString() const {
    return stateToString(getCurrentState());
}

String StateMachine::getStateHistoryString() const {
    String result = "=== State History ===\n";
    
    for (size_t i = 0; i < stateHistory_.size(); ++i) {
        result += "[" + std::to_string(i) + "] " + stateToString(stateHistory_[i]) + "\n";
    }
    
    return result;
}

void StateMachine::dumpState() const {
    // 简化实现，避免iostream依赖
}

StateMachine::Statistics StateMachine::getStatistics() const {
    return statistics_;
}

void StateMachine::resetStatistics() {
    statistics_ = Statistics{};
}

String StateMachine::stateToString(const StateInfo& state) const {
    String result = "State{";
    result += "compiler=" + StateUtils::compilerStateToString(state.compilerState);
    result += ", parse=" + StateUtils::parseStateToString(state.parseState);
    result += ", scope=" + StateUtils::scopeTypeToString(state.scopeType);
    result += ", context=\"" + state.context + "\"";
    result += ", pos=" + std::to_string(state.line) + ":" + std::to_string(state.column);
    if (!state.filename.empty()) {
        result += ", file=\"" + state.filename + "\"";
    }
    result += "}";
    return result;
}

void StateMachine::updateStatistics() {
    if (!stateStack_.empty()) {
        statistics_.averageStateDepth = 
            (statistics_.averageStateDepth * (statistics_.stateChanges - 1) + stateStack_.size()) 
            / statistics_.stateChanges;
    }
}

// StateUtils命名空间实现
namespace StateUtils {

String compilerStateToString(CompilerState state) {
    switch (state) {
        case CompilerState::IDLE: return "IDLE";
        case CompilerState::SCANNING: return "SCANNING";
        case CompilerState::LEXING: return "LEXING";
        case CompilerState::PARSING: return "PARSING";
        case CompilerState::AST_BUILDING: return "AST_BUILDING";
        case CompilerState::SEMANTIC_ANALYSIS: return "SEMANTIC_ANALYSIS";
        case CompilerState::CODE_GENERATION: return "CODE_GENERATION";
        case CompilerState::ERROR_RECOVERY: return "ERROR_RECOVERY";
        case CompilerState::COMPLETED: return "COMPLETED";
        default: return "UNKNOWN";
    }
}

String parseStateToString(ParseState state) {
    switch (state) {
        case ParseState::ROOT: return "ROOT";
        case ParseState::ELEMENT: return "ELEMENT";
        case ParseState::ATTRIBUTE: return "ATTRIBUTE";
        case ParseState::TEXT_NODE: return "TEXT_NODE";
        case ParseState::STYLE_BLOCK: return "STYLE_BLOCK";
        case ParseState::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case ParseState::TEMPLATE_DECL: return "TEMPLATE_DECL";
        case ParseState::CUSTOM_DECL: return "CUSTOM_DECL";
        case ParseState::ORIGIN_DECL: return "ORIGIN_DECL";
        case ParseState::IMPORT_DECL: return "IMPORT_DECL";
        case ParseState::NAMESPACE_DECL: return "NAMESPACE_DECL";
        case ParseState::CONFIGURATION_DECL: return "CONFIGURATION_DECL";
        case ParseState::EXPRESSION: return "EXPRESSION";
        case ParseState::STATEMENT: return "STATEMENT";
        case ParseState::COMMENT: return "COMMENT";
        default: return "UNKNOWN";
    }
}

String scopeTypeToString(ScopeType scope) {
    switch (scope) {
        case ScopeType::GLOBAL: return "GLOBAL";
        case ScopeType::NAMESPACE: return "NAMESPACE";
        case ScopeType::TEMPLATE: return "TEMPLATE";
        case ScopeType::CUSTOM: return "CUSTOM";
        case ScopeType::ELEMENT: return "ELEMENT";
        case ScopeType::STYLE: return "STYLE";
        case ScopeType::SCRIPT: return "SCRIPT";
        case ScopeType::FUNCTION: return "FUNCTION";
        case ScopeType::BLOCK: return "BLOCK";
        default: return "UNKNOWN";
    }
}

bool isValidStateTransition(CompilerState from, CompilerState to) {
    // 基本的状态转换验证
    switch (from) {
        case CompilerState::IDLE:
            return to == CompilerState::SCANNING || to == CompilerState::ERROR_RECOVERY;
        case CompilerState::SCANNING:
            return to == CompilerState::LEXING || to == CompilerState::ERROR_RECOVERY;
        case CompilerState::LEXING:
            return to == CompilerState::PARSING || to == CompilerState::ERROR_RECOVERY;
        case CompilerState::PARSING:
            return to == CompilerState::AST_BUILDING || to == CompilerState::ERROR_RECOVERY;
        case CompilerState::AST_BUILDING:
            return to == CompilerState::SEMANTIC_ANALYSIS || to == CompilerState::ERROR_RECOVERY;
        case CompilerState::SEMANTIC_ANALYSIS:
            return to == CompilerState::CODE_GENERATION || to == CompilerState::ERROR_RECOVERY;
        case CompilerState::CODE_GENERATION:
            return to == CompilerState::COMPLETED || to == CompilerState::ERROR_RECOVERY;
        case CompilerState::ERROR_RECOVERY:
            return true; // 错误恢复可以转换到任何状态
        case CompilerState::COMPLETED:
            return to == CompilerState::IDLE; // 完成后可以重新开始
        default:
            return false;
    }
}

bool isValidParseTransition(ParseState from, ParseState to) {
    // 解析状态转换通常比较灵活，这里提供基本验证
    return true; // 暂时允许所有转换
}

bool isValidScopeNesting(ScopeType parent, ScopeType child) {
    // 作用域嵌套规则
    switch (parent) {
        case ScopeType::GLOBAL:
            return true; // 全局作用域可以包含任何子作用域
        case ScopeType::NAMESPACE:
            return child != ScopeType::GLOBAL; // 命名空间不能包含全局作用域
        case ScopeType::ELEMENT:
            return child == ScopeType::STYLE || child == ScopeType::SCRIPT || child == ScopeType::ELEMENT;
        case ScopeType::STYLE:
        case ScopeType::SCRIPT:
            return child == ScopeType::BLOCK || child == ScopeType::FUNCTION;
        default:
            return true;
    }
}

} // namespace StateUtils

} // namespace chtl