#include "test.h"
#include <iostream>
#include <regex>

using namespace CHTL::CJMOD;

// ============================================================================
// 全局注册表实现
// ============================================================================

std::unordered_map<std::string, std::string> g_virtualObjectRegistry;
std::unordered_map<std::string, std::string> g_globalFunctionRegistry;
std::unordered_map<std::string, std::vector<std::string>> g_eventDelegateRegistry;

// ============================================================================
// 基础CJMOD API实现（简化版本，用于演示）
// ============================================================================

namespace CHTL { namespace CJMOD {

// Arg类实现
Arg::Arg(const std::string& name, bool isPlaceholder) 
    : name_(name), isPlaceholder_(isPlaceholder), hasBind_(false), hasValue_(false) {}

void Arg::bind(const std::function<std::string()>& func) {
    bindFunction_ = [func](const std::string&) { return func(); };
    hasBind_ = true;
}

void Arg::match(const std::string& value) {
    rawValue_ = value;
    if (hasBind_ && bindFunction_) {
        processedValue_ = bindFunction_(value);
    } else {
        processedValue_ = value;
    }
    hasValue_ = true;
    applyTransform();
}

void Arg::transform(const std::string& jsTemplate) {
    jsTemplate_ = jsTemplate;
    applyTransform();
}

void Arg::applyTransform() {
    if (!jsTemplate_.empty() && hasValue_) {
        jsCode_ = jsTemplate_;
        // 简单的模板替换
        size_t pos = jsCode_.find("$");
        if (pos != std::string::npos) {
            jsCode_.replace(pos, 1, processedValue_);
        }
    }
}

// Syntax类实现
void Syntax::match(const std::string& name, const std::string& value) {
    auto* arg = findArg(name);
    if (arg) {
        arg->match(value);
    }
}

void Syntax::transform(const std::string& name, const std::string& jsTemplate) {
    auto* arg = findArg(name);
    if (arg) {
        arg->transform(jsTemplate);
    }
}

std::string Syntax::result() {
    std::string result;
    for (const auto& arg : args) {
        if (arg.hasValue()) {
            if (!result.empty()) result += ", ";
            result += arg.getValue();
        }
    }
    return result;
}

void Syntax::addArg(const std::string& name, bool isPlaceholder) {
    args.emplace_back(name, isPlaceholder);
    nameToIndex_[name] = args.size() - 1;
}

void Syntax::setTriggerKeyword(const std::string& keyword) {
    triggerKeyword_ = keyword;
}

Arg* Syntax::findArg(const std::string& name) {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        return &args[it->second];
    }
    return nullptr;
}

// CHTLJSFunction类实现
CHTLJSFunction::CHTLJSFunction() : hasFragment_(false), currentPolicy_(Policy::NORMAL) {}

std::unique_ptr<Syntax> CHTLJSFunction::syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    
    // 简单的语法分析：查找参数
    std::regex argRegex(R"(\b(\w+)\s*:\s*\$)");
    std::sregex_iterator iter(pattern.begin(), pattern.end(), argRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string argName = (*iter)[1].str();
        syntax->addArg(argName, false);
    }
    
    // 查找占位符
    std::regex placeholderRegex(R"(\$)");
    std::sregex_iterator placeholderIter(pattern.begin(), pattern.end(), placeholderRegex);
    size_t placeholderCount = std::distance(placeholderIter, std::sregex_iterator{});
    
    for (size_t i = 0; i < placeholderCount; ++i) {
        syntax->addArg("$" + std::to_string(i), true);
    }
    
    return syntax;
}

void CHTLJSFunction::scanKeyword(const std::string& keyword, std::function<void()> handler) {
    keywordHandlers_[keyword] = handler;
}

void CHTLJSFunction::scanKeyword(const Arg& arg, std::function<void()> handler) {
    keywordHandlers_[arg.getName()] = handler;
}

std::string CHTLJSFunction::peekKeyword(int offset) const {
    if (!hasFragment_) return "";
    
    // 简化实现：从当前代码片段中提取值
    if (offset == 1) {
        // 返回下一个值（模拟）
        return "nextValue";
    } else if (offset == -1) {
        // 返回上一个值（模拟）
        return "prevValue";
    }
    return "";
}

void CHTLJSFunction::policyChangeBegin(const std::string& trigger, Policy policy) {
    policyStack_.push_back({trigger, currentPolicy_});
    currentPolicy_ = policy;
}

std::string CHTLJSFunction::policyChangeEnd(const std::string& trigger, Policy policy) {
    if (!policyStack_.empty()) {
        currentPolicy_ = policyStack_.back().policy;
        policyStack_.pop_back();
    }
    return "policyResult";
}

std::string CHTLJSFunction::generateCode(const Syntax& syntax) {
    std::string jsCode = "// Generated JavaScript Code\n";
    
    for (const auto& arg : syntax.args) {
        if (!arg.getJSCode().empty()) {
            jsCode += arg.getJSCode() + "\n";
        }
    }
    
    return jsCode;
}

bool CHTLJSFunction::isObject(const std::string& content) {
    return content.find("{") != std::string::npos && content.find("}") != std::string::npos;
}

bool CHTLJSFunction::isFunction(const std::string& content) {
    return content.find("function") != std::string::npos || content.find("=>") != std::string::npos;
}

std::string CHTLJSFunction::slice(const std::string& content, size_t start, size_t end) {
    if (end == std::string::npos) {
        return content.substr(start);
    }
    return content.substr(start, end - start);
}

void CHTLJSFunction::setCodeFragment(const CodeFragment& fragment) {
    currentFragment_ = fragment;
    hasFragment_ = true;
}

std::string CHTLJSFunction::processSourceCode(const std::string& sourceCode, const std::string& pattern) {
    // 设置代码片段
    CodeFragment fragment(sourceCode, "testFunction", 0);
    setCodeFragment(fragment);
    
    // 触发关键字处理器
    for (const auto& handler : keywordHandlers_) {
        if (sourceCode.find(handler.first) != std::string::npos) {
            handler.second();
        }
    }
    
    return sourceCode;
}

// 全局便捷函数实现
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    CHTLJSFunction processor;
    return processor.syntaxAnalys(pattern, ignoreChars);
}

std::string generateCode(const Syntax& syntax) {
    CHTLJSFunction processor;
    return processor.generateCode(syntax);
}

std::unique_ptr<CHTLJSFunction> createCJMODProcessor() {
    return std::make_unique<CHTLJSFunction>();
}

} // namespace CJMOD
} // namespace CHTL

// ============================================================================
// Test CJMOD 扩展函数实现
// ============================================================================

/**
 * @brief testFunction - 演示标准流程，从外部获取值
 */
std::string testFunction(const std::string& sourceCode) {
    std::cout << "\n=== testFunction 标准流程演示 ===\n";
    std::cout << "输入源代码: " << sourceCode << "\n\n";
    
    // 创建CJMOD处理器
    auto processor = createCJMODProcessor();
    
    // 1️⃣ syntaxAnalys - 语法分析
    std::cout << "1️⃣ syntaxAnalys - 分析testFunction语法\n";
    auto syntax = processor->syntaxAnalys(R"(
        testFunction({
            name: $,
            value: $,
            type: $
        });
    )", ",:{};()");
    
    std::cout << "   解析出 " << syntax->args.size() << " 个参数\n";
    
    // 2️⃣ bind - 绑定处理函数
    std::cout << "\n2️⃣ bind - 绑定参数处理函数\n";
    syntax->bind("name", [](const std::string& name) -> std::string {
        std::cout << "     处理name参数: " << name << "\n";
        return "\"" + name + "\"";
    });
    
    syntax->bind("value", [](const std::string& value) -> std::string {
        std::cout << "     处理value参数: " << value << "\n";
        return value;
    });
    
    syntax->bind("type", [](const std::string& type) -> std::string {
        std::cout << "     处理type参数: " << type << "\n";
        return "\"" + type + "\"";
    });
    
    // 3️⃣ transform - 转换为JS代码
    std::cout << "\n3️⃣ transform - 设置JS代码模板\n";
    syntax->transform("name", "const testName = $;");
    syntax->transform("value", "const testValue = $;");
    syntax->transform("type", "const testType = $;");
    
    // 4️⃣ scanKeyword - 设置扫描关键字
    std::cout << "\n4️⃣ scanKeyword - 设置扫描关键字\n";
    processor->scanKeyword("testFunction", [&]() {
        std::cout << "   扫描到testFunction关键字\n";
        
        // 5️⃣ match - 从外部源代码中提取值
        std::cout << "\n5️⃣ match - 从外部源代码中提取值\n";
        
        // 从sourceCode中提取实际值
        std::regex nameRegex(R"(name:\s*['"](.*?)['"])");
        std::regex valueRegex(R"(value:\s*(\d+))");
        std::regex typeRegex(R"(type:\s*['"](.*?)['"])");
        
        std::smatch match;
        
        if (std::regex_search(sourceCode, match, nameRegex)) {
            std::string nameValue = match[1].str();
            syntax->match("name", nameValue);
            std::cout << "     从源代码提取name: " << nameValue << "\n";
        }
        
        if (std::regex_search(sourceCode, match, valueRegex)) {
            std::string valueValue = match[1].str();
            syntax->match("value", valueValue);
            std::cout << "     从源代码提取value: " << valueValue << "\n";
        }
        
        if (std::regex_search(sourceCode, match, typeRegex)) {
            std::string typeValue = match[1].str();
            syntax->match("type", typeValue);
            std::cout << "     从源代码提取type: " << typeValue << "\n";
        }
    });
    
    // 处理源代码
    processor->processSourceCode(sourceCode, "testFunction");
    
    // 6️⃣ result - 组合结果
    std::cout << "\n6️⃣ result - 组合结果\n";
    std::string result = syntax->result();
    std::cout << "   组合结果: " << result << "\n";
    
    // 7️⃣ generateCode - 生成最终JS代码
    std::cout << "\n7️⃣ generateCode - 生成最终JS代码\n";
    std::string jsCode = processor->generateCode(*syntax);
    
    std::cout << "生成的JavaScript代码:\n";
    std::cout << "----------------------------------------\n";
    std::cout << jsCode;
    std::cout << "----------------------------------------\n";
    
    return jsCode;
}

/**
 * @brief simpleTest - 简单测试函数
 */
std::string simpleTest(const std::string& input, const std::string& mode) {
    std::cout << "\n=== simpleTest 简单测试 ===\n";
    std::cout << "输入: " << input << ", 模式: " << mode << "\n";
    
    // 最基础的CJMOD扩展
    std::string result;
    
    if (mode == "upper") {
        result = "console.log('" + input + "'.toUpperCase());";
    } else if (mode == "lower") {
        result = "console.log('" + input + "'.toLowerCase());";
    } else {
        result = "console.log('" + input + "');";
    }
    
    std::cout << "生成的JS代码: " << result << "\n";
    return result;
}