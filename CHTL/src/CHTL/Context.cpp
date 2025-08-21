#include "CHTL/Context.h"
#include "CHTL/GlobalMap.h"
#include "Utils/Logger.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

// ContextManager内部实现
class ContextManager::Impl {
public:
    std::stack<ContextInfo> contextStack_;
    ContextInfo currentContext_;
    std::shared_ptr<GlobalMap> globalMap_;
    std::shared_ptr<StateMachine> stateMachine_;
    
    Impl() {
        currentContext_ = ContextInfo(ContextType::Global);
        currentContext_.name = "<global>";
    }
};

// ContextManager实现
ContextManager::ContextManager() : impl_(std::make_unique<Impl>()) {}
ContextManager::~ContextManager() = default;

void ContextManager::PushContext(const ContextInfo& context) {
    impl_->contextStack_.push(impl_->currentContext_);
    impl_->currentContext_ = context;
    
    LOG_DEBUG("推入上下文: " + ContextUtils::GetContextTypeName(context.type) + 
              " (" + context.name + ")");
}

bool ContextManager::PopContext() {
    if (impl_->contextStack_.empty()) {
        LOG_WARNING("上下文栈为空，无法弹出");
        return false;
    }
    
    ContextInfo previousContext = impl_->currentContext_;
    impl_->currentContext_ = impl_->contextStack_.top();
    impl_->contextStack_.pop();
    
    LOG_DEBUG("弹出上下文: " + ContextUtils::GetContextTypeName(previousContext.type) + 
              " -> " + ContextUtils::GetContextTypeName(impl_->currentContext_.type));
    return true;
}

ContextInfo& ContextManager::GetCurrentContext() {
    return impl_->currentContext_;
}

const ContextInfo& ContextManager::GetCurrentContext() const {
    return impl_->currentContext_;
}

ContextType ContextManager::GetCurrentContextType() const {
    return impl_->currentContext_.type;
}

std::string ContextManager::GetCurrentContextName() const {
    return impl_->currentContext_.name;
}

bool ContextManager::IsInContext(ContextType type) const {
    if (impl_->currentContext_.type == type) {
        return true;
    }
    
    // 检查栈中是否有指定类型的上下文
    std::stack<ContextInfo> temp = impl_->contextStack_;
    while (!temp.empty()) {
        if (temp.top().type == type) {
            return true;
        }
        temp.pop();
    }
    
    return false;
}

size_t ContextManager::GetContextDepth() const {
    return impl_->contextStack_.size();
}

std::vector<ContextInfo> ContextManager::GetContextStack() const {
    std::vector<ContextInfo> result;
    std::stack<ContextInfo> temp = impl_->contextStack_;
    
    while (!temp.empty()) {
        result.push_back(temp.top());
        temp.pop();
    }
    
    std::reverse(result.begin(), result.end());
    result.push_back(impl_->currentContext_);
    return result;
}

void ContextManager::SetAttribute(const std::string& key, const std::string& value) {
    impl_->currentContext_.attributes[key] = value;
    LOG_DEBUG("设置属性: " + key + " = " + value);
}

std::string ContextManager::GetAttribute(const std::string& key) const {
    auto it = impl_->currentContext_.attributes.find(key);
    return it != impl_->currentContext_.attributes.end() ? it->second : "";
}

bool ContextManager::HasAttribute(const std::string& key) const {
    return impl_->currentContext_.attributes.find(key) != 
           impl_->currentContext_.attributes.end();
}

void ContextManager::RemoveAttribute(const std::string& key) {
    impl_->currentContext_.attributes.erase(key);
}

void ContextManager::AddChild(const std::string& childName) {
    impl_->currentContext_.children.push_back(childName);
}

std::vector<std::string> ContextManager::GetChildren() const {
    return impl_->currentContext_.children;
}

bool ContextManager::HasChild(const std::string& childName) const {
    const auto& children = impl_->currentContext_.children;
    return std::find(children.begin(), children.end(), childName) != children.end();
}

void ContextManager::EnterStyleContext(bool isLocal) {
    ContextInfo styleContext = ContextUtils::CreateStyleContext(isLocal);
    PushContext(styleContext);
}

void ContextManager::ExitStyleContext() {
    if (impl_->currentContext_.type == ContextType::Style) {
        PopContext();
    }
}

void ContextManager::SetCurrentSelector(const std::string& selector) {
    if (impl_->currentContext_.type == ContextType::Style) {
        impl_->currentContext_.currentSelector = selector;
    }
}

std::string ContextManager::GetCurrentSelector() const {
    return impl_->currentContext_.currentSelector;
}

bool ContextManager::IsInLocalStyle() const {
    return impl_->currentContext_.isLocalStyle;
}

bool ContextManager::IsInGlobalStyle() const {
    return impl_->currentContext_.isGlobalStyle;
}

void ContextManager::EnterScriptContext(bool isLocal) {
    ContextInfo scriptContext = ContextUtils::CreateScriptContext(isLocal);
    PushContext(scriptContext);
}

void ContextManager::ExitScriptContext() {
    if (impl_->currentContext_.type == ContextType::Script) {
        PopContext();
    }
}

bool ContextManager::IsInLocalScript() const {
    return impl_->currentContext_.isLocalScript;
}

bool ContextManager::IsInGlobalScript() const {
    return impl_->currentContext_.isGlobalScript;
}

void ContextManager::EnterSpecialization() {
    impl_->currentContext_.isSpecialization = true;
}

void ContextManager::ExitSpecialization() {
    impl_->currentContext_.isSpecialization = false;
}

bool ContextManager::IsInSpecialization() const {
    return impl_->currentContext_.isSpecialization;
}

void ContextManager::SetGlobalMap(std::shared_ptr<GlobalMap> globalMap) {
    impl_->globalMap_ = globalMap;
}

std::shared_ptr<GlobalMap> ContextManager::GetGlobalMap() const {
    return impl_->globalMap_;
}

void ContextManager::SetStateMachine(std::shared_ptr<StateMachine> stateMachine) {
    impl_->stateMachine_ = stateMachine;
}

std::shared_ptr<StateMachine> ContextManager::GetStateMachine() const {
    return impl_->stateMachine_;
}

void ContextManager::Reset() {
    while (!impl_->contextStack_.empty()) {
        impl_->contextStack_.pop();
    }
    impl_->currentContext_ = ContextInfo(ContextType::Global);
    impl_->currentContext_.name = "<global>";
}

void ContextManager::DumpContextStack() const {
    std::cout << "=== 上下文栈 ===" << std::endl;
    auto stack = GetContextStack();
    for (const auto& ctx : stack) {
        std::cout << "  " << ContextUtils::GetContextTypeName(ctx.type) 
                  << " (" << ctx.name << ")" << std::endl;
    }
}

std::string ContextManager::GetContextPath() const {
    std::stringstream ss;
    auto stack = GetContextStack();
    
    for (size_t i = 0; i < stack.size(); ++i) {
        if (i > 0) ss << "/";
        ss << stack[i].name;
    }
    
    return ss.str();
}

// ContextUtils实现
std::string ContextUtils::GetContextTypeName(ContextType type) {
    switch (type) {
        case ContextType::Global: return "Global";
        case ContextType::Element: return "Element";
        case ContextType::Style: return "Style";
        case ContextType::Script: return "Script";
        case ContextType::Template: return "Template";
        case ContextType::Custom: return "Custom";
        case ContextType::Namespace: return "Namespace";
        case ContextType::Import: return "Import";
        case ContextType::Configuration: return "Configuration";
        default: return "Unknown";
    }
}

bool ContextUtils::IsBlockContext(ContextType type) {
    return type == ContextType::Element ||
           type == ContextType::Style ||
           type == ContextType::Script ||
           type == ContextType::Template ||
           type == ContextType::Custom ||
           type == ContextType::Namespace;
}

bool ContextUtils::CanContainElements(ContextType type) {
    return type == ContextType::Global ||
           type == ContextType::Element ||
           type == ContextType::Template ||
           type == ContextType::Custom ||
           type == ContextType::Namespace;
}

bool ContextUtils::CanContainStyles(ContextType type) {
    return type == ContextType::Element ||
           type == ContextType::Template ||
           type == ContextType::Custom;
}

bool ContextUtils::CanContainScripts(ContextType type) {
    return type == ContextType::Element ||
           type == ContextType::Template ||
           type == ContextType::Custom;
}

bool ContextUtils::RequiresBraces(ContextType type) {
    return IsBlockContext(type);
}

ContextInfo ContextUtils::CreateElementContext(const std::string& elementName) {
    ContextInfo ctx(ContextType::Element);
    ctx.name = elementName;
    ctx.elementType = elementName;
    return ctx;
}

ContextInfo ContextUtils::CreateStyleContext(bool isLocal) {
    ContextInfo ctx(ContextType::Style);
    ctx.name = isLocal ? "style(local)" : "style(global)";
    ctx.isLocalStyle = isLocal;
    ctx.isGlobalStyle = !isLocal;
    return ctx;
}

ContextInfo ContextUtils::CreateScriptContext(bool isLocal) {
    ContextInfo ctx(ContextType::Script);
    ctx.name = isLocal ? "script(local)" : "script(global)";
    ctx.isLocalScript = isLocal;
    ctx.isGlobalScript = !isLocal;
    return ctx;
}

ContextInfo ContextUtils::CreateTemplateContext(const std::string& templateType) {
    ContextInfo ctx(ContextType::Template);
    ctx.name = "Template:" + templateType;
    ctx.templateType = templateType;
    return ctx;
}

ContextInfo ContextUtils::CreateCustomContext(const std::string& customType) {
    ContextInfo ctx(ContextType::Custom);
    ctx.name = "Custom:" + customType;
    ctx.templateType = customType;
    return ctx;
}

ContextInfo ContextUtils::CreateNamespaceContext(const std::string& namespaceName) {
    ContextInfo ctx(ContextType::Namespace);
    ctx.name = namespaceName;
    return ctx;
}

} // namespace CHTL