#include "iNeverAway.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace cjmod {
namespace chtholly {

// 静态成员初始化
int INeverAwayProcessor::functionCounter_ = 0;

INeverAwayProcessor::INeverAwayProcessor() = default;
INeverAwayProcessor::~INeverAwayProcessor() = default;

// === 按语法文档：iNeverAway处理 ===

INeverAwayObject INeverAwayProcessor::processINeverAwayDefinition(const std::string& virName, const std::string& functionObject) {
    // 按语法文档：vir Test = iNeverAway({ Void<A>: function() {}, Void<B>: function() {} });
    
    INeverAwayObject obj;
    obj.virName = virName;
    
    // 解析函数对象
    obj.functions = parseFunctionObject(functionObject);
    
    // 按语法文档：处理状态重载
    for (const auto& [key, function] : obj.functions) {
        if (isStateOverloadKey(key)) {
            StateOverloadKey stateKey = parseStateOverloadKey(key);
            obj.stateOverloads[stateKey.baseName].push_back(function);
        }
    }
    
    // 注册到全局对象表
    virObjects_[virName] = obj;
    
    return obj;
}

std::string INeverAwayProcessor::processINeverAwayCall(const std::string& virName, const std::string& functionKey) {
    // 按语法文档：Test->Void<A>(); 或 Test->Ax();
    
    auto it = virObjects_.find(virName);
    if (it == virObjects_.end()) {
        return "// iNeverAway错误：虚对象不存在 " + virName;
    }
    
    auto& obj = it->second;
    auto funcIt = obj.functions.find(functionKey);
    if (funcIt == obj.functions.end()) {
        return "// iNeverAway错误：函数键不存在 " + functionKey;
    }
    
    // 按语法文档：在调用时才生成对应的JS函数代码
    if (!funcIt->second.isGenerated) {
        std::string globalFunc = generateGlobalFunction(funcIt->second, virName);
        obj.generatedFunctionNames.push_back(funcIt->second.generatedName);
        const_cast<MarkerFunction&>(funcIt->second).isGenerated = true;
    }
    
    // 按语法文档：vir对象本身不存在，最终转变成相对应的函数的引用
    return funcIt->second.generatedName + "()";
}

std::unordered_map<std::string, MarkerFunction> INeverAwayProcessor::parseFunctionObject(const std::string& functionObject) {
    std::unordered_map<std::string, MarkerFunction> functions;
    
    // 移除外层大括号
    std::string content = functionObject;
    if (content.front() == '{' && content.back() == '}') {
        content = content.substr(1, content.length() - 2);
    }
    
    // 解析键值对
    std::vector<std::pair<std::string, std::string>> keyValuePairs = parseKeyValuePairs(content);
    
    for (const auto& [key, value] : keyValuePairs) {
        MarkerFunction function;
        function.key = key;
        function.functionCode = cleanFunctionCode(value);
        function.generatedName = generateUniqueFunctionName("", key);
        function.isStateOverload = isStateOverloadKey(key);
        
        if (function.isStateOverload) {
            function.stateInfo = parseStateOverloadKey(key);
        }
        
        // 解析参数类型
        function.paramTypes = parseParameterTypes(value);
        
        functions[key] = function;
    }
    
    return functions;
}

StateOverloadKey INeverAwayProcessor::parseStateOverloadKey(const std::string& key) {
    // 按语法文档：从 "Void<A>" 中解析出 baseName="Void", state="A"
    
    StateOverloadKey stateKey;
    stateKey.fullKey = key;
    
    std::regex stateRegex(R"((\w+)<(\w+)>)");
    std::smatch match;
    
    if (std::regex_match(key, match, stateRegex)) {
        stateKey.baseName = match[1].str();
        stateKey.state = match[2].str();
    } else {
        stateKey.baseName = key;
        stateKey.state = "";
    }
    
    return stateKey;
}

std::string INeverAwayProcessor::generateGlobalFunction(const MarkerFunction& function, const std::string& virName) {
    // 按语法文档：创建一组JS全局函数，名称由CHTL编译器统一管理
    
    std::string code = "// 按CHTL语法文档：iNeverAway标记函数 " + function.key + "\n";
    code += "function " + function.generatedName + "() {\n";
    code += "    // 虚对象: " + virName + "\n";
    code += "    // 原始键: " + function.key + "\n";
    
    if (function.isStateOverload) {
        code += "    // 状态重载: " + function.stateInfo.baseName + "<" + function.stateInfo.state + ">\n";
    }
    
    code += "    \n";
    code += "    try {\n";
    code += "        " + function.functionCode + "\n";
    code += "    } catch (error) {\n";
    code += "        console.error('iNeverAway函数执行错误 (" + function.key + "):', error);\n";
    code += "    }\n";
    code += "}\n";
    
    return code;
}

std::string INeverAwayProcessor::generateFunctionCall(const std::string& virName, const std::string& functionKey) {
    // 按语法文档：在调用时才生成对应的JS函数代码
    
    auto it = virObjects_.find(virName);
    if (it == virObjects_.end()) {
        return "null /* 虚对象不存在: " + virName + " */";
    }
    
    auto& obj = it->second;
    auto funcIt = obj.functions.find(functionKey);
    if (funcIt == obj.functions.end()) {
        return "null /* 函数键不存在: " + functionKey + " */";
    }
    
    return funcIt->second.generatedName + "()";
}

std::string INeverAwayProcessor::processStateOverload(const std::string& virName, const StateOverloadKey& stateKey) {
    // 按语法文档：通过状态区分同名的键
    
    auto it = virObjects_.find(virName);
    if (it == virObjects_.end()) {
        return "null /* 虚对象不存在: " + virName + " */";
    }
    
    auto& obj = it->second;
    auto stateIt = obj.stateOverloads.find(stateKey.baseName);
    if (stateIt == obj.stateOverloads.end()) {
        return "null /* 状态重载组不存在: " + stateKey.baseName + " */";
    }
    
    // 查找匹配的状态
    for (const auto& overload : stateIt->second) {
        if (overload.stateInfo.state == stateKey.state) {
            return overload.generatedName + "()";
        }
    }
    
    return "null /* 状态不存在: " + stateKey.fullKey + " */";
}

std::string INeverAwayProcessor::generateAllMarkerFunctions(const INeverAwayObject& obj) {
    // 按语法文档：生成所有标记函数的JavaScript代码
    
    std::string code = "// 按CHTL语法文档：iNeverAway虚对象 " + obj.virName + " 的所有标记函数\n";
    code += "(function() {\n";
    code += "    'use strict';\n";
    code += "    \n";
    code += "    // iNeverAway全局函数注册表\n";
    code += "    if (!window.CHTL_INEVERAWAY_REGISTRY) {\n";
    code += "        window.CHTL_INEVERAWAY_REGISTRY = new Map();\n";
    code += "    }\n";
    code += "    \n";
    
    // 生成所有函数
    for (const auto& [key, function] : obj.functions) {
        code += "    // 标记函数: " + key + "\n";
        code += "    function " + function.generatedName + "() {\n";
        
        if (function.isStateOverload) {
            code += "        // 状态重载: " + function.stateInfo.baseName + "<" + function.stateInfo.state + ">\n";
        }
        
        code += "        try {\n";
        code += "            " + function.functionCode + "\n";
        code += "        } catch (error) {\n";
        code += "            console.error('iNeverAway执行错误:', error);\n";
        code += "        }\n";
        code += "    }\n";
        code += "    \n";
        
        // 注册到全局表
        code += "    window.CHTL_INEVERAWAY_REGISTRY.set('" + obj.virName + "." + key + "', " + function.generatedName + ");\n";
        code += "    \n";
    }
    
    code += "})();\n";
    
    return code;
}

// === 辅助方法实现 ===

bool INeverAwayProcessor::isValidFunctionKey(const std::string& key) {
    if (key.empty()) return false;
    
    // 检查普通键名格式
    std::regex normalKeyRegex(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
    if (std::regex_match(key, normalKeyRegex)) {
        return true;
    }
    
    // 检查状态重载格式
    std::regex stateKeyRegex(R"(^[a-zA-Z_][a-zA-Z0-9_]*<[a-zA-Z0-9_]+>$)");
    return std::regex_match(key, stateKeyRegex);
}

bool INeverAwayProcessor::isStateOverloadKey(const std::string& key) {
    return key.find('<') != std::string::npos && key.find('>') != std::string::npos;
}

std::string INeverAwayProcessor::generateUniqueFunctionName(const std::string& virName, const std::string& functionKey) {
    // 按语法文档：名称由CHTL编译器统一管理
    
    std::string safeName = sanitizeName(virName);
    std::string safeKey = sanitizeName(functionKey);
    
    return "chtl_ineveraway_" + safeName + "_" + safeKey + "_" + std::to_string(++functionCounter_);
}

std::string INeverAwayProcessor::sanitizeName(const std::string& name) {
    std::string sanitized = name;
    
    // 替换特殊字符
    std::replace(sanitized.begin(), sanitized.end(), '<', '_');
    std::replace(sanitized.begin(), sanitized.end(), '>', '_');
    std::replace(sanitized.begin(), sanitized.end(), ' ', '_');
    std::replace(sanitized.begin(), sanitized.end(), '-', '_');
    
    return sanitized;
}

std::string INeverAwayProcessor::cleanFunctionCode(const std::string& code) {
    std::string cleaned = code;
    
    // 移除前后空白
    cleaned.erase(0, cleaned.find_first_not_of(" \t\n"));
    cleaned.erase(cleaned.find_last_not_of(" \t\n") + 1);
    
    return cleaned;
}

std::vector<std::string> INeverAwayProcessor::parseParameterTypes(const std::string& functionSignature) {
    std::vector<std::string> types;
    
    // 简化实现：从函数签名中解析参数类型
    // function(int, int) → ["int", "int"]
    
    std::regex paramRegex(R"(function\s*\(([^)]*)\))");
    std::smatch match;
    
    if (std::regex_search(functionSignature, match, paramRegex)) {
        std::string params = match[1].str();
        
        std::stringstream ss(params);
        std::string param;
        
        while (std::getline(ss, param, ',')) {
            param.erase(0, param.find_first_not_of(" \t"));
            param.erase(param.find_last_not_of(" \t") + 1);
            if (!param.empty()) {
                types.push_back(param);
            }
        }
    }
    
    return types;
}

std::vector<std::pair<std::string, std::string>> INeverAwayProcessor::parseKeyValuePairs(const std::string& content) {
    std::vector<std::pair<std::string, std::string>> pairs;
    
    // 智能分割（考虑嵌套结构）
    std::vector<std::string> entries;
    std::string currentEntry;
    int braceLevel = 0;
    int parenLevel = 0;
    
    for (char c : content) {
        if (c == '{') {
            braceLevel++;
        } else if (c == '}') {
            braceLevel--;
        } else if (c == '(') {
            parenLevel++;
        } else if (c == ')') {
            parenLevel--;
        } else if (c == ',' && braceLevel == 0 && parenLevel == 0) {
            // 顶层逗号，分割条目
            if (!currentEntry.empty()) {
                entries.push_back(currentEntry);
                currentEntry.clear();
            }
            continue;
        }
        
        currentEntry += c;
    }
    
    if (!currentEntry.empty()) {
        entries.push_back(currentEntry);
    }
    
    // 解析每个条目
    for (const auto& entry : entries) {
        size_t colonPos = entry.find(':');
        if (colonPos != std::string::npos) {
            std::string key = entry.substr(0, colonPos);
            std::string value = entry.substr(colonPos + 1);
            
            // 清理空白
            key.erase(0, key.find_first_not_of(" \t\n"));
            key.erase(key.find_last_not_of(" \t\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\n"));
            value.erase(value.find_last_not_of(" \t\n") + 1);
            
            pairs.emplace_back(key, value);
        }
    }
    
    return pairs;
}

// === CHTL JS接口函数实现 ===

extern "C" {
    const char* chtl_iNeverAway_create(const char* virName, const char* functionObjectJson) {
        try {
            INeverAwayProcessor processor;
            
            std::string name = virName ? virName : "";
            std::string funcObj = functionObjectJson ? functionObjectJson : "";
            
            if (name.empty() || funcObj.empty()) {
                std::string error = "iNeverAway错误：缺少必要参数";
                char* errorCopy = new char[error.length() + 1];
                std::strcpy(errorCopy, error.c_str());
                return errorCopy;
            }
            
            INeverAwayObject obj = processor.processINeverAwayDefinition(name, funcObj);
            std::string result = processor.generateAllMarkerFunctions(obj);
            
            // 分配内存返回结果
            char* resultCopy = new char[result.length() + 1];
            std::strcpy(resultCopy, result.c_str());
            
            return resultCopy;
            
        } catch (const std::exception& e) {
            std::string error = "iNeverAway异常: " + std::string(e.what());
            char* errorCopy = new char[error.length() + 1];
            std::strcpy(errorCopy, error.c_str());
            return errorCopy;
        }
    }
    
    const char* chtl_iNeverAway_call(const char* virName, const char* functionKey) {
        try {
            INeverAwayProcessor processor;
            
            std::string name = virName ? virName : "";
            std::string key = functionKey ? functionKey : "";
            
            if (name.empty() || key.empty()) {
                std::string error = "iNeverAway调用错误：缺少必要参数";
                char* errorCopy = new char[error.length() + 1];
                std::strcpy(errorCopy, error.c_str());
                return errorCopy;
            }
            
            std::string result = processor.processINeverAwayCall(name, key);
            
            // 分配内存返回结果
            char* resultCopy = new char[result.length() + 1];
            std::strcpy(resultCopy, result.c_str());
            
            return resultCopy;
            
        } catch (const std::exception& e) {
            std::string error = "iNeverAway调用异常: " + std::string(e.what());
            char* errorCopy = new char[error.length() + 1];
            std::strcpy(errorCopy, error.c_str());
            return errorCopy;
        }
    }
    
    void chtl_iNeverAway_cleanup(const char* result) {
        delete[] result;
    }
}

} // namespace chtholly
} // namespace cjmod
} // namespace chtl