#include "CJMOD/SophisticatedCJMODApi.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <iostream>
#include <cctype>

namespace CHTL {
namespace CJMOD {

// ========== Arg类实现 ==========
Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder) {}

void Arg::match(const std::string& value) {
    rawValue_ = value;
    if (bindFunction_) {
        transformedValue_ = bindFunction_(value);
    } else {
        transformedValue_ = value;  // 默认直接使用原值
    }
}

void Arg::match(const ArgValue& value) {
    std::string strValue;
    std::visit([&strValue](const auto& v) {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
            strValue = v;
        } else {
            strValue = std::to_string(v);
        }
    }, value);
    match(strValue);
}

const std::string& Arg::transform(const std::string& jsCode) {
    transformedValue_ = jsCode;
    return transformedValue_;
}

bool Arg::isObject() const {
    if (rawValue_.empty()) return false;
    std::string trimmed = rawValue_;
    // 移除前后空白
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return !trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}';
}

bool Arg::isFunction() const {
    if (rawValue_.empty()) return false;
    
    // 检查箭头函数
    if (rawValue_.find("=>") != std::string::npos) {
        return true;
    }
    
    // 检查function关键字
    if (rawValue_.find("function") != std::string::npos) {
        return true;
    }
    
    // 检查函数调用模式 (name(...))
    std::regex funcCallPattern(R"(\w+\s*\([^)]*\))");
    return std::regex_search(rawValue_, funcCallPattern);
}

void Arg::executeBinding() {
    if (bindFunction_ && !rawValue_.empty()) {
        transformedValue_ = bindFunction_(rawValue_);
    }
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
        throw std::out_of_range("Arg index out of range: " + std::to_string(index));
    }
    return args_[index];
}

const Arg& Args::operator[](size_t index) const {
    if (index >= args_.size()) {
        throw std::out_of_range("Arg index out of range: " + std::to_string(index));
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

const Arg* Args::find(const std::string& name) const {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        return &args_[it->second];
    }
    return nullptr;
}

void Args::match(const std::string& name, const std::string& value) {
    if (name == "$") {
        // 匹配下一个未匹配的占位符
        Arg* placeholder = getNextPlaceholder();
        if (placeholder) {
            placeholder->match(value);
        }
    } else {
        Arg* arg = find(name);
        if (arg) {
            arg->match(value);
        }
    }
}

void Args::match(const std::string& name, const ArgValue& value) {
    if (name == "$") {
        Arg* placeholder = getNextPlaceholder();
        if (placeholder) {
            placeholder->match(value);
        }
    } else {
        Arg* arg = find(name);
        if (arg) {
            arg->match(value);
        }
    }
}

void Args::transform(const std::string& name, const std::string& jsCode) {
    Arg* arg = find(name);
    if (arg) {
        arg->transform(jsCode);
    }
}

std::string Args::result() const {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& arg : args_) {
        const std::string& value = arg.getValue();
        if (!value.empty()) {
            if (!first) {
                oss << " ";
            }
            oss << value;
            first = false;
        }
    }
    
    return oss.str();
}

Arg* Args::getNextPlaceholder() {
    for (size_t i = currentPlaceholderIndex_; i < args_.size(); ++i) {
        if (args_[i].isPlaceholder() && args_[i].getValue().empty()) {
            currentPlaceholderIndex_ = i + 1;
            return &args_[i];
        }
    }
    return nullptr;
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
    // 保存当前策略状态到栈中
    policyStack_.push_back({currentPolicy_, trigger, collectBuffer_});
    
    currentPolicy_ = policy;
    if (policy == Policy::COLLECT) {
        collectBuffer_.clear();
    }
}

std::string CJMODScanner::policyChangeEnd(const std::string& trigger, Policy policy) {
    std::string result;
    
    if (currentPolicy_ == Policy::COLLECT) {
        result = collectBuffer_;
    }
    
    // 恢复之前的策略状态
    if (!policyStack_.empty()) {
        auto& prevState = policyStack_.back();
        currentPolicy_ = prevState.policy;
        collectBuffer_ = prevState.buffer;
        policyStack_.pop_back();
    } else {
        currentPolicy_ = policy;
        collectBuffer_.clear();
    }
    
    return result;
}

bool CJMODScanner::isObject(const std::string& content) const {
    if (content.empty()) return false;
    
    std::string trimmed = content;
    // 移除前后空白
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    if (trimmed.length() < 2) return false;
    
    // 检查是否以{}包围
    if (trimmed.front() != '{' || trimmed.back() != '}') {
        return false;
    }
    
    // 简单的括号匹配检查
    int braceCount = 0;
    for (char c : trimmed) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        if (braceCount < 0) return false;
    }
    
    return braceCount == 0;
}

bool CJMODScanner::isFunction(const std::string& content) const {
    if (content.empty()) return false;
    
    // 检查箭头函数
    if (content.find("=>") != std::string::npos) {
        return true;
    }
    
    // 检查function关键字
    if (content.find("function") != std::string::npos) {
        return true;
    }
    
    // 检查函数调用模式
    std::regex funcCallPattern(R"(\w+\s*\([^)]*\))");
    return std::regex_search(content, funcCallPattern);
}

void CJMODScanner::setTokens(const std::vector<std::string>& tokens) {
    currentTokens_ = tokens;
    currentPosition_ = 0;
}

void CJMODScanner::executeScan(const std::string& input) {
    tokenize(input);
    
    for (currentPosition_ = 0; currentPosition_ < currentTokens_.size(); ++currentPosition_) {
        const std::string& token = currentTokens_[currentPosition_];
        processToken(token);
    }
}

void CJMODScanner::tokenize(const std::string& input) {
    currentTokens_.clear();
    
    std::string currentToken;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < input.length(); ++i) {
        char c = input[i];
        
        if (!inString && (c == '"' || c == '\'')) {
            if (!currentToken.empty()) {
                currentTokens_.push_back(currentToken);
                currentToken.clear();
            }
            inString = true;
            stringChar = c;
            currentToken += c;
        } else if (inString && c == stringChar) {
            currentToken += c;
            currentTokens_.push_back(currentToken);
            currentToken.clear();
            inString = false;
            stringChar = '\0';
        } else if (inString) {
            currentToken += c;
        } else if (std::isspace(c)) {
            if (!currentToken.empty()) {
                currentTokens_.push_back(currentToken);
                currentToken.clear();
            }
        } else if (c == '(' || c == ')' || c == '{' || c == '}' || 
                   c == '[' || c == ']' || c == ',' || c == ';' || 
                   c == ':' || c == '=' || c == '+' || c == '-' || 
                   c == '*' || c == '/' || c == '.' || c == '!') {
            if (!currentToken.empty()) {
                currentTokens_.push_back(currentToken);
                currentToken.clear();
            }
            currentTokens_.push_back(std::string(1, c));
        } else {
            currentToken += c;
        }
    }
    
    if (!currentToken.empty()) {
        currentTokens_.push_back(currentToken);
    }
}

void CJMODScanner::processToken(const std::string& token) {
    handlePolicyChange(token);
    
    if (currentPolicy_ == Policy::SKIP) {
        return;
    }
    
    if (currentPolicy_ == Policy::COLLECT) {
        if (!collectBuffer_.empty()) {
            collectBuffer_ += " ";
        }
        collectBuffer_ += token;
        return;
    }
    
    // NORMAL策略下处理关键字
    auto it = keywordHandlers_.find(token);
    if (it != keywordHandlers_.end()) {
        it->second();  // 执行处理函数
    }
}

void CJMODScanner::handlePolicyChange(const std::string& token) {
    // 检查是否匹配当前策略的结束触发器
    if (!policyStack_.empty()) {
        const auto& currentState = policyStack_.back();
        if (matchesTrigger(token, currentState.trigger)) {
            // 触发策略结束
            policyChangeEnd(token, Policy::NORMAL);
        }
    }
}

bool CJMODScanner::matchesTrigger(const std::string& token, const std::string& trigger) const {
    return token == trigger;
}

// ========== 全局函数实现 ==========
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    syntax->originalPattern_ = pattern;
    syntax->ignoreChars_ = ignoreChars;
    
    // 预处理：移除注释
    std::string cleanPattern = pattern;
    std::regex commentRegex(R"(//.*?(?=\n|$))");
    cleanPattern = std::regex_replace(cleanPattern, commentRegex, "");
    
    // 分词
    std::vector<std::string> tokens = tokenizePattern(cleanPattern, ignoreChars);
    
    // 构建参数列表
    for (const auto& token : tokens) {
        if (!token.empty() && std::all_of(token.begin(), token.end(), [](char c) {
            return !std::isspace(c);
        })) {
            bool isPlaceholder = (token == "$");
            syntax->args.addArg(token, isPlaceholder);
        }
    }
    
    return syntax;
}

std::string generateCode(const Syntax& syntax) {
    return syntax.result();
}

bool isValidIdentifier(const std::string& str) {
    if (str.empty()) return false;
    
    // 检查第一个字符
    if (!std::isalpha(str[0]) && str[0] != '_' && str[0] != '$') {
        return false;
    }
    
    // 检查其余字符
    for (size_t i = 1; i < str.length(); ++i) {
        if (!std::isalnum(str[i]) && str[i] != '_' && str[i] != '$') {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> tokenizePattern(const std::string& pattern, const std::string& ignoreChars) {
    std::vector<std::string> tokens;
    std::string currentToken;
    bool inString = false;
    char stringChar = '\0';
    
    for (size_t i = 0; i < pattern.length(); ++i) {
        char c = pattern[i];
        
        if (!inString && (c == '"' || c == '\'')) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            inString = true;
            stringChar = c;
            currentToken += c;
        } else if (inString && c == stringChar) {
            currentToken += c;
            tokens.push_back(currentToken);
            currentToken.clear();
            inString = false;
            stringChar = '\0';
        } else if (inString) {
            currentToken += c;
        } else if (std::isspace(c)) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
        } else if (ignoreChars.find(c) != std::string::npos) {
            if (!currentToken.empty()) {
                tokens.push_back(currentToken);
                currentToken.clear();
            }
            // 忽略字符不添加到tokens中
        } else {
            currentToken += c;
        }
    }
    
    if (!currentToken.empty()) {
        tokens.push_back(currentToken);
    }
    
    return tokens;
}

} // namespace CJMOD
} // namespace CHTL