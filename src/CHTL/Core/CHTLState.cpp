#include "CHTL/Core/CHTLState.h"
#include "Utils/ErrorHandler.h"
#include <sstream>
#include <algorithm>

namespace CHTL {
namespace Core {

CHTLState::CHTLState() : currentState_(CompileState::INITIAL) {
    InitializeDefaultTransitions();
}

CompileState CHTLState::GetPreviousState() const {
    if (stateHistory_.size() >= 2) {
        return stateHistory_[stateHistory_.size() - 2];
    }
    return CompileState::INITIAL;
}

bool CHTLState::TransitionTo(CompileState newState, const CHTLToken& token) {
    // 检查是否可以转换
    if (!CanTransitionTo(newState, token)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "无效的状态转换: " + GetStateName(currentState_) + " -> " + GetStateName(newState),
            token.GetFileName(), token.GetLine(), token.GetColumn()
        );
        return false;
    }
    
    // 执行转换副作用
    ExecuteTransitionSideEffects(currentState_, newState, token);
    
    // 更新状态
    CompileState previousState = currentState_;
    currentState_ = newState;
    
    // 记录历史
    stateHistory_.push_back(newState);
    
    // 更新计数器
    stateCounters_[newState]++;
    
    // 更新上下文
    UpdateContext(newState, token);
    
    return true;
}

bool CHTLState::CanTransitionTo(CompileState targetState, const CHTLToken& token) const {
    // 检查是否有适用的转换规则
    const StateTransition* transition = FindTransition(currentState_, targetState, token);
    if (!transition) {
        return false;
    }
    
    // 验证转换条件
    return ValidateTransition(currentState_, targetState, token);
}

void CHTLState::PushState(CompileState state, const StateContext& context) {
    stateStack_.push(currentState_);
    contextStack_.push(currentContext_);
    currentState_ = state;
    currentContext_ = context;
    currentContext_.nestingLevel++;
}

CompileState CHTLState::PopState() {
    if (stateStack_.empty()) {
        return CompileState::INITIAL;
    }
    
    CompileState previousState = currentState_;
    currentState_ = stateStack_.top();
    stateStack_.pop();
    
    if (!contextStack_.empty()) {
        currentContext_ = contextStack_.top();
        contextStack_.pop();
    }
    
    return previousState;
}

bool CHTLState::IsInAnyState(const std::vector<CompileState>& states) const {
    return std::find(states.begin(), states.end(), currentState_) != states.end();
}

void CHTLState::Reset() {
    currentState_ = CompileState::INITIAL;
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    while (!contextStack_.empty()) {
        contextStack_.pop();
    }
    currentContext_ = StateContext();
    stateHistory_.clear();
    stateCounters_.clear();
}

std::string CHTLState::GetStateName(CompileState state) {
    switch (state) {
        case CompileState::INITIAL: return "INITIAL";
        case CompileState::PARSING_TEMPLATE: return "PARSING_TEMPLATE";
        case CompileState::PARSING_CUSTOM: return "PARSING_CUSTOM";
        case CompileState::PARSING_ORIGIN: return "PARSING_ORIGIN";
        case CompileState::PARSING_IMPORT: return "PARSING_IMPORT";
        case CompileState::PARSING_NAMESPACE: return "PARSING_NAMESPACE";
        case CompileState::PARSING_CONFIGURATION: return "PARSING_CONFIGURATION";
        case CompileState::PARSING_HTML_ELEMENT: return "PARSING_HTML_ELEMENT";
        case CompileState::PARSING_ATTRIBUTES: return "PARSING_ATTRIBUTES";
        case CompileState::PARSING_TEXT_NODE: return "PARSING_TEXT_NODE";
        case CompileState::PARSING_STYLE_BLOCK: return "PARSING_STYLE_BLOCK";
        case CompileState::PARSING_SCRIPT_BLOCK: return "PARSING_SCRIPT_BLOCK";
        case CompileState::PARSING_CSS_SELECTOR: return "PARSING_CSS_SELECTOR";
        case CompileState::PARSING_CSS_PROPERTIES: return "PARSING_CSS_PROPERTIES";
        case CompileState::PARSING_VARIABLE_GROUP: return "PARSING_VARIABLE_GROUP";
        case CompileState::PARSING_INHERITANCE: return "PARSING_INHERITANCE";
        case CompileState::PARSING_DELETION: return "PARSING_DELETION";
        case CompileState::PARSING_INSERTION: return "PARSING_INSERTION";
        case CompileState::PARSING_INDEX_ACCESS: return "PARSING_INDEX_ACCESS";
        case CompileState::PARSING_CONSTRAINTS: return "PARSING_CONSTRAINTS";
        case CompileState::PARSING_COMMENT: return "PARSING_COMMENT";
        case CompileState::ERROR_STATE: return "ERROR_STATE";
        case CompileState::FINISHED: return "FINISHED";
        default: return "UNKNOWN_STATE";
    }
}

bool CHTLState::IsParsingState(CompileState state) {
    return state >= CompileState::PARSING_TEMPLATE && state <= CompileState::PARSING_COMMENT;
}

bool CHTLState::IsErrorState(CompileState state) {
    return state == CompileState::ERROR_STATE;
}

bool CHTLState::IsFinalState(CompileState state) {
    return state == CompileState::FINISHED || state == CompileState::ERROR_STATE;
}

void CHTLState::AddTransition(const StateTransition& transition) {
    transitions_.push_back(transition);
}

void CHTLState::RemoveTransition(CompileState fromState, CompileState toState, TokenType triggerToken) {
    transitions_.erase(
        std::remove_if(transitions_.begin(), transitions_.end(),
                      [fromState, toState, triggerToken](const StateTransition& t) {
                          return t.fromState == fromState && 
                                 t.toState == toState && 
                                 t.triggerToken == triggerToken;
                      }),
        transitions_.end()
    );
}

std::string CHTLState::GetStatistics() const {
    std::ostringstream oss;
    
    oss << "CHTL状态机统计:\n";
    oss << "  当前状态: " << GetStateName(currentState_) << "\n";
    oss << "  状态栈深度: " << stateStack_.size() << "\n";
    oss << "  状态转换历史长度: " << stateHistory_.size() << "\n";
    oss << "  转换规则数量: " << transitions_.size() << "\n";
    
    oss << "\n状态访问计数:\n";
    for (const auto& pair : stateCounters_) {
        oss << "  " << GetStateName(pair.first) << ": " << pair.second << "\n";
    }
    
    oss << "\n当前上下文:\n";
    oss << "  元素类型: " << currentContext_.elementType << "\n";
    oss << "  命名空间: " << currentContext_.namespaceName << "\n";
    oss << "  模板名称: " << currentContext_.templateName << "\n";
    oss << "  嵌套级别: " << currentContext_.nestingLevel << "\n";
    oss << "  在局部样式块中: " << (currentContext_.inLocalStyleBlock ? "是" : "否") << "\n";
    oss << "  在局部脚本块中: " << (currentContext_.inLocalScriptBlock ? "是" : "否") << "\n";
    
    return oss.str();
}

void CHTLState::InitializeDefaultTransitions() {
    // 从初始状态的转换
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_TEMPLATE, TokenType::TEMPLATE));
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_CUSTOM, TokenType::CUSTOM));
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_ORIGIN, TokenType::ORIGIN));
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_IMPORT, TokenType::IMPORT));
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_NAMESPACE, TokenType::NAMESPACE));
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_CONFIGURATION, TokenType::CONFIGURATION));
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_HTML_ELEMENT, TokenType::HTML_TAG));
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_COMMENT, TokenType::SINGLE_LINE_COMMENT));
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_COMMENT, TokenType::MULTI_LINE_COMMENT));
    AddTransition(StateTransition(CompileState::INITIAL, CompileState::PARSING_COMMENT, TokenType::GENERATOR_COMMENT));
    
    // HTML元素相关转换
    AddTransition(StateTransition(CompileState::PARSING_HTML_ELEMENT, CompileState::PARSING_ATTRIBUTES, TokenType::IDENTIFIER));
    AddTransition(StateTransition(CompileState::PARSING_HTML_ELEMENT, CompileState::PARSING_TEXT_NODE, TokenType::TEXT));
    AddTransition(StateTransition(CompileState::PARSING_HTML_ELEMENT, CompileState::PARSING_STYLE_BLOCK, TokenType::STYLE));
    AddTransition(StateTransition(CompileState::PARSING_HTML_ELEMENT, CompileState::PARSING_SCRIPT_BLOCK, TokenType::SCRIPT));
    AddTransition(StateTransition(CompileState::PARSING_HTML_ELEMENT, CompileState::PARSING_HTML_ELEMENT, TokenType::HTML_TAG));
    
    // 样式块相关转换
    AddTransition(StateTransition(CompileState::PARSING_STYLE_BLOCK, CompileState::PARSING_CSS_SELECTOR, TokenType::CLASS_SELECTOR));
    AddTransition(StateTransition(CompileState::PARSING_STYLE_BLOCK, CompileState::PARSING_CSS_SELECTOR, TokenType::ID_SELECTOR));
    AddTransition(StateTransition(CompileState::PARSING_STYLE_BLOCK, CompileState::PARSING_CSS_SELECTOR, TokenType::AMPERSAND));
    AddTransition(StateTransition(CompileState::PARSING_STYLE_BLOCK, CompileState::PARSING_CSS_PROPERTIES, TokenType::IDENTIFIER));
    AddTransition(StateTransition(CompileState::PARSING_STYLE_BLOCK, CompileState::PARSING_VARIABLE_GROUP, TokenType::AT_STYLE));
    AddTransition(StateTransition(CompileState::PARSING_STYLE_BLOCK, CompileState::PARSING_VARIABLE_GROUP, TokenType::AT_VAR));
    
    // CSS相关转换
    AddTransition(StateTransition(CompileState::PARSING_CSS_SELECTOR, CompileState::PARSING_CSS_PROPERTIES, TokenType::LEFT_BRACE));
    AddTransition(StateTransition(CompileState::PARSING_CSS_PROPERTIES, CompileState::PARSING_CSS_SELECTOR, TokenType::RIGHT_BRACE));
    
    // 继承和删除操作
    AddTransition(StateTransition(CompileState::PARSING_TEMPLATE, CompileState::PARSING_INHERITANCE, TokenType::INHERIT));
    AddTransition(StateTransition(CompileState::PARSING_CUSTOM, CompileState::PARSING_INHERITANCE, TokenType::INHERIT));
    AddTransition(StateTransition(CompileState::PARSING_TEMPLATE, CompileState::PARSING_DELETION, TokenType::DELETE));
    AddTransition(StateTransition(CompileState::PARSING_CUSTOM, CompileState::PARSING_DELETION, TokenType::DELETE));
    AddTransition(StateTransition(CompileState::PARSING_CUSTOM, CompileState::PARSING_INSERTION, TokenType::INSERT));
    
    // 索引访问
    AddTransition(StateTransition(CompileState::PARSING_HTML_ELEMENT, CompileState::PARSING_INDEX_ACCESS, TokenType::LEFT_BRACKET));
    
    // 约束处理
    AddTransition(StateTransition(CompileState::PARSING_HTML_ELEMENT, CompileState::PARSING_CONSTRAINTS, TokenType::EXCEPT));
    
    // 返回到初始状态或完成状态
    AddTransition(StateTransition(CompileState::PARSING_TEMPLATE, CompileState::INITIAL, TokenType::RIGHT_BRACE));
    AddTransition(StateTransition(CompileState::PARSING_CUSTOM, CompileState::INITIAL, TokenType::RIGHT_BRACE));
    AddTransition(StateTransition(CompileState::PARSING_ORIGIN, CompileState::INITIAL, TokenType::RIGHT_BRACE));
    AddTransition(StateTransition(CompileState::PARSING_NAMESPACE, CompileState::INITIAL, TokenType::RIGHT_BRACE));
    AddTransition(StateTransition(CompileState::PARSING_CONFIGURATION, CompileState::INITIAL, TokenType::RIGHT_BRACE));
    AddTransition(StateTransition(CompileState::PARSING_HTML_ELEMENT, CompileState::INITIAL, TokenType::RIGHT_BRACE));
}

const StateTransition* CHTLState::FindTransition(CompileState fromState, CompileState toState, 
                                                 const CHTLToken& token) const {
    for (const auto& transition : transitions_) {
        if (transition.fromState == fromState && 
            transition.toState == toState && 
            transition.triggerToken == token.GetType()) {
            return &transition;
        }
    }
    return nullptr;
}

bool CHTLState::ValidateTransition(CompileState fromState, CompileState toState, 
                                  const CHTLToken& token) const {
    const StateTransition* transition = FindTransition(fromState, toState, token);
    if (!transition) {
        return false;
    }
    
    // 如果有验证器，执行验证
    if (transition->validator) {
        return transition->validator();
    }
    
    // 基本验证规则
    // 1. 不能从终止状态转换到其他状态
    if (IsFinalState(fromState) && !IsFinalState(toState)) {
        return false;
    }
    
    // 2. 检查上下文相关的约束
    if (toState == CompileState::PARSING_STYLE_BLOCK && 
        currentContext_.inLocalScriptBlock) {
        return false;  // 不能在脚本块中解析样式
    }
    
    if (toState == CompileState::PARSING_SCRIPT_BLOCK && 
        currentContext_.inLocalStyleBlock) {
        return false;  // 不能在样式块中解析脚本
    }
    
    return true;
}

void CHTLState::ExecuteTransitionSideEffects(CompileState fromState, CompileState toState, 
                                            const CHTLToken& token) {
    // 根据状态转换执行相应的副作用
    
    // 进入嵌套结构时增加嵌套级别
    if (token.GetType() == TokenType::LEFT_BRACE) {
        currentContext_.nestingLevel++;
    }
    
    // 退出嵌套结构时减少嵌套级别
    if (token.GetType() == TokenType::RIGHT_BRACE) {
        currentContext_.nestingLevel = std::max(0, currentContext_.nestingLevel - 1);
    }
    
    // 进入样式块
    if (toState == CompileState::PARSING_STYLE_BLOCK) {
        currentContext_.inLocalStyleBlock = true;
        currentContext_.inGlobalScope = false;
    }
    
    // 退出样式块
    if (fromState == CompileState::PARSING_STYLE_BLOCK && 
        toState != CompileState::PARSING_CSS_SELECTOR && 
        toState != CompileState::PARSING_CSS_PROPERTIES) {
        currentContext_.inLocalStyleBlock = false;
    }
    
    // 进入脚本块
    if (toState == CompileState::PARSING_SCRIPT_BLOCK) {
        currentContext_.inLocalScriptBlock = true;
        currentContext_.inGlobalScope = false;
    }
    
    // 退出脚本块
    if (fromState == CompileState::PARSING_SCRIPT_BLOCK) {
        currentContext_.inLocalScriptBlock = false;
    }
}

void CHTLState::UpdateContext(CompileState newState, const CHTLToken& token) {
    // 根据新状态和Token更新上下文信息
    
    switch (newState) {
        case CompileState::PARSING_TEMPLATE:
            currentContext_.templateName = token.GetValue();
            break;
            
        case CompileState::PARSING_CUSTOM:
            currentContext_.customName = token.GetValue();
            break;
            
        case CompileState::PARSING_HTML_ELEMENT:
            currentContext_.elementType = token.GetValue();
            break;
            
        case CompileState::PARSING_NAMESPACE:
            currentContext_.namespaceName = token.GetValue();
            break;
            
        case CompileState::PARSING_ATTRIBUTES:
            if (token.GetType() == TokenType::IDENTIFIER) {
                currentContext_.attributeNames.push_back(token.GetValue());
            }
            break;
            
        default:
            // 其他状态不需要特殊处理
            break;
    }
}

// StateGuard 实现
StateGuard::StateGuard(CHTLState& stateManager, CompileState newState, const StateContext& context)
    : stateManager_(stateManager), isValid_(false) {
    stateManager_.PushState(newState, context);
    isValid_ = true;
}

StateGuard::~StateGuard() {
    if (isValid_) {
        stateManager_.PopState();
    }
}

} // namespace Core
} // namespace CHTL
