#include "CHTL/Compiler/State.h"
#include <iostream>
#include <algorithm>
#include <vector>

namespace CHTL {
namespace Compiler {

// StateManager实现类
class StateManager::Impl {
public:
    std::stack<StateContext> stateStack;
    
    Impl() {
        // 初始状态为全局
        stateStack.push(StateContext(CompileState::GLOBAL, "", "", 0, 1, 1));
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
        // 遍历状态栈查找
        std::stack<StateContext> temp = stateStack;
        while (!temp.empty()) {
            if (temp.top().state == state) {
                return true;
            }
            temp.pop();
        }
        return false;
    }
    
    bool IsInDeclaration() const {
        CompileState current = GetCurrentState();
        return current == CompileState::IN_TEMPLATE ||
               current == CompileState::IN_CUSTOM ||
               current == CompileState::IN_ORIGIN ||
               current == CompileState::IN_CONFIGURATION ||
               current == CompileState::IN_NAMESPACE ||
               current == CompileState::IN_IMPORT;
    }
    
    bool IsInElement() const {
        return IsInState(CompileState::IN_HTML_ELEMENT) ||
               IsInState(CompileState::IN_TEXT_NODE);
    }
    
    bool IsInStyleContext() const {
        return IsInState(CompileState::IN_STYLE_BLOCK) ||
               IsInState(CompileState::IN_STYLE_SELECTOR) ||
               IsInState(CompileState::IN_STYLE_PROPERTIES);
    }
    
    bool IsInScriptContext() const {
        return IsInState(CompileState::IN_SCRIPT_BLOCK);
    }
    
    std::string GetCurrentNamespace() const {
        // 从栈底向上查找最近的命名空间
        std::stack<StateContext> temp = stateStack;
        std::vector<StateContext> contexts;
        
        while (!temp.empty()) {
            contexts.push_back(temp.top());
            temp.pop();
        }
        
        // 反向遍历（从栈底开始）
        for (auto it = contexts.rbegin(); it != contexts.rend(); ++it) {
            if (!it->namespacePath.empty()) {
                return it->namespacePath;
            }
        }
        
        return "";
    }
    
    size_t GetDepth() const {
        return stateStack.size() - 1; // 减去初始的GLOBAL状态
    }
    
    bool CanEnterState(CompileState newState) const {
        CompileState current = GetCurrentState();
        
        // 定义状态转换规则
        switch (newState) {
            case CompileState::IN_TEMPLATE:
            case CompileState::IN_CUSTOM:
            case CompileState::IN_ORIGIN:
            case CompileState::IN_CONFIGURATION:
            case CompileState::IN_IMPORT:
                // 这些只能在全局或命名空间中
                return current == CompileState::GLOBAL || 
                       current == CompileState::IN_NAMESPACE;
                
            case CompileState::IN_NAMESPACE:
                // 命名空间可以嵌套
                return current == CompileState::GLOBAL || 
                       current == CompileState::IN_NAMESPACE;
                
            case CompileState::IN_HTML_ELEMENT:
                // HTML元素可以在多种上下文中
                return current == CompileState::GLOBAL ||
                       current == CompileState::IN_HTML_ELEMENT ||
                       current == CompileState::IN_TEMPLATE ||
                       current == CompileState::IN_CUSTOM ||
                       current == CompileState::IN_SPECIALIZATION;
                       
            case CompileState::IN_STYLE_BLOCK:
            case CompileState::IN_SCRIPT_BLOCK:
                // 局部样式和脚本只能在HTML元素中
                return current == CompileState::IN_HTML_ELEMENT;
                
            case CompileState::IN_TEXT_NODE:
                // 文本节点只能在HTML元素中
                return current == CompileState::IN_HTML_ELEMENT;
                
            case CompileState::IN_SPECIALIZATION:
                // 特例化只能在使用自定义/模板时
                return current == CompileState::IN_HTML_ELEMENT ||
                       current == CompileState::IN_STYLE_BLOCK;
                
            default:
                return true;
        }
    }
    
    void PushState(CompileState state, const std::string& contextName) {
        StateContext newContext(state, contextName);
        
        // 继承命名空间路径
        if (!stateStack.empty()) {
            const auto& parent = stateStack.top();
            newContext.namespacePath = parent.namespacePath;
            newContext.depth = parent.depth + 1;
            
            // 如果进入新的命名空间，更新路径
            if (state == CompileState::IN_NAMESPACE && !contextName.empty()) {
                if (newContext.namespacePath.empty()) {
                    newContext.namespacePath = contextName;
                } else {
                    newContext.namespacePath += "." + contextName;
                }
            }
        }
        
        stateStack.push(newContext);
    }
    
    void PopState() {
        if (stateStack.size() > 1) { // 保留初始的GLOBAL状态
            stateStack.pop();
        }
    }
    
    void DumpStateStack() const {
        std::cout << "=== 状态栈 ===" << std::endl;
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
            if (!it->namespacePath.empty()) {
                std::cout << " [ns: " << it->namespacePath << "]";
            }
            std::cout << std::endl;
        }
        std::cout << "=============" << std::endl;
    }
    
private:
    std::string StateToString(CompileState state) const {
        switch (state) {
            case CompileState::GLOBAL: return "GLOBAL";
            case CompileState::IN_TEMPLATE: return "IN_TEMPLATE";
            case CompileState::IN_CUSTOM: return "IN_CUSTOM";
            case CompileState::IN_ORIGIN: return "IN_ORIGIN";
            case CompileState::IN_CONFIGURATION: return "IN_CONFIGURATION";
            case CompileState::IN_NAMESPACE: return "IN_NAMESPACE";
            case CompileState::IN_IMPORT: return "IN_IMPORT";
            case CompileState::IN_HTML_ELEMENT: return "IN_HTML_ELEMENT";
            case CompileState::IN_TEXT_NODE: return "IN_TEXT_NODE";
            case CompileState::IN_STYLE_BLOCK: return "IN_STYLE_BLOCK";
            case CompileState::IN_SCRIPT_BLOCK: return "IN_SCRIPT_BLOCK";
            case CompileState::IN_STYLE_SELECTOR: return "IN_STYLE_SELECTOR";
            case CompileState::IN_STYLE_PROPERTIES: return "IN_STYLE_PROPERTIES";
            case CompileState::IN_SPECIALIZATION: return "IN_SPECIALIZATION";
            case CompileState::IN_DELETE_STATEMENT: return "IN_DELETE_STATEMENT";
            case CompileState::IN_INSERT_STATEMENT: return "IN_INSERT_STATEMENT";
            case CompileState::IN_VAR_EXPRESSION: return "IN_VAR_EXPRESSION";
            case CompileState::IN_FROM_EXPRESSION: return "IN_FROM_EXPRESSION";
            case CompileState::IN_CONFIG_NAME: return "IN_CONFIG_NAME";
            case CompileState::IN_CONFIG_ORIGINTYPE: return "IN_CONFIG_ORIGINTYPE";
            case CompileState::IN_INFO_BLOCK: return "IN_INFO_BLOCK";
            case CompileState::IN_EXPORT_BLOCK: return "IN_EXPORT_BLOCK";
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

bool StateManager::IsInDeclaration() const {
    return pImpl->IsInDeclaration();
}

bool StateManager::IsInElement() const {
    return pImpl->IsInElement();
}

bool StateManager::IsInStyleContext() const {
    return pImpl->IsInStyleContext();
}

bool StateManager::IsInScriptContext() const {
    return pImpl->IsInScriptContext();
}

std::string StateManager::GetCurrentNamespace() const {
    return pImpl->GetCurrentNamespace();
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

} // namespace Compiler
} // namespace CHTL