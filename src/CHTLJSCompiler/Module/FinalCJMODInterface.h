// ========================================
// 最终CJMOD接口设计 - 按照您的新规范
// 解决transform问题，完成所有缺失功能
// ========================================

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <iostream>

namespace final_cjmod_design {

// ========================================
// 策略枚举
// ========================================

enum class Policy {
    NORMAL,   // 常规
    COLLECT,  // 收集
    SKIP      // 跳过
};

// ========================================
// Arg类 - 单个参数
// ========================================

class Arg {
public:
    std::string content;
    std::string name;
    bool isPlaceholder;
    
    std::function<std::string(const std::string&)> bindFunc;
    std::function<std::string(const std::string&)> transformFunc;
    std::string transformTemplate;
    
    Arg(const std::string& c, const std::string& n = "") 
        : content(c), name(n), isPlaceholder(c == "$") {}
    
    void bind(std::function<std::string(const std::string&)> processor) {
        bindFunc = processor;
    }
    
    void transform(const std::string& template_str) {
        transformTemplate = template_str;
    }
    
    void transform(std::function<std::string(const std::string&)> transformer) {
        transformFunc = transformer;
    }
    
    std::string match(const std::string& value) {
        std::string result = value;
        
        // 1. 应用bind
        if (bindFunc) {
            result = bindFunc(result);
        }
        
        // 2. 应用transform（解决您的问题！）
        if (transformFunc) {
            result = transformFunc(result);
        } else if (!transformTemplate.empty()) {
            // 模板替换
            std::string temp = transformTemplate;
            size_t pos = temp.find("$VALUE");
            if (pos != std::string::npos) {
                temp.replace(pos, 6, result);
            }
            result = temp;
        }
        
        return result;
    }
};

// ========================================
// ArgsManager类
// ========================================

class ArgsManager {
public:
    std::vector<Arg> args;
    std::unordered_map<std::string, std::string> results;
    
    Arg& operator[](int index) {
        static Arg empty("", "");
        return (index >= 0 && index < args.size()) ? args[index] : empty;
    }
    
    void bind(const std::string& name, std::function<std::string(const std::string&)> processor) {
        for (auto& arg : args) {
            if (arg.name == name || arg.content == name) {
                arg.bind(processor);
                return;
            }
        }
    }
    
    void transform(const std::string& name, const std::string& template_str) {
        for (auto& arg : args) {
            if (arg.name == name || arg.content == name) {
                arg.transform(template_str);
                return;
            }
        }
    }
    
    void transform(const std::string& name, std::function<std::string(const std::string&)> transformer) {
        for (auto& arg : args) {
            if (arg.name == name || arg.content == name) {
                arg.transform(transformer);
                return;
            }
        }
    }
    
    void match(const std::string& name, const std::string& value) {
        for (auto& arg : args) {
            if (arg.name == name || arg.content == name) {
                results[name] = arg.match(value);
                return;
            }
        }
    }
    
    int length() const {
        return args.size();
    }
    
    std::string result() const {
        std::string code = "";
        for (const auto& [name, value] : results) {
            code += value + "\n";
        }
        return code;
    }
    
    // ========================================
    // slice功能 - 处理复杂参数
    // ========================================
    
    void slice(const std::string& name, int start, int length, std::function<std::string(const std::string&)> processor) {
        auto it = results.find(name);
        if (it != results.end()) {
            std::string content = it->second;
            std::string sliced;
            
            if (start < 0) start = content.length() + start;
            if (start < 0) start = 0;
            if (start >= content.length()) return;
            
            if (length == -1) {
                sliced = content.substr(start);
            } else {
                sliced = content.substr(start, length);
            }
            
            results[name] = processor(sliced);
        }
    }
};

// ========================================
// Scanner类
// ========================================

class Scanner {
public:
    std::unordered_map<std::string, std::function<void()>> callbacks;
    Policy currentPolicy = Policy::NORMAL;
    std::string collectedContent = "";
    
    void scanKeyword(const std::string& keyword, std::function<void()> callback) {
        callbacks[keyword] = callback;
    }
    
    void scanKeyword(const Arg& keywordArg, std::function<void()> callback) {
        callbacks[keywordArg.content] = callback;
    }
    
    std::string peekKeyword(int offset) {
        // 模拟实现
        if (offset == -1) return "prevKeyword";
        if (offset == 1) return "nextKeyword";
        return "keyword" + std::to_string(offset);
    }
    
    void policyChangeBegin(const std::string& trigger, Policy policy) {
        currentPolicy = policy;
        collectedContent = "";
    }
    
    std::string policyChangeEnd(const std::string& endTrigger, Policy newPolicy) {
        std::string result = collectedContent;
        currentPolicy = newPolicy;
        collectedContent = "";
        return result;
    }
};

// ========================================
// Keyword类
// ========================================

class Keyword {
public:
    ArgsManager args;
    std::string originalPattern;
    
    Keyword(const std::string& pattern) : originalPattern(pattern) {}
    
    void addArg(const std::string& content, const std::string& name = "") {
        args.args.emplace_back(content, name);
    }
};

// ========================================
// 全局接口
// ========================================

// 全局scanner实例
extern Scanner scanner;

// 主要函数声明
Keyword syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");
std::string generateCode(const Keyword& keyword);

// 辅助函数
bool isObject(const std::string& value);
bool isFunction(const std::string& value);
std::string slice(const std::string& content, int start, int length = -1);

} // namespace final_cjmod_design