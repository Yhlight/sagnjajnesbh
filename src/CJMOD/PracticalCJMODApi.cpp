#include "CJMOD/PracticalCJMODApi.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// ============ Arg类实现 ============

Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder), hasBind_(false) {
}

std::string Arg::operator()(const std::string& peekedValue) {
    // 专门处理peekKeyword的结果
    // 这不是通用处理器，而是处理扫描器返回的关键字
    std::cout << "处理peekKeyword结果: " << peekedValue << " (参数: " << name_ << ")" << std::endl;
    
    if (hasBind_ && valueProcessor_) {
        std::string processed = valueProcessor_(peekedValue);
        processedValue_ = processed;
        
        // 如果有JS模板，立即应用转换
        if (!jsTemplate_.empty()) {
            applyJSTransform();
        }
        
        return processed;
    }
    
    return peekedValue;
}

void Arg::transform(const std::string& jsTemplate) {
    jsTemplate_ = jsTemplate;
    std::cout << "设置JS模板: " << jsTemplate << " (参数: " << name_ << ")" << std::endl;
    
    // 如果已经有处理过的值，立即应用转换
    if (!processedValue_.empty()) {
        applyJSTransform();
    }
}

void Arg::match(const std::string& rawValue) {
    rawValue_ = rawValue;
    std::cout << "匹配原始值: " << rawValue << " (参数: " << name_ << ")" << std::endl;
    
    if (hasBind_ && valueProcessor_) {
        processedValue_ = valueProcessor_(rawValue);
        std::cout << "  处理后的值: " << processedValue_ << std::endl;
    } else {
        processedValue_ = rawValue;
    }
    
    // 应用JS转换
    if (!jsTemplate_.empty()) {
        applyJSTransform();
    }
}

void Arg::applyJSTransform() {
    if (jsTemplate_.empty()) {
        jsCode_ = processedValue_;
        return;
    }
    
    // 应用JS模板转换
    std::string result = jsTemplate_;
    
    // 替换${value}占位符
    size_t pos = 0;
    while ((pos = result.find("${value}", pos)) != std::string::npos) {
        result.replace(pos, 8, processedValue_);
        pos += processedValue_.length();
    }
    
    // 替换${name}占位符
    pos = 0;
    while ((pos = result.find("${name}", pos)) != std::string::npos) {
        result.replace(pos, 7, name_);
        pos += name_.length();
    }
    
    jsCode_ = result;
    std::cout << "  生成JS代码: " << jsCode_ << std::endl;
}

// ============ Args类实现 ============

void Args::addArg(const std::string& name, bool isPlaceholder) {
    args_.emplace_back(name, isPlaceholder);
    if (!isPlaceholder) {
        nameToIndex_[name] = args_.size() - 1;
    }
}

Arg& Args::operator[](size_t index) {
    if (index >= args_.size()) {
        throw std::out_of_range("参数索引超出范围");
    }
    return args_[index];
}

const Arg& Args::operator[](size_t index) const {
    if (index >= args_.size()) {
        throw std::out_of_range("参数索引超出范围");
    }
    return args_[index];
}

void Args::transform(const std::string& name, const std::string& jsTemplate) {
    if (name == "$") {
        // 占位符转换：找到下一个未转换的占位符
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && !arg.hasTransform()) {
                arg.transform(jsTemplate);
                break;
            }
        }
    } else {
        // 命名参数转换
        Arg* arg = findArg(name);
        if (arg) {
            arg->transform(jsTemplate);
        }
    }
}

void Args::match(const std::string& name, const std::string& value) {
    if (name == "$") {
        // 占位符匹配：找到下一个未匹配的占位符
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && arg.getValue().empty()) {
                arg.match(value);
                return;
            }
        }
    } else {
        // 命名参数匹配
        Arg* arg = findArg(name);
        if (arg) {
            arg->match(value);
        }
    }
}

std::string Args::result() const {
    std::string result;
    for (const auto& arg : args_) {
        if (!result.empty()) result += " ";
        
        // 优先使用JS代码，其次使用处理后的值
        if (!arg.getJSCode().empty()) {
            result += arg.getJSCode();
        } else if (!arg.getValue().empty()) {
            result += arg.getValue();
        }
    }
    return result;
}

Arg* Args::findArg(const std::string& name) {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        return &args_[it->second];
    }
    return nullptr;
}

// ============ CJMODScanner类实现 ============

CJMODScanner::CJMODScanner() 
    : currentPosition_(0), currentPolicy_(Policy::NORMAL) {
}

void CJMODScanner::scanKeyword(const Arg& arg, std::function<void()> handler) {
    scanKeyword(arg.getName(), handler);
}

void CJMODScanner::scanKeyword(const std::string& keyword, std::function<void()> handler) {
    keywordHandlers_[keyword] = handler;
    std::cout << "注册关键字扫描: " << keyword << std::endl;
}

std::string CJMODScanner::peekKeyword(int offset) const {
    // 这是原生API的核心实用功能
    int targetPos = static_cast<int>(currentPosition_) + offset;
    
    if (targetPos >= 0 && targetPos < static_cast<int>(currentTokens_.size())) {
        std::string result = currentTokens_[targetPos];
        std::cout << "peekKeyword(" << offset << ") = \"" << result << "\" (当前位置: " << currentPosition_ << ")" << std::endl;
        return result;
    }
    
    std::cout << "peekKeyword(" << offset << ") = \"\" (超出范围)" << std::endl;
    return "";
}

void CJMODScanner::policyChangeBegin(const std::string& trigger, Policy policy) {
    currentPolicy_ = policy;
    std::cout << "策略改变开始: " << trigger << " -> ";
    
    switch (policy) {
        case Policy::COLLECT:
            std::cout << "COLLECT";
            collectBuffer_.clear();
            break;
        case Policy::SKIP:
            std::cout << "SKIP";
            break;
        case Policy::NORMAL:
            std::cout << "NORMAL";
            break;
    }
    std::cout << std::endl;
}

std::string CJMODScanner::policyChangeEnd(const std::string& trigger, Policy policy) {
    std::string result;
    
    if (currentPolicy_ == Policy::COLLECT) {
        result = collectBuffer_;
        std::cout << "策略改变结束: " << trigger << ", 收集到: \"" << result << "\"" << std::endl;
        collectBuffer_.clear();
    }
    
    currentPolicy_ = policy;
    return result;
}

bool CJMODScanner::isObject(const std::string& content) const {
    // 实用的对象判断函数
    std::string trimmed = content;
    
    // 移除首尾空格
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    if (trimmed.length() < 2) return false;
    
    // 基本的大括号匹配
    if (trimmed.front() == '{' && trimmed.back() == '}') {
        // 简单的括号匹配验证
        int braceCount = 0;
        for (char c : trimmed) {
            if (c == '{') braceCount++;
            else if (c == '}') braceCount--;
        }
        return braceCount == 0;
    }
    
    return false;
}

bool CJMODScanner::isFunction(const std::string& content) const {
    // 实用的函数判断
    std::string trimmed = content;
    
    // 检查常见的函数模式
    if (trimmed.find("function") != std::string::npos ||
        trimmed.find("=>") != std::string::npos ||
        trimmed.find("()") != std::string::npos) {
        return true;
    }
    
    return false;
}

void CJMODScanner::setTokens(const std::vector<std::string>& tokens, size_t position) {
    currentTokens_ = tokens;
    currentPosition_ = position;
    std::cout << "设置扫描上下文: " << tokens.size() << " 个token, 位置: " << position << std::endl;
}

void CJMODScanner::processCollectPolicy(const std::string& content) {
    if (currentPolicy_ == Policy::COLLECT) {
        collectBuffer_ += content;
    }
}

void CJMODScanner::processSkipPolicy() {
    // 跳过策略：什么都不做
}

// ============ CHTLJSFunction类实现 ============

CHTLJSFunction::CHTLJSFunction(const std::string& functionName, 
                               const std::vector<std::string>& paramNames)
    : functionName_(functionName), paramNames_(paramNames) {
    std::cout << "创建CHTLJSFunction: " << functionName_ << std::endl;
}

void CHTLJSFunction::transformKey(const std::string& keyName, const std::string& jsTemplate) {
    if (!syntax_) createSyntax();
    syntax_->args.transform(keyName, jsTemplate);
}

std::unique_ptr<Syntax> CHTLJSFunction::createSyntax() {
    syntax_ = std::make_unique<Syntax>();
    
    if (paramNames_.empty()) {
        // 自动生成占位符参数
        for (int i = 0; i < 5; ++i) {  // 默认5个占位符
            syntax_->args.addArg("$", true);
        }
    } else {
        // 使用指定的参数名
        for (const auto& paramName : paramNames_) {
            bool isPlaceholder = (paramName == "$");
            syntax_->args.addArg(paramName, isPlaceholder);
        }
    }
    
    return std::make_unique<Syntax>(*syntax_);
}

void CHTLJSFunction::scanKeyword(CJMODScanner& scanner) {
    std::cout << "扫描关键字: " << functionName_ << std::endl;
    
    scanner.scanKeyword(functionName_, [this]() {
        std::cout << "检测到CHTL JS函数: " << functionName_ << std::endl;
        // 这里可以添加具体的处理逻辑
    });
}

void CHTLJSFunction::matchKeyword(const std::string& keyword, const std::string& value) {
    if (!syntax_) createSyntax();
    syntax_->args.match(keyword, value);
}

std::string CHTLJSFunction::generateCode() {
    if (!syntax_) return "";
    
    std::string code = functionName_ + "({\n";
    
    for (size_t i = 0; i < paramNames_.size() && i < syntax_->args.length(); ++i) {
        if (i > 0) code += ",\n";
        code += "  " + paramNames_[i] + ": ";
        
        // 使用JS代码或处理后的值
        const auto& arg = syntax_->args[i];
        if (!arg.getJSCode().empty()) {
            code += arg.getJSCode();
        } else {
            code += arg.getValue();
        }
    }
    
    code += "\n});";
    return code;
}

std::string CHTLJSFunction::generateSyntaxPattern() {
    std::string pattern = functionName_ + "({\n";
    for (size_t i = 0; i < paramNames_.size(); ++i) {
        if (i > 0) pattern += ",\n";
        pattern += "  " + paramNames_[i] + ": $";
    }
    pattern += "\n});";
    return pattern;
}

// ============ 全局函数实现 ============

std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                     const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    
    std::cout << "语法分析: " << pattern.substr(0, 50) << "..." << std::endl;
    std::cout << "忽略字符: " << ignoreChars << std::endl;
    
    // 智能分析模式，识别占位符和参数
    size_t pos = 0;
    int placeholderCount = 0;
    
    // 寻找占位符 $
    while ((pos = pattern.find('$', pos)) != std::string::npos) {
        syntax->args.addArg("$", true);
        placeholderCount++;
        pos++;
    }
    
    // 如果没有占位符，尝试识别命名参数
    if (placeholderCount == 0) {
        std::regex paramRegex(R"((\w+):\s*\$)");
        std::sregex_iterator iter(pattern.begin(), pattern.end(), paramRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string paramName = iter->str(1);
            syntax->args.addArg(paramName, false);
        }
    }
    
    std::cout << "识别到 " << syntax->args.length() << " 个参数" << std::endl;
    
    return syntax;
}

std::string generateCode(const Syntax& syntax) {
    return syntax.result();
}

// 全局扫描器实例
static std::unique_ptr<CJMODScanner> globalScanner = nullptr;

CJMODScanner& getCJMODScanner() {
    if (!globalScanner) {
        globalScanner = std::make_unique<CJMODScanner>();
    }
    return *globalScanner;
}

std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(
    const std::string& functionName,
    const std::vector<std::string>& paramNames) {
    
    return std::make_unique<CHTLJSFunction>(functionName, paramNames);
}

} // namespace CJMOD
} // namespace CHTL