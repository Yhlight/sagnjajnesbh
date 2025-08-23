#include "CJMOD/UnifiedCJMODApi.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// 全局扫描策略
static ScanStrategy globalScanStrategy = ScanStrategy::SLIDING_WINDOW;

// ============ Arg类实现 ============

Arg::Arg(const std::string& name) : name_(name), value_(std::string()) {}

Arg& Arg::operator=(const ArgValue& value) {
    value_ = value;
    return *this;
}

std::string Arg::ToString() const {
    return std::visit([](const auto& v) -> std::string {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, std::string>) {
            return v;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(v)>, bool>) {
            return v ? "true" : "false";
        } else {
            return std::to_string(v);
        }
    }, value_);
}

// ============ CHTLJSFunction类实现 ============

CHTLJSFunction::CHTLJSFunction(const std::string& name) 
    : name_(name), virSupported_(false) {}

void CHTLJSFunction::AddParam(const std::string& name, const std::string& defaultValue) {
    params_.emplace_back(name, defaultValue);
}

void CHTLJSFunction::SetBodyTemplate(const std::string& bodyTemplate) {
    bodyTemplate_ = bodyTemplate;
}

std::string CHTLJSFunction::GenerateJS() const {
    std::ostringstream js;
    
    // 生成函数签名
    js << "function " << name_ << "(";
    
    // 添加参数
    for (size_t i = 0; i < params_.size(); ++i) {
        if (i > 0) js << ", ";
        js << params_[i].first;
        if (!params_[i].second.empty()) {
            js << " = " << params_[i].second;
        }
    }
    
    js << ") {\n";
    
    // 添加函数体
    if (!bodyTemplate_.empty()) {
        js << "    " << bodyTemplate_ << "\n";
    } else {
        js << "    // TODO: 实现函数体\n";
    }
    
    js << "}";
    
    return js.str();
}

// ============ Syntax类实现 ============

Syntax::Syntax() : scanStrategy_(globalScanStrategy) {
    InitializeScanner();
}

Syntax::~Syntax() = default;

bool Syntax::AnalyzePattern(const std::string& pattern) {
    currentPattern_ = pattern;
    return ParseCHTLJSSyntax(pattern);
}

void Syntax::SetArg(const std::string& name, const ArgValue& value) {
    if (args_.find(name) == args_.end()) {
        args_[name] = Arg(name);
    }
    args_[name] = value;
}

Arg& Syntax::GetArg(const std::string& name) {
    if (args_.find(name) == args_.end()) {
        args_[name] = Arg(name);
    }
    return args_[name];
}

std::string Syntax::GenerateCode() const {
    if (customTransform_) {
        return customTransform_(args_);
    } else {
        return DefaultTransform();
    }
}

void Syntax::SetTransformRule(const std::function<std::string(const std::unordered_map<std::string, Arg>&)>& func) {
    customTransform_ = func;
}

void Syntax::RegisterFunction(const CHTLJSFunction& func) {
    functions_[func.GetName()] = func;
    
    // 如果函数支持vir，自动添加到支持列表
    if (func.IsVirSupported()) {
        BindVirFunction(func.GetName());
    }
}

void Syntax::SetScanStrategy(ScanStrategy strategy) {
    scanStrategy_ = strategy;
}

std::string Syntax::ScanAndProcess(const std::string& source) {
    switch (scanStrategy_) {
        case ScanStrategy::SLIDING_WINDOW:
            return SlidingWindowScan(source);
        case ScanStrategy::FRONT_EXTRACT:
            return FrontExtractScan(source);
        default:
            return source; // 无处理
    }
}

std::string Syntax::SlidingWindowScan(const std::string& source) {
    // 重置扫描器状态
    ResetScannerState();
    
    std::ostringstream result;
    size_t frontPtr = 0;
    size_t backPtr = 0;
    bool collectMode = false;
    std::string collectBuffer;
    
    // 初始扫描：避免语法片段位于开头
    for (size_t i = 0; i < std::min(size_t(1000), source.length()); ++i) {
        if (DetectCHTLJSSyntaxAtPos(source, i)) {
            // 处理前面的普通代码
            if (i > 0) {
                result << source.substr(0, i);
            }
            
            // 处理CHTL JS语法
            size_t syntaxEnd = FindSyntaxEnd(source, i);
            std::string syntaxFragment = source.substr(i, syntaxEnd - i);
            result << ProcessCHTLJSSyntax(syntaxFragment);
            
            frontPtr = syntaxEnd;
            backPtr = syntaxEnd;
            break;
        }
    }
    
    // 主扫描循环：双指针同时向前移动
    while (frontPtr < source.length()) {
        // 前指针检测关键字
        if (DetectCHTLJSSyntaxAtPos(source, frontPtr)) {
            if (!collectMode) {
                // 推送普通代码
                if (backPtr < frontPtr) {
                    result << source.substr(backPtr, frontPtr - backPtr);
                }
                
                // 进入收集模式
                collectMode = true;
                collectBuffer.clear();
            }
            
            // 收集CHTL JS语法
            size_t syntaxEnd = FindSyntaxEnd(source, frontPtr);
            std::string syntaxFragment = source.substr(frontPtr, syntaxEnd - frontPtr);
            collectBuffer += syntaxFragment;
            
            // 检查语法结束
            if (IsSyntaxComplete(collectBuffer)) {
                result << ProcessCHTLJSSyntax(collectBuffer);
                collectMode = false;
                frontPtr = syntaxEnd;
                backPtr = syntaxEnd;
                collectBuffer.clear();
                continue;
            } else {
                frontPtr = syntaxEnd;
                continue;
            }
        }
        
        // 双指针同时向前移动
        frontPtr++;
        if (!collectMode) {
            backPtr = frontPtr;
        }
    }
    
    // 处理剩余内容
    if (backPtr < source.length()) {
        if (collectMode && !collectBuffer.empty()) {
            result << ProcessCHTLJSSyntax(collectBuffer);
        } else {
            result << source.substr(backPtr);
        }
    }
    
    return result.str();
}

std::string Syntax::FrontExtractScan(const std::string& source) {
    std::ostringstream result;
    size_t pos = 0;
    
    while (pos < source.length()) {
        // 查找下一个CHTL JS语法位置
        size_t syntaxPos = FindNextCHTLJSSyntax(source, pos);
        
        if (syntaxPos != std::string::npos) {
            // 截取前置代码（不发送给编译器，仅记录）
            if (syntaxPos > pos) {
                std::string frontSegment = source.substr(pos, syntaxPos - pos);
                // 前置代码不添加到结果中，避免发送给编译器
                // 这里可以记录或进行其他处理
            }
            
            // 处理CHTL JS语法
            size_t syntaxEnd = FindSyntaxEnd(source, syntaxPos);
            std::string syntaxFragment = source.substr(syntaxPos, syntaxEnd - syntaxPos);
            result << ProcessCHTLJSSyntax(syntaxFragment);
            
            pos = syntaxEnd;
        } else {
            // 没有更多CHTL JS语法，处理剩余代码
            // 剩余代码也不发送给编译器
            break;
        }
    }
    
    return result.str();
}

void Syntax::BindVirFunction(const std::string& functionName) {
    virSupportedFunctions_.insert(functionName);
}

bool Syntax::IsVirSupported(const std::string& functionName) const {
    return virSupportedFunctions_.find(functionName) != virSupportedFunctions_.end();
}

std::vector<std::string> Syntax::GetRegisteredFunctions() const {
    std::vector<std::string> result;
    for (const auto& func : functions_) {
        result.push_back(func.first);
    }
    return result;
}

std::vector<std::string> Syntax::GetArgNames() const {
    std::vector<std::string> result;
    for (const auto& arg : args_) {
        result.push_back(arg.first);
    }
    return result;
}

void Syntax::Clear() {
    args_.clear();
    currentPattern_.clear();
    customTransform_ = nullptr;
    ResetScannerState();
}

// ============ Syntax类私有方法实现 ============

bool Syntax::ParseCHTLJSSyntax(const std::string& pattern) {
    // 使用正则表达式解析CHTL JS模式
    // 例如：functionName({key1: value1, key2: value2})
    
    std::regex functionPattern(R"((\w+)\s*\(\s*\{([^}]*)\}\s*\))");
    std::smatch match;
    
    if (std::regex_match(pattern, match, functionPattern)) {
        std::string functionName = match[1].str();
        std::string argsStr = match[2].str();
        
        // 解析参数
        std::regex argPattern(R"((\w+)\s*:\s*([^,}]+))");
        std::sregex_iterator iter(argsStr.begin(), argsStr.end(), argPattern);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string argName = (*iter)[1].str();
            std::string argValue = (*iter)[2].str();
            
            // 去除引号和空白
            argValue.erase(std::remove_if(argValue.begin(), argValue.end(), 
                [](char c) { return c == '"' || c == '\'' || std::isspace(c); }), argValue.end());
            
            SetArg(argName, argValue);
        }
        
        return true;
    }
    
    return false;
}

std::string Syntax::DefaultTransform() const {
    if (currentPattern_.empty()) {
        return "";
    }
    
    // 默认转换：将CHTL JS语法转换为标准JavaScript
    std::string result = currentPattern_;
    
    // 替换参数值
    for (const auto& arg : args_) {
        std::string placeholder = "{" + arg.first + "}";
        std::string value = arg.second.ToString();
        
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    
    return result;
}

void Syntax::InitializeScanner() {
    scannerState_.frontPointer = 0;
    scannerState_.backPointer = 0;
    scannerState_.collectMode = false;
    scannerState_.collectBuffer.clear();
    
    // 注册默认关键字
    scannerState_.keywords = {"vir", "printMylove", "iNeverAway", "listen", "delegate"};
}

void Syntax::ResetScannerState() {
    scannerState_.frontPointer = 0;
    scannerState_.backPointer = 0;
    scannerState_.collectMode = false;
    scannerState_.collectBuffer.clear();
}

bool Syntax::DetectCHTLJSSyntaxAtPos(const std::string& source, size_t pos) {
    // 检测是否为CHTL JS语法起始位置
    for (const auto& keyword : scannerState_.keywords) {
        if (pos + keyword.length() <= source.length()) {
            if (source.substr(pos, keyword.length()) == keyword) {
                return true;
            }
        }
    }
    return false;
}

size_t Syntax::FindSyntaxEnd(const std::string& source, size_t start) {
    // 简化实现：查找语法结束位置
    size_t pos = start;
    int braceCount = 0;
    bool inBraces = false;
    
    while (pos < source.length()) {
        char c = source[pos];
        
        if (c == '{') {
            braceCount++;
            inBraces = true;
        } else if (c == '}') {
            braceCount--;
            if (braceCount == 0 && inBraces) {
                return pos + 1; // 包含闭合括号
            }
        } else if (c == ';' && braceCount == 0) {
            return pos + 1; // 包含分号
        }
        
        pos++;
    }
    
    return pos;
}

size_t Syntax::FindNextCHTLJSSyntax(const std::string& source, size_t start) {
    size_t nearestPos = std::string::npos;
    
    for (const auto& keyword : scannerState_.keywords) {
        size_t pos = source.find(keyword, start);
        if (pos != std::string::npos && (nearestPos == std::string::npos || pos < nearestPos)) {
            nearestPos = pos;
        }
    }
    
    return nearestPos;
}

bool Syntax::IsSyntaxComplete(const std::string& syntax) {
    // 检查语法是否完整
    int braceCount = 0;
    for (char c : syntax) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
    }
    
    return braceCount == 0 && (syntax.find('}') != std::string::npos || syntax.find(';') != std::string::npos);
}

std::string Syntax::ProcessCHTLJSSyntax(const std::string& syntax) {
    // 处理CHTL JS语法，转换为标准JavaScript
    
    // 保存当前模式
    std::string savedPattern = currentPattern_;
    
    // 分析语法
    if (AnalyzePattern(syntax)) {
        std::string result = GenerateCode();
        
        // 恢复模式
        currentPattern_ = savedPattern;
        
        return result;
    } else {
        // 如果无法解析，返回原始语法
        return syntax;
    }
}

// ============ 全局便捷函数实现 ============

CHTLJSFunction CreateCHTLJSFunction(const std::string& name, const std::vector<std::string>& keyNames) {
    CHTLJSFunction func(name);
    
    // 自动添加参数
    for (const auto& key : keyNames) {
        func.AddParam(key);
    }
    
    // 自动生成函数体模板
    if (!keyNames.empty()) {
        std::ostringstream body;
        body << "// 处理参数: ";
        for (size_t i = 0; i < keyNames.size(); ++i) {
            if (i > 0) body << ", ";
            body << keyNames[i];
        }
        body << "\n    return {}; // TODO: 实现逻辑";
        func.SetBodyTemplate(body.str());
    }
    
    // 默认支持vir
    func.EnableVirSupport(true);
    
    return func;
}

std::string QuickProcess(const std::string& pattern, const std::unordered_map<std::string, ArgValue>& args) {
    Syntax syntax;
    
    if (syntax.AnalyzePattern(pattern)) {
        // 设置参数
        for (const auto& arg : args) {
            syntax.SetArg(arg.first, arg.second);
        }
        
        return syntax.GenerateCode();
    }
    
    return pattern; // 如果无法处理，返回原始模式
}

void VirFunctionBind(const std::string& functionName) {
    // 这里可以添加到全局的vir支持函数列表
    // 简化实现：输出调试信息
    std::cout << "[CJMOD] 绑定函数到vir支持: " << functionName << std::endl;
}

void SetGlobalScanStrategy(ScanStrategy strategy) {
    globalScanStrategy = strategy;
    std::cout << "[CJMOD] 设置全局扫描策略: " << 
        (strategy == ScanStrategy::SLIDING_WINDOW ? "SLIDING_WINDOW" : "FRONT_EXTRACT") << std::endl;
}

} // namespace CJMOD
} // namespace CHTL