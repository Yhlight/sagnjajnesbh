#include "../../include/common/GlobalMap.h"
#include <algorithm>

namespace chtl {

// SymbolTable实现
bool SymbolTable::addSymbol(const std::string& name, std::shared_ptr<SymbolInfo> info) {
    if (symbols_.find(name) != symbols_.end()) {
        return false;  // 符号已存在
    }
    symbols_[name] = info;
    return true;
}

std::shared_ptr<SymbolInfo> SymbolTable::lookupSymbol(const std::string& name) const {
    // 先在当前作用域查找
    auto it = symbols_.find(name);
    if (it != symbols_.end()) {
        return it->second;
    }
    
    // 在父作用域查找
    if (auto parent = parent_.lock()) {
        return parent->lookupSymbol(name);
    }
    
    return nullptr;
}

std::shared_ptr<SymbolInfo> SymbolTable::lookupLocal(const std::string& name) const {
    auto it = symbols_.find(name);
    return it != symbols_.end() ? it->second : nullptr;
}

std::vector<std::shared_ptr<SymbolInfo>> SymbolTable::getAllSymbols() const {
    std::vector<std::shared_ptr<SymbolInfo>> result;
    for (const auto& pair : symbols_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<std::shared_ptr<SymbolInfo>> SymbolTable::getSymbolsByType(SymbolType type) const {
    std::vector<std::shared_ptr<SymbolInfo>> result;
    for (const auto& pair : symbols_) {
        if (pair.second->type == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

// GlobalMap实现
GlobalMap::GlobalMap() 
    : globalSymbolTable_(std::make_shared<SymbolTable>("global")) {
    initializeStandardElements();
    initializeStandardProperties();
    initializeKeywords();
}

void GlobalMap::initializeStandardElements() {
    // 添加标准HTML元素
    for (const auto& elem : STANDARD_HTML_ELEMENTS) {
        htmlElements_.insert(elem);
    }
}

void GlobalMap::initializeStandardProperties() {
    // 添加标准CSS属性
    for (const auto& prop : STANDARD_CSS_PROPERTIES) {
        cssProperties_.insert(prop);
    }
}

void GlobalMap::initializeKeywords() {
    // 初始化关键字映射
    // 默认映射（标准名称）
    keywordMap_["text"] = "text";
    keywordMap_["style"] = "style";
    keywordMap_["script"] = "script";
    keywordMap_["inherit"] = "inherit";
    keywordMap_["delete"] = "delete";
    keywordMap_["insert"] = "insert";
    keywordMap_["after"] = "after";
    keywordMap_["before"] = "before";
    keywordMap_["replace"] = "replace";
    keywordMap_["at top"] = "at top";
    keywordMap_["at bottom"] = "at bottom";
    keywordMap_["from"] = "from";
    keywordMap_["as"] = "as";
    keywordMap_["except"] = "except";
    
    // 类型标识符映射
    keywordMap_["@Style"] = "@Style";
    keywordMap_["@style"] = "@Style";  // 统一映射到@Style
    keywordMap_["@CSS"] = "@Style";
    keywordMap_["@Css"] = "@Style";
    keywordMap_["@css"] = "@Style";
    keywordMap_["@Element"] = "@Element";
    keywordMap_["@Var"] = "@Var";
    keywordMap_["@Html"] = "@Html";
    keywordMap_["@JavaScript"] = "@JavaScript";
    keywordMap_["@Chtl"] = "@Chtl";
    keywordMap_["@CJmod"] = "@CJmod";
    keywordMap_["@Config"] = "@Config";
    
    // 方括号关键字
    keywordMap_["[Template]"] = "[Template]";
    keywordMap_["[Custom]"] = "[Custom]";
    keywordMap_["[Origin]"] = "[Origin]";
    keywordMap_["[Import]"] = "[Import]";
    keywordMap_["[Namespace]"] = "[Namespace]";
    keywordMap_["[Configuration]"] = "[Configuration]";
    keywordMap_["[Info]"] = "[Info]";
    keywordMap_["[Export]"] = "[Export]";
}

bool GlobalMap::isHTMLElement(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return htmlElements_.find(name) != htmlElements_.end();
}

void GlobalMap::registerHTMLElement(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    htmlElements_.insert(name);
}

std::vector<std::string> GlobalMap::getAllHTMLElements() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::vector<std::string>(htmlElements_.begin(), htmlElements_.end());
}

bool GlobalMap::isCSSProperty(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return cssProperties_.find(name) != cssProperties_.end();
}

void GlobalMap::registerCSSProperty(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    cssProperties_.insert(name);
}

std::vector<std::string> GlobalMap::getAllCSSProperties() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return std::vector<std::string>(cssProperties_.begin(), cssProperties_.end());
}

std::string GlobalMap::mapKeyword(const std::string& keyword) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = keywordMap_.find(keyword);
    return it != keywordMap_.end() ? it->second : keyword;
}

void GlobalMap::registerKeywordMapping(const std::string& from, const std::string& to) {
    std::lock_guard<std::mutex> lock(mutex_);
    keywordMap_[from] = to;
}

std::string GlobalMap::generateUniqueName(const std::string& prefix) {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t& counter = nameCounters_[prefix];
    return prefix + "_" + std::to_string(++counter);
}

void GlobalMap::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // 清空符号表
    globalSymbolTable_ = std::make_shared<SymbolTable>("global");
    
    // 清空计数器
    nameCounters_.clear();
    
    // 重新初始化
    htmlElements_.clear();
    cssProperties_.clear();
    keywordMap_.clear();
    
    initializeStandardElements();
    initializeStandardProperties();
    initializeKeywords();
}

GlobalMap& GlobalMap::getInstance() {
    static GlobalMap instance;
    return instance;
}

} // namespace chtl