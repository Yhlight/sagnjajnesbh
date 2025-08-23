#include "CJMOD/CJMODApi.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>
#include <regex>
#include <sstream>

namespace CHTL {
namespace CJMOD {

// Arg类实现 - 正确的流程：bind设置值处理规则，transform设置JS转换模板
Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder), hasProcessor_(false) {}

void Arg::Match(const ArgValue& value) {
    // match方法：接受输入值，如果有bind的处理器则使用，否则直接设置
    
    if (hasProcessor_) {
        // 使用bind设置的处理器处理值
        if (argValueProcessor_) {
            rawValue_ = argValueProcessor_(value);
        } else if (std::holds_alternative<std::string>(value) && stringProcessor_) {
            rawValue_ = stringProcessor_(std::get<std::string>(value));
        } else if (std::holds_alternative<int>(value) && intProcessor_) {
            rawValue_ = intProcessor_(std::get<int>(value));
        } else if (std::holds_alternative<double>(value) && doubleProcessor_) {
            rawValue_ = doubleProcessor_(std::get<double>(value));
        } else if (std::holds_alternative<bool>(value) && boolProcessor_) {
            rawValue_ = boolProcessor_(std::get<bool>(value));
        } else {
            // 没有对应类型的处理器，直接转换
            SetRawValueDirectly(value);
        }
    } else {
        // 没有处理器，直接设置原始值
        SetRawValueDirectly(value);
    }
}

void Arg::SetRawValueDirectly(const ArgValue& value) {
    // 直接设置原始值
    if (std::holds_alternative<std::string>(value)) {
        rawValue_ = std::get<std::string>(value);
    } else if (std::holds_alternative<int>(value)) {
        rawValue_ = std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        rawValue_ = std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        rawValue_ = std::get<bool>(value) ? "true" : "false";
    }
}

std::string Arg::Transform(const std::string& jsTemplate) {
    // transform方法：将原始值按照JS模板转换为JS代码
    // 这是CJMOD语法糖的核心 - 决定最终生成什么JS代码
    
    jsTemplate_ = jsTemplate;
    
    if (jsTemplate_.empty()) {
        transformedValue_ = rawValue_;
        return transformedValue_;
    }
    
    std::string result = jsTemplate_;
    
    // 基础变量替换
    result = Utils::StringUtils::ReplaceAll(result, "{value}", rawValue_);
    result = Utils::StringUtils::ReplaceAll(result, "{name}", name_);
    
    // 支持类型检测
    std::string valueType = DetectValueType(rawValue_);
    result = Utils::StringUtils::ReplaceAll(result, "{type}", valueType);
    
    // 支持条件替换
    if (result.find("{if_string}") != std::string::npos) {
        std::string replacement = (valueType == "string") ? rawValue_ : "";
        result = Utils::StringUtils::ReplaceAll(result, "{if_string}", replacement);
    }
    
    if (result.find("{if_number}") != std::string::npos) {
        std::string replacement = (valueType == "number") ? rawValue_ : "0";
        result = Utils::StringUtils::ReplaceAll(result, "{if_number}", replacement);
    }
    
    if (result.find("{if_boolean}") != std::string::npos) {
        std::string replacement = (valueType == "boolean") ? rawValue_ : "false";
        result = Utils::StringUtils::ReplaceAll(result, "{if_boolean}", replacement);
    }
    
    // 支持安全转义
    if (result.find("{escaped}") != std::string::npos) {
        std::string escaped = EscapeForJavaScript(rawValue_);
        result = Utils::StringUtils::ReplaceAll(result, "{escaped}", escaped);
    }
    
    // 支持JSON格式化
    if (result.find("{json}") != std::string::npos) {
        std::string jsonValue = FormatAsJSON(rawValue_, valueType);
        result = Utils::StringUtils::ReplaceAll(result, "{json}", jsonValue);
    }
    
    // 支持函数参数格式化
    if (result.find("{param}") != std::string::npos) {
        std::string paramValue = FormatAsParameter(rawValue_, valueType);
        result = Utils::StringUtils::ReplaceAll(result, "{param}", paramValue);
    }
    
    transformedValue_ = result;
    return transformedValue_;
}

std::string Arg::DetectValueType(const std::string& value) const {
    if (value.empty()) return "undefined";
    
    // 检查数字
    if (std::regex_match(value, std::regex(R"(^-?\d+(\.\d+)?$)"))) {
        return "number";
    }
    
    // 检查布尔值
    if (value == "true" || value == "false") {
        return "boolean";
    }
    
    // 检查null
    if (value == "null") {
        return "null";
    }
    
    // 检查函数
    if (value.find("function") != std::string::npos || 
        value.find("=>") != std::string::npos ||
        (value.find("()") != std::string::npos && value.find("{}") != std::string::npos)) {
        return "function";
    }
    
    // 检查对象
    if (!value.empty() && value.front() == '{' && value.back() == '}') {
        return "object";
    }
    
    // 检查数组
    if (!value.empty() && value.front() == '[' && value.back() == ']') {
        return "array";
    }
    
    return "string";
}

std::string Arg::EscapeForJavaScript(const std::string& value) const {
    std::string escaped = value;
    
    // JavaScript字符串转义
    escaped = Utils::StringUtils::ReplaceAll(escaped, "\\", "\\\\");
    escaped = Utils::StringUtils::ReplaceAll(escaped, "\"", "\\\"");
    escaped = Utils::StringUtils::ReplaceAll(escaped, "'", "\\'");
    escaped = Utils::StringUtils::ReplaceAll(escaped, "\n", "\\n");
    escaped = Utils::StringUtils::ReplaceAll(escaped, "\r", "\\r");
    escaped = Utils::StringUtils::ReplaceAll(escaped, "\t", "\\t");
    
    return escaped;
}

std::string Arg::FormatAsJSON(const std::string& value, const std::string& type) const {
    if (type == "string") {
        return "\"" + EscapeForJavaScript(value) + "\"";
    } else if (type == "number" || type == "boolean" || type == "null") {
        return value;
    } else if (type == "function" || type == "object" || type == "array") {
        return value; // 保持原样
    }
    
    return "\"" + EscapeForJavaScript(value) + "\"";
}

std::string Arg::FormatAsParameter(const std::string& value, const std::string& type) const {
    if (type == "string") {
        return "'" + EscapeForJavaScript(value) + "'";
    } else if (type == "number" || type == "boolean" || type == "null") {
        return value;
    } else if (type == "function" || type == "object" || type == "array") {
        return value; // 函数和对象保持原样
    }
    
    return "'" + EscapeForJavaScript(value) + "'";
}

// ArgCollection类实现
void ArgCollection::AddArg(const std::string& name, bool isPlaceholder) {
    args_.emplace_back(name, isPlaceholder);
    if (!name.empty() && name != "$") {
        nameToIndex_[name] = args_.size() - 1;
    }
}

void ArgCollection::Match(const std::string& name, const ArgValue& value) {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        args_[it->second].Match(value);
    } else {
        // 对于占位符，匹配下一个未处理的占位符
        for (auto& arg : args_) {
            if (arg.IsPlaceholder() && arg.GetRawValue().empty()) {
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
    // result方法：组合所有转换后的JS代码
    std::ostringstream result;
    
    for (size_t i = 0; i < args_.size(); ++i) {
        const auto& transformedValue = args_[i].GetTransformedValue();
        if (!transformedValue.empty()) {
            if (i > 0 && !result.str().empty()) {
                result << " ";
            }
            result << transformedValue;
        }
    }
    
    return result.str();
}

// Syntax类实现
Syntax::Syntax(const std::string& pattern, const std::string& ignoreChars)
    : pattern_(pattern), ignoreChars_(ignoreChars) {
    ParsePattern();
}

std::string Syntax::Result() const {
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
    std::string trimmed = Utils::StringUtils::Trim(content);
    return !trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}';
}

bool CJMODScanner::IsFunction(const std::string& content) {
    return content.find("function") != std::string::npos || 
           content.find("=>") != std::string::npos ||
           content.find("() =>") != std::string::npos;
}

// 全局函数实现
std::unique_ptr<Syntax> SyntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    return std::make_unique<Syntax>(pattern, ignoreChars);
}

std::string GenerateCode(const Syntax& syntax) {
    return syntax.Result();
}

// CJMODManager类实现 - 纯API管理，不包含具体扩展
CJMODManager::CJMODManager() {}

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
    // 纯API驱动的CHTL JS处理
    // 不包含具体扩展实现，只提供API基础设施
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD API开始处理CHTL JS代码，长度: " + std::to_string(source.length())
    );
    
    // 扩展通过Initialize方法已经注册了语法处理器
    // 这里只需要提供API支持，具体处理由扩展完成
    
    std::string result = source;
    
    // 调用所有已注册的关键字处理器
    for (const auto& handler : keywordHandlers_) {
        if (result.find(handler.first) != std::string::npos) {
            Utils::ErrorHandler::GetInstance().LogInfo(
                "找到关键字: " + handler.first + "，调用处理器"
            );
            
            // 设置当前处理的源代码
            SetCurrentSource(result);
            
            // 调用处理器
            handler.second();
            
            // 获取处理结果
            result = GetProcessedSource();
        }
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo(
        "CJMOD API处理完成，结果长度: " + std::to_string(result.length())
    );
    
    return result;
}

void CJMODManager::SetCurrentSource(const std::string& source) {
    // 设置当前处理的源代码，供扫描器使用
    currentSource_ = source;
    
    // 简单分词（实际应该集成更完整的词法分析）
    std::istringstream iss(source);
    std::string token;
    scanner_.currentTokens_.clear();
    
    while (iss >> token) {
        scanner_.currentTokens_.push_back(token);
    }
    
    scanner_.currentPosition_ = 0;
}

std::string CJMODManager::GetProcessedSource() const {
    // 获取处理后的源代码
    return currentSource_;
}

} // namespace CJMOD
} // namespace CHTL