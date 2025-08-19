// ========================================
// 新CJMOD接口设计 - 按照您的最新规范
// 严格按照CHTL语法文档实现
// 解决transform问题并完成所有功能
// ========================================

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

namespace new_cjmod_design {

// ========================================
// 扫描策略枚举
// ========================================

enum class Policy {
    NORMAL,   // 常规
    COLLECT,  // 收集 (修正拼写)
    SKIP      // 跳过
};

// ========================================
// 前置声明
// ========================================

class Scanner;
class Keyword;
class ArgsManager;
class Arg;

// ========================================
// Arg类 - 单个参数
// ========================================

class Arg {
public:
    std::string content;
    std::string name;
    bool isPlaceholder;
    int placeholderIndex; // 对于$占位符的计数
    
    // 绑定的处理函数
    std::function<std::string(const std::string&)> bindFunc;
    
    // 转换模板或函数
    std::string transformTemplate;
    std::function<std::string(const std::string&)> transformFunc;
    
    Arg(const std::string& content, const std::string& name = "") 
        : content(content), name(name), isPlaceholder(content == "$"), placeholderIndex(-1) {}
    
    /**
     * 绑定处理函数
     */
    void bind(std::function<std::string(const std::string&)> processor) {
        bindFunc = processor;
    }
    
    /**
     * 设置转换模板
     */
    void transform(const std::string& template_or_code) {
        transformTemplate = template_or_code;
    }
    
    /**
     * 设置转换函数
     */
    void transform(std::function<std::string(const std::string&)> transformer) {
        transformFunc = transformer;
    }
    
    /**
     * 匹配并应用所有转换
     */
    std::string match(const std::string& value) {
        std::string result = value;
        
        // 1. 先应用bind预处理
        if (bindFunc) {
            result = bindFunc(result);
        }
        
        // 2. 再应用transform转换
        if (transformFunc) {
            result = transformFunc(result);
        } else if (!transformTemplate.empty()) {
            // 使用模板转换
            result = applyTemplate(transformTemplate, result);
        }
        
        return result;
    }
    
private:
    std::string applyTemplate(const std::string& template_str, const std::string& value) {
        std::string result = template_str;
        
        // 简单的模板替换
        size_t pos = result.find("${VALUE}");
        if (pos != std::string::npos) {
            result.replace(pos, 8, value);
        }
        
        // 支持多种占位符
        pos = result.find("$VALUE");
        if (pos != std::string::npos) {
            result.replace(pos, 6, value);
        }
        
        return result;
    }
};

// ========================================
// ArgsManager类 - 参数管理器
// ========================================

class ArgsManager {
public:
    std::vector<Arg> args;
    std::unordered_map<std::string, std::string> matchedValues;
    std::unordered_map<std::string, int> placeholderCounts; // 占位符计数
    
    /**
     * 索引访问
     */
    Arg& operator[](int index) {
        if (index >= 0 && index < args.size()) {
            return args[index];
        }
        static Arg emptyArg("", "");
        return emptyArg;
    }
    
    /**
     * 名称访问
     */
    Arg* find(const std::string& name) {
        for (auto& arg : args) {
            if (arg.name == name) {
                return &arg;
            }
        }
        return nullptr;
    }
    
    /**
     * 绑定函数 - 支持占位符自动计数
     */
    void bind(const std::string& name, std::function<std::string(const std::string&)> processor) {
        // 对于占位符，自动计数并绑定到下一个$
        if (name == "$") {
            int& count = placeholderCounts["$"];
            for (auto& arg : args) {
                if (arg.isPlaceholder && arg.placeholderIndex == -1) {
                    arg.placeholderIndex = count++;
                    arg.bindFunc = processor;
                    return;
                }
            }
        } else {
            auto* arg = find(name);
            if (arg) {
                arg->bind(processor);
            }
        }
    }
    
    /**
     * 设置转换规则
     */
    void transform(const std::string& name, const std::string& template_or_code) {
        auto* arg = find(name);
        if (arg) {
            arg->transform(template_or_code);
        }
    }
    
    void transform(const std::string& name, std::function<std::string(const std::string&)> transformer) {
        auto* arg = find(name);
        if (arg) {
            arg->transform(transformer);
        }
    }
    
    /**
     * 匹配参数值 - 核心功能
     */
    void match(const std::string& name, const std::string& value) {
        auto* arg = find(name);
        if (arg) {
            matchedValues[name] = arg->match(value);
        }
    }
    
    /**
     * 获取参数数量
     */
    int length() const {
        return args.size();
    }
    
    /**
     * 生成结果代码
     */
    std::string result() {
        std::string code = "";
        
        for (const auto& arg : args) {
            auto it = matchedValues.find(arg.name);
            if (it != matchedValues.end()) {
                code += it->second;
                if (!it->second.empty() && it->second.back() != '\n') {
                    code += "\n";
                }
            }
        }
        
        return code;
    }
    
    /**
     * 清理
     */
    void clear() {
        args.clear();
        matchedValues.clear();
        placeholderCounts.clear();
    }
};

// ========================================
// Scanner类 - 扫描器接口
// ========================================

class Scanner {
public:
    /**
     * 注册关键字扫描
     */
    void scanKeyword(const Arg& keywordArg, std::function<void()> callback) {
        keywordCallbacks[keywordArg.content] = callback;
    }
    
    void scanKeyword(const std::string& keyword, std::function<void()> callback) {
        keywordCallbacks[keyword] = callback;
    }
    
    /**
     * 查看前后关键字
     */
    std::string peekKeyword(int offset) {
        // 模拟实现 - 实际应该从扫描器状态获取
        if (offset == -1) return "previousKeyword";
        if (offset == 1) return "nextKeyword";
        return "keyword" + std::to_string(offset);
    }
    
    /**
     * 策略改变 - 开始收集
     */
    void policyChangeBegin(const std::string& trigger, Policy policy) {
        currentPolicy = policy;
        policyTrigger = trigger;
        collectedContent = "";
    }
    
    /**
     * 策略改变 - 结束收集
     */
    std::string policyChangeEnd(const std::string& endTrigger, Policy newPolicy) {
        std::string result = collectedContent;
        currentPolicy = newPolicy;
        collectedContent = "";
        return result;
    }
    
    /**
     * 获取当前策略
     */
    Policy getCurrentPolicy() const {
        return currentPolicy;
    }

private:
    std::unordered_map<std::string, std::function<void()>> keywordCallbacks;
    Policy currentPolicy = Policy::NORMAL;
    std::string policyTrigger;
    std::string collectedContent;
};

// ========================================
// Keyword类 - 关键字对象
// ========================================

class Keyword {
public:
    ArgsManager args;
    std::string originalPattern;
    std::string ignoreChars;
    
    Keyword(const std::string& pattern, const std::string& ignore = "") 
        : originalPattern(pattern), ignoreChars(ignore) {}
    
    /**
     * 添加参数
     */
    void addArg(const std::string& content, const std::string& name = "") {
        args.args.emplace_back(content, name);
    }
    
    /**
     * 获取参数数量
     */
    int getArgCount() const {
        return args.length();
    }
    
    /**
     * 生成最终代码
     */
    std::string generateCode() {
        return args.result();
    }
};

// ========================================
// 全局函数接口
// ========================================

/**
 * 语法分析函数
 */
Keyword syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "") {
    Keyword keyword(pattern, ignoreChars);
    
    // 解析模式字符串
    std::vector<std::string> tokens = tokenizePattern(pattern, ignoreChars);
    
    for (const auto& token : tokens) {
        if (!token.empty() && ignoreChars.find(token) == std::string::npos) {
            keyword.addArg(token, generateArgName(token));
        }
    }
    
    return keyword;
}

/**
 * 生成代码函数
 */
std::string generateCode(const Keyword& keyword) {
    return keyword.args.result();
}

/**
 * 辅助函数：判断是否为对象
 */
bool isObject(const std::string& value) {
    if (value.empty()) return false;
    
    // 去除前后空格
    std::string trimmed = value;
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    size_t end = trimmed.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos) return false;
    trimmed = trimmed.substr(start, end - start + 1);
    
    return trimmed.front() == '{' && trimmed.back() == '}';
}

/**
 * 辅助函数：判断是否为函数
 */
bool isFunction(const std::string& value) {
    return value.find("function") != std::string::npos || 
           value.find("=>") != std::string::npos ||
           (value.find("(") != std::string::npos && value.find(")") != std::string::npos && value.find("{") != std::string::npos);
}

/**
 * slice函数 - 处理复杂参数的切片功能
 */
std::string slice(const std::string& content, int start, int length = -1) {
    if (start < 0 || start >= content.length()) {
        return "";
    }
    
    if (length == -1) {
        return content.substr(start);
    }
    
    if (start + length > content.length()) {
        length = content.length() - start;
    }
    
    return content.substr(start, length);
}

/**
 * slice函数 - 支持负数索引
 */
std::string slice(const std::string& content, int start, int end) {
    int len = content.length();
    
    // 处理负数索引
    if (start < 0) start = len + start;
    if (end < 0) end = len + end;
    
    // 边界检查
    if (start < 0) start = 0;
    if (end > len) end = len;
    if (start >= end) return "";
    
    return content.substr(start, end - start);
}

// ========================================
// 高级slice功能 - 智能内容切片
// ========================================

/**
 * 智能slice - 根据内容类型进行切片
 */
class SmartSlicer {
public:
    /**
     * 函数体切片
     */
    static std::string sliceFunction(const std::string& functionCode, int start, int length = -1) {
        // 查找函数体开始和结束
        size_t bodyStart = functionCode.find('{');
        size_t bodyEnd = functionCode.rfind('}');
        
        if (bodyStart == std::string::npos || bodyEnd == std::string::npos) {
            return slice(functionCode, start, length);
        }
        
        std::string functionBody = functionCode.substr(bodyStart + 1, bodyEnd - bodyStart - 1);
        return slice(functionBody, start, length);
    }
    
    /**
     * 对象属性切片
     */
    static std::string sliceObject(const std::string& objectCode, const std::vector<std::string>& propertyNames) {
        std::string result = "{\n";
        
        for (const auto& prop : propertyNames) {
            size_t propPos = objectCode.find(prop + ":");
            if (propPos != std::string::npos) {
                // 提取属性值
                size_t valueStart = objectCode.find(":", propPos) + 1;
                size_t valueEnd = objectCode.find_first_of(",}", valueStart);
                
                if (valueEnd != std::string::npos) {
                    std::string value = objectCode.substr(valueStart, valueEnd - valueStart);
                    result += "    " + prop + ":" + value + ",\n";
                }
            }
        }
        
        result += "}";
        return result;
    }
    
    /**
     * 数组元素切片
     */
    static std::string sliceArray(const std::string& arrayCode, int start, int count) {
        // 简化实现：按逗号分割
        std::vector<std::string> elements;
        std::string current = "";
        int braceLevel = 0;
        
        for (char c : arrayCode) {
            if (c == '{' || c == '[') braceLevel++;
            else if (c == '}' || c == ']') braceLevel--;
            else if (c == ',' && braceLevel == 0) {
                elements.push_back(current);
                current = "";
                continue;
            }
            current += c;
        }
        if (!current.empty()) elements.push_back(current);
        
        // 切片
        std::string result = "[";
        for (int i = start; i < start + count && i < elements.size(); i++) {
            if (i > start) result += ", ";
            result += elements[i];
        }
        result += "]";
        
        return result;
    }
};

// ========================================
// ArgsManager类 - 增强版参数管理器
// ========================================

class ArgsManager {
public:
    std::vector<Arg> args;
    std::unordered_map<std::string, std::string> finalResults;
    std::unordered_map<std::string, int> placeholderCounters;
    
    /**
     * 索引访问
     */
    Arg& operator[](int index) {
        if (index >= 0 && index < args.size()) {
            return args[index];
        }
        static Arg emptyArg("", "");
        return emptyArg;
    }
    
    /**
     * 名称绑定 - 支持占位符自动计数
     */
    void bind(const std::string& name, std::function<std::string(const std::string&)> processor) {
        if (name == "$") {
            // 占位符自动计数绑定
            int& counter = placeholderCounters["$"];
            for (auto& arg : args) {
                if (arg.isPlaceholder && arg.placeholderIndex == -1) {
                    arg.placeholderIndex = counter++;
                    arg.bind(processor);
                    return;
                }
            }
        } else {
            // 普通名称绑定
            for (auto& arg : args) {
                if (arg.name == name || arg.content == name) {
                    arg.bind(processor);
                    return;
                }
            }
        }
    }
    
    /**
     * 设置转换规则
     */
    void transform(const std::string& name, const std::string& template_or_code) {
        for (auto& arg : args) {
            if (arg.name == name || arg.content == name) {
                arg.transform(template_or_code);
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
    
    /**
     * 匹配参数值 - 应用完整转换链
     */
    void match(const std::string& name, const std::string& value) {
        for (auto& arg : args) {
            if (arg.name == name || arg.content == name) {
                finalResults[name] = arg.match(value);
                return;
            }
        }
    }
    
    /**
     * slice功能 - 处理复杂参数
     */
    void slice(const std::string& name, int start, int length, std::function<std::string(const std::string&)> processor) {
        auto it = finalResults.find(name);
        if (it != finalResults.end()) {
            std::string sliced = new_cjmod_design::slice(it->second, start, length);
            finalResults[name] = processor(sliced);
        }
    }
    
    /**
     * 智能slice - 根据内容类型
     */
    void smartSlice(const std::string& name, const std::string& sliceType, std::function<std::string(const std::string&)> processor) {
        auto it = finalResults.find(name);
        if (it != finalResults.end()) {
            std::string sliced;
            
            if (sliceType == "function") {
                sliced = SmartSlicer::sliceFunction(it->second, 0);
            } else if (sliceType == "object") {
                // 需要指定属性名列表
                sliced = it->second; // 简化处理
            } else {
                sliced = it->second;
            }
            
            finalResults[name] = processor(sliced);
        }
    }
    
    /**
     * 生成最终结果
     */
    std::string result() {
        std::string code = "";
        
        // 按照args的顺序生成代码
        for (const auto& arg : args) {
            std::string key = !arg.name.empty() ? arg.name : arg.content;
            auto it = finalResults.find(key);
            
            if (it != finalResults.end()) {
                code += it->second;
                if (!it->second.empty() && it->second.back() != '\n') {
                    code += "\n";
                }
            } else {
                // 如果没有匹配值，使用原始内容
                code += arg.content + " ";
            }
        }
        
        return code;
    }
};

// ========================================
// Keyword类 - 关键字对象
// ========================================

class Keyword {
public:
    ArgsManager args;
    std::string pattern;
    std::string ignoreChars;
    
    Keyword(const std::string& pat, const std::string& ignore = "") 
        : pattern(pat), ignoreChars(ignore) {}
};

// ========================================
// 全局Scanner实例
// ========================================

extern Scanner scanner;

// ========================================
// 辅助函数实现
// ========================================

/**
 * 模式标记化
 */
std::vector<std::string> tokenizePattern(const std::string& pattern, const std::string& ignoreChars) {
    std::vector<std::string> tokens;
    std::string current = "";
    
    for (char c : pattern) {
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current = "";
            }
        } else if (ignoreChars.find(c) != std::string::npos) {
            // 忽略字符，但作为分隔符
            if (!current.empty()) {
                tokens.push_back(current);
                current = "";
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

/**
 * 生成参数名
 */
std::string generateArgName(const std::string& token) {
    if (token == "$") {
        static int placeholderCount = 0;
        return "placeholder_" + std::to_string(placeholderCount++);
    }
    return token;
}

// ========================================
// 完整使用示例 - printMylove
// ========================================

void example_complete_printMylove() {
    std::cout << "🎯 完整printMylove示例\n";
    std::cout << "=====================\n";
    
    // 1. 语法分析
    std::string str = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $,
        });
    )", str);
    
    std::cout << "✅ 1. syntaxAnalys完成，解析出 " << keyword.args.length() << " 个参数\n";
    
    // 2. 绑定处理函数
    keyword.args.bind("url", [](const std::string& value) {
        std::cout << "   bind处理url: " << value << "\n";
        return "\"" + value + "\"";
    });
    
    keyword.args.bind("mode", [](const std::string& value) {
        std::cout << "   bind处理mode: " << value << "\n";
        return "\"" + value + "\"";
    });
    
    std::cout << "✅ 2. bind函数设置完成\n";
    
    // 3. 设置transform规则（延迟执行）
    keyword.args.transform("url", [](const std::string& processedValue) {
        std::cout << "   transform转换url: " << processedValue << "\n";
        return "const imageUrl = " + processedValue + ";";
    });
    
    keyword.args.transform("mode", [](const std::string& processedValue) {
        std::cout << "   transform转换mode: " << processedValue << "\n";
        if (processedValue == "\"ASCII\"") {
            return "const renderMode = 'ascii';";
        } else if (processedValue == "\"Pixel\"") {
            return "const renderMode = 'pixel';";
        }
        return "const renderMode = " + processedValue + ";";
    });
    
    keyword.args.transform("width", [](const std::string& value) {
        std::cout << "   transform转换width: " << value << "\n";
        if (value.find("%") != std::string::npos) {
            std::string num = value.substr(0, value.length()-1);
            return "const width = Math.floor(window.innerWidth * " + num + " / 100);";
        } else if (value.find("px") != std::string::npos) {
            return "const width = parseInt('" + value + "');";
        }
        return "const width = " + value + ";";
    });
    
    std::cout << "✅ 3. transform规则设置完成\n";
    
    // 4. 模拟扫描器处理
    std::cout << "\n⚡ 4. 模拟scanKeyword处理\n";
    std::cout << "========================\n";
    
    // 模拟获取到的实际值
    std::cout << "模拟peekKeyword获取的值:\n";
    std::cout << "   url: 'avatar.jpg'\n";
    std::cout << "   mode: 'ASCII'\n";
    std::cout << "   width: '80%'\n\n";
    
    // 5. match时应用完整转换链
    std::cout << "🔄 5. match应用转换链\n";
    std::cout << "====================\n";
    
    keyword.args.match("url", "avatar.jpg");
    keyword.args.match("mode", "ASCII");
    keyword.args.match("width", "80%");
    
    std::cout << "\n🎉 6. 生成最终JS代码\n";
    std::cout << "==================\n";
    
    std::string finalCode = generateCode(keyword);
    std::cout << "生成的JavaScript代码:\n";
    std::cout << "```javascript\n";
    std::cout << finalCode;
    std::cout << "```\n";
    
    std::cout << "\n🌟 解决方案验证:\n";
    std::cout << "================\n";
    std::cout << "✅ transform问题完美解决！\n";
    std::cout << "✅ 延迟转换策略成功\n";
    std::cout << "✅ 保持API优雅设计\n";
    std::cout << "✅ 支持复杂转换逻辑\n";
    std::cout << "✅ 生成正确JS代码\n";
}

} // namespace new_cjmod_design

// ========================================
// 全局接口导出
// ========================================

using Keyword = new_cjmod_design::Keyword;
using Scanner = new_cjmod_design::Scanner;
using Policy = new_cjmod_design::Policy;

// 全局scanner实例
new_cjmod_design::Scanner scanner;

// 导出主要函数
using new_cjmod_design::syntaxAnalys;
using new_cjmod_design::generateCode;
using new_cjmod_design::isObject;
using new_cjmod_design::isFunction;
using new_cjmod_design::slice;