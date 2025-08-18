#include "ExportGenerator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include <algorithm>

namespace chtl {
namespace export_gen {

// 全局导出生成器实例
std::shared_ptr<ExportGenerator> g_exportGenerator = std::make_shared<ExportGenerator>();

ExportGenerator::ExportGenerator() = default;
ExportGenerator::~ExportGenerator() = default;

bool ExportGenerator::analyzeSourceFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "无法打开源文件: " << filePath << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    return analyzeSourceContent(content, filePath);
}

bool ExportGenerator::analyzeSourceContent(const std::string& content, const std::string& filePath) {
    // 解析各种类型的定义
    parseCustomElements(content);
    parseCustomStyles(content);
    parseCustomVars(content);
    parseTemplateElements(content);
    parseTemplateStyles(content);
    parseTemplateVars(content);
    parseOriginItems(content);
    parseConfigurations(content);
    
    return true;
}

void ExportGenerator::parseCustomElements(const std::string& content) {
    // 匹配 [Custom] @Element 名称
    std::regex customElementRegex(R"(\[Custom\]\s*@Element\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), customElementRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string elementName = iter->str(1);
        
        if (processedNames_.find(elementName) == processedNames_.end()) {
            ExportItem item;
            item.type = ExportType::CUSTOM_ELEMENT;
            item.name = elementName;
            item.fullName = generateFullName(ExportType::CUSTOM_ELEMENT, elementName);
            
            addExportItem(item);
            processedNames_.insert(elementName);
        }
    }
}

void ExportGenerator::parseCustomStyles(const std::string& content) {
    // 匹配 [Custom] @Style 名称
    std::regex customStyleRegex(R"(\[Custom\]\s*@Style\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), customStyleRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string styleName = iter->str(1);
        
        if (processedNames_.find(styleName) == processedNames_.end()) {
            ExportItem item;
            item.type = ExportType::CUSTOM_STYLE;
            item.name = styleName;
            item.fullName = generateFullName(ExportType::CUSTOM_STYLE, styleName);
            
            addExportItem(item);
            processedNames_.insert(styleName);
        }
    }
    
    // 同时解析样式块中的类名
    std::regex styleClassRegex(R"(\.([a-zA-Z][\w-]*)\s*\{)");
    std::sregex_iterator classIter(content.begin(), content.end(), styleClassRegex);
    
    for (; classIter != end; ++classIter) {
        std::string className = classIter->str(1);
        std::string styleItemName = className + "Style";
        
        if (processedNames_.find(styleItemName) == processedNames_.end()) {
            ExportItem item;
            item.type = ExportType::CUSTOM_STYLE;
            item.name = styleItemName;
            item.fullName = generateFullName(ExportType::CUSTOM_STYLE, styleItemName);
            
            addExportItem(item);
            processedNames_.insert(styleItemName);
        }
    }
}

void ExportGenerator::parseCustomVars(const std::string& content) {
    // 匹配 [Custom] @Var 名称
    std::regex customVarRegex(R"(\[Custom\]\s*@Var\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), customVarRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string varName = iter->str(1);
        
        if (processedNames_.find(varName) == processedNames_.end()) {
            ExportItem item;
            item.type = ExportType::CUSTOM_VAR;
            item.name = varName;
            item.fullName = generateFullName(ExportType::CUSTOM_VAR, varName);
            
            addExportItem(item);
            processedNames_.insert(varName);
        }
    }
}

void ExportGenerator::parseTemplateElements(const std::string& content) {
    // 匹配 [Template] @Element 名称
    std::regex templateElementRegex(R"(\[Template\]\s*@Element\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), templateElementRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string elementName = iter->str(1);
        
        if (processedNames_.find(elementName) == processedNames_.end()) {
            ExportItem item;
            item.type = ExportType::TEMPLATE_ELEMENT;
            item.name = elementName;
            item.fullName = generateFullName(ExportType::TEMPLATE_ELEMENT, elementName);
            
            addExportItem(item);
            processedNames_.insert(elementName);
        }
    }
}

void ExportGenerator::parseTemplateStyles(const std::string& content) {
    // 匹配 [Template] @Style 名称
    std::regex templateStyleRegex(R"(\[Template\]\s*@Style\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), templateStyleRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string styleName = iter->str(1);
        
        if (processedNames_.find(styleName) == processedNames_.end()) {
            ExportItem item;
            item.type = ExportType::TEMPLATE_STYLE;
            item.name = styleName;
            item.fullName = generateFullName(ExportType::TEMPLATE_STYLE, styleName);
            
            addExportItem(item);
            processedNames_.insert(styleName);
        }
    }
}

void ExportGenerator::parseTemplateVars(const std::string& content) {
    // 匹配 [Template] @Var 名称
    std::regex templateVarRegex(R"(\[Template\]\s*@Var\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), templateVarRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string varName = iter->str(1);
        
        if (processedNames_.find(varName) == processedNames_.end()) {
            ExportItem item;
            item.type = ExportType::TEMPLATE_VAR;
            item.name = varName;
            item.fullName = generateFullName(ExportType::TEMPLATE_VAR, varName);
            
            addExportItem(item);
            processedNames_.insert(varName);
        }
    }
}

void ExportGenerator::parseOriginItems(const std::string& content) {
    // 匹配各种原始嵌入类型
    std::vector<std::pair<std::string, ExportType>> originTypes = {
        {"@Html", ExportType::ORIGIN_HTML},
        {"@Style", ExportType::ORIGIN_STYLE},
        {"@JavaScript", ExportType::ORIGIN_JAVASCRIPT},
        {"@Vue", ExportType::ORIGIN_CUSTOM},
        {"@React", ExportType::ORIGIN_CUSTOM},
        {"@Angular", ExportType::ORIGIN_CUSTOM}
    };
    
    for (const auto& [typeStr, exportType] : originTypes) {
        std::regex originRegex(R"(\[Origin\]\s*)" + typeStr + R"(\s+(\w+))");
        std::sregex_iterator iter(content.begin(), content.end(), originRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string originName = iter->str(1);
            
            if (processedNames_.find(originName) == processedNames_.end()) {
                ExportItem item;
                item.type = exportType;
                item.name = originName;
                item.fullName = generateFullName(exportType, originName);
                
                addExportItem(item);
                processedNames_.insert(originName);
            }
        }
    }
}

void ExportGenerator::parseConfigurations(const std::string& content) {
    // 匹配 [Configuration] @Config 名称
    std::regex configRegex(R"(\[Configuration\]\s*@Config\s+(\w+))");
    std::sregex_iterator iter(content.begin(), content.end(), configRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string configName = iter->str(1);
        
        if (processedNames_.find(configName) == processedNames_.end()) {
            ExportItem item;
            item.type = ExportType::CONFIGURATION;
            item.name = configName;
            item.fullName = generateFullName(ExportType::CONFIGURATION, configName);
            
            addExportItem(item);
            processedNames_.insert(configName);
        }
    }
}

void ExportGenerator::addExportItem(const ExportItem& item) {
    exportItems_.push_back(item);
}

std::string ExportGenerator::generateExportSection() {
    if (exportItems_.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "// 外部查询表，优化性能用，无实际作用\n";
    oss << "// 不需要写这个，此由系统生成\n";
    oss << "[Export]\n";
    oss << "{\n";
    
    // 按类型分组
    auto groups = groupExportsByType();
    
    for (const auto& group : groups) {
        if (!group.items.empty()) {
            oss << "    " << formatExportType(group.type);
            
            // 输出该类型的所有项
            for (size_t i = 0; i < group.items.size(); ++i) {
                if (i == 0) {
                    oss << " " << group.items[i].name;
                } else {
                    oss << ", " << group.items[i].name;
                }
                
                // 每行最多80字符，超过则换行
                if (i < group.items.size() - 1 && (i + 1) % 3 == 0) {
                    oss << ",\n                    ";
                }
            }
            oss << ";\n\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

std::string ExportGenerator::generateInfoFile(const std::string& name, const std::string& version,
                                             const std::string& description, const std::string& author,
                                             const std::string& license, const std::string& category) {
    std::ostringstream oss;
    oss << "// " << name << " 模块信息\n";
    oss << "// 只需要写这个即可\n";
    oss << "[Info]\n";
    oss << "{\n";
    oss << "    name = \"" << name << "\";\n";
    oss << "    version = \"" << version << "\";\n";
    oss << "    description = \"" << description << "\";\n";
    oss << "    author = \"" << author << "\";\n";
    oss << "    license = \"" << license << "\";\n";
    oss << "    dependencies = \"\";\n";
    oss << "    category = \"" << category << "\";\n";
    oss << "    minCHTLVersion = \"1.0.0\";\n";
    oss << "    maxCHTLVersion = \"2.0.0\";\n";
    oss << "}\n\n";
    
    // 添加导出节
    oss << generateExportSection();
    
    return oss.str();
}

bool ExportGenerator::updateInfoFileExports(const std::string& infoFilePath) {
    // 读取现有Info文件
    std::ifstream file(infoFilePath);
    if (!file.is_open()) {
        std::cerr << "无法打开Info文件: " << infoFilePath << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // 查找并替换[Export]节
    std::regex exportRegex(R"(// 外部查询表.*?\[Export\]\s*\{[^}]*\})");
    std::string newExportSection = generateExportSection();
    
    if (std::regex_search(content, exportRegex)) {
        // 替换现有的[Export]节
        content = std::regex_replace(content, exportRegex, newExportSection);
    } else {
        // 添加新的[Export]节
        content += "\n" + newExportSection;
    }
    
    // 写回文件
    std::ofstream outFile(infoFilePath);
    if (!outFile.is_open()) {
        std::cerr << "无法写入Info文件: " << infoFilePath << std::endl;
        return false;
    }
    
    outFile << content;
    outFile.close();
    
    return true;
}

void ExportGenerator::clearExports() {
    exportItems_.clear();
    processedNames_.clear();
}

std::unordered_map<ExportType, int> ExportGenerator::getExportStatistics() {
    std::unordered_map<ExportType, int> stats;
    
    for (const auto& item : exportItems_) {
        stats[item.type]++;
    }
    
    return stats;
}

std::vector<ExportItem> ExportGenerator::getAllExportItems() const {
    return exportItems_;
}

std::vector<ExportItem> ExportGenerator::getExportItemsByType(ExportType type) const {
    std::vector<ExportItem> items;
    
    for (const auto& item : exportItems_) {
        if (item.type == type) {
            items.push_back(item);
        }
    }
    
    return items;
}

std::string ExportGenerator::formatExportType(ExportType type) {
    switch (type) {
        case ExportType::CUSTOM_STYLE:     return "[Custom] @Style";
        case ExportType::CUSTOM_ELEMENT:   return "[Custom] @Element";
        case ExportType::CUSTOM_VAR:       return "[Custom] @Var";
        case ExportType::TEMPLATE_STYLE:   return "[Template] @Style";
        case ExportType::TEMPLATE_ELEMENT: return "[Template] @Element";
        case ExportType::TEMPLATE_VAR:     return "[Template] @Var";
        case ExportType::ORIGIN_HTML:      return "[Origin] @Html";
        case ExportType::ORIGIN_STYLE:     return "[Origin] @Style";
        case ExportType::ORIGIN_JAVASCRIPT:return "[Origin] @Javascript";
        case ExportType::ORIGIN_CUSTOM:    return "[Origin] @Custom";
        case ExportType::CONFIGURATION:    return "[Configuration] @Config";
        default:                           return "[Unknown]";
    }
}

std::string ExportGenerator::formatExportItem(const ExportItem& item) {
    return item.name;
}

std::vector<ExportGroup> ExportGenerator::groupExportsByType() {
    std::unordered_map<ExportType, std::vector<ExportItem>> groupMap;
    
    for (const auto& item : exportItems_) {
        groupMap[item.type].push_back(item);
    }
    
    std::vector<ExportGroup> groups;
    for (auto& [type, items] : groupMap) {
        ExportGroup group;
        group.type = type;
        group.items = std::move(items);
        groups.push_back(group);
    }
    
    return groups;
}

bool ExportGenerator::isValidExportName(const std::string& name) {
    if (name.empty()) return false;
    
    // 检查是否为有效的标识符
    std::regex identifierRegex(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
    return std::regex_match(name, identifierRegex);
}

std::string ExportGenerator::generateFullName(ExportType type, const std::string& name) {
    return formatExportType(type) + " " + name;
}

// 工具函数实现
namespace utils {

std::string extractModuleName(const std::string& filePath) {
    std::filesystem::path path(filePath);
    return path.stem().string();
}

bool isInfoFile(const std::string& filePath) {
    return filePath.find("/info/") != std::string::npos;
}

bool isSourceFile(const std::string& filePath) {
    return filePath.find("/src/") != std::string::npos && 
           (filePath.length() >= 5 && filePath.substr(filePath.length() - 5) == ".chtl");
}

bool processModuleDirectory(const std::string& moduleDir) {
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(moduleDir)) {
            if (entry.is_regular_file() && isSourceFile(entry.path().string())) {
                g_exportGenerator->analyzeSourceFile(entry.path().string());
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "处理模块目录失败: " << e.what() << std::endl;
        return false;
    }
}

bool updateAllInfoFiles(const std::string& rootDir) {
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(rootDir)) {
            if (entry.is_regular_file() && isInfoFile(entry.path().string())) {
                // 清除之前的导出项
                g_exportGenerator->clearExports();
                
                // 分析对应的源文件
                std::string infoPath = entry.path().string();
                std::string srcPath = std::regex_replace(infoPath, std::regex("/info/"), "/src/");
                
                if (std::filesystem::exists(srcPath)) {
                    g_exportGenerator->analyzeSourceFile(srcPath);
                    g_exportGenerator->updateInfoFileExports(infoPath);
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "更新Info文件失败: " << e.what() << std::endl;
        return false;
    }
}

} // namespace utils

} // namespace export_gen
} // namespace chtl