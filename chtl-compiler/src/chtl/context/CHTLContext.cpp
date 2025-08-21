#include "chtl/context/CHTLContext.h"
#include "utils/Logger.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace compiler {

CHTLContext::CHTLContext()
    : m_LocalStyleDepth(0)
    , m_GlobalStyleDepth(0)
    , m_LocalScriptDepth(0)
    , m_GlobalScriptDepth(0)
    , m_CurrentLine(1)
    , m_CurrentColumn(1) {
    // 初始化全局作用域
    PushScope(CHTLScopeType::GLOBAL);
}

CHTLContext::~CHTLContext() = default;

void CHTLContext::PushScope(CHTLScopeType type, const std::string& name) {
    m_ScopeStack.push(CHTLScope(type, name));
    utils::Logger::GetInstance().Debug("进入作用域: " + name + 
                                     " (类型: " + std::to_string(static_cast<int>(type)) + ")");
}

void CHTLContext::PopScope() {
    if (!m_ScopeStack.empty()) {
        CHTLScope scope = m_ScopeStack.top();
        m_ScopeStack.pop();
        utils::Logger::GetInstance().Debug("退出作用域: " + scope.name);
    }
}

CHTLScope* CHTLContext::GetCurrentScope() {
    return m_ScopeStack.empty() ? nullptr : &m_ScopeStack.top();
}

const CHTLScope* CHTLContext::GetCurrentScope() const {
    return m_ScopeStack.empty() ? nullptr : &m_ScopeStack.top();
}

void CHTLContext::DefineSymbol(const std::string& name, const std::string& value) {
    if (CHTLScope* scope = GetCurrentScope()) {
        scope->symbols[name] = value;
        utils::Logger::GetInstance().Debug("定义符号: " + name + " = " + value);
    }
}

bool CHTLContext::IsSymbolDefined(const std::string& name) const {
    return FindSymbolInScopes(name) != nullptr;
}

std::string CHTLContext::GetSymbolValue(const std::string& name) const {
    const std::string* value = FindSymbolInScopes(name);
    return value ? *value : "";
}

void CHTLContext::SetCurrentElement(const std::string& name) {
    m_CurrentElementName = name;
}

std::string CHTLContext::GetCurrentElement() const {
    return m_CurrentElementName;
}

void CHTLContext::PushElementContext(std::shared_ptr<CHTLElement> element) {
    m_ElementStack.push(element);
}

void CHTLContext::PopElementContext() {
    if (!m_ElementStack.empty()) {
        m_ElementStack.pop();
    }
}

std::shared_ptr<CHTLElement> CHTLContext::GetCurrentElementContext() const {
    return m_ElementStack.empty() ? nullptr : m_ElementStack.top();
}

void CHTLContext::AddAttribute(const std::string& name, const std::string& value) {
    m_CurrentAttributes.push_back({name, value});
}

std::vector<std::pair<std::string, std::string>> CHTLContext::GetCurrentAttributes() const {
    return m_CurrentAttributes;
}

void CHTLContext::ClearAttributes() {
    m_CurrentAttributes.clear();
}

void CHTLContext::EnterStyleBlock(bool isLocal) {
    if (isLocal) {
        m_LocalStyleDepth++;
        utils::Logger::GetInstance().Debug("进入局部样式块");
    } else {
        m_GlobalStyleDepth++;
        utils::Logger::GetInstance().Debug("进入全局样式块");
    }
}

void CHTLContext::ExitStyleBlock() {
    if (m_LocalStyleDepth > 0) {
        m_LocalStyleDepth--;
        utils::Logger::GetInstance().Debug("退出局部样式块");
    } else if (m_GlobalStyleDepth > 0) {
        m_GlobalStyleDepth--;
        utils::Logger::GetInstance().Debug("退出全局样式块");
    }
}

bool CHTLContext::IsInLocalStyleBlock() const {
    return m_LocalStyleDepth > 0;
}

bool CHTLContext::IsInGlobalStyleBlock() const {
    return m_GlobalStyleDepth > 0;
}

void CHTLContext::AddStyleRule(std::shared_ptr<CHTLStyleRule> rule) {
    m_StyleRules.push_back(rule);
}

void CHTLContext::EnterScriptBlock(bool isLocal) {
    if (isLocal) {
        m_LocalScriptDepth++;
        utils::Logger::GetInstance().Debug("进入局部脚本块");
    } else {
        m_GlobalScriptDepth++;
        utils::Logger::GetInstance().Debug("进入全局脚本块");
    }
}

void CHTLContext::ExitScriptBlock() {
    if (m_LocalScriptDepth > 0) {
        m_LocalScriptDepth--;
        utils::Logger::GetInstance().Debug("退出局部脚本块");
    } else if (m_GlobalScriptDepth > 0) {
        m_GlobalScriptDepth--;
        utils::Logger::GetInstance().Debug("退出全局脚本块");
    }
}

bool CHTLContext::IsInLocalScriptBlock() const {
    return m_LocalScriptDepth > 0;
}

bool CHTLContext::IsInGlobalScriptBlock() const {
    return m_GlobalScriptDepth > 0;
}

void CHTLContext::AddImport(std::shared_ptr<CHTLImportInfo> import) {
    m_Imports.push_back(import);
}

std::vector<std::shared_ptr<CHTLImportInfo>> CHTLContext::GetImports() const {
    return m_Imports;
}

void CHTLContext::SetCurrentFile(const std::string& filename) {
    m_CurrentFile = filename;
}

std::string CHTLContext::GetCurrentFile() const {
    return m_CurrentFile;
}

void CHTLContext::SetCurrentLine(size_t line) {
    m_CurrentLine = line;
}

size_t CHTLContext::GetCurrentLine() const {
    return m_CurrentLine;
}

void CHTLContext::SetCurrentColumn(size_t column) {
    m_CurrentColumn = column;
}

size_t CHTLContext::GetCurrentColumn() const {
    return m_CurrentColumn;
}

void CHTLContext::SetConfigValue(const std::string& key, const std::string& value) {
    m_ConfigValues[key] = value;
}

std::string CHTLContext::GetConfigValue(const std::string& key) const {
    auto it = m_ConfigValues.find(key);
    return it != m_ConfigValues.end() ? it->second : "";
}

bool CHTLContext::HasConfigValue(const std::string& key) const {
    return m_ConfigValues.find(key) != m_ConfigValues.end();
}

void CHTLContext::Clear() {
    while (!m_ScopeStack.empty()) {
        m_ScopeStack.pop();
    }
    PushScope(CHTLScopeType::GLOBAL);
    
    while (!m_ElementStack.empty()) {
        m_ElementStack.pop();
    }
    
    m_CurrentElementName.clear();
    m_CurrentAttributes.clear();
    m_LocalStyleDepth = 0;
    m_GlobalStyleDepth = 0;
    m_LocalScriptDepth = 0;
    m_GlobalScriptDepth = 0;
    m_StyleRules.clear();
    m_Imports.clear();
    m_CurrentFile.clear();
    m_CurrentLine = 1;
    m_CurrentColumn = 1;
    m_ConfigValues.clear();
}

void CHTLContext::DumpScopes() const {
    std::cout << "=== 作用域栈 ===" << std::endl;
    std::stack<CHTLScope> temp = m_ScopeStack;
    std::vector<CHTLScope> scopes;
    
    // 反转栈以正确顺序显示
    while (!temp.empty()) {
        scopes.push_back(temp.top());
        temp.pop();
    }
    
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        std::cout << "作用域: " << it->name << " (类型: " << static_cast<int>(it->type) << ")" << std::endl;
        for (const auto& symbol : it->symbols) {
            std::cout << "  " << symbol.first << " = " << symbol.second << std::endl;
        }
    }
}

std::string CHTLContext::GetContextTrace() const {
    std::stringstream ss;
    ss << "文件: " << m_CurrentFile << "\n";
    ss << "位置: " << m_CurrentLine << ":" << m_CurrentColumn << "\n";
    ss << "当前元素: " << m_CurrentElementName << "\n";
    ss << "元素栈深度: " << m_ElementStack.size() << "\n";
    ss << "样式深度: 局部=" << m_LocalStyleDepth << ", 全局=" << m_GlobalStyleDepth << "\n";
    ss << "脚本深度: 局部=" << m_LocalScriptDepth << ", 全局=" << m_GlobalScriptDepth << "\n";
    return ss.str();
}

std::string* CHTLContext::FindSymbolInScopes(const std::string& name) {
    std::stack<CHTLScope> temp = m_ScopeStack;
    
    while (!temp.empty()) {
        CHTLScope& scope = temp.top();
        auto it = scope.symbols.find(name);
        if (it != scope.symbols.end()) {
            return &it->second;
        }
        temp.pop();
    }
    
    return nullptr;
}

const std::string* CHTLContext::FindSymbolInScopes(const std::string& name) const {
    std::stack<CHTLScope> temp = m_ScopeStack;
    
    while (!temp.empty()) {
        const CHTLScope& scope = temp.top();
        auto it = scope.symbols.find(name);
        if (it != scope.symbols.end()) {
            return &it->second;
        }
        temp.pop();
    }
    
    return nullptr;
}

} // namespace compiler
} // namespace chtl