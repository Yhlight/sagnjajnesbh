#include "chtl_js/context/CHTLJSContext.h"
#include "utils/Logger.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace chtljs {

CHTLJSContext::CHTLJSContext()
    : m_ObjectLiteralDepth(0)
    , m_CurrentLine(1)
    , m_CurrentColumn(1) {
    // 初始化script块作用域
    PushScope(CHTLJSScopeType::SCRIPT_BLOCK);
}

CHTLJSContext::~CHTLJSContext() = default;

void CHTLJSContext::PushScope(CHTLJSScopeType type, const std::string& name) {
    m_ScopeStack.push(CHTLJSScope(type, name));
    utils::Logger::GetInstance().Debug("CHTL JS进入作用域: " + name + 
                                     " (类型: " + std::to_string(static_cast<int>(type)) + ")");
}

void CHTLJSContext::PopScope() {
    if (!m_ScopeStack.empty()) {
        CHTLJSScope scope = m_ScopeStack.top();
        m_ScopeStack.pop();
        utils::Logger::GetInstance().Debug("CHTL JS退出作用域: " + scope.name);
    }
}

CHTLJSScope* CHTLJSContext::GetCurrentScope() {
    return m_ScopeStack.empty() ? nullptr : &m_ScopeStack.top();
}

const CHTLJSScope* CHTLJSContext::GetCurrentScope() const {
    return m_ScopeStack.empty() ? nullptr : &m_ScopeStack.top();
}

void CHTLJSContext::DefineVirtualObject(const std::string& name, 
                                       const std::string& functionName) {
    if (CHTLJSScope* scope = GetCurrentScope()) {
        scope->virtualObjects[name] = functionName;
        utils::Logger::GetInstance().Debug("定义虚对象: " + name + " -> " + functionName);
    }
}

bool CHTLJSContext::IsVirtualObjectDefined(const std::string& name) const {
    return FindVirtualObject(name) != nullptr;
}

std::string CHTLJSContext::GetVirtualObjectFunction(const std::string& name) const {
    const std::string* func = FindVirtualObject(name);
    return func ? *func : "";
}

void CHTLJSContext::EnterSelector(const std::string& selector) {
    m_SelectorStack.push(selector);
    utils::Logger::GetInstance().Debug("进入选择器: " + selector);
}

void CHTLJSContext::ExitSelector() {
    if (!m_SelectorStack.empty()) {
        std::string selector = m_SelectorStack.top();
        m_SelectorStack.pop();
        utils::Logger::GetInstance().Debug("退出选择器: " + selector);
    }
}

std::string CHTLJSContext::GetCurrentSelector() const {
    return m_SelectorStack.empty() ? "" : m_SelectorStack.top();
}

bool CHTLJSContext::IsInSelector() const {
    return !m_SelectorStack.empty();
}

void CHTLJSContext::StartCallChain(const std::string& object) {
    m_CurrentChainObject = object;
    m_CallChain.clear();
    m_CallChain.push_back(object);
    utils::Logger::GetInstance().Debug("开始调用链: " + object);
}

void CHTLJSContext::AddMethodToChain(const std::string& method) {
    m_CallChain.push_back(method);
    utils::Logger::GetInstance().Debug("添加方法到调用链: " + method);
}

void CHTLJSContext::EndCallChain() {
    utils::Logger::GetInstance().Debug("结束调用链");
    m_CallChain.clear();
    m_CurrentChainObject.clear();
}

std::vector<std::string> CHTLJSContext::GetCallChain() const {
    return m_CallChain;
}

bool CHTLJSContext::IsInCallChain() const {
    return !m_CallChain.empty();
}

void CHTLJSContext::EnterFunction(const std::string& functionName) {
    m_FunctionStack.push(functionName);
    utils::Logger::GetInstance().Debug("进入CHTL JS函数: " + functionName);
}

void CHTLJSContext::ExitFunction() {
    if (!m_FunctionStack.empty()) {
        std::string func = m_FunctionStack.top();
        m_FunctionStack.pop();
        utils::Logger::GetInstance().Debug("退出CHTL JS函数: " + func);
    }
}

std::string CHTLJSContext::GetCurrentFunction() const {
    return m_FunctionStack.empty() ? "" : m_FunctionStack.top();
}

bool CHTLJSContext::IsInCHTLJSFunction() const {
    if (m_FunctionStack.empty()) return false;
    
    std::string func = m_FunctionStack.top();
    return func == "listen" || func == "delegate" || func == "animate" ||
           func == "iNeverAway" || func == "printMylove";
}

void CHTLJSContext::EnterObjectLiteral() {
    m_ObjectLiteralDepth++;
    utils::Logger::GetInstance().Debug("进入对象字面量，深度: " + 
                                     std::to_string(m_ObjectLiteralDepth));
}

void CHTLJSContext::ExitObjectLiteral() {
    if (m_ObjectLiteralDepth > 0) {
        m_ObjectLiteralDepth--;
        utils::Logger::GetInstance().Debug("退出对象字面量，深度: " + 
                                         std::to_string(m_ObjectLiteralDepth));
    }
}

void CHTLJSContext::SetCurrentObjectKey(const std::string& key) {
    m_CurrentObjectKey = key;
}

std::string CHTLJSContext::GetCurrentObjectKey() const {
    return m_CurrentObjectKey;
}

bool CHTLJSContext::IsInObjectLiteral() const {
    return m_ObjectLiteralDepth > 0;
}

int CHTLJSContext::GetObjectLiteralDepth() const {
    return m_ObjectLiteralDepth;
}

void CHTLJSContext::RegisterEventHandler(const std::string& event, 
                                        const std::string& handler) {
    m_EventHandlers[event] = handler;
    utils::Logger::GetInstance().Debug("注册事件处理器: " + event + " -> " + handler);
}

std::string CHTLJSContext::GetEventHandler(const std::string& event) const {
    auto it = m_EventHandlers.find(event);
    return it != m_EventHandlers.end() ? it->second : "";
}

std::vector<std::string> CHTLJSContext::GetAllEvents() const {
    std::vector<std::string> events;
    for (const auto& pair : m_EventHandlers) {
        events.push_back(pair.first);
    }
    return events;
}

void CHTLJSContext::EnterAnimationContext(const std::string& animId) {
    m_AnimationStack.push(animId);
    utils::Logger::GetInstance().Debug("进入动画上下文: " + animId);
}

void CHTLJSContext::ExitAnimationContext() {
    if (!m_AnimationStack.empty()) {
        std::string animId = m_AnimationStack.top();
        m_AnimationStack.pop();
        utils::Logger::GetInstance().Debug("退出动画上下文: " + animId);
    }
}

bool CHTLJSContext::IsInAnimationContext() const {
    return !m_AnimationStack.empty();
}

std::string CHTLJSContext::GetCurrentAnimationId() const {
    return m_AnimationStack.empty() ? "" : m_AnimationStack.top();
}

void CHTLJSContext::SetCurrentFile(const std::string& filename) {
    m_CurrentFile = filename;
}

std::string CHTLJSContext::GetCurrentFile() const {
    return m_CurrentFile;
}

void CHTLJSContext::SetCurrentLine(size_t line) {
    m_CurrentLine = line;
}

size_t CHTLJSContext::GetCurrentLine() const {
    return m_CurrentLine;
}

void CHTLJSContext::SetCurrentColumn(size_t column) {
    m_CurrentColumn = column;
}

size_t CHTLJSContext::GetCurrentColumn() const {
    return m_CurrentColumn;
}

void CHTLJSContext::Clear() {
    while (!m_ScopeStack.empty()) {
        m_ScopeStack.pop();
    }
    PushScope(CHTLJSScopeType::SCRIPT_BLOCK);
    
    while (!m_SelectorStack.empty()) {
        m_SelectorStack.pop();
    }
    
    m_CallChain.clear();
    m_CurrentChainObject.clear();
    
    while (!m_FunctionStack.empty()) {
        m_FunctionStack.pop();
    }
    
    m_ObjectLiteralDepth = 0;
    m_CurrentObjectKey.clear();
    m_EventHandlers.clear();
    
    while (!m_AnimationStack.empty()) {
        m_AnimationStack.pop();
    }
    
    m_CurrentFile.clear();
    m_CurrentLine = 1;
    m_CurrentColumn = 1;
}

void CHTLJSContext::DumpScopes() const {
    std::cout << "=== CHTL JS作用域栈 ===" << std::endl;
    std::stack<CHTLJSScope> temp = m_ScopeStack;
    std::vector<CHTLJSScope> scopes;
    
    // 反转栈以正确顺序显示
    while (!temp.empty()) {
        scopes.push_back(temp.top());
        temp.pop();
    }
    
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        std::cout << "作用域: " << it->name << " (类型: " << 
                     static_cast<int>(it->type) << ")" << std::endl;
        for (const auto& vir : it->virtualObjects) {
            std::cout << "  虚对象: " << vir.first << " -> " << vir.second << std::endl;
        }
    }
}

std::string CHTLJSContext::GetContextTrace() const {
    std::stringstream ss;
    ss << "文件: " << m_CurrentFile << "\n";
    ss << "位置: " << m_CurrentLine << ":" << m_CurrentColumn << "\n";
    ss << "选择器栈深度: " << m_SelectorStack.size() << "\n";
    ss << "调用链: ";
    for (const auto& item : m_CallChain) {
        ss << item << " -> ";
    }
    ss << "\n";
    ss << "函数栈深度: " << m_FunctionStack.size() << "\n";
    ss << "对象字面量深度: " << m_ObjectLiteralDepth << "\n";
    ss << "动画栈深度: " << m_AnimationStack.size() << "\n";
    return ss.str();
}

std::string* CHTLJSContext::FindVirtualObject(const std::string& name) {
    std::stack<CHTLJSScope> temp = m_ScopeStack;
    
    while (!temp.empty()) {
        CHTLJSScope& scope = temp.top();
        auto it = scope.virtualObjects.find(name);
        if (it != scope.virtualObjects.end()) {
            return &it->second;
        }
        temp.pop();
    }
    
    return nullptr;
}

const std::string* CHTLJSContext::FindVirtualObject(const std::string& name) const {
    std::stack<CHTLJSScope> temp = m_ScopeStack;
    
    while (!temp.empty()) {
        const CHTLJSScope& scope = temp.top();
        auto it = scope.virtualObjects.find(name);
        if (it != scope.virtualObjects.end()) {
            return &it->second;
        }
        temp.pop();
    }
    
    return nullptr;
}

} // namespace chtljs
} // namespace chtl