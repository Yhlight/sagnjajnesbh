#include "CJMOD/SimpleCJMODApi.h"
#include "Utils/StringUtils.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// ========== Arg类实现 - 按照您的原始设计哲学 ==========
Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder), current_(0) {}

void Arg::bind(const std::function<std::string(const std::string&)>& func) {
    bindFunction_ = func;
}

void Arg::bind(const std::function<std::string()>& func) {
    noArgFunction_ = func;
}

void Arg::match(const std::string& value) {
    value_ = value;
    if (bindFunction_) {
        transformedValue_ = bindFunction_(value);
    } else {
        transformedValue_ = value;  // 默认行为：直接使用原值
    }
}

const std::string& Arg::transform(const std::string& jsCode) {
    transformedValue_ = jsCode;
    return transformedValue_;
}

// ========== Args类实现 - 按照您的原始设计哲学 ==========
void Args::addArg(const std::string& name, bool isPlaceholder) {
    args_.emplace_back(name, isPlaceholder);
    if (!isPlaceholder) {
        nameToIndex_[name] = args_.size() - 1;
    } else {
        placeholderCount_++;
    }
}

Arg& Args::operator[](size_t index) {
    if (index >= args_.size()) {
        throw std::out_of_range("Arg index out of range");
    }
    return args_[index];
}

const Arg& Args::operator[](size_t index) const {
    if (index >= args_.size()) {
        throw std::out_of_range("Arg index out of range");
    }
    return args_[index];
}

Arg* Args::find(const std::string& name) {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        return &args_[it->second];
    }
    return nullptr;
}

void Args::bind(const std::string& name, const std::function<std::string(const std::string&)>& func) {
    if (name == "$") {
        // 占位符绑定：按照您的设计，自动计数并绑定到下一个未绑定的占位符
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && !arg.isBound()) {
                arg.bind(func);
                break;
            }
        }
    } else {
        // 普通参数绑定
        Arg* arg = find(name);
        if (arg) {
            arg->bind(func);
        }
    }
}

void Args::match(const std::string& name, const std::string& value) {
    if (name == "$") {
        // 占位符匹配：按照您的设计，自动计数并匹配到下一个未匹配的占位符
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && arg.getValue().empty()) {
                arg.match(value);
                break;
            }
        }
    } else {
        // 普通参数匹配
        Arg* arg = find(name);
        if (arg) {
            arg->match(value);
        }
    }
}

std::string Args::result() const {
    // 按照您的设计：将所有转换后的值组合起来
    std::ostringstream oss;
    for (const auto& arg : args_) {
        const std::string& value = arg.getValue();
        if (!value.empty()) {
            oss << value;
            // 根据参数类型智能添加分隔符
            if (!arg.isPlaceholder()) {
                oss << " ";
            }
        }
    }
    std::string result = oss.str();
    // 移除末尾多余的空格
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}

// ========== CJMODScanner类实现 - 按照您的原始设计哲学 ==========
CJMODScanner::CJMODScanner() 
    : currentPosition_(0), currentPolicy_(Policy::NORMAL) {}

void CJMODScanner::scanKeyword(const Arg& arg, const std::function<void()>& handler) {
    scanKeyword(arg.getName(), handler);
}

void CJMODScanner::scanKeyword(const std::string& keyword, const std::function<void()>& handler) {
    keywordHandlers_[keyword] = handler;
}

std::string CJMODScanner::peekKeyword(int offset) const {
    // 按照您的设计：支持正负偏移查看
    int targetPos = static_cast<int>(currentPosition_) + offset;
    if (targetPos >= 0 && targetPos < static_cast<int>(currentTokens_.size())) {
        return currentTokens_[targetPos];
    }
    return "";
}

void CJMODScanner::policyChangeBegin(const std::string& trigger, Policy policy) {
    currentPolicy_ = policy;
    if (policy == Policy::COLLECT) {
        collectBuffer_.clear();
        // 开始收集模式：记录触发点，准备收集内容
    }
}

std::string CJMODScanner::policyChangeEnd(const std::string& trigger, Policy policy) {
    std::string result;
    if (currentPolicy_ == Policy::COLLECT) {
        result = collectBuffer_;
        collectBuffer_.clear();
    }
    currentPolicy_ = policy;
    return result;
}

bool CJMODScanner::isObject(const std::string& content) const {
    // 按照您的设计：智能判断是否为对象
    std::string trimmed = Utils::StringUtils::Trim(content);
    if (trimmed.length() < 2) return false;
    
    // 基本的大括号匹配
    if (trimmed.front() == '{' && trimmed.back() == '}') {
        // 更精确的判断：检查是否有合法的对象结构
        int braceCount = 0;
        bool inString = false;
        char prevChar = '\0';
        
        for (char c : trimmed) {
            if (c == '"' && prevChar != '\\') {
                inString = !inString;
            } else if (!inString) {
                if (c == '{') braceCount++;
                else if (c == '}') braceCount--;
            }
            prevChar = c;
        }
        return braceCount == 0;
    }
    return false;
}

bool CJMODScanner::isFunction(const std::string& content) const {
    // 按照您的设计：智能判断是否为函数
    std::string trimmed = Utils::StringUtils::Trim(content);
    
    // 检查箭头函数
    if (trimmed.find("=>") != std::string::npos) {
        return true;
    }
    
    // 检查传统函数声明
    if (trimmed.find("function") != std::string::npos) {
        return true;
    }
    
    // 检查函数调用模式
    size_t parenPos = trimmed.find('(');
    if (parenPos != std::string::npos && parenPos > 0) {
        // 检查括号匹配
        int parenCount = 0;
        for (size_t i = parenPos; i < trimmed.length(); i++) {
            if (trimmed[i] == '(') parenCount++;
            else if (trimmed[i] == ')') parenCount--;
        }
        return parenCount == 0;
    }
    
    return false;
}

void CJMODScanner::setTokens(const std::vector<std::string>& tokens, size_t position) {
    currentTokens_ = tokens;
    currentPosition_ = position;
}

void CJMODScanner::processCollectPolicy(const std::string& content) {
    if (currentPolicy_ == Policy::COLLECT) {
        collectBuffer_ += content;
    }
}

void CJMODScanner::processSkipPolicy() {
    // 跳过策略：什么都不做，让扫描器继续
}

// ========== 全局函数实现 - 按照您的原始设计哲学 ==========
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    
    // 按照您的设计：复杂的内部实现，简单的外部使用
    std::string cleanPattern = pattern;
    
    // 步骤1：移除注释（更精确的注释处理）
    std::regex commentRegex(R"(//[^\r\n]*)");
    cleanPattern = std::regex_replace(cleanPattern, commentRegex, "");
    
    // 步骤2：智能分词，考虑到忽略字符
    std::vector<std::string> tokens;
    std::string currentToken;
    bool inString = false;
    char stringDelimiter = '\0';
    
    for (size_t i = 0; i < cleanPattern.length(); i++) {
        char c = cleanPattern[i];
        
        // 处理字符串字面量
        if ((c == '"' || c == '\'') && (i == 0 || cleanPattern[i-1] != '\\')) {
            if (!inString) {
                inString = true;
                stringDelimiter = c;
                currentToken += c;
            } else if (c == stringDelimiter) {
                inString = false;
                currentToken += c;
                stringDelimiter = '\0';
            } else {
                currentToken += c;
            }
            continue;
        }
        
        if (inString) {
            currentToken += c;
            continue;
        }
        
        // 处理分隔符和忽略字符
        if (std::isspace(c) || ignoreChars.find(c) != std::string::npos) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            // 对于某些特殊字符，也将其作为token添加
            if (ignoreChars.find(c) != std::string::npos && !std::isspace(c)) {
                // 某些符号可能需要保留为独立token
                std::string specialChars = "{}()[]";
                if (specialChars.find(c) != std::string::npos) {
                    tokens.push_back(std::string(1, c));
                }
            }
        } else {
            currentToken += c;
        }
    }
    
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    
    // 步骤3：构建Args结构
    for (const auto& token : tokens) {
        if (!token.empty()) {
            bool isPlaceholder = (token == "$");
            syntax->args.addArg(token, isPlaceholder);
        }
    }
    
    return syntax;
}

std::string generateCode(const Syntax& syntax) {
    // 按照您的设计：简单的接口，智能的内部处理
    std::string result = syntax.args.result();
    
    // 后处理：格式化生成的JS代码
    if (!result.empty()) {
        // 基本的代码格式化
        std::regex multipleSpaces(R"(\s+)");
        result = std::regex_replace(result, multipleSpaces, " ");
        
        // 去除首尾空格
        result = Utils::StringUtils::Trim(result);
    }
    
    return result;
}

// 全局CJMODScanner实例管理
static std::unique_ptr<CJMODScanner> globalScanner = nullptr;

CJMODScanner& getCJMODScanner() {
    if (!globalScanner) {
        globalScanner = std::make_unique<CJMODScanner>();
    }
    return *globalScanner;
}

} // namespace CJMOD
} // namespace CHTL