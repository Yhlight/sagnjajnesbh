#include "CJMODInterface.h"
#include <regex>
#include <random>
#include <algorithm>

namespace chtl {
namespace cjmod {

// ========================================
// CJMODHelper实现 - 开发者辅助工具
// ========================================

std::string CJMODHelper::parseSelector(const std::string& selector) {
    if (selector.empty()) {
        return "null";
    }
    
    // {{#id}} → document.getElementById('id')
    if (selector[0] == '#') {
        std::string id = selector.substr(1);
        return "document.getElementById('" + id + "')";
    }
    
    // {{.class}} → document.querySelector('.class')
    if (selector[0] == '.') {
        std::string className = selector.substr(1);
        return "document.querySelector('." + className + "')";
    }
    
    // {{element[0]}} → document.querySelectorAll('element')[0]
    std::regex indexRegex(R"(^(.+)\[(\d+)\]$)");
    std::smatch indexMatch;
    if (std::regex_match(selector, indexMatch, indexRegex)) {
        std::string element = indexMatch[1].str();
        std::string index = indexMatch[2].str();
        return "document.querySelectorAll('" + element + "')[" + index + "]";
    }
    
    // {{element}} → document.querySelector('element')
    return "document.querySelector('" + selector + "')";
}

std::vector<std::string> CJMODHelper::parseFunctionParameters(const std::string& functionCode) {
    std::vector<std::string> params;
    
    // 匹配函数参数：function(a, b, c)
    std::regex paramRegex(R"(function\s*\(\s*([^)]*)\s*\))");
    std::smatch matches;
    
    if (std::regex_search(functionCode, matches, paramRegex)) {
        std::string paramStr = matches[1].str();
        
        if (!paramStr.empty()) {
            std::stringstream ss(paramStr);
            std::string param;
            
            while (std::getline(ss, param, ',')) {
                // 去除前后空白
                param.erase(0, param.find_first_not_of(" \t"));
                param.erase(param.find_last_not_of(" \t") + 1);
                
                if (!param.empty()) {
                    params.push_back(param);
                }
            }
        }
    }
    
    return params;
}

std::string CJMODHelper::parseFunctionBody(const std::string& functionCode) {
    // 匹配函数体：function(...) { body }
    std::regex bodyRegex(R"(function\s*\([^)]*\)\s*\{\s*(.*)\s*\})");
    std::smatch matches;
    
    if (std::regex_search(functionCode, matches, bodyRegex)) {
        return matches[1].str();
    }
    
    return "";
}

std::string CJMODHelper::generateSafeIdentifier(const std::string& name) {
    std::string safe = name;
    
    // 替换非法字符
    std::regex illegalRegex(R"([^a-zA-Z0-9_])");
    safe = std::regex_replace(safe, illegalRegex, "_");
    
    // 确保以字母或下划线开头
    if (!safe.empty() && std::isdigit(safe[0])) {
        safe = "_" + safe;
    }
    
    return safe;
}

std::string CJMODHelper::escapeJavaScriptString(const std::string& str) {
    std::string escaped = str;
    
    // 转义特殊字符
    std::vector<std::pair<std::string, std::string>> escapes = {
        {"\\", "\\\\"},
        {"\"", "\\\""},
        {"'", "\\'"},
        {"\n", "\\n"},
        {"\r", "\\r"},
        {"\t", "\\t"}
    };
    
    for (const auto& escape : escapes) {
        size_t pos = 0;
        while ((pos = escaped.find(escape.first, pos)) != std::string::npos) {
            escaped.replace(pos, escape.first.length(), escape.second);
            pos += escape.second.length();
        }
    }
    
    return escaped;
}

bool CJMODHelper::isValidJavaScript(const std::string& jsCode) {
    // 简单的JavaScript语法检查
    if (jsCode.empty()) {
        return false;
    }
    
    // 检查括号匹配
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    
    for (char c : jsCode) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
    }
    
    return braceCount == 0 && parenCount == 0 && bracketCount == 0;
}

std::string CJMODHelper::generateUniqueId(const std::string& prefix) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    
    return prefix + "_" + std::to_string(dis(gen)) + "_" + std::to_string(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count() % 10000);
}

std::unordered_map<std::string, std::string> CJMODHelper::parseJsonParameters(const std::string& jsonStr) {
    std::unordered_map<std::string, std::string> params;
    
    // 简化的JSON解析（仅支持基本的key: value格式）
    std::regex jsonRegex(R"((\w+):\s*['"](.*?)['"])");
    std::sregex_iterator iter(jsonStr.begin(), jsonStr.end(), jsonRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        params[key] = value;
    }
    
    return params;
}

std::string CJMODHelper::formatJavaScript(const std::string& jsCode, int indentLevel) {
    std::string formatted = jsCode;
    std::string indent(indentLevel * 4, ' ');
    
    // 简单的格式化：在每行前添加缩进
    std::regex lineRegex(R"(\n)");
    formatted = std::regex_replace(formatted, lineRegex, "\n" + indent);
    
    return indent + formatted;
}

} // namespace cjmod
} // namespace chtl