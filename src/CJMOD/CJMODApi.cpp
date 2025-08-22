#include "CJMOD/CJMODApi.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {
namespace CJMOD {

// Arg类实现
Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder), matchCount_(0) {}

void Arg::Bind(const ArgBindFunction& func) {
    bindFunction_ = func;
}

std::string Arg::Match(const ArgValue& value) {
    if (!bindFunction_) {
        // 如果没有绑定函数，直接转换为字符串
        if (std::holds_alternative<std::string>(value)) {
            currentValue_ = std::get<std::string>(value);
        } else if (std::holds_alternative<int>(value)) {
            currentValue_ = std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            currentValue_ = std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            currentValue_ = std::get<bool>(value) ? "true" : "false";
        }
        return currentValue_;
    }
    
    // 使用绑定函数处理
    currentValue_ = bindFunction_(value);
    matchCount_++;
    
    return currentValue_;
}

void Arg::Transform(const std::string& jsTemplate) {
    jsTemplate_ = jsTemplate;
    
    // 应用模板转换
    if (!jsTemplate_.empty() && !currentValue_.empty()) {
        // 简单的模板替换：将{value}替换为实际值
        currentValue_ = Utils::StringUtils::ReplaceAll(jsTemplate_, "{value}", currentValue_);
    }
}

void Arg::Reset() {
    currentValue_ = "";
    matchCount_ = 0;
}

// ArgCollection类实现
void ArgCollection::AddArg(const std::string& name, bool isPlaceholder) {
    args_.emplace_back(name, isPlaceholder);
    if (!name.empty() && name != "$") {
        nameToIndex_[name] = args_.size() - 1;
    }
}

void ArgCollection::Bind(const std::string& name, const ArgBindFunction& func) {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        args_[it->second].Bind(func);
    } else {
        // 如果是占位符，绑定所有未绑定的占位符
        for (auto& arg : args_) {
            if (arg.IsPlaceholder() && !arg.IsBound()) {
                arg.Bind(func);
                break; // 只绑定第一个未绑定的占位符
            }
        }
    }
}

void ArgCollection::Bind(size_t index, const ArgBindFunction& func) {
    if (index < args_.size()) {
        args_[index].Bind(func);
    }
}

void ArgCollection::Match(const std::string& name, const ArgValue& value) {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        args_[it->second].Match(value);
    } else {
        // 如果是占位符，匹配下一个未处理的占位符
        for (auto& arg : args_) {
            if (arg.IsPlaceholder() && arg.GetCurrentValue().empty()) {
                arg.Match(value);
                break;
            }
        }
    }
}

void ArgCollection::Match(size_t index, const ArgValue& value) {
    if (index < args_.size()) {
        args_[index].Match(value);
    }
}

void ArgCollection::Transform(const std::string& name, const std::string& jsTemplate) {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        args_[it->second].Transform(jsTemplate);
    }
}

Arg& ArgCollection::operator[](size_t index) {
    static Arg emptyArg("", false);
    return (index < args_.size()) ? args_[index] : emptyArg;
}

const Arg& ArgCollection::operator[](size_t index) const {
    static Arg emptyArg("", false);
    return (index < args_.size()) ? args_[index] : emptyArg;
}

std::string ArgCollection::Result() const {
    std::ostringstream result;
    
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) {
            result << " ";
        }
        result << args_[i].GetCurrentValue();
    }
    
    return result.str();
}

void ArgCollection::Reset() {
    for (auto& arg : args_) {
        arg.Reset();
    }
}

// Syntax类实现
Syntax::Syntax(const std::string& pattern, const std::string& ignoreChars)
    : pattern_(pattern), ignoreChars_(ignoreChars) {
    ParsePattern();
}

std::string Syntax::GenerateCode() const {
    return args_.Result();
}

void Syntax::ParsePattern() {
    // 解析语法模式，提取参数
    std::string cleanPattern = pattern_;
    
    // 移除注释
    std::regex commentRegex(R"(//[^\n]*\n?)");
    cleanPattern = std::regex_replace(cleanPattern, commentRegex, "");
    
    // 移除忽略的字符
    for (char c : ignoreChars_) {
        cleanPattern = Utils::StringUtils::ReplaceAll(cleanPattern, std::string(1, c), " ");
    }
    
    // 按空白字符分割
    std::istringstream iss(cleanPattern);
    std::string token;
    
    while (iss >> token) {
        if (!token.empty()) {
            bool isPlaceholder = (token == "$");
            args_.AddArg(token, isPlaceholder);
        }
    }
}

// CJMODScanner类实现
CJMODScanner::CJMODScanner() : currentPosition_(0), currentPolicy_(ScanPolicy::NORMAL) {}

void CJMODScanner::ScanKeyword(const std::string& keyword, std::function<void()> handler) {
    keywordHandlers_[keyword] = handler;
}

void CJMODScanner::ScanKeyword(const Arg& arg, std::function<void()> handler) {
    keywordHandlers_[arg.GetName()] = handler;
}

std::string CJMODScanner::PeekKeyword(int offset) const {
    int targetPosition = static_cast<int>(currentPosition_) + offset;
    
    if (targetPosition < 0 || targetPosition >= static_cast<int>(currentTokens_.size())) {
        return "";
    }
    
    return currentTokens_[targetPosition];
}

void CJMODScanner::PolicyChangeBegin(const std::string& trigger, ScanPolicy policy) {
    currentPolicy_ = policy;
    
    if (policy == ScanPolicy::COLLECT) {
        collectBuffer_.clear();
    }
}

std::string CJMODScanner::PolicyChangeEnd(const std::string& trigger, ScanPolicy policy) {
    std::string result = collectBuffer_;
    currentPolicy_ = policy;
    collectBuffer_.clear();
    return result;
}

bool CJMODScanner::IsObject(const std::string& content) {
    // 简单检查：是否以{开始并以}结束
    std::string trimmed = Utils::StringUtils::Trim(content);
    return !trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}';
}

bool CJMODScanner::IsFunction(const std::string& content) {
    // 检查是否包含function关键字或箭头函数语法
    return content.find("function") != std::string::npos || 
           content.find("=>") != std::string::npos ||
           content.find("() =>") != std::string::npos;
}

// 全局函数实现
std::unique_ptr<Syntax> SyntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    return std::make_unique<Syntax>(pattern, ignoreChars);
}

std::string GenerateCode(const Syntax& syntax) {
    return syntax.GenerateCode();
}

// CJMODManager类实现
bool CJMODManager::RegisterExtension(std::unique_ptr<CJMODExtension> extension) {
    if (!extension) {
        return false;
    }
    
    // 初始化扩展
    if (!extension->Initialize(scanner_)) {
        Utils::ErrorHandler::GetInstance().LogError(
            "CJMOD扩展初始化失败: " + extension->GetName()
        );
        return false;
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "成功注册CJMOD扩展: " + extension->GetName() + " v" + extension->GetVersion()
    );
    
    extensions_.push_back(std::move(extension));
    return true;
}

std::string CJMODManager::ProcessCHTLJS(const std::string& source) {
    // 处理CHTL JS代码，应用所有注册的扩展
    std::string result = source;
    
    for (const auto& extension : extensions_) {
        // 这里需要调用扩展的处理逻辑
        // 简化实现：直接返回源代码
    }
    
    return result;
}

} // namespace CJMOD
} // namespace CHTL