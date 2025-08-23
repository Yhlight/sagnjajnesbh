#include "CJMOD/RestructuredCJMODApi.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// 全局扫描策略
static ScanStrategy globalScanStrategy = ScanStrategy::SLIDING_WINDOW;

// ============ Arg类实现 ============

Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder), hasBind_(false) {}

std::string Arg::operator()(const std::string& value) {
    if (hasBind_ && bindFunction_) {
        return bindFunction_(value);
    }
    return value;
}

void Arg::match(const std::string& value) {
    rawValue_ = value;
    if (hasBind_ && bindFunction_) {
        transformedValue_ = bindFunction_(value);
    } else {
        transformedValue_ = value;
    }
}

const std::string& Arg::transform(const std::string& jsTemplate) {
    transformedValue_ = jsTemplate;
    return transformedValue_;
}

// ============ Syntax类实现 ============

Syntax::Syntax() 
    : placeholderCounter_(0), scanStrategy_(globalScanStrategy), 
      currentPosition_(0), currentPolicy_(Policy::NORMAL) {}

Syntax::~Syntax() = default;

bool Syntax::analyzePattern(const std::string& pattern, const std::string& ignoreChars) {
    // 清空现有参数
    args_.clear();
    nameToIndex_.clear();
    placeholderCounter_ = 0;
    
    // 简单的模式分析：提取参数名和占位符
    std::regex paramRegex(R"(\b(\w+)\b|\$)");
    std::sregex_iterator iter(pattern.begin(), pattern.end(), paramRegex);
    std::sregex_iterator end;
    
    size_t index = 0;
    for (; iter != end; ++iter) {
        std::string match = iter->str();
        
        // 跳过忽略字符
        if (ignoreChars.find(match) != std::string::npos) {
            continue;
        }
        
        // 跳过常见的关键字
        if (match == "function" || match == "const" || match == "let" || match == "var") {
            continue;
        }
        
        bool isPlaceholder = (match == "$");
        addArg(match, isPlaceholder);
        index++;
    }
    
    return true;
}

Arg& Syntax::operator[](size_t index) {
    if (index >= args_.size()) {
        throw std::out_of_range("参数索引超出范围");
    }
    return args_[index];
}

const Arg& Syntax::operator[](size_t index) const {
    if (index >= args_.size()) {
        throw std::out_of_range("参数索引超出范围");
    }
    return args_[index];
}

void Syntax::match(const std::string& name, const std::string& value) {
    if (name == "$") {
        // 占位符匹配：智能找到下一个未匹配的占位符
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && arg.getValue().empty()) {
                arg.match(value);
                break;
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

std::string Syntax::scanAndProcess(const std::string& source) {
    switch (scanStrategy_) {
        case ScanStrategy::SLIDING_WINDOW:
            return slidingWindowScan(source);
        case ScanStrategy::FRONT_EXTRACT:
            return frontExtractScan(source);
        default:
            return source;
    }
}

void Syntax::scanKeyword(const std::string& keyword, std::function<void()> handler) {
    keywordHandlers_[keyword] = handler;
}

std::string Syntax::peekKeyword(int offset) const {
    if (currentTokens_.empty()) {
        return "";
    }
    
    int targetPos = static_cast<int>(currentPosition_) + offset;
    if (targetPos < 0 || targetPos >= static_cast<int>(currentTokens_.size())) {
        return "";
    }
    
    return currentTokens_[targetPos];
}

void Syntax::policyChangeBegin(const std::string& trigger, Policy policy) {
    PolicyState state;
    state.trigger = trigger;
    state.policy = currentPolicy_;
    state.buffer = collectBuffer_;
    
    policyStack_.push_back(state);
    currentPolicy_ = policy;
    
    if (policy == Policy::COLLECT) {
        collectBuffer_.clear();
    }
}

std::string Syntax::policyChangeEnd(const std::string& trigger, Policy policy) {
    std::string result = collectBuffer_;
    
    if (!policyStack_.empty()) {
        PolicyState prevState = policyStack_.back();
        policyStack_.pop_back();
        
        currentPolicy_ = prevState.policy;
        collectBuffer_ = prevState.buffer;
    } else {
        currentPolicy_ = policy;
        collectBuffer_.clear();
    }
    
    return result;
}

std::string Syntax::generateCode() const {
    return result();
}

std::string Syntax::result() const {
    std::ostringstream output;
    
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) output << ", ";
        output << args_[i].getValue();
    }
    
    return output.str();
}

void Syntax::addArg(const std::string& name, bool isPlaceholder) {
    args_.emplace_back(name, isPlaceholder);
    nameToIndex_[name] = args_.size() - 1;
}

Arg* Syntax::findArg(const std::string& name) {
    auto it = nameToIndex_.find(name);
    if (it != nameToIndex_.end()) {
        return &args_[it->second];
    }
    return nullptr;
}

// ======== 正确的滑动窗口双指针扫描实现 ========
std::string Syntax::slidingWindowScan(const std::string& source) {
    resetScannerState();
    
    std::ostringstream result;
    size_t frontPtr = 0;  // 前指针：寻找CHTL JS语法
    size_t backPtr = 0;   // 后指针：标记已处理位置
    bool collectMode = false;
    std::string collectBuffer;
    
    std::cout << "[滑动窗口扫描] 开始扫描，源代码长度: " << source.length() << std::endl;
    
    // 主扫描循环：双指针协同向前移动
    while (frontPtr < source.length()) {
        // 前指针检测CHTL JS语法
        if (detectCHTLJSSyntaxAtPos(source, frontPtr)) {
            std::cout << "  [前指针] 在位置 " << frontPtr << " 检测到CHTL JS语法" << std::endl;
            
            if (!collectMode) {
                // 输出前面的普通代码到结果
                if (backPtr < frontPtr) {
                    std::string normalCode = source.substr(backPtr, frontPtr - backPtr);
                    result << normalCode;
                    std::cout << "    [输出] 普通代码: " << normalCode.substr(0, 50) << "..." << std::endl;
                }
                
                // 进入收集模式
                collectMode = true;
                collectBuffer.clear();
                std::cout << "    [状态] 进入收集模式" << std::endl;
            }
            
            // 收集CHTL JS语法片段
            size_t syntaxEnd = findSyntaxEnd(source, frontPtr);
            std::string syntaxFragment = source.substr(frontPtr, syntaxEnd - frontPtr);
            collectBuffer += syntaxFragment;
            std::cout << "    [收集] 语法片段: " << syntaxFragment << std::endl;
            
            // 检查语法是否完整
            if (isSyntaxComplete(collectBuffer)) {
                std::cout << "    [处理] 语法完整，开始处理" << std::endl;
                
                // 处理完整的CHTL JS语法
                std::string processedCode = processCHTLJSSyntax(collectBuffer);
                result << processedCode;
                
                // 退出收集模式，双指针同步
                collectMode = false;
                frontPtr = syntaxEnd;
                backPtr = syntaxEnd;
                collectBuffer.clear();
                
                std::cout << "    [状态] 退出收集模式，指针同步到位置: " << backPtr << std::endl;
                continue;
            } else {
                // 语法不完整，继续收集
                frontPtr = syntaxEnd;
                std::cout << "    [状态] 语法不完整，继续收集" << std::endl;
                continue;
            }
        }
        
        // 前指针向前移动
        frontPtr++;
        
        // 如果不在收集模式，后指针跟随前指针
        if (!collectMode) {
            backPtr = frontPtr;
        }
    }
    
    // 处理剩余内容
    if (backPtr < source.length()) {
        if (collectMode && !collectBuffer.empty()) {
            std::cout << "  [结束] 处理剩余的CHTL JS语法" << std::endl;
            result << processCHTLJSSyntax(collectBuffer);
        } else {
            std::cout << "  [结束] 输出剩余的普通代码" << std::endl;
            result << source.substr(backPtr);
        }
    }
    
    std::cout << "[滑动窗口扫描] 扫描完成" << std::endl;
    return result.str();
}

// ======== 前置代码截取扫描实现（不发送给编译器）========
std::string Syntax::frontExtractScan(const std::string& source) {
    std::ostringstream result;
    size_t pos = 0;
    
    std::cout << "[前置截取扫描] 开始扫描，源代码长度: " << source.length() << std::endl;
    
    while (pos < source.length()) {
        // 查找下一个CHTL JS语法位置
        size_t syntaxPos = findNextCHTLJSSyntax(source, pos);
        
        if (syntaxPos != std::string::npos) {
            // *** 关键：前置代码不发送给编译器 ***
            if (syntaxPos > pos) {
                std::string frontSegment = source.substr(pos, syntaxPos - pos);
                std::cout << "  [截取] 前置代码（不发送给编译器）: " 
                         << frontSegment.substr(0, 50) << "..." << std::endl;
                // 注意：这里不添加到result中！
                // 前置代码被截取但不输出到最终结果
            }
            
            // 只处理CHTL JS语法
            size_t syntaxEnd = findSyntaxEnd(source, syntaxPos);
            std::string syntaxFragment = source.substr(syntaxPos, syntaxEnd - syntaxPos);
            std::string processedCode = processCHTLJSSyntax(syntaxFragment);
            result << processedCode;
            
            std::cout << "  [处理] CHTL JS语法: " << syntaxFragment << std::endl;
            std::cout << "  [输出] 处理结果: " << processedCode << std::endl;
            
            pos = syntaxEnd;
        } else {
            // 没有更多CHTL JS语法，剩余代码也不发送给编译器
            std::string remainingCode = source.substr(pos);
            std::cout << "  [截取] 剩余代码（不发送给编译器）: " 
                     << remainingCode.substr(0, 50) << "..." << std::endl;
            // 注意：这里也不添加到result中！
            break;
        }
    }
    
    std::cout << "[前置截取扫描] 扫描完成，只输出处理后的CHTL JS代码" << std::endl;
    return result.str();
}

// ======== 扫描辅助方法实现 ========

bool Syntax::detectCHTLJSSyntaxAtPos(const std::string& source, size_t pos) const {
    if (pos >= source.length()) return false;
    
    // 检测常见的CHTL JS语法模式
    std::vector<std::string> patterns = {
        "printMylove", "iNeverAway", "listen", "animate", "vir "
    };
    
    for (const auto& pattern : patterns) {
        if (pos + pattern.length() <= source.length()) {
            if (source.substr(pos, pattern.length()) == pattern) {
                return true;
            }
        }
    }
    
    return false;
}

size_t Syntax::findSyntaxEnd(const std::string& source, size_t start) const {
    size_t pos = start;
    int braceCount = 0;
    bool inString = false;
    char stringChar = 0;
    
    while (pos < source.length()) {
        char ch = source[pos];
        
        if (!inString) {
            if (ch == '"' || ch == '\'') {
                inString = true;
                stringChar = ch;
            } else if (ch == '{') {
                braceCount++;
            } else if (ch == '}') {
                braceCount--;
                if (braceCount <= 0) {
                    return pos + 1;
                }
            } else if (ch == ';' && braceCount == 0) {
                return pos + 1;
            }
        } else {
            if (ch == stringChar && (pos == 0 || source[pos-1] != '\\')) {
                inString = false;
            }
        }
        
        pos++;
    }
    
    return pos;
}

std::string Syntax::processCHTLJSSyntax(const std::string& syntax) {
    // 简化的CHTL JS语法处理
    // 在实际实现中，这里会调用CJMOD扩展进行处理
    
    std::cout << "    [CJMOD] 处理语法: " << syntax << std::endl;
    
    // 触发关键字处理器
    for (const auto& [keyword, handler] : keywordHandlers_) {
        if (syntax.find(keyword) != std::string::npos) {
            std::cout << "    [CJMOD] 触发关键字处理器: " << keyword << std::endl;
            handler();
            break;
        }
    }
    
    // 返回处理后的JavaScript代码
    return "/* Processed: " + syntax + " */\n";
}

bool Syntax::isSyntaxComplete(const std::string& syntax) const {
    // 简单的完整性检查：括号匹配
    int braceCount = 0;
    bool inString = false;
    char stringChar = 0;
    
    for (size_t i = 0; i < syntax.length(); ++i) {
        char ch = syntax[i];
        
        if (!inString) {
            if (ch == '"' || ch == '\'') {
                inString = true;
                stringChar = ch;
            } else if (ch == '{') {
                braceCount++;
            } else if (ch == '}') {
                braceCount--;
            }
        } else {
            if (ch == stringChar && (i == 0 || syntax[i-1] != '\\')) {
                inString = false;
            }
        }
    }
    
    return braceCount == 0 && !inString;
}

void Syntax::resetScannerState() {
    currentTokens_.clear();
    currentPosition_ = 0;
    currentPolicy_ = Policy::NORMAL;
    collectBuffer_.clear();
    policyStack_.clear();
}

size_t Syntax::findNextCHTLJSSyntax(const std::string& source, size_t start) const {
    for (size_t pos = start; pos < source.length(); ++pos) {
        if (detectCHTLJSSyntaxAtPos(source, pos)) {
            return pos;
        }
    }
    return std::string::npos;
}

void Syntax::processCollectPolicy(const std::string& content) {
    collectBuffer_ += content;
}

void Syntax::processSkipPolicy() {
    // 跳过当前内容
}

bool Syntax::matchesTrigger(const std::string& content, const std::string& trigger) const {
    return content.find(trigger) != std::string::npos;
}

// ============ CHTLJSFunction类实现 ============

CHTLJSFunction::CHTLJSFunction(const std::string& name)
    : name_(name), virSupported_(false), boundSyntax_(nullptr) {}

void CHTLJSFunction::addParam(const std::string& name, const std::string& defaultValue) {
    params_.emplace_back(name, defaultValue);
    keyNames_.push_back(name);
}

void CHTLJSFunction::setBodyTemplate(const std::string& bodyTemplate) {
    bodyTemplate_ = bodyTemplate;
}

bool CHTLJSFunction::bindToSyntax(Syntax& syntax) {
    boundSyntax_ = &syntax;
    
    // 将函数的键名添加到语法对象
    for (const auto& keyName : keyNames_) {
        syntax.addArg(keyName, false);
    }
    
    return true;
}

std::string CHTLJSFunction::generateJS() const {
    std::ostringstream js;
    
    // 生成函数签名
    js << "function " << name_ << "(";
    
    // 添加参数
    if (!params_.empty()) {
        js << "config = {}";
    }
    
    js << ") {\n";
    
    // 添加函数体
    if (!bodyTemplate_.empty()) {
        js << "    " << bodyTemplate_ << "\n";
    } else {
        js << "    // TODO: 实现函数体\n";
        js << "    console.log('" << name_ << " called with:', config);\n";
    }
    
    js << "}";
    
    return js.str();
}

std::string CHTLJSFunction::generateVirAccess(const std::string& objectName, const std::string& keyAccess) const {
    if (!virSupported_) {
        return "";
    }
    
    std::string globalFunctionName = generateGlobalFunctionName(keyAccess);
    
    std::ostringstream code;
    code << "function " << globalFunctionName << "() {\n";
    code << "    return " << objectName << "_result." << keyAccess << "();\n";
    code << "}\n";
    
    return code.str();
}

std::string CHTLJSFunction::generateGlobalFunctionName(const std::string& keyName) const {
    return "chtl_vir_" + name_ + "_" + keyName;
}

std::string CHTLJSFunction::generateVirFunctionMapping() const {
    std::ostringstream mapping;
    
    for (const auto& keyName : keyNames_) {
        mapping << "// " << name_ << "." << keyName << " -> " << generateGlobalFunctionName(keyName) << "\n";
    }
    
    return mapping.str();
}

// ============ 全局函数实现 ============

std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    syntax->analyzePattern(pattern, ignoreChars);
    return syntax;
}

std::string generateCode(const Syntax& syntax) {
    return syntax.generateCode();
}

std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(const std::string& functionName, 
                                                   const std::vector<std::string>& keyNames) {
    auto func = std::make_unique<CHTLJSFunction>(functionName);
    
    // 添加键名作为参数
    for (const auto& keyName : keyNames) {
        func->addParam(keyName, "undefined");
    }
    
    // 默认启用虚对象支持
    func->enableVirSupport(true);
    
    return func;
}

void setGlobalScanStrategy(ScanStrategy strategy) {
    globalScanStrategy = strategy;
    std::cout << "[CJMOD] 设置全局扫描策略: " << 
        (strategy == ScanStrategy::SLIDING_WINDOW ? "滑动窗口双指针" : "前置代码截取") << std::endl;
}

ScanStrategy getGlobalScanStrategy() {
    return globalScanStrategy;
}

} // namespace CJMOD
} // namespace CHTL