#ifndef CHTL_COMMON_CONTEXT_H
#define CHTL_COMMON_CONTEXT_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include "Error.h"

namespace chtl {

// 前向声明
class Configuration;
class NamespaceInfo;
class ImportManager;
class SymbolTable;

// 编译上下文
class CompileContext {
private:
    // 诊断收集器
    std::unique_ptr<DiagnosticCollector> diagnostics_;
    
    // 配置信息
    std::unique_ptr<Configuration> config_;
    
    // 导入管理器
    std::unique_ptr<ImportManager> importManager_;
    
    // 符号表栈
    std::stack<std::shared_ptr<SymbolTable>> symbolTableStack_;
    
    // 当前命名空间栈
    std::stack<std::shared_ptr<NamespaceInfo>> namespaceStack_;
    
    // 当前文件信息
    std::string currentFile_;
    std::string currentDirectory_;
    
    // 编译选项
    struct Options {
        bool debugMode = false;
        bool strictMode = true;
        bool enableOptimization = true;
        std::string outputDirectory = "./output";
        std::vector<std::string> includePaths;
        std::vector<std::string> modulePaths;
    } options_;
    
public:
    CompileContext();
    ~CompileContext();
    
    // 诊断相关
    DiagnosticCollector& getDiagnostics() { return *diagnostics_; }
    const DiagnosticCollector& getDiagnostics() const { return *diagnostics_; }
    
    // 配置相关
    Configuration& getConfig() { return *config_; }
    const Configuration& getConfig() const { return *config_; }
    void setConfig(std::unique_ptr<Configuration> cfg);
    
    // 导入管理
    ImportManager& getImportManager() { return *importManager_; }
    const ImportManager& getImportManager() const { return *importManager_; }
    
    // 符号表管理
    void pushSymbolTable(std::shared_ptr<SymbolTable> table);
    void popSymbolTable();
    std::shared_ptr<SymbolTable> getCurrentSymbolTable() const;
    
    // 命名空间管理
    void pushNamespace(std::shared_ptr<NamespaceInfo> ns);
    void popNamespace();
    std::shared_ptr<NamespaceInfo> getCurrentNamespace() const;
    std::vector<std::string> getNamespacePath() const;
    
    // 文件信息
    void setCurrentFile(const std::string& file);
    const std::string& getCurrentFile() const { return currentFile_; }
    const std::string& getCurrentDirectory() const { return currentDirectory_; }
    
    // 编译选项
    Options& getOptions() { return options_; }
    const Options& getOptions() const { return options_; }
    
    // 调试模式
    bool isDebugMode() const { return options_.debugMode; }
    void setDebugMode(bool debug) { options_.debugMode = debug; }
    
    // 严格模式
    bool isStrictMode() const { return options_.strictMode; }
    void setStrictMode(bool strict) { options_.strictMode = strict; }
};

// RAII上下文保护器
class ContextGuard {
private:
    CompileContext& context_;
    bool shouldPopSymbolTable_ = false;
    bool shouldPopNamespace_ = false;
    
public:
    explicit ContextGuard(CompileContext& ctx) : context_(ctx) {}
    
    ~ContextGuard() {
        if (shouldPopSymbolTable_) {
            context_.popSymbolTable();
        }
        if (shouldPopNamespace_) {
            context_.popNamespace();
        }
    }
    
    // 禁止拷贝
    ContextGuard(const ContextGuard&) = delete;
    ContextGuard& operator=(const ContextGuard&) = delete;
    
    // 设置需要弹出的内容
    void setShouldPopSymbolTable() { shouldPopSymbolTable_ = true; }
    void setShouldPopNamespace() { shouldPopNamespace_ = true; }
};

// 局部上下文
class LocalContext {
public:
    enum class ContextType {
        GLOBAL,
        NAMESPACE,
        ELEMENT,
        STYLE_BLOCK,      // 局部样式块
        SCRIPT_BLOCK,     // 局部脚本块
        TEMPLATE_DEFINITION,
        CUSTOM_DEFINITION,
        CONFIGURATION_BLOCK,
        IMPORT_SCOPE
    };
    
private:
    ContextType type_;
    std::string name_;
    std::unordered_map<std::string, std::string> attributes_;
    
public:
    LocalContext(ContextType t, const std::string& n = "")
        : type_(t), name_(n) {}
    
    ContextType getType() const { return type_; }
    const std::string& getName() const { return name_; }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value) {
        attributes_[key] = value;
    }
    
    std::string getAttribute(const std::string& key) const {
        auto it = attributes_.find(key);
        return it != attributes_.end() ? it->second : "";
    }
    
    bool hasAttribute(const std::string& key) const {
        return attributes_.find(key) != attributes_.end();
    }
    
    // 判断上下文类型
    bool isStyleContext() const {
        return type_ == ContextType::STYLE_BLOCK;
    }
    
    bool isScriptContext() const {
        return type_ == ContextType::SCRIPT_BLOCK;
    }
    
    bool isTemplateContext() const {
        return type_ == ContextType::TEMPLATE_DEFINITION;
    }
    
    bool isCustomContext() const {
        return type_ == ContextType::CUSTOM_DEFINITION;
    }
};

// 上下文栈管理器
class ContextStack {
private:
    std::vector<std::unique_ptr<LocalContext>> stack_;
    
public:
    void push(std::unique_ptr<LocalContext> ctx) {
        stack_.push_back(std::move(ctx));
    }
    
    void pop() {
        if (!stack_.empty()) {
            stack_.pop_back();
        }
    }
    
    LocalContext* getCurrent() const {
        return stack_.empty() ? nullptr : stack_.back().get();
    }
    
    LocalContext* getParent() const {
        return stack_.size() > 1 ? stack_[stack_.size() - 2].get() : nullptr;
    }
    
    // 查找特定类型的上下文
    LocalContext* findContext(LocalContext::ContextType type) const {
        for (auto it = stack_.rbegin(); it != stack_.rend(); ++it) {
            if ((*it)->getType() == type) {
                return it->get();
            }
        }
        return nullptr;
    }
    
    // 判断是否在特定上下文中
    bool isInContext(LocalContext::ContextType type) const {
        return findContext(type) != nullptr;
    }
    
    size_t size() const { return stack_.size(); }
    bool empty() const { return stack_.empty(); }
};

} // namespace chtl

#endif // CHTL_COMMON_CONTEXT_H