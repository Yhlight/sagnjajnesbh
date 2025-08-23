#include "CJMOD/SimpleCJMODApi.h"
#include "Utils/StringUtils.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {
namespace CJMOD {

// ========== Arg类实现 ==========
Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder) {}

void Arg::bind(const std::function<std::string(const std::string&)>& func) {
    bindFunction_ = func;
}

void Arg::match(const std::string& value) {
    value_ = value;
    if (bindFunction_) {
        transformedValue_ = bindFunction_(value);
    } else {
        transformedValue_ = value;  // 默认直接使用原值
    }
}

const std::string& Arg::transform(const std::string& jsCode) {
    transformedValue_ = jsCode;
    return transformedValue_;
}

// ========== Args类实现 ==========
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
    // 按照您的设计：支持占位符的重复绑定
    if (name == "$") {
        // 占位符绑定：找到下一个未绑定的占位符
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && !arg.getValue().empty() == false) {
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
    // 按照您的设计：支持占位符的重复匹配
    if (name == "$") {
        // 占位符匹配：找到下一个未匹配的占位符
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
    // 按照您的设计：简单地拼接所有转换后的值
    std::ostringstream oss;
    for (const auto& arg : args_) {
        if (!arg.getValue().empty()) {
            oss << arg.getValue() << " ";
        }
    }
    std::string result = oss.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();  // 移除最后的空格
    }
    return result;
}

// ========== CJMODScanner类实现 ==========
CJMODScanner::CJMODScanner() 
    : currentPosition_(0), currentPolicy_(Policy::NORMAL) {}

void CJMODScanner::scanKeyword(const Arg& arg, std::function<void()> handler) {
    scanKeyword(arg.getName(), handler);
}

void CJMODScanner::scanKeyword(const std::string& keyword, std::function<void()> handler) {
    keywordHandlers_[keyword] = handler;
}

std::string CJMODScanner::peekKeyword(int offset) const {
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
    }
}

std::string CJMODScanner::policyChangeEnd(const std::string& trigger, Policy policy) {
    std::string result = collectBuffer_;
    currentPolicy_ = policy;
    collectBuffer_.clear();
    return result;
}

bool CJMODScanner::isObject(const std::string& content) {
    // 简单判断：是否以{开头}结尾
    std::string trimmed = Utils::StringUtils::Trim(content);
    return trimmed.front() == '{' && trimmed.back() == '}';
}

bool CJMODScanner::isFunction(const std::string& content) {
    // 简单判断：是否包含=>或function关键字
    return content.find("=>") != std::string::npos || 
           content.find("function") != std::string::npos;
}

// ========== 全局函数实现 ==========
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    
    // 按照您的设计：简单的字符串分析
    // 将模式字符串分解为关键字和占位符
    
    std::string cleanPattern = pattern;
    
    // 移除注释（// 开头的行）
    std::regex commentRegex("//.*");
    cleanPattern = std::regex_replace(cleanPattern, commentRegex, "");
    
    // 分割成token
    std::vector<std::string> tokens;
    std::string currentToken;
    
    for (char c : cleanPattern) {
        if (std::isspace(c)) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else if (ignoreChars.find(c) != std::string::npos) {
            // 忽略字符，但作为分隔符
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else {
            currentToken += c;
        }
    }
    
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    
    // 添加到args
    for (const auto& token : tokens) {
        if (!token.empty()) {
            bool isPlaceholder = (token == "$");
            syntax->args.addArg(token, isPlaceholder);
        }
    }
    
    return syntax;
}

std::string generateCode(const Syntax& syntax) {
    // 按照您的设计：直接返回result()的结果
    return syntax.args.result();
}

} // namespace CJMOD
} // namespace CHTL