#include "../../include/common/State.h"

namespace chtl {

// StateMachine实现
StateMachine::StateMachine() 
    : context_(std::make_unique<StateContext>()) {
    initializeStateNames();
}

void StateMachine::initializeStateNames() {
    stateNames_[StateType::INITIAL] = "INITIAL";
    stateNames_[StateType::GLOBAL] = "GLOBAL";
    
    // CHTL状态
    stateNames_[StateType::CHTL_ELEMENT] = "CHTL_ELEMENT";
    stateNames_[StateType::CHTL_ATTRIBUTE] = "CHTL_ATTRIBUTE";
    stateNames_[StateType::CHTL_TEXT] = "CHTL_TEXT";
    stateNames_[StateType::CHTL_STYLE] = "CHTL_STYLE";
    stateNames_[StateType::CHTL_SCRIPT] = "CHTL_SCRIPT";
    stateNames_[StateType::CHTL_COMMENT] = "CHTL_COMMENT";
    
    // 模板和自定义状态
    stateNames_[StateType::TEMPLATE_DEFINITION] = "TEMPLATE_DEFINITION";
    stateNames_[StateType::CUSTOM_DEFINITION] = "CUSTOM_DEFINITION";
    stateNames_[StateType::ORIGIN_BLOCK] = "ORIGIN_BLOCK";
    
    // 配置和导入状态
    stateNames_[StateType::CONFIGURATION_BLOCK] = "CONFIGURATION_BLOCK";
    stateNames_[StateType::IMPORT_STATEMENT] = "IMPORT_STATEMENT";
    stateNames_[StateType::NAMESPACE_BLOCK] = "NAMESPACE_BLOCK";
    
    // CSS相关状态
    stateNames_[StateType::CSS_RULE] = "CSS_RULE";
    stateNames_[StateType::CSS_SELECTOR] = "CSS_SELECTOR";
    stateNames_[StateType::CSS_DECLARATION] = "CSS_DECLARATION";
    stateNames_[StateType::CSS_VALUE] = "CSS_VALUE";
    
    // JavaScript相关状态
    stateNames_[StateType::JS_CODE] = "JS_CODE";
    stateNames_[StateType::JS_STRING] = "JS_STRING";
    stateNames_[StateType::JS_COMMENT] = "JS_COMMENT";
    
    // CHTL JS特殊状态
    stateNames_[StateType::CHTLJS_SELECTOR] = "CHTLJS_SELECTOR";
    stateNames_[StateType::CHTLJS_VIR] = "CHTLJS_VIR";
    stateNames_[StateType::CHTLJS_FUNCTION_CALL] = "CHTLJS_FUNCTION_CALL";
    
    // 字符串状态
    stateNames_[StateType::STRING_SINGLE] = "STRING_SINGLE";
    stateNames_[StateType::STRING_DOUBLE] = "STRING_DOUBLE";
    stateNames_[StateType::STRING_UNQUOTED] = "STRING_UNQUOTED";
    
    // 错误状态
    stateNames_[StateType::ERROR] = "ERROR";
}

void StateMachine::addTransition(const StateTransition& transition) {
    transitions_.push_back(transition);
}

void StateMachine::addTransition(StateType from, StateType to, StateCondition condition) {
    transitions_.emplace_back(from, to, condition);
}

bool StateMachine::transition(StateType targetState) {
    StateType currentState = context_->getCurrentState();
    
    // 查找匹配的转换
    for (const auto& trans : transitions_) {
        if (trans.fromState == currentState && trans.toState == targetState) {
            // 检查条件
            if (!trans.condition || trans.condition(*context_)) {
                // 执行退出动作
                if (trans.exitAction) {
                    trans.exitAction(*context_);
                }
                
                // 转换状态
                context_->replaceState(targetState);
                
                // 执行进入动作
                if (trans.enterAction) {
                    trans.enterAction(*context_);
                }
                
                return true;
            }
        }
    }
    
    // 如果没有找到特定的转换规则，尝试直接转换
    if (canTransition(currentState, targetState)) {
        context_->replaceState(targetState);
        return true;
    }
    
    return false;
}

bool StateMachine::autoTransition() {
    StateType currentState = context_->getCurrentState();
    
    // 查找所有可能的转换
    for (const auto& trans : transitions_) {
        if (trans.fromState == currentState) {
            // 检查条件
            if (!trans.condition || trans.condition(*context_)) {
                return transition(trans.toState);
            }
        }
    }
    
    return false;
}

std::string StateMachine::getStateName(StateType state) const {
    auto it = stateNames_.find(state);
    return it != stateNames_.end() ? it->second : "UNKNOWN";
}

void StateMachine::reset() {
    context_ = std::make_unique<StateContext>();
}

bool StateMachine::canTransition(StateType from, StateType to) const {
    // 基本的状态转换规则
    // 这里可以定义哪些状态转换是合法的
    
    // 从INITIAL可以转到任何状态
    if (from == StateType::INITIAL) {
        return true;
    }
    
    // 从ERROR状态只能转到INITIAL
    if (from == StateType::ERROR) {
        return to == StateType::INITIAL;
    }
    
    // 字符串状态只能转到自己或结束
    if (isInStringContext(*context_)) {
        return to == from || to == StateType::GLOBAL || 
               to == StateType::CHTL_ELEMENT || to == StateType::CSS_DECLARATION;
    }
    
    // 其他情况，暂时允许所有转换
    // 后续可以根据需要添加更多限制
    return true;
}

} // namespace chtl