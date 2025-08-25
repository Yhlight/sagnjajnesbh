#include "CHTL/Compiler/Context.h"
#include <iostream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <unordered_set>

namespace CHTL {
namespace Compiler {

// LocalScope 实现
std::shared_ptr<ASTNode> LocalScope::FindVariable(const std::string& name) const {
    // 先在当前作用域查找
    auto it = localVariables.find(name);
    if (it != localVariables.end()) {
        return it->second;
    }
    
    // 递归在父作用域查找
    if (parent) {
        return parent->FindVariable(name);
    }
    
    return nullptr;
}

// CompileContext 实现类
class CompileContext::Impl {
public:
    ContextConfig config;
    GlobalMap globalMap;
    StateManager stateManager;
    
    std::shared_ptr<LocalScope> currentScope;
    std::string currentFile;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::unordered_set<std::string> importedFiles;
    std::vector<std::string> modulePaths;
    
    size_t autoClassCounter = 0;
    size_t autoIdCounter = 0;
    
    Impl() {
        // 初始化默认模块路径
        modulePaths.push_back("./modules/official");
        modulePaths.push_back("./modules");
        modulePaths.push_back(".");
        
        // 创建全局作用域
        currentScope = std::make_shared<LocalScope>();
        currentScope->name = "global";
    }
    
    void SetConfig(const ContextConfig& cfg) {
        config = cfg;
    }
    
    void EnterScope(const std::string& name) {
        auto newScope = std::make_shared<LocalScope>();
        newScope->name = name.empty() ? "anonymous" : name;
        newScope->parent = currentScope;
        currentScope = newScope;
    }
    
    void ExitScope() {
        if (currentScope && currentScope->parent) {
            currentScope = currentScope->parent;
        }
    }
    
    bool RegisterLocalVariable(const std::string& name, std::shared_ptr<ASTNode> node) {
        if (!currentScope) return false;
        
        // 检查是否已存在
        if (currentScope->localVariables.find(name) != currentScope->localVariables.end()) {
            return false;
        }
        
        currentScope->localVariables[name] = node;
        return true;
    }
    
    std::shared_ptr<ASTNode> FindLocalVariable(const std::string& name) const {
        if (!currentScope) return nullptr;
        return currentScope->FindVariable(name);
    }
    
    std::string GenerateAutoClass() {
        std::stringstream ss;
        ss << "chtl-auto-class-" << ++autoClassCounter;
        return ss.str();
    }
    
    std::string GenerateAutoId() {
        std::stringstream ss;
        ss << "chtl-auto-id-" << ++autoIdCounter;
        return ss.str();
    }
    
    void RegisterAutoClass(const std::string& className) {
        if (currentScope) {
            currentScope->autoClasses.push_back(className);
        }
    }
    
    void RegisterAutoId(const std::string& id) {
        if (currentScope) {
            currentScope->autoIds.push_back(id);
        }
    }
    
    void AddError(const std::string& message, size_t line, size_t column) {
        std::stringstream ss;
        ss << currentFile << ":" << line << ":" << column << " 错误: " << message;
        errors.push_back(ss.str());
    }
    
    void AddWarning(const std::string& message, size_t line, size_t column) {
        std::stringstream ss;
        ss << currentFile << ":" << line << ":" << column << " 警告: " << message;
        warnings.push_back(ss.str());
    }
    
    void AddImportedFile(const std::string& filePath) {
        std::filesystem::path absPath = std::filesystem::absolute(filePath);
        importedFiles.insert(absPath.string());
    }
    
    bool IsFileImported(const std::string& filePath) const {
        std::filesystem::path absPath = std::filesystem::absolute(filePath);
        return importedFiles.find(absPath.string()) != importedFiles.end();
    }
    
    void AddModulePath(const std::string& path) {
        if (std::find(modulePaths.begin(), modulePaths.end(), path) == modulePaths.end()) {
            modulePaths.push_back(path);
        }
    }
    
    std::string ResolveModulePath(const std::string& moduleName, const std::string& extension) const {
        // 如果是绝对路径或相对路径，直接返回
        if (moduleName.find('/') != std::string::npos || 
            moduleName.find('\\') != std::string::npos ||
            moduleName.find('.') == 0) {
            return moduleName;
        }
        
        // 在模块路径中查找
        for (const auto& basePath : modulePaths) {
            // 尝试不同的文件扩展名
            std::vector<std::string> extensions;
            if (!extension.empty()) {
                extensions.push_back(extension);
            } else {
                extensions = {".cmod", ".chtl", ".cjmod"};
            }
            
            for (const auto& ext : extensions) {
                std::filesystem::path fullPath = std::filesystem::path(basePath) / (moduleName + ext);
                if (std::filesystem::exists(fullPath)) {
                    return fullPath.string();
                }
            }
        }
        
        // 如果没找到，返回原始名称
        return moduleName;
    }
    
    void DumpContext() const {
        std::cout << "=== CHTL编译上下文 ===" << std::endl;
        std::cout << "当前文件: " << currentFile << std::endl;
        std::cout << "配置:" << std::endl;
        std::cout << "  索引初始值: " << config.indexInitialCount << std::endl;
        std::cout << "  调试模式: " << (config.debugMode ? "开启" : "关闭") << std::endl;
        
        std::cout << "\n模块路径:" << std::endl;
        for (const auto& path : modulePaths) {
            std::cout << "  - " << path << std::endl;
        }
        
        std::cout << "\n已导入文件:" << std::endl;
        for (const auto& file : importedFiles) {
            std::cout << "  - " << file << std::endl;
        }
        
        if (!errors.empty()) {
            std::cout << "\n错误:" << std::endl;
            for (const auto& error : errors) {
                std::cout << "  " << error << std::endl;
            }
        }
        
        if (!warnings.empty()) {
            std::cout << "\n警告:" << std::endl;
            for (const auto& warning : warnings) {
                std::cout << "  " << warning << std::endl;
            }
        }
        
        std::cout << "\n作用域栈:" << std::endl;
        DumpScope(currentScope, 0);
        
        std::cout << "\n全局映射表:" << std::endl;
        globalMap.Dump();
        
        std::cout << "\n状态栈:" << std::endl;
        stateManager.DumpStateStack();
        
        std::cout << "===================" << std::endl;
    }
    
private:
    void DumpScope(std::shared_ptr<LocalScope> scope, int depth) const {
        if (!scope) return;
        
        std::string indent(depth * 2, ' ');
        std::cout << indent << "作用域: " << scope->name << std::endl;
        
        if (!scope->localVariables.empty()) {
            std::cout << indent << "  局部变量:" << std::endl;
            for (const auto& [name, node] : scope->localVariables) {
                std::cout << indent << "    - " << name << std::endl;
            }
        }
        
        if (!scope->autoClasses.empty()) {
            std::cout << indent << "  自动类名:" << std::endl;
            for (const auto& cls : scope->autoClasses) {
                std::cout << indent << "    - " << cls << std::endl;
            }
        }
        
        if (!scope->autoIds.empty()) {
            std::cout << indent << "  自动ID:" << std::endl;
            for (const auto& id : scope->autoIds) {
                std::cout << indent << "    - " << id << std::endl;
            }
        }
    }
};

// CompileContext 实现
CompileContext::CompileContext() : pImpl(std::make_unique<Impl>()) {}
CompileContext::~CompileContext() = default;

void CompileContext::SetConfig(const ContextConfig& config) {
    pImpl->SetConfig(config);
}

const ContextConfig& CompileContext::GetConfig() const {
    return pImpl->config;
}

GlobalMap& CompileContext::GetGlobalMap() {
    return pImpl->globalMap;
}

const GlobalMap& CompileContext::GetGlobalMap() const {
    return pImpl->globalMap;
}

StateManager& CompileContext::GetStateManager() {
    return pImpl->stateManager;
}

const StateManager& CompileContext::GetStateManager() const {
    return pImpl->stateManager;
}

void CompileContext::EnterScope(const std::string& name) {
    pImpl->EnterScope(name);
}

void CompileContext::ExitScope() {
    pImpl->ExitScope();
}

std::shared_ptr<LocalScope> CompileContext::GetCurrentScope() const {
    return pImpl->currentScope;
}

bool CompileContext::RegisterLocalVariable(const std::string& name, std::shared_ptr<ASTNode> node) {
    return pImpl->RegisterLocalVariable(name, node);
}

std::shared_ptr<ASTNode> CompileContext::FindLocalVariable(const std::string& name) const {
    return pImpl->FindLocalVariable(name);
}

std::string CompileContext::GenerateAutoClass() {
    return pImpl->GenerateAutoClass();
}

std::string CompileContext::GenerateAutoId() {
    return pImpl->GenerateAutoId();
}

void CompileContext::RegisterAutoClass(const std::string& className) {
    pImpl->RegisterAutoClass(className);
}

void CompileContext::RegisterAutoId(const std::string& id) {
    pImpl->RegisterAutoId(id);
}

void CompileContext::AddError(const std::string& message, size_t line, size_t column) {
    pImpl->AddError(message, line, column);
}

void CompileContext::AddWarning(const std::string& message, size_t line, size_t column) {
    pImpl->AddWarning(message, line, column);
}

bool CompileContext::HasErrors() const {
    return !pImpl->errors.empty();
}

const std::vector<std::string>& CompileContext::GetErrors() const {
    return pImpl->errors;
}

const std::vector<std::string>& CompileContext::GetWarnings() const {
    return pImpl->warnings;
}

void CompileContext::SetCurrentFile(const std::string& filePath) {
    pImpl->currentFile = filePath;
}

std::string CompileContext::GetCurrentFile() const {
    return pImpl->currentFile;
}

void CompileContext::AddImportedFile(const std::string& filePath) {
    pImpl->AddImportedFile(filePath);
}

bool CompileContext::IsFileImported(const std::string& filePath) const {
    return pImpl->IsFileImported(filePath);
}

void CompileContext::AddModulePath(const std::string& path) {
    pImpl->AddModulePath(path);
}

std::vector<std::string> CompileContext::GetModulePaths() const {
    return pImpl->modulePaths;
}

std::string CompileContext::ResolveModulePath(const std::string& moduleName, const std::string& extension) const {
    return pImpl->ResolveModulePath(moduleName, extension);
}

void CompileContext::DumpContext() const {
    pImpl->DumpContext();
}

// ScopeGuard 实现
ScopeGuard::ScopeGuard(CompileContext* context, const std::string& scopeName)
    : context_(context), active_(true) {
    if (context_) {
        context_->EnterScope(scopeName);
    }
}

ScopeGuard::~ScopeGuard() {
    if (active_ && context_) {
        context_->ExitScope();
    }
}

ScopeGuard::ScopeGuard(ScopeGuard&& other) noexcept
    : context_(other.context_), active_(other.active_) {
    other.active_ = false;
}

ScopeGuard& ScopeGuard::operator=(ScopeGuard&& other) noexcept {
    if (this != &other) {
        if (active_ && context_) {
            context_->ExitScope();
        }
        context_ = other.context_;
        active_ = other.active_;
        other.active_ = false;
    }
    return *this;
}

} // namespace Compiler
} // namespace CHTL