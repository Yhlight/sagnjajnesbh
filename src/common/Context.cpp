#include "../../include/common/Context.h"
#include "../../include/common/GlobalMap.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace chtl {

// Configuration类的简单定义（后续会扩展）
class Configuration {
public:
    // 配置项
    int indexInitialCount = 0;
    bool disableNameGroup = false;
    bool disableCustomOriginType = false;
    bool debugMode = false;
    
    // Name配置
    std::unordered_map<std::string, std::vector<std::string>> nameConfig;
    
    // OriginType配置
    std::unordered_map<std::string, std::string> originTypeConfig;
    
    Configuration() {
        // 初始化默认配置
        initializeDefaults();
    }
    
private:
    void initializeDefaults() {
        // 默认的Name配置
        nameConfig["CUSTOM_STYLE"] = {"@Style", "@style", "@CSS", "@Css", "@css"};
        nameConfig["CUSTOM_ELEMENT"] = {"@Element"};
        nameConfig["CUSTOM_VAR"] = {"@Var"};
        // ... 更多默认配置
    }
};

// ImportManager类的简单定义
class ImportManager {
private:
    std::unordered_set<std::string> importedFiles_;
    std::unordered_map<std::string, std::string> moduleCache_;
    
public:
    bool isImported(const std::string& path) const {
        return importedFiles_.find(path) != importedFiles_.end();
    }
    
    void markImported(const std::string& path) {
        importedFiles_.insert(path);
    }
    
    void clear() {
        importedFiles_.clear();
        moduleCache_.clear();
    }
};

// NamespaceInfo类的简单定义
class NamespaceInfo {
public:
    std::string name;
    std::weak_ptr<NamespaceInfo> parent;
    std::unordered_map<std::string, std::shared_ptr<SymbolInfo>> symbols;
    
    explicit NamespaceInfo(const std::string& n) : name(n) {}
};

// CompileContext实现
CompileContext::CompileContext() 
    : diagnostics_(std::make_unique<DiagnosticCollector>()),
      config_(std::make_unique<Configuration>()),
      importManager_(std::make_unique<ImportManager>()) {
    // 创建全局符号表
    auto globalTable = std::make_shared<SymbolTable>("global");
    symbolTableStack_.push(globalTable);
}

CompileContext::~CompileContext() = default;

void CompileContext::setConfig(std::unique_ptr<Configuration> cfg) {
    config_ = std::move(cfg);
}

void CompileContext::pushSymbolTable(std::shared_ptr<SymbolTable> table) {
    if (!symbolTableStack_.empty()) {
        table->setParent(symbolTableStack_.top());
    }
    symbolTableStack_.push(table);
}

void CompileContext::popSymbolTable() {
    if (symbolTableStack_.size() > 1) {  // 保留全局符号表
        symbolTableStack_.pop();
    }
}

std::shared_ptr<SymbolTable> CompileContext::getCurrentSymbolTable() const {
    return symbolTableStack_.empty() ? nullptr : symbolTableStack_.top();
}

void CompileContext::pushNamespace(std::shared_ptr<NamespaceInfo> ns) {
    if (!namespaceStack_.empty()) {
        ns->parent = namespaceStack_.top();
    }
    namespaceStack_.push(ns);
}

void CompileContext::popNamespace() {
    if (!namespaceStack_.empty()) {
        namespaceStack_.pop();
    }
}

std::shared_ptr<NamespaceInfo> CompileContext::getCurrentNamespace() const {
    return namespaceStack_.empty() ? nullptr : namespaceStack_.top();
}

std::vector<std::string> CompileContext::getNamespacePath() const {
    std::vector<std::string> path;
    std::stack<std::shared_ptr<NamespaceInfo>> temp = namespaceStack_;
    
    while (!temp.empty()) {
        path.insert(path.begin(), temp.top()->name);
        temp.pop();
    }
    
    return path;
}

void CompileContext::setCurrentFile(const std::string& file) {
    currentFile_ = file;
    
    // 更新当前目录
    fs::path filePath(file);
    currentDirectory_ = filePath.parent_path().string();
    if (currentDirectory_.empty()) {
        currentDirectory_ = ".";
    }
}

} // namespace chtl