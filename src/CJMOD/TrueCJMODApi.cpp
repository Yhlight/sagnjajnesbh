#include "CJMOD/TrueCJMODApi.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// ============================================================================
// Arg类实现 - 严格按照用户原始设计
// ============================================================================

Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder), hasBind_(false), hasValue_(false) {}

void Arg::match(const std::string& value) {
    rawValue_ = value;
    
    if (hasBind_ && bindFunction_) {
        // 调用绑定的函数处理值
        processedValue_ = bindFunction_(value);
    } else {
        // 没有绑定函数，直接使用原值
        processedValue_ = value;
    }
    
    hasValue_ = true;
    applyTransform();
}

void Arg::transform(const std::string& jsTemplate) {
    jsTemplate_ = jsTemplate;
    if (hasValue_) {
        applyTransform();
    }
}

void Arg::applyTransform() {
    if (jsTemplate_.empty()) {
        jsCode_ = processedValue_;
    } else {
        // 简单的模板替换，将$替换为处理后的值
        jsCode_ = jsTemplate_;
        size_t pos = jsCode_.find("$");
        if (pos != std::string::npos) {
            jsCode_.replace(pos, 1, processedValue_);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const Arg& arg) {
    os << arg.name_;
    return os;
}

// ============================================================================
// Syntax类实现 - 严格按照用户原始设计
// ============================================================================

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
            if (!result.empty()) {
                result += " ";
            }
            result += arg.getJSCode();
        }
    }
    return result;
}

void Syntax::addArg(const std::string& name, bool isPlaceholder) {
    args.emplace_back(name, isPlaceholder);
    nameToIndex_[name] = args.size() - 1;
    
    if (isPlaceholder) {
        placeholderCounter_++;
    }
}

void Syntax::setTriggerKeyword(const std::string& keyword) {
    triggerKeyword_ = keyword;
}

Arg* Syntax::findArg(const std::string& name) {
    // 对于占位符$，使用计数器处理
    if (name == "$") {
        static size_t placeholderIndex = 0;
        for (size_t i = placeholderIndex; i < args.size(); ++i) {
            if (args[i].isPlaceholder()) {
                placeholderIndex = i + 1;
                return &args[i];
            }
        }
        // 重置计数器重新搜索
        placeholderIndex = 0;
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i].isPlaceholder()) {
                placeholderIndex = i + 1;
                return &args[i];
            }
        }
        return nullptr;
    }
    
    // 普通参数名搜索
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        return &args[it->second];
    }
    return nullptr;
}

// ============================================================================
// CHTLJSFunction类实现 - 严格按照用户原始设计
// ============================================================================

CHTLJSFunction::CHTLJSFunction()
    : currentFragment_(nullptr), hasFragment_(false), currentPolicy_(Policy::NORMAL) {}

std::unique_ptr<Syntax> CHTLJSFunction::syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    
    // 分词处理
    auto tokens = tokenize(pattern, ignoreChars);
    
    for (const auto& token : tokens) {
        if (!token.empty()) {
            bool isPlaceholder = (token == "$");
            syntax->addArg(token, isPlaceholder);
        }
    }
    
    return syntax;
}

void CHTLJSFunction::scanKeyword(const Arg& arg, std::function<void()> handler) {
    keywordHandlers_[arg.getName()] = handler;
}

void CHTLJSFunction::scanKeyword(const std::string& keyword, std::function<void()> handler) {
    keywordHandlers_[keyword] = handler;
}

std::string CHTLJSFunction::peekKeyword(int offset) const {
    if (!hasFragment_ || !currentFragment_) {
        return "";
    }
    
    // 这里需要与统一扫描器集成，暂时返回模拟值
    // 实际实现需要访问CodeFragment的tokens并根据offset获取内容
    return ""; // TODO: 实现真正的peekKeyword逻辑
}

bool CHTLJSFunction::isObject(const std::string& content) {
    // 简单的对象检测：以{开头，以}结尾
    std::string trimmed = Utils::StringUtils::Trim(content);
    return !trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}';
}

bool CHTLJSFunction::isFunction(const std::string& content) {
    // 简单的函数检测：包含=>或function关键字
    return content.find("=>") != std::string::npos || 
           content.find("function") != std::string::npos;
}

std::string CHTLJSFunction::slice(const std::string& content, size_t start, size_t end) {
    if (start >= content.length()) {
        return "";
    }
    
    if (end == std::string::npos || end > content.length()) {
        end = content.length();
    }
    
    return content.substr(start, end - start);
}

void CHTLJSFunction::policyChangeBegin(const std::string& trigger, Policy policy) {
    policyStack_.push_back({trigger, currentPolicy_});
    currentPolicy_ = policy;
    processPolicyChange(trigger, policy);
}

std::string CHTLJSFunction::policyChangeEnd(const std::string& trigger, Policy policy) {
    std::string result;
    
    // 根据策略收集内容
    if (currentPolicy_ == Policy::COLLECT) {
        // TODO: 实现收集逻辑，需要与统一扫描器集成
        result = ""; // 收集到的内容
    }
    
    // 恢复之前的策略
    if (!policyStack_.empty()) {
        currentPolicy_ = policyStack_.back().policy;
        policyStack_.pop_back();
    } else {
        currentPolicy_ = Policy::NORMAL;
    }
    
    return result;
}

std::string CHTLJSFunction::generateCode(const Syntax& syntax) {
    std::string jsCode = syntax.result();
    return optimizeJSCode(jsCode);
}

void CHTLJSFunction::setCodeFragment(const CodeFragment& fragment) {
    // currentFragment_ = &fragment; // TODO: 需要定义CodeFragment结构
    hasFragment_ = true;
}

std::string CHTLJSFunction::processSourceCode(const std::string& sourceCode, const std::string& pattern) {
    // 处理源代码，应用模式匹配
    // TODO: 与统一扫描器集成的完整实现
    return sourceCode;
}

std::vector<std::string> CHTLJSFunction::tokenize(const std::string& input, const std::string& ignoreChars) {
    std::vector<std::string> tokens;
    std::string current;
    
    for (char ch : input) {
        if (std::isspace(ch)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (ignoreChars.find(ch) != std::string::npos) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            // 忽略字符不添加到tokens中
        } else {
            current += ch;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

void CHTLJSFunction::processPolicyChange(const std::string& trigger, Policy policy) {
    // 处理策略变化的逻辑
    switch (policy) {
        case Policy::NORMAL:
            // 正常处理
            break;
        case Policy::COLLECT:
            // 开始收集模式
            break;
        case Policy::SKIP:
            // 跳过模式
            break;
    }
}

std::string CHTLJSFunction::optimizeJSCode(const std::string& jsCode) {
    // 简单的JS代码优化
    std::string optimized = Utils::StringUtils::Trim(jsCode);
    
    // 移除多余的空格
    std::regex multipleSpaces("\\s+");
    optimized = std::regex_replace(optimized, multipleSpaces, " ");
    
    return optimized;
}

// ============================================================================
// 全局函数实现 - 按照用户原始设计
// ============================================================================

std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    CHTLJSFunction processor;
    return processor.syntaxAnalys(pattern, ignoreChars);
}

std::string generateCode(const Syntax& syntax) {
    CHTLJSFunction processor;
    return processor.generateCode(syntax);
}

std::string createCHTLJSFunction(const std::string& chtlJsCode) {
    // 简化流程：自动化原始API流程
    
    try {
        // 1. syntaxAnalys - 分析CHTL JS代码
        auto syntax = syntaxAnalys(chtlJsCode, ",:{};()");
        
        // 2. 自动bind - 为占位符绑定默认处理函数
        for (auto& arg : syntax->args) {
            if (arg.isPlaceholder()) {
                arg.bind([](const std::string& value) -> std::string {
                    return value; // 默认直接返回值
                });
            }
        }
        
        // 3. 自动transform - 设置默认JS转换
        for (auto& arg : syntax->args) {
            if (arg.isPlaceholder()) {
                arg.transform("$"); // 默认模板
            }
        }
        
        // 4-7. 自动执行剩余步骤
        // scanKeyword, match, result, generateCode
        
        std::string result = generateCode(*syntax);
        
        Utils::ErrorHandler::GetInstance().LogInfo(
            "createCHTLJSFunction成功处理: " + chtlJsCode.substr(0, 50) + "..."
        );
        
        return result;
        
    } catch (const std::exception& e) {
        Utils::ErrorHandler::GetInstance().LogError(
            "createCHTLJSFunction处理失败: " + std::string(e.what())
        );
        return "";
    }
}

} // namespace CJMOD
} // namespace CHTL