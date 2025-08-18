#include "OriginManager.h"
#include <iostream>
#include <regex>
#include <algorithm>

namespace chtl {
namespace origin_system {

// 全局原始嵌入管理器实例
std::shared_ptr<OriginManager> g_originManager = std::make_shared<OriginManager>();

OriginManager::OriginManager() {
    initializeDefaultOriginTypes();
}

OriginManager::~OriginManager() = default;

// === 原始嵌入管理 ===

void OriginManager::addOriginItem(const OriginItem& item) {
    originItems_[item.typeName].push_back(std::make_shared<OriginItem>(item));
}

std::shared_ptr<OriginItem> OriginManager::getOriginItem(const std::string& typeName, const std::string& name) {
    auto it = originItems_.find(typeName);
    if (it == originItems_.end()) {
        return nullptr;
    }
    
    for (const auto& item : it->second) {
        if (name.empty() || item->name == name) {
            return item;
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<OriginItem>> OriginManager::getOriginItemsByType(OriginType type) {
    std::vector<std::shared_ptr<OriginItem>> result;
    
    for (const auto& [typeName, items] : originItems_) {
        for (const auto& item : items) {
            if (item->type == type) {
                result.push_back(item);
            }
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<OriginItem>> OriginManager::getOriginItemsByTypeName(const std::string& typeName) {
    auto it = originItems_.find(typeName);
    if (it != originItems_.end()) {
        return it->second;
    }
    return {};
}

// === 带名原始嵌入 ===

void OriginManager::addNamedOrigin(const std::string& typeName, const std::string& name, const std::string& content) {
    OriginItem item;
    item.type = parseOriginType(typeName);
    item.typeName = typeName;
    item.name = name;
    item.content = content;
    item.isNamed = true;
    
    addOriginItem(item);
}

std::string OriginManager::getNamedOriginContent(const std::string& typeName, const std::string& name) {
    auto item = getOriginItem(typeName, name);
    return item ? item->content : "";
}

bool OriginManager::hasNamedOrigin(const std::string& typeName, const std::string& name) const {
    auto it = originItems_.find(typeName);
    if (it == originItems_.end()) {
        return false;
    }
    
    for (const auto& item : it->second) {
        if (item->name == name) {
            return true;
        }
    }
    
    return false;
}

// === 自定义原始类型管理 ===

void OriginManager::registerCustomOriginType(const std::string& typeName, const std::string& description) {
    if (!isValidOriginTypeName(typeName)) {
        errors_.push_back("无效的原始类型名称: " + typeName);
        return;
    }
    
    auto customType = std::make_shared<CustomOriginType>();
    customType->typeName = typeName;
    customType->description = description;
    customType->isEnabled = true;
    
    customOriginTypes_[typeName] = customType;
}

void OriginManager::unregisterCustomOriginType(const std::string& typeName) {
    customOriginTypes_.erase(typeName);
}

bool OriginManager::isCustomOriginTypeRegistered(const std::string& typeName) const {
    return customOriginTypes_.find(typeName) != customOriginTypes_.end();
}

std::vector<std::string> OriginManager::getAllCustomOriginTypes() const {
    std::vector<std::string> types;
    for (const auto& [typeName, customType] : customOriginTypes_) {
        if (customType->isEnabled) {
            types.push_back(typeName);
        }
    }
    return types;
}

// === 原始嵌入解析 ===

bool OriginManager::parseOriginBlock(const std::string& content, const std::string& typeName, const std::string& name) {
    if (!validateOriginType(typeName)) {
        errors_.push_back("无效的原始嵌入类型: " + typeName);
        return false;
    }
    
    if (!name.empty() && !isValidIdentifier(name)) {
        errors_.push_back("无效的原始嵌入名称: " + name);
        return false;
    }
    
    if (!validateOriginContent(content, typeName)) {
        errors_.push_back("原始嵌入内容验证失败");
        return false;
    }
    
    // 创建原始嵌入项
    OriginItem item;
    item.type = parseOriginType(typeName);
    item.typeName = typeName;
    item.name = name;
    item.content = content;
    item.isNamed = !name.empty();
    
    addOriginItem(item);
    return true;
}

// === 原始嵌入生成 ===

std::string OriginManager::generateHTMLOrigin(const std::string& name) {
    auto item = getOriginItem("@Html", name);
    return item ? item->content : "";
}

std::string OriginManager::generateStyleOrigin(const std::string& name) {
    auto item = getOriginItem("@Style", name);
    if (item) {
        return "<style>\n" + item->content + "\n</style>";
    }
    return "";
}

std::string OriginManager::generateJavaScriptOrigin(const std::string& name) {
    auto item = getOriginItem("@JavaScript", name);
    if (item) {
        return "<script>\n" + item->content + "\n</script>";
    }
    return "";
}

std::string OriginManager::generateCustomOrigin(const std::string& typeName, const std::string& name) {
    auto item = getOriginItem(typeName, name);
    if (item) {
        // 自定义类型的处理方式由类型决定
        if (typeName == "@Vue") {
            return "<!-- Vue Component: " + name + " -->\n" + item->content;
        } else if (typeName == "@React") {
            return "<!-- React Component: " + name + " -->\n" + item->content;
        } else {
            return "<!-- Custom Origin: " + typeName + " " + name + " -->\n" + item->content;
        }
    }
    return "";
}

// === 原始嵌入使用 ===

bool OriginManager::useOriginItem(const std::string& typeName, const std::string& name) {
    if (name.empty()) {
        // 无名原始嵌入：直接查找第一个匹配类型的项
        auto items = getOriginItemsByTypeName(typeName);
        return !items.empty();
    } else {
        // 带名原始嵌入：查找特定名称的项
        return hasNamedOrigin(typeName, name);
    }
}

std::string OriginManager::getOriginContent(const std::string& typeName, const std::string& name) {
    auto item = getOriginItem(typeName, name);
    return item ? item->content : "";
}

// === 验证 ===

bool OriginManager::validateOriginType(const std::string& typeName) {
    // 检查是否为标准类型
    if (isStandardOriginType(typeName)) {
        return true;
    }
    
    // 检查是否为已注册的自定义类型
    if (isCustomOriginTypeRegistered(typeName)) {
        return true;
    }
    
    errors_.push_back("未知的原始嵌入类型: " + typeName);
    return false;
}

bool OriginManager::validateOriginContent(const std::string& content, const std::string& typeName) {
    if (content.empty()) {
        return true; // 允许空内容
    }
    
    // 按语法文档：原始嵌入内容不被CHTL处理，直接输出
    // 因此内容验证主要检查基本格式
    
    if (typeName == "@Html") {
        return validateHTMLContent(content);
    } else if (typeName == "@Style") {
        return validateStyleContent(content);
    } else if (typeName == "@JavaScript") {
        return validateJavaScriptContent(content);
    } else if (isCustomOriginTypeRegistered(typeName)) {
        return validateCustomContent(content, typeName);
    }
    
    return true; // 默认允许
}

bool OriginManager::validateHTMLContent(const std::string& content) {
    // 基础HTML内容验证
    // 检查基本的标签匹配（简化实现）
    int openTags = 0;
    int closeTags = 0;
    
    std::regex openTagRegex(R"(<[^/][^>]*>)");
    std::regex closeTagRegex(R"(<\/[^>]*>)");
    
    std::sregex_iterator openIter(content.begin(), content.end(), openTagRegex);
    std::sregex_iterator closeIter(content.begin(), content.end(), closeTagRegex);
    std::sregex_iterator end;
    
    for (; openIter != end; ++openIter) openTags++;
    for (; closeIter != end; ++closeIter) closeTags++;
    
    // 简化验证：开标签和闭标签数量应该相近
    if (abs(openTags - closeTags) > 2) {
        errors_.push_back("HTML标签可能不匹配");
        return false;
    }
    
    return true;
}

bool OriginManager::validateStyleContent(const std::string& content) {
    // 基础CSS内容验证
    // 检查大括号匹配
    int braceCount = 0;
    for (char c : content) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
    }
    
    if (braceCount != 0) {
        errors_.push_back("CSS大括号不匹配");
        return false;
    }
    
    return true;
}

bool OriginManager::validateJavaScriptContent(const std::string& content) {
    // 基础JavaScript内容验证
    // 检查基本的语法结构
    int braceCount = 0;
    int parenCount = 0;
    
    for (char c : content) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        else if (c == '(') parenCount++;
        else if (c == ')') parenCount--;
    }
    
    if (braceCount != 0 || parenCount != 0) {
        errors_.push_back("JavaScript语法结构不匹配");
        return false;
    }
    
    return true;
}

bool OriginManager::validateCustomContent(const std::string& content, const std::string& typeName) {
    // 自定义类型内容验证
    // 按语法文档：自定义类型的验证规则由类型定义决定
    
    auto customType = customOriginTypes_.find(typeName);
    if (customType != customOriginTypes_.end()) {
        // TODO: 根据自定义类型的规则进行验证
        return true;
    }
    
    return true; // 默认允许
}

// === 配置集成 ===

void OriginManager::updateFromConfiguration(const std::unordered_map<std::string, std::string>& originTypeConfig) {
    // 从配置组更新原始类型定义
    for (const auto& [configKey, configValue] : originTypeConfig) {
        if (configKey.find("ORIGINTYPE_") == 0) {
            std::string typeName = configValue;
            std::string description = "从配置组注册的自定义类型";
            registerCustomOriginType(typeName, description);
        }
    }
}

// === 清理和统计 ===

void OriginManager::clear() {
    originItems_.clear();
    customOriginTypes_.clear();
    clearErrors();
    initializeDefaultOriginTypes();
}

size_t OriginManager::getOriginItemCount() const {
    size_t count = 0;
    for (const auto& [typeName, items] : originItems_) {
        count += items.size();
    }
    return count;
}

// === 内部方法 ===

OriginType OriginManager::parseOriginType(const std::string& typeName) {
    if (typeName == "@Html") return OriginType::HTML;
    if (typeName == "@Style") return OriginType::STYLE;
    if (typeName == "@JavaScript") return OriginType::JAVASCRIPT;
    return OriginType::CUSTOM;
}

std::string OriginManager::formatOriginTypeName(const std::string& typeName) {
    // 按语法文档：原始类型名称必须以@开头
    if (typeName.empty() || typeName[0] != '@') {
        return "@" + typeName;
    }
    return typeName;
}

bool OriginManager::isValidOriginTypeName(const std::string& typeName) {
    // 按语法文档：原始类型名称必须以@开头
    if (typeName.empty() || typeName[0] != '@') {
        return false;
    }
    
    // 检查名称格式
    std::regex typeNameRegex(R"(@[A-Z][a-zA-Z0-9]*)");
    return std::regex_match(typeName, typeNameRegex);
}

bool OriginManager::isStandardOriginType(const std::string& typeName) {
    return typeName == "@Html" || typeName == "@Style" || typeName == "@JavaScript";
}

bool OriginManager::isValidIdentifier(const std::string& identifier) {
    if (identifier.empty()) return true; // 允许空名称（无名原始嵌入）
    
    // 检查标识符格式
    std::regex identifierRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(identifier, identifierRegex);
}

void OriginManager::initializeDefaultOriginTypes() {
    // 按语法文档初始化标准原始嵌入类型
    
    // @Html - HTML原始嵌入
    auto htmlType = std::make_shared<CustomOriginType>();
    htmlType->typeName = "@Html";
    htmlType->description = "HTML原始嵌入类型";
    htmlType->isEnabled = true;
    customOriginTypes_["@Html"] = htmlType;
    
    // @Style - CSS原始嵌入
    auto styleType = std::make_shared<CustomOriginType>();
    styleType->typeName = "@Style";
    styleType->description = "CSS原始嵌入类型";
    styleType->isEnabled = true;
    customOriginTypes_["@Style"] = styleType;
    
    // @JavaScript - JavaScript原始嵌入
    auto jsType = std::make_shared<CustomOriginType>();
    jsType->typeName = "@JavaScript";
    jsType->description = "JavaScript原始嵌入类型";
    jsType->isEnabled = true;
    customOriginTypes_["@JavaScript"] = jsType;
}

} // namespace origin_system
} // namespace chtl