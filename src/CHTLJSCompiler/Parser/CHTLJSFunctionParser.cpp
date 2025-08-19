#include "CHTLJSFunctionParser.h"
#include <iostream>
#include <unordered_set>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace chtljs {

CHTLJSFunctionParser::CHTLJSFunctionParser() : strict_mode_(true) {
    kv_manager_ = std::make_unique<CHTLJSKeyValueManager>();
    event_registry_ = &JSEventRegistry::getInstance();
}

CHTLJSFunctionParser::ParsedFunction CHTLJSFunctionParser::parseFunction(const std::string& function_call) {
    ParsedFunction result;
    clearErrors();
    
    try {
        // 1. 提取函数名
        result.function_name = extractFunctionName(function_call);
        if (result.function_name.empty()) {
            addError("无法识别函数名");
            return result;
        }
        
        // 2. 验证函数名
        if (!isValidFunctionName(result.function_name)) {
            addError("不支持的CHTL JS函数: " + result.function_name);
            return result;
        }
        
        // 3. 提取键值对块
        std::string kv_block = extractKeyValueBlock(function_call);
        if (kv_block.empty()) {
            addError("无法找到键值对块");
            return result;
        }
        
        // 4. 解析键值对
        result.key_value_pairs = parseKeyValueBlock(kv_block);
        
        // 5. 验证键值对
        if (!validateKeyValuePairs(result.key_value_pairs, result.function_name)) {
            addError("键值对验证失败");
            return result;
        }
        
        // 6. 检查必需键
        if (!checkRequiredKeys(result.key_value_pairs, result.function_name)) {
            addError("缺少必需的键");
            return result;
        }
        
        // 7. 标准化键顺序（支持无序）
        result.key_value_pairs = normalizeKeyOrder(result.key_value_pairs, result.function_name);
        
        // 8. 验证可选键
        result.key_value_pairs = validateOptionalKeys(result.key_value_pairs, result.function_name);
        
        result.is_valid = true;
        result.errors = errors_;
        result.warnings = warnings_;
        
    } catch (const std::exception& e) {
        addError("解析异常: " + std::string(e.what()));
        result.is_valid = false;
    }
    
    return result;
}

std::vector<CHTLJSKeyValueManager::KeyValueInfo> CHTLJSFunctionParser::parseKeyValueBlock(const std::string& block_content) {
    return kv_manager_->parseKeyValuePairs(block_content);
}

bool CHTLJSFunctionParser::validateKeyValuePairs(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs, const std::string& function_name) {
    for (const auto& kv : pairs) {
        if (!kv_manager_->validateKeyValuePair(kv, function_name)) {
            addError("无效的键值对: " + kv.key + " : " + kv.value);
            return false;
        }
        
        // 特殊验证：事件键
        if ((function_name == "listen" || function_name == "delegate") && !validateEventKey(kv.key)) {
            addWarning("未知的事件类型: " + kv.key);
        }
        
        // 验证函数值
        if (kv.is_function && !validateFunctionValue(kv.value)) {
            addError("无效的函数值: " + kv.value);
            return false;
        }
    }
    
    return true;
}

std::vector<CHTLJSKeyValueManager::KeyValueInfo> CHTLJSFunctionParser::normalizeKeyOrder(
    const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs, const std::string& function_name) {
    
    // 支持无序键值对：不强制特定顺序，保持用户编写的顺序
    std::vector<CHTLJSKeyValueManager::KeyValueInfo> normalized = pairs;
    
    // 可以根据需要重新排序，但通常保持用户顺序
    // 例如：将重要的事件（如click）放在前面
    if (function_name == "listen") {
        std::stable_sort(normalized.begin(), normalized.end(), 
            [](const CHTLJSKeyValueManager::KeyValueInfo& a, const CHTLJSKeyValueManager::KeyValueInfo& b) {
                // click事件优先
                if (a.key == "click") return true;
                if (b.key == "click") return false;
                return false; // 保持原有顺序
            });
    }
    
    return normalized;
}

std::vector<CHTLJSKeyValueManager::KeyValueInfo> CHTLJSFunctionParser::validateOptionalKeys(
    const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs, const std::string& function_name) {
    
    std::vector<CHTLJSKeyValueManager::KeyValueInfo> validated;
    auto all_keys = kv_manager_->getAllKeys(function_name);
    
    for (const auto& kv : pairs) {
        // 检查键是否在允许的键列表中
        if (std::find(all_keys.begin(), all_keys.end(), kv.key) != all_keys.end()) {
            validated.push_back(kv);
        } else {
            addWarning("忽略未知的键: " + kv.key);
        }
    }
    
    return validated;
}

bool CHTLJSFunctionParser::checkRequiredKeys(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs, const std::string& function_name) {
    auto required_keys = kv_manager_->getRequiredKeys(function_name);
    
    for (const std::string& required_key : required_keys) {
        bool found = false;
        for (const auto& kv : pairs) {
            if (kv.key == required_key) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            addError("缺少必需的键: " + required_key);
            return false;
        }
    }
    
    return true;
}

std::string CHTLJSFunctionParser::generateOptimizedCode(const ParsedFunction& parsed_function) {
    if (!parsed_function.is_valid) {
        return "// 解析失败的CHTL JS函数";
    }
    
    switch (parsed_function.function_name[0]) {
        case 'l': // listen
            if (parsed_function.function_name == "listen") {
                return generateListenCode(parsed_function.key_value_pairs);
            }
            break;
        case 'd': // delegate
            if (parsed_function.function_name == "delegate") {
                return generateDelegateCode(parsed_function.key_value_pairs);
            }
            break;
        case 'a': // animate
            if (parsed_function.function_name == "animate") {
                return generateAnimateCode(parsed_function.key_value_pairs);
            }
            break;
    }
    
    return "// 未知的CHTL JS函数: " + parsed_function.function_name;
}

std::string CHTLJSFunctionParser::generateListenCode(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs) {
    std::ostringstream code;
    
    // 为每个事件生成addEventListener调用
    for (const auto& kv : pairs) {
        if (event_registry_->isValidEvent(kv.key)) {
            code << generateEventBinding(kv.key, kv.value) << ";\n";
        }
    }
    
    return code.str();
}

std::string CHTLJSFunctionParser::generateDelegateCode(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs) {
    std::ostringstream code;
    
    // 查找selector
    std::string selector;
    for (const auto& kv : pairs) {
        if (kv.key == "selector") {
            selector = kv.value;
            break;
        }
    }
    
    if (selector.empty()) {
        return "// 错误：delegate函数缺少selector";
    }
    
    // 生成事件委托代码
    for (const auto& kv : pairs) {
        if (kv.key != "selector" && event_registry_->isValidEvent(kv.key)) {
            code << "document.addEventListener('" << kv.key << "', function(event) {\n";
            code << "    if (event.target.matches(" << selector << ")) {\n";
            code << "        (" << kv.value << ")(event);\n";
            code << "    }\n";
            code << "});\n";
        }
    }
    
    return code.str();
}

std::string CHTLJSFunctionParser::generateAnimateCode(const std::vector<CHTLJSKeyValueManager::KeyValueInfo>& pairs) {
    std::ostringstream code;
    
    code << "animate({\n";
    
    for (size_t i = 0; i < pairs.size(); ++i) {
        const auto& kv = pairs[i];
        code << "    " << kv.key << ": " << kv.value;
        if (i < pairs.size() - 1) {
            code << ",";
        }
        code << "\n";
    }
    
    code << "})";
    
    return code.str();
}

std::string CHTLJSFunctionParser::extractFunctionName(const std::string& function_call) {
    // 使用正则表达式提取函数名
    std::regex function_pattern(R"((\w+)\s*\()");
    std::smatch match;
    
    if (std::regex_search(function_call, match, function_pattern)) {
        return match[1].str();
    }
    
    return "";
}

std::string CHTLJSFunctionParser::extractKeyValueBlock(const std::string& function_call) {
    // 查找第一个{和最后一个}之间的内容
    size_t start = function_call.find('{');
    size_t end = function_call.rfind('}');
    
    if (start == std::string::npos || end == std::string::npos || start >= end) {
        return "";
    }
    
    return function_call.substr(start + 1, end - start - 1);
}

bool CHTLJSFunctionParser::isValidFunctionName(const std::string& name) {
    static const std::unordered_set<std::string> valid_functions = {
        "listen", "delegate", "animate", "vir"
    };
    
    return valid_functions.find(name) != valid_functions.end();
}

bool CHTLJSFunctionParser::validateEventKey(const std::string& key) {
    return event_registry_->isValidEvent(key);
}

bool CHTLJSFunctionParser::validateFunctionValue(const std::string& value) {
    // 简化的函数值验证
    return !value.empty() && 
           (value.find("=>") != std::string::npos || 
            value.find("function") != std::string::npos ||
            std::isalpha(value[0])); // 函数引用
}

std::string CHTLJSFunctionParser::generateEventBinding(const std::string& event, const std::string& handler) {
    return "addEventListener('" + event + "', " + optimizeFunctionReference(handler) + ")";
}

std::string CHTLJSFunctionParser::optimizeFunctionReference(const std::string& function_ref) {
    // 如果是箭头函数或function关键字，直接返回
    if (function_ref.find("=>") != std::string::npos || 
        function_ref.find("function") != std::string::npos) {
        return function_ref;
    }
    
    // 如果是函数引用，直接返回
    return function_ref;
}

void CHTLJSFunctionParser::addError(const std::string& error) {
    errors_.push_back(error);
    
    if (strict_mode_) {
        std::cerr << "CHTL JS函数解析错误: " << error << std::endl;
    }
}

void CHTLJSFunctionParser::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
    
    if (strict_mode_) {
        std::cout << "CHTL JS函数解析警告: " << warning << std::endl;
    }
}

} // namespace chtljs
} // namespace chtl