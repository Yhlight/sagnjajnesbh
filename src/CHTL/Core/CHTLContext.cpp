#include "CHTL/Core/CHTLContext.h"
#include "Utils/ErrorHandler.h"
#include <sstream>

namespace CHTL {
namespace Core {

CHTLContext::CHTLContext(const std::string& sourceFile) 
    : sourceFile_(sourceFile) {
    
    // 初始化全局上下文
    ContextStackItem globalContext;
    globalContext.type = ContextType::GLOBAL;
    globalContext.scope = ContextScope::FILE;
    globalContext.name = "global";
    globalContext.depth = 0;
    
    contextStack_.push(globalContext);
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL Context初始化完成，源文件: " + sourceFile_
    );
}

CHTLContext::~CHTLContext() {
    // RAII自动清理
}

void CHTLContext::EnterContext(ContextType type, const std::string& name, ContextScope scope) {
    ContextStackItem newContext;
    newContext.type = type;
    newContext.scope = scope;
    newContext.name = name;
    newContext.depth = GetContextDepth() + 1;
    
    contextStack_.push(newContext);
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "进入CHTL上下文: " + name + "，类型: " + std::to_string(static_cast<int>(type)) +
        "，深度: " + std::to_string(newContext.depth)
    );
}

void CHTLContext::ExitContext() {
    if (contextStack_.size() <= 1) {
        Utils::ErrorHandler::GetInstance().LogWarning(
            "尝试退出全局上下文，操作被忽略"
        );
        return;
    }
    
    const auto& currentContext = contextStack_.top();
    Utils::ErrorHandler::GetInstance().LogInfo(
        "退出CHTL上下文: " + currentContext.name + 
        "，深度: " + std::to_string(currentContext.depth)
    );
    
    contextStack_.pop();
}

ContextType CHTLContext::GetCurrentContextType() const {
    if (IsContextStackEmpty()) {
        return ContextType::GLOBAL;
    }
    
    return GetCurrentContext().type;
}

std::string CHTLContext::GetCurrentContextName() const {
    if (IsContextStackEmpty()) {
        return "global";
    }
    
    return GetCurrentContext().name;
}

ContextScope CHTLContext::GetCurrentScope() const {
    if (IsContextStackEmpty()) {
        return ContextScope::FILE;
    }
    
    return GetCurrentContext().scope;
}

int CHTLContext::GetContextDepth() const {
    return static_cast<int>(contextStack_.size()) - 1; // 减去全局上下文
}

void CHTLContext::AddSymbol(const CHTLSymbol& symbol) {
    // 根据符号的作用域决定存储位置
    if (symbol.scope == ContextScope::FILE || symbol.scope == ContextScope::NAMESPACE) {
        // 全局符号
        globalSymbols_[symbol.name] = symbol;
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "添加全局符号: " + symbol.name + "，类型: " + std::to_string(static_cast<int>(symbol.contextType))
        );
    } else {
        // 局部符号
        if (!IsContextStackEmpty()) {
            GetCurrentContext().symbols[symbol.name] = symbol;
            
            Utils::ErrorHandler::GetInstance().LogInfo(
                "添加局部符号: " + symbol.name + "，上下文: " + GetCurrentContextName()
            );
        }
    }
}

const CHTLSymbol* CHTLContext::FindSymbol(const std::string& name, bool searchGlobal) const {
    // 首先在当前上下文中查找
    if (!IsContextStackEmpty()) {
        const auto& currentContext = GetCurrentContext();
        auto it = currentContext.symbols.find(name);
        if (it != currentContext.symbols.end()) {
            return &it->second;
        }
    }
    
    // 然后在上级上下文中查找
    std::stack<ContextStackItem> tempStack = contextStack_;
    tempStack.pop(); // 跳过当前上下文
    
    while (!tempStack.empty()) {
        const auto& context = tempStack.top();
        auto it = context.symbols.find(name);
        if (it != context.symbols.end()) {
            return &it->second;
        }
        tempStack.pop();
    }
    
    // 最后在全局符号表中查找
    if (searchGlobal) {
        auto it = globalSymbols_.find(name);
        if (it != globalSymbols_.end()) {
            return &it->second;
        }
    }
    
    return nullptr;
}

void CHTLContext::AddVariable(const std::string& name, const std::string& value) {
    if (!IsContextStackEmpty()) {
        GetCurrentContext().variables[name] = value;
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "添加变量: " + name + " = " + value + "，上下文: " + GetCurrentContextName()
        );
    }
}

std::string CHTLContext::GetVariable(const std::string& name) const {
    // 在当前上下文及上级上下文中查找变量
    std::stack<ContextStackItem> tempStack = contextStack_;
    
    while (!tempStack.empty()) {
        const auto& context = tempStack.top();
        auto it = context.variables.find(name);
        if (it != context.variables.end()) {
            return it->second;
        }
        tempStack.pop();
    }
    
    return ""; // 未找到
}

bool CHTLContext::HasVariable(const std::string& name) const {
    return !GetVariable(name).empty();
}

void CHTLContext::SetCurrentNamespace(const std::string& namespaceName) {
    currentNamespace_ = namespaceName;
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "设置当前命名空间: " + namespaceName
    );
}

std::string CHTLContext::GetCurrentNamespace() const {
    return currentNamespace_;
}

std::string CHTLContext::GetFullSymbolName(const std::string& symbolName) const {
    if (currentNamespace_.empty()) {
        return symbolName;
    } else {
        return currentNamespace_ + "::" + symbolName;
    }
}

bool CHTLContext::IsSymbolInCurrentScope(const std::string& symbolName) const {
    if (IsContextStackEmpty()) {
        return false;
    }
    
    const auto& currentContext = GetCurrentContext();
    return currentContext.symbols.find(symbolName) != currentContext.symbols.end();
}

std::unordered_map<std::string, CHTLSymbol> CHTLContext::GetCurrentContextSymbols() const {
    if (IsContextStackEmpty()) {
        return {};
    }
    
    return GetCurrentContext().symbols;
}

const std::unordered_map<std::string, CHTLSymbol>& CHTLContext::GetGlobalSymbols() const {
    return globalSymbols_;
}

bool CHTLContext::IsOperationAllowed(const std::string& operation) const {
    ContextType currentType = GetCurrentContextType();
    
    // 根据当前上下文类型判断操作是否允许
    // 按照CHTL语法文档的约束规则
    
    if (operation == "inherit") {
        // 继承操作只能在模板和自定义上下文中使用
        return currentType == ContextType::TEMPLATE_STYLE ||
               currentType == ContextType::TEMPLATE_ELEMENT ||
               currentType == ContextType::TEMPLATE_VAR ||
               currentType == ContextType::CUSTOM_STYLE ||
               currentType == ContextType::CUSTOM_ELEMENT ||
               currentType == ContextType::CUSTOM_VAR;
    } else if (operation == "delete") {
        // 删除操作只能在自定义上下文中使用
        return currentType == ContextType::CUSTOM_STYLE ||
               currentType == ContextType::CUSTOM_ELEMENT ||
               currentType == ContextType::CUSTOM_VAR;
    } else if (operation == "insert") {
        // 插入操作只能在自定义元素上下文中使用
        return currentType == ContextType::CUSTOM_ELEMENT;
    } else if (operation == "index_access") {
        // 索引访问只能在自定义元素上下文中使用
        return currentType == ContextType::CUSTOM_ELEMENT;
    } else if (operation == "specialization") {
        // 特化操作只能在自定义上下文中使用
        return currentType == ContextType::CUSTOM_STYLE ||
               currentType == ContextType::CUSTOM_ELEMENT ||
               currentType == ContextType::CUSTOM_VAR;
    }
    
    return true; // 默认允许
}

void CHTLContext::AddImportRecord(const std::string& importPath, const std::string& importType) {
    importRecords_.push_back({importPath, importType});
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "添加导入记录: " + importType + " from " + importPath
    );
}

std::vector<std::pair<std::string, std::string>> CHTLContext::GetImportRecords() const {
    return importRecords_;
}

void CHTLContext::Reset() {
    // 清空上下文栈，保留全局上下文
    while (contextStack_.size() > 1) {
        contextStack_.pop();
    }
    
    // 清空符号表和变量
    globalSymbols_.clear();
    importRecords_.clear();
    currentNamespace_.clear();
    
    // 重新初始化全局上下文
    if (!contextStack_.empty()) {
        auto& globalContext = contextStack_.top();
        globalContext.variables.clear();
        globalContext.symbols.clear();
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CHTL Context已重置"
    );
}

std::string CHTLContext::GetContextStatus() const {
    std::ostringstream status;
    status << "CHTL Context状态:\n";
    status << "  源文件: " << sourceFile_ << "\n";
    status << "  当前命名空间: " << currentNamespace_ << "\n";
    status << "  上下文深度: " << GetContextDepth() << "\n";
    status << "  当前上下文: " << GetCurrentContextName() << "\n";
    status << "  当前作用域: " << static_cast<int>(GetCurrentScope()) << "\n";
    status << "  全局符号数量: " << globalSymbols_.size() << "\n";
    status << "  导入记录数量: " << importRecords_.size() << "\n";
    
    if (!IsContextStackEmpty()) {
        const auto& currentContext = GetCurrentContext();
        status << "  当前上下文符号数量: " << currentContext.symbols.size() << "\n";
        status << "  当前上下文变量数量: " << currentContext.variables.size() << "\n";
    }
    
    return status.str();
}

ContextStackItem& CHTLContext::GetCurrentContext() {
    if (IsContextStackEmpty()) {
        throw std::runtime_error("上下文栈为空");
    }
    
    return const_cast<ContextStackItem&>(contextStack_.top());
}

const ContextStackItem& CHTLContext::GetCurrentContext() const {
    if (IsContextStackEmpty()) {
        throw std::runtime_error("上下文栈为空");
    }
    
    return contextStack_.top();
}

bool CHTLContext::IsContextStackEmpty() const {
    return contextStack_.empty();
}

} // namespace Core
} // namespace CHTL
