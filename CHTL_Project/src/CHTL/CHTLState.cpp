#include "CHTL/CHTLState.hpp"

namespace CHTL {

CHTLStateManager::CHTLStateManager() {
    InitializeStateTransitions();
    // 初始状态
    stateStack_.push(CHTLStateContext(CHTLState::Initial));
}

void CHTLStateManager::PushState(CHTLState newState, const String& contextName, 
                                 const String& contextType, const SourceLocation& location) {
    CHTLStateContext context(newState, contextName, contextType, GetNestingLevel() + 1, location);
    stateStack_.push(context);
}

bool CHTLStateManager::PopState() {
    if (stateStack_.size() > 1) { // 保持至少一个状态
        stateStack_.pop();
        return true;
    }
    return false;
}

CHTLState CHTLStateManager::GetCurrentState() const {
    if (!stateStack_.empty()) {
        return stateStack_.top().state;
    }
    return CHTLState::Initial;
}

const CHTLStateContext& CHTLStateManager::GetCurrentContext() const {
    return stateStack_.top();
}

bool CHTLStateManager::IsInState(CHTLState state) const {
    return GetCurrentState() == state;
}

bool CHTLStateManager::IsInAnyState(const std::vector<CHTLState>& states) const {
    CHTLState current = GetCurrentState();
    for (CHTLState state : states) {
        if (current == state) {
            return true;
        }
    }
    return false;
}

size_t CHTLStateManager::GetStateDepth() const {
    return stateStack_.size();
}

size_t CHTLStateManager::GetNestingLevel() const {
    if (!stateStack_.empty()) {
        return stateStack_.top().nestingLevel;
    }
    return 0;
}

void CHTLStateManager::SetContextProperty(const String& key, const String& value) {
    if (!stateStack_.empty()) {
        stateStack_.top().properties[key] = value;
    }
}

Optional<String> CHTLStateManager::GetContextProperty(const String& key) const {
    if (!stateStack_.empty()) {
        const auto& props = stateStack_.top().properties;
        auto it = props.find(key);
        if (it != props.end()) {
            return it->second;
        }
    }
    return {};
}

bool CHTLStateManager::HasContextProperty(const String& key) const {
    return GetContextProperty(key).has_value();
}

std::vector<CHTLStateContext> CHTLStateManager::GetStateHistory() const {
    std::vector<CHTLStateContext> history;
    std::stack<CHTLStateContext> temp = stateStack_;
    
    while (!temp.empty()) {
        history.insert(history.begin(), temp.top());
        temp.pop();
    }
    
    return history;
}

CHTLStateContext CHTLStateManager::GetParentContext(size_t levelsUp) const {
    std::stack<CHTLStateContext> temp = stateStack_;
    
    // 跳过当前上下文和指定层数
    for (size_t i = 0; i <= levelsUp && !temp.empty(); ++i) {
        temp.pop();
    }
    
    if (!temp.empty()) {
        return temp.top();
    }
    
    return CHTLStateContext(); // 返回默认上下文
}

bool CHTLStateManager::CanTransitionTo(CHTLState newState) const {
    return IsValidStateTransition(GetCurrentState(), newState);
}

bool CHTLStateManager::IsValidStateTransition(CHTLState from, CHTLState to) const {
    auto it = validTransitions_.find(from);
    if (it != validTransitions_.end()) {
        return it->second.find(to) != it->second.end();
    }
    return false;
}

String CHTLStateManager::GetCurrentContextName() const {
    if (!stateStack_.empty()) {
        return stateStack_.top().contextName;
    }
    return "";
}

String CHTLStateManager::GetCurrentContextType() const {
    if (!stateStack_.empty()) {
        return stateStack_.top().contextType;
    }
    return "";
}

bool CHTLStateManager::IsInTemplate() const {
    return IsInAnyState({CHTLState::InTemplate, CHTLState::InTemplateStyle, 
                         CHTLState::InTemplateElement, CHTLState::InTemplateVar});
}

bool CHTLStateManager::IsInCustom() const {
    return IsInAnyState({CHTLState::InCustom, CHTLState::InCustomStyle, 
                         CHTLState::InCustomElement, CHTLState::InCustomVar, 
                         CHTLState::InCustomSpecialization});
}

bool CHTLStateManager::IsInStyle() const {
    return IsInAnyState({CHTLState::InLocalStyle, CHTLState::InGlobalStyle, 
                         CHTLState::InStyleRule, CHTLState::InStyleProperties});
}

bool CHTLStateManager::IsInScript() const {
    return IsInAnyState({CHTLState::InLocalScript, CHTLState::InGlobalScript});
}

bool CHTLStateManager::IsInElement() const {
    return IsInAnyState({CHTLState::InElement, CHTLState::InElementAttributes});
}

bool CHTLStateManager::IsInComment() const {
    return IsInAnyState({CHTLState::InLineComment, CHTLState::InBlockComment, 
                         CHTLState::InGeneratorComment});
}

bool CHTLStateManager::IsInString() const {
    return IsInAnyState({CHTLState::InDoubleQuotedString, CHTLState::InSingleQuotedString, 
                         CHTLState::InUnquotedLiteral});
}

void CHTLStateManager::Reset() {
    // 清空栈并重新初始化
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    stateStack_.push(CHTLStateContext(CHTLState::Initial));
}

void CHTLStateManager::Clear() {
    Reset();
}

CHTLStateManager::StateGuard CHTLStateManager::CreateStateGuard(CHTLState state, const String& contextName,
                                                                 const String& contextType, const SourceLocation& location) {
    return StateGuard(*this, state, contextName, contextType, location);
}

void CHTLStateManager::InitializeStateTransitions() {
    // 定义有效的状态转换
    validTransitions_[CHTLState::Initial] = {
        CHTLState::Global, CHTLState::InTemplate, CHTLState::InCustom, 
        CHTLState::InOrigin, CHTLState::InConfiguration, CHTLState::InNamespace,
        CHTLState::InImport, CHTLState::InElement
    };
    
    validTransitions_[CHTLState::Global] = {
        CHTLState::InElement, CHTLState::InTemplate, CHTLState::InCustom,
        CHTLState::InOrigin, CHTLState::InConfiguration, CHTLState::InNamespace,
        CHTLState::InImport, CHTLState::InGlobalStyle, CHTLState::InGlobalScript,
        CHTLState::InComment, CHTLState::End
    };
    
    validTransitions_[CHTLState::InElement] = {
        CHTLState::InElementAttributes, CHTLState::InTextNode, 
        CHTLState::InLocalStyle, CHTLState::InLocalScript,
        CHTLState::InElement, CHTLState::Global
    };
    
    // ... 添加更多状态转换规则
    // 为简化，这里只展示部分规则
}

// StateGuard实现
CHTLStateManager::StateGuard::StateGuard(CHTLStateManager& manager, CHTLState state, 
                                          const String& contextName, const String& contextType,
                                          const SourceLocation& location)
    : manager_(&manager), active_(true) {
    manager_->PushState(state, contextName, contextType, location);
}

CHTLStateManager::StateGuard::~StateGuard() {
    if (active_ && manager_) {
        manager_->PopState();
    }
}

CHTLStateManager::StateGuard::StateGuard(StateGuard&& other) noexcept
    : manager_(other.manager_), active_(other.active_) {
    other.active_ = false;
}

CHTLStateManager::StateGuard& CHTLStateManager::StateGuard::operator=(StateGuard&& other) noexcept {
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

// 状态转换到字符串
String StateToString(CHTLState state) {
    switch (state) {
        case CHTLState::Initial: return "Initial";
        case CHTLState::Global: return "Global";
        case CHTLState::InElement: return "InElement";
        case CHTLState::InElementAttributes: return "InElementAttributes";
        case CHTLState::InTextNode: return "InTextNode";
        case CHTLState::InLocalStyle: return "InLocalStyle";
        case CHTLState::InGlobalStyle: return "InGlobalStyle";
        case CHTLState::InStyleRule: return "InStyleRule";
        case CHTLState::InStyleProperties: return "InStyleProperties";
        case CHTLState::InLocalScript: return "InLocalScript";
        case CHTLState::InGlobalScript: return "InGlobalScript";
        case CHTLState::InTemplate: return "InTemplate";
        case CHTLState::InTemplateStyle: return "InTemplateStyle";
        case CHTLState::InTemplateElement: return "InTemplateElement";
        case CHTLState::InTemplateVar: return "InTemplateVar";
        case CHTLState::InCustom: return "InCustom";
        case CHTLState::InCustomStyle: return "InCustomStyle";
        case CHTLState::InCustomElement: return "InCustomElement";
        case CHTLState::InCustomVar: return "InCustomVar";
        case CHTLState::InCustomSpecialization: return "InCustomSpecialization";
        case CHTLState::InOrigin: return "InOrigin";
        case CHTLState::InOriginHtml: return "InOriginHtml";
        case CHTLState::InOriginStyle: return "InOriginStyle";
        case CHTLState::InOriginScript: return "InOriginScript";
        case CHTLState::InImport: return "InImport";
        case CHTLState::InConfiguration: return "InConfiguration";
        case CHTLState::InNameConfiguration: return "InNameConfiguration";
        case CHTLState::InOriginTypeConfiguration: return "InOriginTypeConfiguration";
        case CHTLState::InNamespace: return "InNamespace";
        case CHTLState::InNestedNamespace: return "InNestedNamespace";
        case CHTLState::InLineComment: return "InLineComment";
        case CHTLState::InBlockComment: return "InBlockComment";
        case CHTLState::InGeneratorComment: return "InGeneratorComment";
        case CHTLState::InDoubleQuotedString: return "InDoubleQuotedString";
        case CHTLState::InSingleQuotedString: return "InSingleQuotedString";
        case CHTLState::InUnquotedLiteral: return "InUnquotedLiteral";
        case CHTLState::Error: return "Error";
        case CHTLState::End: return "End";
        default: return "Unknown";
    }
}

} // namespace CHTL