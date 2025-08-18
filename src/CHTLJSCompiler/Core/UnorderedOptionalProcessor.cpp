#include "UnorderedOptionalProcessor.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace chtljs {

UnorderedOptionalProcessor::UnorderedOptionalProcessor() {
    initializeCommonUndecoratedValues();
    initializeContextRules();
}

UnorderedOptionalProcessor::~UnorderedOptionalProcessor() = default;

// === 无修饰字面量处理 ===

std::string UnorderedOptionalProcessor::processUndecoratedLiterals(const std::string& code) {
    // 在CHTL JS中支持无引号的字符串字面量
    
    std::string processed = code;
    
    // 查找键值对中的无修饰字面量
    std::regex keyValueRegex(R"((\w+)\s*:\s*([^,}\n;]+))");
    std::sregex_iterator iter(code.begin(), code.end(), keyValueRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
        // 清理值
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t,") + 1);
        
        if (isUndecoratedLiteral(value)) {
            std::string convertedValue = convertUndecoratedLiteral(value);
            std::string originalPair = key + ": " + value;
            std::string newPair = key + ": " + convertedValue;
            
            size_t pos = processed.find(originalPair);
            if (pos != std::string::npos) {
                processed.replace(pos, originalPair.length(), newPair);
            }
        }
    }
    
    return processed;
}

bool UnorderedOptionalProcessor::isUndecoratedLiteral(const std::string& value) {
    // 检测是否为无修饰字面量
    
    if (value.empty()) return false;
    
    // 已经有引号的不是无修饰字面量
    if ((value.front() == '"' && value.back() == '"') ||
        (value.front() == '\'' && value.back() == '\'')) {
        return false;
    }
    
    // 函数调用不是字面量
    if (value.find('(') != std::string::npos) {
        return false;
    }
    
    // 数字不需要转换
    if (std::regex_match(value, std::regex(R"(^-?\d+(\.\d+)?$)"))) {
        return false;
    }
    
    // 布尔值不需要转换
    if (value == "true" || value == "false") {
        return false;
    }
    
    // 检查是否为常见的无修饰值
    return commonUndecoratedValues_.find(value) != commonUndecoratedValues_.end() ||
           std::regex_match(value, std::regex(R"(^[a-zA-Z][\w-]*$)")); // 简单标识符
}

std::string UnorderedOptionalProcessor::convertUndecoratedLiteral(const std::string& literal) {
    // 转换无修饰字面量为标准JavaScript
    return "\"" + literal + "\"";
}

// === 无序键值对处理 ===

UnorderedOptionalProcessor::ProcessedObject UnorderedOptionalProcessor::processUnorderedKeyValues(const std::string& objectCode) {
    ProcessedObject result;
    
    // 解析对象内容
    result.pairs = parseObjectContent(objectCode);
    
    // 重排序键值对
    result.pairs = reorderKeyValuePairs(result.pairs);
    
    // 生成处理后的代码
    result.processedCode = generateOptimizedObject(result);
    
    return result;
}

std::vector<UnorderedOptionalProcessor::KeyValuePair> UnorderedOptionalProcessor::reorderKeyValuePairs(const std::vector<KeyValuePair>& pairs) {
    std::vector<KeyValuePair> reordered = pairs;
    
    // 按重要性排序：
    // 1. target (最重要)
    // 2. 事件类型 (click, mouseenter等)
    // 3. 配置参数 (duration, easing等)
    // 4. 回调函数 (callback等)
    
    std::sort(reordered.begin(), reordered.end(), [](const KeyValuePair& a, const KeyValuePair& b) {
        // 定义优先级
        auto getPriority = [](const std::string& key) {
            if (key == "target") return 1000;
            if (key == "click" || key == "mouseenter" || key == "mouseleave") return 900;
            if (key == "duration" || key == "easing" || key == "delay") return 800;
            if (key == "begin" || key == "end" || key == "when") return 700;
            if (key == "callback" || key == "complete") return 600;
            return 500; // 默认优先级
        };
        
        return getPriority(a.key) > getPriority(b.key);
    });
    
    return reordered;
}

std::unordered_map<std::string, std::vector<std::string>> UnorderedOptionalProcessor::analyzeKeyDependencies(const std::vector<KeyValuePair>& pairs) {
    std::unordered_map<std::string, std::vector<std::string>> dependencies;
    
    // 分析键值对之间的依赖关系
    for (const auto& pair : pairs) {
        if (pair.key == "target") {
            // target是其他键的依赖
            for (const auto& other : pairs) {
                if (other.key != "target") {
                    dependencies[other.key].push_back("target");
                }
            }
        }
        
        if (pair.key == "begin" || pair.key == "end") {
            // begin/end依赖于duration
            dependencies[pair.key].push_back("duration");
        }
    }
    
    return dependencies;
}

// === 可选键值对处理 ===

UnorderedOptionalProcessor::ProcessedObject UnorderedOptionalProcessor::processOptionalKeyValues(const std::string& objectCode) {
    ProcessedObject result;
    
    // 解析对象内容
    result.pairs = parseObjectContent(objectCode);
    
    // 标记可选键值对
    for (auto& pair : result.pairs) {
        pair.isOptional = isOptionalKey(pair.key, "default");
    }
    
    // 生成处理后的代码
    result.processedCode = generateOptimizedObject(result);
    
    return result;
}

bool UnorderedOptionalProcessor::isOptionalKey(const std::string& key, const std::string& context) {
    // 检测可选键值对
    
    ContextRules rules = getContextRules(context);
    
    return std::find(rules.optionalKeys.begin(), rules.optionalKeys.end(), key) != rules.optionalKeys.end();
}

std::string UnorderedOptionalProcessor::getDefaultValue(const std::string& key, const std::string& context) {
    // 为缺失的可选键值对提供默认值
    
    ContextRules rules = getContextRules(context);
    
    auto it = rules.defaultValues.find(key);
    if (it != rules.defaultValues.end()) {
        return it->second;
    }
    
    // 通用默认值
    if (key == "duration") return "1000";
    if (key == "easing") return "\"ease\"";
    if (key == "delay") return "0";
    if (key == "loop") return "1";
    if (key == "direction") return "\"normal\"";
    
    return "undefined";
}

bool UnorderedOptionalProcessor::validateRequiredKeys(const std::vector<KeyValuePair>& pairs, const std::string& context) {
    ContextRules rules = getContextRules(context);
    
    std::unordered_set<std::string> presentKeys;
    for (const auto& pair : pairs) {
        presentKeys.insert(pair.key);
    }
    
    for (const auto& requiredKey : rules.requiredKeys) {
        if (presentKeys.find(requiredKey) == presentKeys.end()) {
            addError("缺少必需的键值对: " + requiredKey);
            return false;
        }
    }
    
    return true;
}

// === 上下文感知处理 ===

UnorderedOptionalProcessor::ProcessedObject UnorderedOptionalProcessor::processContextAwareKeyValues(const std::string& objectCode, const std::string& context) {
    ProcessedObject result;
    
    // 获取上下文规则
    ContextRules rules = getContextRules(context);
    
    // 解析对象内容
    result.pairs = parseObjectContent(objectCode);
    
    // 应用上下文规则
    if (rules.allowsUnordered) {
        result.pairs = reorderKeyValuePairs(result.pairs);
    }
    
    // 标记可选键值对
    for (auto& pair : result.pairs) {
        pair.isOptional = std::find(rules.optionalKeys.begin(), rules.optionalKeys.end(), pair.key) != rules.optionalKeys.end();
    }
    
    // 验证必需键值对
    if (!validateRequiredKeys(result.pairs, context)) {
        result.supportsOptional = false;
    }
    
    // 生成处理后的代码
    result.processedCode = generateOptimizedObject(result);
    
    // 注入默认值
    std::string defaultInjection = generateDefaultValueInjection(result.pairs, rules);
    if (!defaultInjection.empty()) {
        result.processedCode = defaultInjection + "\n" + result.processedCode;
    }
    
    return result;
}

UnorderedOptionalProcessor::ContextRules UnorderedOptionalProcessor::getContextRules(const std::string& context) {
    auto it = contextRulesCache_.find(context);
    if (it != contextRulesCache_.end()) {
        return it->second;
    }
    
    ContextRules rules;
    
    if (context == "listen") {
        rules.requiredKeys = {}; // listen没有必需键，所有事件都是可选的
        rules.optionalKeys = {"click", "mouseenter", "mouseleave", "mousemove", "keydown", "keyup", "focus", "blur"};
        rules.allowsUnordered = true;
        rules.keyOrder = {"click", "mouseenter", "mouseleave", "mousemove", "keydown", "keyup"};
    } else if (context == "delegate") {
        rules.requiredKeys = {"target"}; // delegate必须有target
        rules.optionalKeys = {"click", "mouseenter", "mouseleave", "mousemove", "keydown", "keyup"};
        rules.allowsUnordered = true;
        rules.keyOrder = {"target", "click", "mouseenter", "mouseleave"};
    } else if (context == "animate") {
        rules.requiredKeys = {"target"}; // animate必须有target
        rules.optionalKeys = {"duration", "easing", "delay", "loop", "direction", "callback", "begin", "end", "when"};
        rules.defaultValues = {
            {"duration", "1000"},
            {"easing", "\"ease\""},
            {"delay", "0"},
            {"loop", "1"},
            {"direction", "\"normal\""}
        };
        rules.allowsUnordered = true;
        rules.keyOrder = {"target", "duration", "easing", "begin", "when", "end", "loop", "direction", "delay", "callback"};
    } else if (context == "iNeverAway") {
        rules.requiredKeys = {}; // iNeverAway所有键都是可选的
        rules.optionalKeys = {}; // 动态键，无法预定义
        rules.allowsUnordered = true;
    }
    
    contextRulesCache_[context] = rules;
    return rules;
}

// === 智能解析 ===

std::vector<UnorderedOptionalProcessor::KeyValuePair> UnorderedOptionalProcessor::parseObjectContent(const std::string& content) {
    std::vector<KeyValuePair> pairs;
    
    // 移除外层大括号
    std::string cleanContent = content;
    if (cleanContent.front() == '{' && cleanContent.back() == '}') {
        cleanContent = cleanContent.substr(1, cleanContent.length() - 2);
    }
    
    // 智能分割
    std::vector<std::string> pairStrings = smartSplitKeyValuePairs(cleanContent);
    
    int order = 0;
    for (const auto& pairStr : pairStrings) {
        if (isValidKeyValueFormat(pairStr)) {
            auto [key, value] = extractKeyValue(pairStr);
            
            KeyValuePair pair;
            pair.key = normalizeKey(key);
            pair.value = value;
            pair.originalOrder = order++;
            pair.hasUndecoratedLiteral = isUndecoratedLiteral(value);
            
            pairs.push_back(pair);
        }
    }
    
    return pairs;
}

std::vector<std::string> UnorderedOptionalProcessor::smartSplitKeyValuePairs(const std::string& content) {
    std::vector<std::string> pairs;
    std::string currentPair;
    int braceLevel = 0;
    int parenLevel = 0;
    int bracketLevel = 0;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        
        // 处理字符串状态
        if ((c == '"' || c == '\'') && (i == 0 || content[i-1] != '\\')) {
            if (!inString) {
                inString = true;
                stringChar = c;
            } else if (c == stringChar) {
                inString = false;
                stringChar = '\0';
            }
        }
        
        if (!inString) {
            // 处理嵌套层级
            if (c == '{') braceLevel++;
            else if (c == '}') braceLevel--;
            else if (c == '(') parenLevel++;
            else if (c == ')') parenLevel--;
            else if (c == '[') bracketLevel++;
            else if (c == ']') bracketLevel--;
            else if (c == ',' && braceLevel == 0 && parenLevel == 0 && bracketLevel == 0) {
                // 顶层逗号，分割键值对
                if (!currentPair.empty()) {
                    pairs.push_back(currentPair);
                    currentPair.clear();
                }
                continue;
            }
        }
        
        currentPair += c;
    }
    
    if (!currentPair.empty()) {
        pairs.push_back(currentPair);
    }
    
    return pairs;
}

// === 代码生成 ===

std::string UnorderedOptionalProcessor::generateOptimizedObject(const ProcessedObject& processed) {
    std::stringstream result;
    
    result << "{\n";
    
    for (size_t i = 0; i < processed.pairs.size(); ++i) {
        const auto& pair = processed.pairs[i];
        
        result << "    " << pair.key << ": ";
        
        if (pair.hasUndecoratedLiteral) {
            result << convertUndecoratedLiteral(pair.value);
        } else {
            result << pair.value;
        }
        
        if (i < processed.pairs.size() - 1) {
            result << ",";
        }
        
        if (pair.isOptional) {
            result << " // 可选";
        }
        
        result << "\n";
    }
    
    result << "}";
    
    return result.str();
}

std::string UnorderedOptionalProcessor::generateDefaultValueInjection(const std::vector<KeyValuePair>& pairs, const ContextRules& rules) {
    std::stringstream code;
    
    // 检查缺失的可选键值对
    std::unordered_set<std::string> presentKeys;
    for (const auto& pair : pairs) {
        presentKeys.insert(pair.key);
    }
    
    bool hasDefaults = false;
    for (const auto& optionalKey : rules.optionalKeys) {
        if (presentKeys.find(optionalKey) == presentKeys.end()) {
            auto defaultIt = rules.defaultValues.find(optionalKey);
            if (defaultIt != rules.defaultValues.end()) {
                if (!hasDefaults) {
                    code << "// 自动注入缺失的可选键值对默认值\n";
                    code << "const defaultValues = {\n";
                    hasDefaults = true;
                }
                code << "    " << optionalKey << ": " << defaultIt->second << ",\n";
            }
        }
    }
    
    if (hasDefaults) {
        code << "};\n";
        code << "const mergedConfig = Object.assign({}, defaultValues, config);\n";
    }
    
    return code.str();
}

// === 辅助方法 ===

std::string UnorderedOptionalProcessor::normalizeKey(const std::string& key) {
    std::string normalized = key;
    
    // 移除前后空白
    normalized.erase(0, normalized.find_first_not_of(" \t"));
    normalized.erase(normalized.find_last_not_of(" \t") + 1);
    
    return normalized;
}

bool UnorderedOptionalProcessor::isValidKeyValueFormat(const std::string& pair) {
    // 检查是否为有效的键值对格式
    return pair.find(':') != std::string::npos;
}

std::pair<std::string, std::string> UnorderedOptionalProcessor::extractKeyValue(const std::string& pair) {
    size_t colonPos = pair.find(':');
    if (colonPos == std::string::npos) {
        return {"", ""};
    }
    
    std::string key = pair.substr(0, colonPos);
    std::string value = pair.substr(colonPos + 1);
    
    // 清理空白
    key.erase(0, key.find_first_not_of(" \t"));
    key.erase(key.find_last_not_of(" \t") + 1);
    value.erase(0, value.find_first_not_of(" \t"));
    value.erase(value.find_last_not_of(" \t") + 1);
    
    return {key, value};
}

void UnorderedOptionalProcessor::initializeCommonUndecoratedValues() {
    // 初始化常见的无修饰字面量值
    commonUndecoratedValues_ = {
        // CSS颜色
        "red", "blue", "green", "yellow", "orange", "purple", "pink", "black", "white", "gray",
        "transparent", "inherit", "initial", "unset",
        
        // CSS单位相关
        "auto", "none", "normal", "bold", "italic", "underline",
        "left", "right", "center", "top", "bottom", "middle",
        "block", "inline", "flex", "grid", "absolute", "relative", "fixed",
        
        // 动画相关
        "ease", "ease-in", "ease-out", "ease-in-out", "linear",
        "forwards", "backwards", "both", "infinite", "alternate",
        
        // 事件相关
        "click", "mouseenter", "mouseleave", "mousemove", "mousedown", "mouseup",
        "keydown", "keyup", "focus", "blur", "change", "input", "submit",
        
        // CHTL JS特殊值
        "ASCII", "Pixel", "normal", "reverse"
    };
}

void UnorderedOptionalProcessor::initializeContextRules() {
    // 在getContextRules中动态初始化
}

// === 错误处理 ===

void UnorderedOptionalProcessor::addError(const std::string& message) {
    errors_.push_back("UnorderedOptional: " + message);
}

void UnorderedOptionalProcessor::addWarning(const std::string& message) {
    warnings_.push_back("UnorderedOptional: " + message);
}

void UnorderedOptionalProcessor::clearErrors() {
    errors_.clear();
    warnings_.clear();
}

bool UnorderedOptionalProcessor::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> UnorderedOptionalProcessor::getErrors() const {
    return errors_;
}

std::vector<std::string> UnorderedOptionalProcessor::getWarnings() const {
    return warnings_;
}

} // namespace chtljs
} // namespace chtl