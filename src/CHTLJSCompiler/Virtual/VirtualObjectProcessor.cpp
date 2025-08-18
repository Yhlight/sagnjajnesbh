#include "VirtualObjectProcessor.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <sstream>

namespace chtl {
namespace chtljs {

// 静态成员初始化
int VirtualObjectProcessor::functionCounter_ = 0;

VirtualObjectProcessor::VirtualObjectProcessor() {
    selectorProcessor_ = std::make_shared<EnhancedSelectorProcessor>();
}

VirtualObjectProcessor::~VirtualObjectProcessor() = default;

// === 按语法文档：vir虚对象处理 ===

std::string VirtualObjectProcessor::processVirtualObjectDefinition(const std::string& virCode) {
    // 按语法文档：vir test = listen({ click: () => {} }); 或 vir Test = iNeverAway({ ... });
    
    if (virCode.empty()) {
        addError("空的vir语句");
        return "";
    }
    
    // 解析vir定义
    VirtualObject virtualObj = parseVirtualObjectDefinition(virCode);
    
    if (virtualObj.name.empty()) {
        addError("vir语句缺少虚对象名称");
        return "";
    }
    
    if (virtualObj.sourceFunction.empty()) {
        addError("vir语句缺少源函数");
        return "";
    }
    
    // 注册虚对象
    virtualObjectRegistry_[virtualObj.name] = virtualObj;
    
    // 生成JavaScript实现
    std::string jsCode = generateVirtualObjectImplementation(virtualObj);
    
    return jsCode;
}

std::string VirtualObjectProcessor::processVirtualObjectCall(const std::string& callCode) {
    // 按语法文档：test->click(); 或 Test->Void<A>();
    
    auto [virName, functionKey] = parseVirtualCall(callCode);
    
    if (virName.empty() || functionKey.empty()) {
        addError("无效的虚对象调用: " + callCode);
        return "";
    }
    
    // 检查虚对象是否存在
    auto it = virtualObjectRegistry_.find(virName);
    if (it == virtualObjectRegistry_.end()) {
        addError("虚对象不存在: " + virName);
        return "";
    }
    
    // 检查是否为状态重载调用
    if (functionKey.find('<') != std::string::npos) {
        return processStateOverloadCall(virName, functionKey);
    } else {
        return convertToFunctionReference(virName, functionKey);
    }
}

VirtualObjectProcessor::VirtualObject VirtualObjectProcessor::parseVirtualObjectDefinition(const std::string& virCode) {
    VirtualObject virtualObj;
    
    // 提取vir名称
    virtualObj.name = extractVirName(virCode);
    
    // 提取源函数名称
    virtualObj.sourceFunction = extractSourceFunction(virCode);
    
    // 提取函数对象
    std::string functionObject = extractFunctionObject(virCode);
    
    if (virtualObj.sourceFunction == "listen") {
        virtualObj = parseListenVirtualObject(virtualObj.name, functionObject);
    } else if (virtualObj.sourceFunction == "iNeverAway") {
        virtualObj = parseINeverAwayVirtualObject(virtualObj.name, functionObject);
    } else {
        addError("不支持的vir源函数: " + virtualObj.sourceFunction);
    }
    
    return virtualObj;
}

std::string VirtualObjectProcessor::generateVirtualObjectImplementation(const VirtualObject& virtualObj) {
    // 按语法文档：创建特殊命名的全局函数，然后解析成函数引用
    
    std::string code = "// 按CHTL语法文档：虚对象 " + virtualObj.name + " 的实现\n";
    code += "(function() {\n";
    code += "    'use strict';\n";
    code += "    \n";
    
    // 生成函数引用表
    code += "    // 虚对象函数引用表\n";
    code += "    if (!window.CHTL_VIR_REGISTRY) {\n";
    code += "        window.CHTL_VIR_REGISTRY = new Map();\n";
    code += "    }\n";
    code += "    \n";
    
    // 为每个函数键生成全局函数
    for (const auto& [key, funcRef] : virtualObj.functionMap) {
        if (!funcRef.isGenerated) {
            code += "    // 函数键: " + key + "\n";
            code += "    function " + funcRef.globalFunctionName + "() {\n";
            code += "        " + funcRef.functionCode + "\n";
            code += "    }\n";
            code += "    \n";
            
            // 注册到全局函数表
            code += "    window.CHTL_VIR_REGISTRY.set('" + virtualObj.name + "." + key + "', " + funcRef.globalFunctionName + ");\n";
            code += "    \n";
        }
    }
    
    if (virtualObj.sourceFunction == "listen") {
        // 按语法文档：listen会按原样生成JS代码
        code += "    // 原始listen函数调用\n";
        code += "    // 注意：vir本身不存在，是编译期间的语法糖\n";
    }
    
    code += "})();\n";
    
    return code;
}

// === listen虚对象处理 ===

VirtualObjectProcessor::VirtualObject VirtualObjectProcessor::parseListenVirtualObject(const std::string& name, const std::string& listenObject) {
    // 按语法文档：vir test = listen({ click: () => {} });
    
    VirtualObject virtualObj;
    virtualObj.name = name;
    virtualObj.sourceFunction = "listen";
    virtualObj.functionMap = extractListenFunctions(listenObject);
    
    return virtualObj;
}

std::unordered_map<std::string, VirtualObjectProcessor::FunctionReference> VirtualObjectProcessor::extractListenFunctions(const std::string& listenObject) {
    // 按语法文档：解析listen函数中的函数键，并创建对应表
    
    std::unordered_map<std::string, FunctionReference> functionMap;
    
    // 解析键值对
    auto keyValuePairs = parseConfigKeyValuePairs(listenObject);
    
    for (const auto& [key, value] : keyValuePairs) {
        FunctionReference funcRef;
        funcRef.originalKey = key;
        funcRef.functionCode = value;
        funcRef.globalFunctionName = generateGlobalFunctionName("", key);
        funcRef.isGenerated = false;
        
        functionMap[key] = funcRef;
    }
    
    return functionMap;
}

// === iNeverAway虚对象处理 ===

VirtualObjectProcessor::VirtualObject VirtualObjectProcessor::parseINeverAwayVirtualObject(const std::string& name, const std::string& iNeverAwayObject) {
    // 按语法文档：vir Test = iNeverAway({ Void<A>: function() {}, Void<B>: function() {} });
    
    VirtualObject virtualObj;
    virtualObj.name = name;
    virtualObj.sourceFunction = "iNeverAway";
    
    // 解析状态重载
    std::vector<StateOverload> overloads = parseStateOverloads(iNeverAwayObject);
    
    // 转换为函数映射
    for (const auto& overload : overloads) {
        FunctionReference funcRef;
        funcRef.originalKey = overload.fullName;
        funcRef.functionCode = overload.functionCode;
        funcRef.globalFunctionName = generateGlobalFunctionName(name, overload.fullName);
        funcRef.isGenerated = false;
        
        virtualObj.functionMap[overload.fullName] = funcRef;
    }
    
    return virtualObj;
}

std::vector<VirtualObjectProcessor::StateOverload> VirtualObjectProcessor::parseStateOverloads(const std::string& iNeverAwayObject) {
    // 按语法文档：解析状态重载函数
    
    std::vector<StateOverload> overloads;
    
    // 解析键值对
    auto keyValuePairs = parseConfigKeyValuePairs(iNeverAwayObject);
    
    for (const auto& [key, value] : keyValuePairs) {
        StateOverload overload;
        
        // 检查是否为状态重载格式：Void<A>
        if (key.find('<') != std::string::npos && key.find('>') != std::string::npos) {
            auto [baseName, state] = parseStateIdentifier(key);
            overload.baseName = baseName;
            overload.state = state;
            overload.fullName = key;
        } else {
            // 普通函数键
            overload.baseName = key;
            overload.state = "";
            overload.fullName = key;
        }
        
        overload.functionCode = value;
        overloads.push_back(overload);
    }
    
    return overloads;
}

// === 全局函数管理 ===

std::string VirtualObjectProcessor::generateGlobalFunctionName(const std::string& virName, const std::string& functionKey) {
    // 按语法文档：名称由CHTL编译器统一管理
    
    std::string safeName = virName;
    std::string safeKey = functionKey;
    
    // 清理特殊字符
    std::replace(safeName.begin(), safeName.end(), '<', '_');
    std::replace(safeName.begin(), safeName.end(), '>', '_');
    std::replace(safeKey.begin(), safeKey.end(), '<', '_');
    std::replace(safeKey.begin(), safeKey.end(), '>', '_');
    
    return "chtl_vir_" + safeName + "_" + safeKey + "_" + std::to_string(++functionCounter_);
}

void VirtualObjectProcessor::registerGlobalFunction(const std::string& functionName, const std::string& functionCode) {
    // 按语法文档：加入注册表防止重复创建
    
    if (registeredFunctionNames_.find(functionName) == registeredFunctionNames_.end()) {
        globalFunctionRegistry_[functionName] = functionCode;
        registeredFunctionNames_.insert(functionName);
    }
}

std::string VirtualObjectProcessor::generateAllGlobalFunctions() {
    std::string code = "// 按CHTL语法文档：所有虚对象的全局函数\n";
    code += "(function() {\n";
    code += "    'use strict';\n";
    code += "    \n";
    
    for (const auto& [functionName, functionCode] : globalFunctionRegistry_) {
        code += "    // 全局函数: " + functionName + "\n";
        code += "    window." + functionName + " = " + functionCode + ";\n";
        code += "    \n";
    }
    
    code += "})();\n";
    
    return code;
}

// === 虚对象调用处理 ===

std::pair<std::string, std::string> VirtualObjectProcessor::parseVirtualCall(const std::string& callCode) {
    // 解析 test->click() 或 Test->Void<A>()
    
    std::regex callRegex(R"((\w+)->([^(]+)\(\))");
    std::smatch match;
    
    if (std::regex_match(callCode, match, callRegex)) {
        std::string virName = match[1].str();
        std::string functionKey = match[2].str();
        return {virName, functionKey};
    }
    
    return {"", ""};
}

std::string VirtualObjectProcessor::convertToFunctionReference(const std::string& virName, const std::string& functionKey) {
    // 按语法文档：将虚对象调用替换成函数引用(函数的名称)
    
    auto it = virtualObjectRegistry_.find(virName);
    if (it == virtualObjectRegistry_.end()) {
        addError("虚对象不存在: " + virName);
        return "";
    }
    
    auto funcIt = it->second.functionMap.find(functionKey);
    if (funcIt == it->second.functionMap.end()) {
        addError("虚对象函数键不存在: " + virName + "->" + functionKey);
        return "";
    }
    
    // 按语法文档：在调用时才生成对应的JS函数代码
    if (!funcIt->second.isGenerated) {
        registerGlobalFunction(funcIt->second.globalFunctionName, funcIt->second.functionCode);
        const_cast<FunctionReference&>(funcIt->second).isGenerated = true;
    }
    
    // 返回函数引用
    return funcIt->second.globalFunctionName + "()";
}

std::string VirtualObjectProcessor::processStateOverloadCall(const std::string& virName, const std::string& stateFunction) {
    // 按语法文档：Test->Void<A>() 转换为对应的全局函数调用
    
    return convertToFunctionReference(virName, stateFunction);
}

// === 辅助方法 ===

std::string VirtualObjectProcessor::extractVirName(const std::string& virCode) {
    // 从 vir test = ... 中提取 test
    std::regex nameRegex(R"(vir\s+(\w+)\s*=)");
    std::smatch match;
    
    if (std::regex_search(virCode, match, nameRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string VirtualObjectProcessor::extractSourceFunction(const std::string& virCode) {
    // 从 vir test = listen(...) 中提取 listen
    std::regex funcRegex(R"(=\s*(\w+)\s*\()");
    std::smatch match;
    
    if (std::regex_search(virCode, match, funcRegex)) {
        return match[1].str();
    }
    
    return "";
}

std::string VirtualObjectProcessor::extractFunctionObject(const std::string& virCode) {
    // 从 listen({ ... }) 或 iNeverAway({ ... }) 中提取 { ... }
    
    size_t startPos = virCode.find('{');
    if (startPos == std::string::npos) {
        return "";
    }
    
    // 找到匹配的右大括号
    int braceCount = 1;
    size_t endPos = startPos + 1;
    
    while (endPos < virCode.length() && braceCount > 0) {
        if (virCode[endPos] == '{') {
            braceCount++;
        } else if (virCode[endPos] == '}') {
            braceCount--;
        }
        endPos++;
    }
    
    if (braceCount == 0) {
        return virCode.substr(startPos, endPos - startPos);
    }
    
    return "";
}

std::pair<std::string, std::string> VirtualObjectProcessor::parseStateIdentifier(const std::string& stateFunction) {
    // 从 Void<A> 中提取 "Void" 和 "A"
    
    std::regex stateRegex(R"((\w+)<(\w+)>)");
    std::smatch match;
    
    if (std::regex_match(stateFunction, match, stateRegex)) {
        return {match[1].str(), match[2].str()};
    }
    
    return {stateFunction, ""};
}

std::vector<std::pair<std::string, std::string>> VirtualObjectProcessor::parseConfigKeyValuePairs(const std::string& configObject) {
    std::vector<std::pair<std::string, std::string>> pairs;
    
    // 移除外层大括号
    std::string content = configObject;
    if (content.front() == '{' && content.back() == '}') {
        content = content.substr(1, content.length() - 2);
    }
    
    // 智能分割（考虑嵌套结构）
    std::vector<std::string> entries = splitConfigEntries(content);
    
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

std::vector<std::string> VirtualObjectProcessor::splitConfigEntries(const std::string& content) {
    std::vector<std::string> entries;
    std::string currentEntry;
    int braceLevel = 0;
    int parenLevel = 0;
    int bracketLevel = 0;
    
    for (char c : content) {
        if (c == '{') {
            braceLevel++;
        } else if (c == '}') {
            braceLevel--;
        } else if (c == '(') {
            parenLevel++;
        } else if (c == ')') {
            parenLevel--;
        } else if (c == '[') {
            bracketLevel++;
        } else if (c == ']') {
            bracketLevel--;
        } else if (c == ',' && braceLevel == 0 && parenLevel == 0 && bracketLevel == 0) {
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
    
    return entries;
}

bool VirtualObjectProcessor::isValidVirName(const std::string& name) {
    // 验证虚对象名称格式
    if (name.empty()) return false;
    
    // 必须以字母或下划线开头
    if (!std::isalpha(name[0]) && name[0] != '_') {
        return false;
    }
    
    // 只能包含字母、数字、下划线
    for (char c : name) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool VirtualObjectProcessor::isValidFunctionKey(const std::string& key) {
    // 验证函数键名称格式
    return isValidVirName(key) || 
           (key.find('<') != std::string::npos && key.find('>') != std::string::npos); // 支持状态重载格式
}

bool VirtualObjectProcessor::isFunctionRegistered(const std::string& functionName) {
    return registeredFunctionNames_.find(functionName) != registeredFunctionNames_.end();
}

// === 错误处理 ===

void VirtualObjectProcessor::addError(const std::string& message) {
    errors_.push_back("VirtualObject: " + message);
}

void VirtualObjectProcessor::clearErrors() {
    errors_.clear();
}

bool VirtualObjectProcessor::hasErrors() const {
    return !errors_.empty();
}

std::vector<std::string> VirtualObjectProcessor::getErrors() const {
    return errors_;
}

} // namespace chtljs
} // namespace chtl