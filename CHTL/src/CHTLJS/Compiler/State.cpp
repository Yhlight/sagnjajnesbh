#include "CHTLJS/Compiler/State.h"
#include <iostream>
#include <algorithm>
#include <vector>

namespace CHTL {
namespace JSCompiler {

// StateManager实现类
class StateManager::Impl {
public:
    std::stack<StateContext> stateStack;
    std::string currentVirObject;
    std::string currentSelector;
    
    Impl() {
        // 初始状态为全局
        stateStack.push(StateContext(CompileState::GLOBAL, "", 0, 1, 1));
    }
    
    CompileState GetCurrentState() const {
        if (!stateStack.empty()) {
            return stateStack.top().state;
        }
        return CompileState::GLOBAL;
    }
    
    const StateContext& GetCurrentContext() const {
        static StateContext defaultContext(CompileState::GLOBAL);
        if (!stateStack.empty()) {
            return stateStack.top();
        }
        return defaultContext;
    }
    
    bool IsInState(CompileState state) const {
        std::stack<StateContext> temp = stateStack;
        while (!temp.empty()) {
            if (temp.top().state == state) {
                return true;
            }
            temp.pop();
        }
        return false;
    }
    
    bool IsInSelector() const {
        return IsInState(CompileState::IN_SELECTOR);
    }
    
    bool IsInVirContext() const {
        return IsInState(CompileState::IN_VIR_DECLARATION) ||
               !currentVirObject.empty();
    }
    
    bool IsInObjectLiteral() const {
        return IsInState(CompileState::IN_OBJECT_LITERAL);
    }
    
    bool IsInFunctionContext() const {
        return IsInState(CompileState::IN_FUNCTION_PARAMS) ||
               IsInState(CompileState::IN_FUNCTION_BODY);
    }
    
    size_t GetDepth() const {
        return stateStack.size() - 1; // 减去初始的GLOBAL状态
    }
    
    bool CanEnterState(CompileState newState) const {
        CompileState current = GetCurrentState();
        
        // 定义状态转换规则
        switch (newState) {
            case CompileState::IN_SELECTOR:
                // 选择器可以在全局、JS代码、对象值中出现
                return current == CompileState::GLOBAL ||
                       current == CompileState::IN_JS_CODE ||
                       current == CompileState::IN_PROPERTY_VALUE ||
                       current == CompileState::IN_ARRAY_LITERAL;
                       
            case CompileState::IN_VIR_DECLARATION:
                // vir声明只能在全局或JS代码中
                return current == CompileState::GLOBAL ||
                       current == CompileState::IN_JS_CODE;
                       
            case CompileState::IN_ARROW_EXPRESSION:
                // ->表达式可以跟在选择器或标识符后
                return current == CompileState::IN_SELECTOR ||
                       current == CompileState::IN_JS_CODE;
                       
            case CompileState::IN_LISTEN_CALL:
            case CompileState::IN_DELEGATE_CALL:
            case CompileState::IN_ANIMATE_CALL:
            case CompileState::IN_INEVERAWAY_CALL:
                // 这些函数调用可以在多种上下文中
                return current == CompileState::GLOBAL ||
                       current == CompileState::IN_JS_CODE ||
                       current == CompileState::IN_ARROW_EXPRESSION ||
                       current == CompileState::IN_VIR_DECLARATION;
                       
            case CompileState::IN_OBJECT_LITERAL:
                // 对象字面量可以作为参数或值
                return current == CompileState::IN_LISTEN_CALL ||
                       current == CompileState::IN_DELEGATE_CALL ||
                       current == CompileState::IN_ANIMATE_CALL ||
                       current == CompileState::IN_INEVERAWAY_CALL ||
                       current == CompileState::IN_PROPERTY_VALUE ||
                       current == CompileState::IN_JS_CODE;
                       
            default:
                return true;
        }
    }
    
    void PushState(CompileState state, const std::string& contextName) {
        StateContext newContext(state, contextName);
        
        if (!stateStack.empty()) {
            const auto& parent = stateStack.top();
            newContext.depth = parent.depth + 1;
            
            // 继承虚对象和选择器信息
            newContext.virObjectName = currentVirObject;
            newContext.selectorContent = currentSelector;
        }
        
        stateStack.push(newContext);
    }
    
    void PopState() {
        if (stateStack.size() > 1) { // 保留初始的GLOBAL状态
            stateStack.pop();
        }
    }
    
    void DumpStateStack() const {
        std::cout << "=== CHTL JS 状态栈 ===" << std::endl;
        std::stack<StateContext> temp = stateStack;
        std::vector<StateContext> contexts;
        
        while (!temp.empty()) {
            contexts.push_back(temp.top());
            temp.pop();
        }
        
        for (auto it = contexts.rbegin(); it != contexts.rend(); ++it) {
            std::cout << std::string(it->depth * 2, ' ') 
                      << StateToString(it->state);
            if (!it->contextName.empty()) {
                std::cout << " (" << it->contextName << ")";
            }
            if (!it->virObjectName.empty()) {
                std::cout << " [vir: " << it->virObjectName << "]";
            }
            if (!it->selectorContent.empty()) {
                std::cout << " [sel: " << it->selectorContent << "]";
            }
            std::cout << std::endl;
        }
        std::cout << "==================" << std::endl;
    }
    
private:
    std::string StateToString(CompileState state) const {
        switch (state) {
            case CompileState::GLOBAL: return "GLOBAL";
            case CompileState::IN_SELECTOR: return "IN_SELECTOR";
            case CompileState::IN_VIR_DECLARATION: return "IN_VIR_DECLARATION";
            case CompileState::IN_ARROW_EXPRESSION: return "IN_ARROW_EXPRESSION";
            case CompileState::IN_LISTEN_CALL: return "IN_LISTEN_CALL";
            case CompileState::IN_DELEGATE_CALL: return "IN_DELEGATE_CALL";
            case CompileState::IN_ANIMATE_CALL: return "IN_ANIMATE_CALL";
            case CompileState::IN_INEVERAWAY_CALL: return "IN_INEVERAWAY_CALL";
            case CompileState::IN_OBJECT_LITERAL: return "IN_OBJECT_LITERAL";
            case CompileState::IN_ARRAY_LITERAL: return "IN_ARRAY_LITERAL";
            case CompileState::IN_PROPERTY_KEY: return "IN_PROPERTY_KEY";
            case CompileState::IN_PROPERTY_VALUE: return "IN_PROPERTY_VALUE";
            case CompileState::IN_FUNCTION_PARAMS: return "IN_FUNCTION_PARAMS";
            case CompileState::IN_FUNCTION_BODY: return "IN_FUNCTION_BODY";
            case CompileState::IN_JS_CODE: return "IN_JS_CODE";
            default: return "UNKNOWN";
        }
    }
};

// StateGuard 实现
StateGuard::StateGuard(StateManager* manager, CompileState state, 
                       const std::string& contextName)
    : manager_(manager), active_(true) {
    if (manager_) {
        manager_->PushState(state, contextName);
    }
}

StateGuard::~StateGuard() {
    if (active_ && manager_) {
        manager_->PopState();
    }
}

StateGuard::StateGuard(StateGuard&& other) noexcept
    : manager_(other.manager_), active_(other.active_) {
    other.active_ = false;
}

StateGuard& StateGuard::operator=(StateGuard&& other) noexcept {
    if (this != &other) {
        if (active_ && manager_) {
            manager_->PopState();
        }
        manager_ = other.manager_;
        active_ = other.active_;
        other.active_ = false;
    }
    return *this;
}

// StateManager 实现
StateManager::StateManager() : pImpl(std::make_unique<Impl>()) {}
StateManager::~StateManager() = default;

CompileState StateManager::GetCurrentState() const {
    return pImpl->GetCurrentState();
}

const StateContext& StateManager::GetCurrentContext() const {
    return pImpl->GetCurrentContext();
}

bool StateManager::IsInState(CompileState state) const {
    return pImpl->IsInState(state);
}

bool StateManager::IsInSelector() const {
    return pImpl->IsInSelector();
}

bool StateManager::IsInVirContext() const {
    return pImpl->IsInVirContext();
}

bool StateManager::IsInObjectLiteral() const {
    return pImpl->IsInObjectLiteral();
}

bool StateManager::IsInFunctionContext() const {
    return pImpl->IsInFunctionContext();
}

std::string StateManager::GetCurrentVirObject() const {
    return pImpl->currentVirObject;
}

void StateManager::SetCurrentVirObject(const std::string& name) {
    pImpl->currentVirObject = name;
}

std::string StateManager::GetCurrentSelector() const {
    return pImpl->currentSelector;
}

void StateManager::SetCurrentSelector(const std::string& selector) {
    pImpl->currentSelector = selector;
}

size_t StateManager::GetDepth() const {
    return pImpl->GetDepth();
}

StateGuard StateManager::EnterState(CompileState state, const std::string& contextName) {
    if (!CanEnterState(state)) {
        throw std::runtime_error("无法从当前状态进入目标状态");
    }
    return StateGuard(this, state, contextName);
}

bool StateManager::CanEnterState(CompileState newState) const {
    return pImpl->CanEnterState(newState);
}

void StateManager::DumpStateStack() const {
    pImpl->DumpStateStack();
}

void StateManager::PushState(CompileState state, const std::string& contextName) {
    pImpl->PushState(state, contextName);
}

void StateManager::PopState() {
    pImpl->PopState();
}

} // namespace JSCompiler
} // namespace CHTL