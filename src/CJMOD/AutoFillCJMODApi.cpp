#include "CJMOD/AutoFillCJMODApi.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>
#include <iomanip>

namespace CHTL {
namespace CJMOD {

// ============ Arg类实现 - 增强自动填充 ============

Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder), hasBind_(false), expectedType_(ParamType::UNKNOWN) {
}

std::string Arg::operator()(const std::string& peekedValue) {
    std::cout << "处理peekKeyword结果: " << peekedValue << " (参数: " << name_ << ")" << std::endl;
    
    if (hasBind_ && valueProcessor_) {
        std::string processed = valueProcessor_(peekedValue);
        processedValue_ = processed;
        
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
    
    if (!jsTemplate_.empty()) {
        applyJSTransform();
    }
}

void Arg::setAutoFillCandidate(const ParamCandidate& candidate) {
    autoFillCandidate_ = candidate;
    std::cout << "设置自动填充候选项: " << candidate.value 
              << " (置信度: " << std::fixed << std::setprecision(2) << candidate.confidence << ")" << std::endl;
}

bool Arg::acceptAutoFill(double minConfidence) {
    if (autoFillCandidate_.confidence >= minConfidence) {
        match(autoFillCandidate_.value);
        std::cout << "✅ 接受自动填充: " << autoFillCandidate_.value << std::endl;
        return true;
    } else {
        std::cout << "❌ 拒绝自动填充: 置信度不足 (" << autoFillCandidate_.confidence 
                  << " < " << minConfidence << ")" << std::endl;
        return false;
    }
}

void Arg::applyJSTransform() {
    if (jsTemplate_.empty()) {
        jsCode_ = processedValue_;
        return;
    }
    
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

// ============ Args类实现 - 自动填充增强 ============

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

Args& Args::transform(const std::string& name, const std::string& jsTemplate) {
    if (name == "$") {
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && !arg.hasTransform()) {
                arg.transform(jsTemplate);
                break;
            }
        }
    } else {
        Arg* arg = findArg(name);
        if (arg) {
            arg->transform(jsTemplate);
        }
    }
    return *this;
}

Args& Args::match(const std::string& name, const std::string& value) {
    if (name == "$") {
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && arg.getValue().empty()) {
                arg.match(value);
                return *this;
            }
        }
    } else {
        Arg* arg = findArg(name);
        if (arg) {
            arg.match(value);
        }
    }
    return *this;
}

Args& Args::expectTypes(const std::vector<ParamType>& types) {
    std::cout << "设置期望参数类型，数量: " << types.size() << std::endl;
    
    for (size_t i = 0; i < types.size() && i < args_.size(); ++i) {
        args_[i].setExpectedType(types[i]);
        std::cout << "  参数[" << i << "] 期望类型: " << static_cast<int>(types[i]) << std::endl;
    }
    return *this;
}

Args& Args::setPatterns(const std::vector<std::string>& patterns) {
    std::cout << "设置参数模式，数量: " << patterns.size() << std::endl;
    
    for (size_t i = 0; i < patterns.size() && i < args_.size(); ++i) {
        args_[i].setPattern(patterns[i]);
        std::cout << "  参数[" << i << "] 模式: " << patterns[i] << std::endl;
    }
    return *this;
}

AutoFillResult Args::autoFill(const std::vector<std::string>& sourceTokens, FillStrategy strategy) {
    AutoFillResult result;
    std::cout << "\n🚀 开始自动填充，策略: " << static_cast<int>(strategy) << std::endl;
    std::cout << "源token数量: " << sourceTokens.size() << ", 参数数量: " << args_.size() << std::endl;
    
    // 创建自动填充扫描器进行填充
    auto scanner = createAutoFillScanner();
    scanner->setTokens(sourceTokens);
    scanner->setFillStrategy(strategy);
    
    result = scanner->performAutoFill(*this);
    
    if (result.success) {
        std::cout << "✅ 自动填充成功，总体置信度: " << result.overallConfidence << std::endl;
        for (size_t i = 0; i < result.filledValues.size(); ++i) {
            std::cout << "  填充值[" << i << "]: " << result.filledValues[i] << std::endl;
        }
    } else {
        std::cout << "❌ 自动填充失败: " << result.errorMessage << std::endl;
    }
    
    return result;
}

Args& Args::acceptAutoFill(double minConfidence) {
    std::cout << "接受自动填充，最低置信度: " << minConfidence << std::endl;
    
    for (auto& arg : args_) {
        if (arg.hasAutoFillCandidate()) {
            arg.acceptAutoFill(minConfidence);
        }
    }
    return *this;
}

std::string Args::result() const {
    std::string result;
    for (const auto& arg : args_) {
        if (!result.empty()) result += " ";
        
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

// ============ 双指针扫描器实现 - 深度版本 ============

DualPointerScanner::DualPointerScanner() 
    : frontPointer_(0), backPointer_(0), initialScanRange_(10) {
}

void DualPointerScanner::setTokens(const std::vector<std::string>& tokens) {
    tokens_ = tokens;
    frontPointer_ = 0;
    backPointer_ = 0;
    std::cout << "双指针扫描器设置token，数量: " << tokens.size() << std::endl;
}

void DualPointerScanner::setKeywords(const std::vector<std::string>& keywords) {
    keywords_ = keywords;
    std::cout << "双指针扫描器设置关键字，数量: " << keywords.size() << std::endl;
}

ScanPosition DualPointerScanner::scanForKeyword(const std::string& keyword) {
    ScanPosition pos;
    std::cout << "\n🔍 双指针扫描关键字: " << keyword << std::endl;
    
    // 第一阶段：初始范围扫描，确定不存在语法片段
    std::cout << "阶段1: 初始范围扫描 (0 到 " << initialScanRange_ << ")" << std::endl;
    
    size_t scanEnd = std::min(initialScanRange_, tokens_.size());
    bool foundInInitial = false;
    
    for (size_t i = 0; i < scanEnd; ++i) {
        if (tokens_[i] == keyword) {
            foundInInitial = true;
            pos.frontPointer = i;
            pos.backPointer = 0;
            pos.hasKeyword = true;
            pos.detectedKeyword = keyword;
            std::cout << "  在初始范围找到关键字，位置: " << i << std::endl;
            break;
        }
    }
    
    if (!foundInInitial) {
        // 第二阶段：前指针移动到合适位置，同步扫描
        std::cout << "阶段2: 同步扫描模式" << std::endl;
        frontPointer_ = initialScanRange_;
        backPointer_ = initialScanRange_;
        
        while (frontPointer_ < tokens_.size()) {
            if (tokens_[frontPointer_] == keyword) {
                std::cout << "  前指针检测到关键字: " << keyword << " (位置: " << frontPointer_ << ")" << std::endl;
                
                // 通知后指针进行收集
                pos.frontPointer = frontPointer_;
                pos.backPointer = backPointer_;
                pos.hasKeyword = true;
                pos.detectedKeyword = keyword;
                
                collectFragment(pos);
                break;
            }
            
            synchronizedMove();
        }
    }
    
    return pos;
}

std::vector<ScanPosition> DualPointerScanner::scanAllKeywords() {
    std::vector<ScanPosition> positions;
    std::cout << "\n🔍 双指针扫描所有关键字" << std::endl;
    
    for (const auto& keyword : keywords_) {
        reset();
        ScanPosition pos = scanForKeyword(keyword);
        if (pos.hasKeyword) {
            positions.push_back(pos);
        }
    }
    
    std::cout << "扫描完成，找到 " << positions.size() << " 个关键字位置" << std::endl;
    return positions;
}

std::vector<std::string> DualPointerScanner::extractBetweenPointers(const ScanPosition& pos) {
    std::vector<std::string> extracted;
    
    std::cout << "提取指针间内容: [" << pos.backPointer << ", " << pos.frontPointer << ")" << std::endl;
    
    for (size_t i = pos.backPointer; i < pos.frontPointer && i < tokens_.size(); ++i) {
        extracted.push_back(tokens_[i]);
        std::cout << "  提取token[" << i << "]: " << tokens_[i] << std::endl;
    }
    
    return extracted;
}

std::string DualPointerScanner::extractRange(size_t start, size_t end) {
    std::string result;
    
    for (size_t i = start; i < end && i < tokens_.size(); ++i) {
        if (!result.empty()) result += " ";
        result += tokens_[i];
    }
    
    std::cout << "提取范围 [" << start << ", " << end << "): " << result << std::endl;
    return result;
}

void DualPointerScanner::reset() {
    frontPointer_ = 0;
    backPointer_ = 0;
}

bool DualPointerScanner::isKeyword(const std::string& token) {
    return std::find(keywords_.begin(), keywords_.end(), token) != keywords_.end();
}

void DualPointerScanner::synchronizedMove() {
    frontPointer_++;
    backPointer_++;
    
    if (frontPointer_ % 10 == 0) {  // 每10步输出一次状态
        std::cout << "  同步移动: 前指针=" << frontPointer_ << ", 后指针=" << backPointer_ << std::endl;
    }
}

void DualPointerScanner::collectFragment(const ScanPosition& pos) {
    std::cout << "收集代码片段: ";
    for (size_t i = pos.backPointer; i <= pos.frontPointer && i < tokens_.size(); ++i) {
        std::cout << tokens_[i] << " ";
    }
    std::cout << std::endl;
}

// ============ 前置截取机制实现 - 深度版本 ============

FrontExtractMechanism::FrontExtractMechanism() {
    initializeDefaultPatterns();
}

void FrontExtractMechanism::setExtractPatterns(const std::vector<std::string>& patterns) {
    extractPatterns_ = patterns;
    std::cout << "前置截取设置模式，数量: " << patterns.size() << std::endl;
}

std::vector<std::string> FrontExtractMechanism::extractFrontParameters(const std::string& fragment, 
                                                                       const std::string& keyword) {
    std::vector<std::string> extracted;
    std::cout << "\n✂️ 前置截取参数，关键字: " << keyword << std::endl;
    std::cout << "源片段: " << fragment << std::endl;
    
    // 检查是否需要前置截取
    if (!needsFrontExtraction(keyword)) {
        std::cout << "  不需要前置截取" << std::endl;
        return extracted;
    }
    
    // 查找关键字前的参数
    size_t keywordPos = fragment.find(keyword);
    if (keywordPos == std::string::npos) {
        std::cout << "  未找到关键字" << std::endl;
        return extracted;
    }
    
    std::string frontPart = fragment.substr(0, keywordPos);
    std::cout << "  前置部分: " << frontPart << std::endl;
    
    // 使用正则表达式提取参数
    std::regex paramRegex(R"(\b(\w+)\s*(?=\s*\*\*|\s*\+\+|\s*--|\s*[=<>!]+))");
    std::sregex_iterator iter(frontPart.begin(), frontPart.end(), paramRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string param = iter->str(1);
        extracted.push_back(param);
        std::cout << "  截取参数: " << param << std::endl;
    }
    
    return extracted;
}

std::vector<std::string> FrontExtractMechanism::extractByPattern(const std::string& fragment, 
                                                                const std::string& pattern) {
    std::vector<std::string> extracted;
    std::cout << "按模式截取: " << pattern << std::endl;
    
    try {
        std::regex patternRegex(pattern);
        std::sregex_iterator iter(fragment.begin(), fragment.end(), patternRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            if (iter->size() > 1) {  // 有捕获组
                extracted.push_back(iter->str(1));
                std::cout << "  提取: " << iter->str(1) << std::endl;
            } else {
                extracted.push_back(iter->str(0));
                std::cout << "  提取: " << iter->str(0) << std::endl;
            }
        }
    } catch (const std::regex_error& e) {
        std::cout << "  正则表达式错误: " << e.what() << std::endl;
    }
    
    return extracted;
}

bool FrontExtractMechanism::needsFrontExtraction(const std::string& keyword) {
    // 检查关键字是否需要前置截取
    auto it = keywordPatterns_.find(keyword);
    if (it != keywordPatterns_.end()) {
        std::cout << "关键字 " << keyword << " 需要前置截取" << std::endl;
        return true;
    }
    
    // 检查是否包含需要前置截取的操作符
    std::vector<std::string> needExtractOps = {"**", "++", "--", "+=", "-=", "*=", "/="};
    for (const auto& op : needExtractOps) {
        if (keyword.find(op) != std::string::npos) {
            std::cout << "关键字 " << keyword << " 包含操作符 " << op << "，需要前置截取" << std::endl;
            return true;
        }
    }
    
    return false;
}

void FrontExtractMechanism::initializeDefaultPatterns() {
    // 为常见关键字设置前置截取模式
    keywordPatterns_["**"] = R"((\w+)\s*\*\*)";          // arg ** arg 模式
    keywordPatterns_["++"] = R"((\w+)\s*\+\+)";          // arg++ 模式
    keywordPatterns_["--"] = R"((\w+)\s*--)";            // arg-- 模式
    keywordPatterns_["+="] = R"((\w+)\s*\+=)";           // arg += 模式
    keywordPatterns_["-="] = R"((\w+)\s*-=)";            // arg -= 模式
    keywordPatterns_["*="] = R"((\w+)\s*\*=)";           // arg *= 模式
    keywordPatterns_["/="] = R"((\w+)\s*/=)";            // arg /= 模式
    
    std::cout << "初始化默认前置截取模式，数量: " << keywordPatterns_.size() << std::endl;
}

// ============ 参数类型检测器实现 ============

ParamType ParamTypeDetector::detectType(const std::string& value) {
    if (value.empty()) return ParamType::UNKNOWN;
    
    if (isNumber(value)) return ParamType::NUMBER;
    if (isURL(value)) return ParamType::URL;
    if (isColor(value)) return ParamType::COLOR;
    if (isSize(value)) return ParamType::SIZE;
    if (isObject(value)) return ParamType::OBJECT;
    if (isFunction(value)) return ParamType::FUNCTION;
    if (isArray(value)) return ParamType::ARRAY;
    
    if (value == "true" || value == "false") return ParamType::BOOLEAN;
    
    return ParamType::STRING;  // 默认为字符串
}

double ParamTypeDetector::calculateTypeConfidence(const std::string& value, ParamType expectedType) {
    ParamType detectedType = detectType(value);
    
    if (detectedType == expectedType) {
        return 1.0;  // 完全匹配
    }
    
    // 某些类型间的兼容性
    if (expectedType == ParamType::STRING) {
        return 0.8;  // 字符串可以接受任何类型
    }
    
    if (expectedType == ParamType::NUMBER && detectedType == ParamType::SIZE) {
        return 0.9;  // 尺寸通常也是数字
    }
    
    if (expectedType == ParamType::URL && detectedType == ParamType::STRING) {
        if (value.find("http") == 0 || value.find("www") == 0 || value.find("/") == 0) {
            return 0.7;  // 可能是URL
        }
    }
    
    return 0.3;  // 类型不匹配，但仍有可能
}

bool ParamTypeDetector::isValidForType(const std::string& value, ParamType type) {
    return calculateTypeConfidence(value, type) >= 0.5;
}

bool ParamTypeDetector::isNumber(const std::string& value) {
    if (value.empty()) return false;
    
    try {
        std::stod(value);
        return true;
    } catch (...) {
        return false;
    }
}

bool ParamTypeDetector::isURL(const std::string& value) {
    if (value.length() < 3) return false;
    
    return value.find("http://") == 0 || 
           value.find("https://") == 0 || 
           value.find("ftp://") == 0 ||
           value.find("www.") == 0 ||
           (value.find(".") != std::string::npos && value.find(" ") == std::string::npos);
}

bool ParamTypeDetector::isColor(const std::string& value) {
    if (value.empty()) return false;
    
    // 十六进制颜色
    if (value[0] == '#' && (value.length() == 4 || value.length() == 7)) {
        return std::all_of(value.begin() + 1, value.end(), [](char c) {
            return std::isxdigit(c);
        });
    }
    
    // RGB/RGBA
    if (value.find("rgb") == 0) return true;
    
    // 颜色名称
    std::vector<std::string> colorNames = {
        "red", "green", "blue", "white", "black", "yellow", "purple", "orange", "pink", "gray"
    };
    return std::find(colorNames.begin(), colorNames.end(), value) != colorNames.end();
}

bool ParamTypeDetector::isSize(const std::string& value) {
    if (value.empty()) return false;
    
    // 检查是否以数字开头，以单位结尾
    std::regex sizeRegex(R"(^\d+(\.\d+)?(px|em|rem|%|vh|vw|pt|pc|in|cm|mm)$)");
    return std::regex_match(value, sizeRegex);
}

bool ParamTypeDetector::isObject(const std::string& value) {
    if (value.length() < 2) return false;
    
    std::string trimmed = value;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return trimmed.front() == '{' && trimmed.back() == '}';
}

bool ParamTypeDetector::isFunction(const std::string& value) {
    return value.find("function") != std::string::npos ||
           value.find("=>") != std::string::npos ||
           value.find("()") != std::string::npos;
}

bool ParamTypeDetector::isArray(const std::string& value) {
    if (value.length() < 2) return false;
    
    std::string trimmed = value;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return trimmed.front() == '[' && trimmed.back() == ']';
}

// ============ 智能参数匹配器实现 ============

SmartParameterMatcher::SmartParameterMatcher() : strategy_(FillStrategy::AUTO) {
    initializeFunctionHints();
}

std::vector<ParamCandidate> SmartParameterMatcher::findCandidates(
    const std::vector<std::string>& tokens,
    const std::vector<ParamType>& expectedTypes,
    const std::vector<std::string>& patterns) {
    
    std::vector<ParamCandidate> candidates;
    std::cout << "\n🎯 智能参数匹配，token数量: " << tokens.size() << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& token = tokens[i];
        
        for (size_t typeIdx = 0; typeIdx < expectedTypes.size(); ++typeIdx) {
            ParamType expectedType = expectedTypes[typeIdx];
            
            double confidence = ParamTypeDetector::calculateTypeConfidence(token, expectedType);
            
            if (confidence > 0.5) {  // 只考虑置信度超过0.5的候选项
                ParamCandidate candidate(token, ParamTypeDetector::detectType(token), 
                                       confidence, i, "");
                
                // 如果有模式，进一步验证
                if (typeIdx < patterns.size() && !patterns[typeIdx].empty()) {
                    try {
                        std::regex patternRegex(patterns[typeIdx]);
                        if (std::regex_match(token, patternRegex)) {
                            candidate.confidence += 0.2;  // 模式匹配加分
                        }
                    } catch (...) {
                        // 忽略正则表达式错误
                    }
                }
                
                candidates.push_back(candidate);
                std::cout << "  候选项: " << token << " (类型: " << static_cast<int>(expectedType) 
                          << ", 置信度: " << std::fixed << std::setprecision(2) << confidence << ")" << std::endl;
            }
        }
    }
    
    // 按置信度排序
    std::sort(candidates.begin(), candidates.end(), 
              [](const ParamCandidate& a, const ParamCandidate& b) {
                  return a.confidence > b.confidence;
              });
    
    std::cout << "找到 " << candidates.size() << " 个候选项" << std::endl;
    return candidates;
}

std::vector<ParamCandidate> SmartParameterMatcher::contextAwareMatch(
    const std::vector<std::string>& tokens,
    const std::string& functionName,
    const std::vector<std::string>& paramNames) {
    
    std::vector<ParamCandidate> candidates;
    std::cout << "\n🧠 上下文感知匹配，函数: " << functionName << std::endl;
    
    // 从函数提示中获取期望类型
    auto it = functionTypeHints_.find(functionName);
    std::vector<ParamType> expectedTypes;
    
    if (it != functionTypeHints_.end()) {
        expectedTypes = it->second;
        std::cout << "使用函数类型提示，参数类型数量: " << expectedTypes.size() << std::endl;
    } else {
        // 根据参数名推断类型
        for (const auto& paramName : paramNames) {
            if (paramName.find("url") != std::string::npos || 
                paramName.find("src") != std::string::npos) {
                expectedTypes.push_back(ParamType::URL);
            } else if (paramName.find("width") != std::string::npos || 
                      paramName.find("height") != std::string::npos ||
                      paramName.find("size") != std::string::npos) {
                expectedTypes.push_back(ParamType::SIZE);
            } else if (paramName.find("color") != std::string::npos) {
                expectedTypes.push_back(ParamType::COLOR);
            } else if (paramName.find("count") != std::string::npos ||
                      paramName.find("index") != std::string::npos) {
                expectedTypes.push_back(ParamType::NUMBER);
            } else {
                expectedTypes.push_back(ParamType::STRING);
            }
        }
        std::cout << "推断参数类型，数量: " << expectedTypes.size() << std::endl;
    }
    
    candidates = findCandidates(tokens, expectedTypes, {});
    
    // 应用上下文置信度调整
    for (auto& candidate : candidates) {
        double contextBonus = calculateContextConfidence(candidate, functionName);
        candidate.confidence = std::min(1.0, candidate.confidence + contextBonus);
    }
    
    return candidates;
}

void SmartParameterMatcher::initializeFunctionHints() {
    // 为常见函数设置类型提示
    functionTypeHints_["printMylove"] = {ParamType::URL, ParamType::STRING, ParamType::SIZE, ParamType::SIZE, ParamType::NUMBER};
    functionTypeHints_["loadImage"] = {ParamType::URL, ParamType::SIZE, ParamType::SIZE};
    functionTypeHints_["setColor"] = {ParamType::COLOR};
    functionTypeHints_["animate"] = {ParamType::STRING, ParamType::NUMBER, ParamType::STRING};
    functionTypeHints_["playAudio"] = {ParamType::URL, ParamType::NUMBER};
    
    std::cout << "初始化函数类型提示，数量: " << functionTypeHints_.size() << std::endl;
}

double SmartParameterMatcher::calculateContextConfidence(const ParamCandidate& candidate, 
                                                        const std::string& context) {
    double bonus = 0.0;
    
    // 根据上下文调整置信度
    if (context == "printMylove") {
        if (candidate.type == ParamType::URL && candidate.value.find(".jpg") != std::string::npos) {
            bonus += 0.2;  // 图片URL加分
        }
        if (candidate.type == ParamType::SIZE && ParamTypeDetector::isNumber(candidate.value)) {
            bonus += 0.15;  // 数字尺寸加分
        }
    }
    
    return bonus;
}

// ============ 自动填充扫描器实现 - 核心类 ============

AutoFillScanner::AutoFillScanner() 
    : strategy_(FillStrategy::AUTO), minConfidence_(0.7), 
      useDualPointer_(true), useFrontExtract_(true) {
    
    dualPointer_ = std::make_unique<DualPointerScanner>();
    frontExtract_ = std::make_unique<FrontExtractMechanism>();
    matcher_ = std::make_unique<SmartParameterMatcher>();
    
    std::cout << "创建自动填充扫描器" << std::endl;
}

void AutoFillScanner::setTokens(const std::vector<std::string>& tokens) {
    tokens_ = tokens;
    if (useDualPointer_) {
        dualPointer_->setTokens(tokens);
    }
    std::cout << "自动填充扫描器设置token，数量: " << tokens.size() << std::endl;
}

void AutoFillScanner::setKeywords(const std::vector<std::string>& keywords) {
    keywords_ = keywords;
    if (useDualPointer_) {
        dualPointer_->setKeywords(keywords);
    }
    std::cout << "自动填充扫描器设置关键字，数量: " << keywords.size() << std::endl;
}

AutoFillResult AutoFillScanner::performAutoFill(Args& args, const std::string& functionName) {
    AutoFillResult result;
    std::cout << "\n🚀 执行自动填充，策略: " << static_cast<int>(strategy_) << std::endl;
    
    switch (strategy_) {
        case FillStrategy::SEQUENTIAL:
            result = performSequentialFill(args);
            break;
        case FillStrategy::PATTERN_MATCH:
            result = performPatternMatchFill(args);
            break;
        case FillStrategy::TYPE_AWARE:
            result = performTypeAwareFill(args);
            break;
        case FillStrategy::CONTEXT_AWARE:
            result = performContextAwareFill(args, functionName);
            break;
        case FillStrategy::AUTO:
        default:
            // 自动策略：先尝试上下文感知，再尝试类型感知
            result = performContextAwareFill(args, functionName);
            if (!result.success || result.overallConfidence < 0.6) {
                std::cout << "上下文感知失败，尝试类型感知策略" << std::endl;
                result = performTypeAwareFill(args);
            }
            break;
    }
    
    if (result.success) {
        optimizeResults(result);
    }
    
    return result;
}

AutoFillResult AutoFillScanner::performSequentialFill(Args& args) {
    AutoFillResult result;
    std::cout << "执行顺序填充策略" << std::endl;
    
    std::vector<std::string> relevantTokens = extractRelevantTokens();
    
    for (size_t i = 0; i < args.length() && i < relevantTokens.size(); ++i) {
        ParamCandidate candidate(relevantTokens[i], ParamTypeDetector::detectType(relevantTokens[i]), 
                               0.8, i, "sequential");
        
        args[i].setAutoFillCandidate(candidate);
        result.filledValues.push_back(relevantTokens[i]);
        result.candidates.push_back(candidate);
        
        std::cout << "顺序填充参数[" << i << "]: " << relevantTokens[i] << std::endl;
    }
    
    result.success = !result.filledValues.empty();
    result.overallConfidence = 0.8;
    
    return result;
}

AutoFillResult AutoFillScanner::performPatternMatchFill(Args& args) {
    AutoFillResult result;
    std::cout << "执行模式匹配填充策略" << std::endl;
    
    for (size_t i = 0; i < args.length(); ++i) {
        const std::string& pattern = args[i].getPattern();
        if (pattern.empty()) continue;
        
        // 在token中查找匹配模式的值
        for (size_t j = 0; j < tokens_.size(); ++j) {
            try {
                std::regex patternRegex(pattern);
                if (std::regex_match(tokens_[j], patternRegex)) {
                    ParamCandidate candidate(tokens_[j], args[i].getExpectedType(), 
                                           0.9, j, "pattern_match");
                    
                    args[i].setAutoFillCandidate(candidate);
                    result.filledValues.push_back(tokens_[j]);
                    result.candidates.push_back(candidate);
                    
                    std::cout << "模式匹配参数[" << i << "]: " << tokens_[j] << " (模式: " << pattern << ")" << std::endl;
                    break;
                }
            } catch (...) {
                std::cout << "模式匹配错误: " << pattern << std::endl;
            }
        }
    }
    
    result.success = result.filledValues.size() == args.length();
    result.overallConfidence = result.success ? 0.9 : 0.5;
    
    return result;
}

AutoFillResult AutoFillScanner::performTypeAwareFill(Args& args) {
    AutoFillResult result;
    std::cout << "执行类型感知填充策略" << std::endl;
    
    // 收集期望类型
    std::vector<ParamType> expectedTypes;
    std::vector<std::string> patterns;
    
    for (size_t i = 0; i < args.length(); ++i) {
        expectedTypes.push_back(args[i].getExpectedType());
        patterns.push_back(args[i].getPattern());
    }
    
    // 使用智能匹配器查找候选项
    std::vector<ParamCandidate> candidates = matcher_->findCandidates(tokens_, expectedTypes, patterns);
    
    // 为每个参数分配最佳候选项
    std::vector<bool> used(candidates.size(), false);
    
    for (size_t i = 0; i < args.length(); ++i) {
        double bestConfidence = 0.0;
        int bestCandidateIdx = -1;
        
        for (size_t j = 0; j < candidates.size(); ++j) {
            if (used[j]) continue;
            
            double typeConfidence = ParamTypeDetector::calculateTypeConfidence(
                candidates[j].value, args[i].getExpectedType());
            
            if (typeConfidence > bestConfidence) {
                bestConfidence = typeConfidence;
                bestCandidateIdx = j;
            }
        }
        
        if (bestCandidateIdx >= 0 && bestConfidence >= minConfidence_) {
            used[bestCandidateIdx] = true;
            candidates[bestCandidateIdx].confidence = bestConfidence;
            
            args[i].setAutoFillCandidate(candidates[bestCandidateIdx]);
            result.filledValues.push_back(candidates[bestCandidateIdx].value);
            result.candidates.push_back(candidates[bestCandidateIdx]);
            
            std::cout << "类型感知填充参数[" << i << "]: " << candidates[bestCandidateIdx].value 
                      << " (置信度: " << bestConfidence << ")" << std::endl;
        }
    }
    
    result.success = result.filledValues.size() >= args.length() / 2;  // 至少填充一半
    
    if (!result.filledValues.empty()) {
        double totalConfidence = 0.0;
        for (const auto& candidate : result.candidates) {
            totalConfidence += candidate.confidence;
        }
        result.overallConfidence = totalConfidence / result.candidates.size();
    }
    
    return result;
}

AutoFillResult AutoFillScanner::performContextAwareFill(Args& args, const std::string& functionName) {
    AutoFillResult result;
    std::cout << "执行上下文感知填充策略，函数: " << functionName << std::endl;
    
    // 收集参数名
    std::vector<std::string> paramNames;
    for (size_t i = 0; i < args.length(); ++i) {
        paramNames.push_back(args[i].getName());
    }
    
    // 使用上下文感知匹配
    std::vector<ParamCandidate> candidates = matcher_->contextAwareMatch(tokens_, functionName, paramNames);
    
    // 智能分配候选项
    std::vector<bool> used(candidates.size(), false);
    
    for (size_t i = 0; i < args.length(); ++i) {
        double bestConfidence = 0.0;
        int bestCandidateIdx = -1;
        
        for (size_t j = 0; j < candidates.size(); ++j) {
            if (used[j]) continue;
            
            if (candidates[j].confidence > bestConfidence) {
                bestConfidence = candidates[j].confidence;
                bestCandidateIdx = j;
            }
        }
        
        if (bestCandidateIdx >= 0 && bestConfidence >= minConfidence_) {
            used[bestCandidateIdx] = true;
            
            args[i].setAutoFillCandidate(candidates[bestCandidateIdx]);
            result.filledValues.push_back(candidates[bestCandidateIdx].value);
            result.candidates.push_back(candidates[bestCandidateIdx]);
            
            std::cout << "上下文感知填充参数[" << i << "]: " << candidates[bestCandidateIdx].value 
                      << " (置信度: " << bestConfidence << ")" << std::endl;
        }
    }
    
    result.success = result.filledValues.size() >= args.length() * 0.7;  // 至少填充70%
    
    if (!result.filledValues.empty()) {
        double totalConfidence = 0.0;
        for (const auto& candidate : result.candidates) {
            totalConfidence += candidate.confidence;
        }
        result.overallConfidence = totalConfidence / result.candidates.size();
    }
    
    return result;
}

std::vector<std::string> AutoFillScanner::extractRelevantTokens() {
    std::vector<std::string> relevant;
    
    // 使用双指针扫描器提取相关token
    if (useDualPointer_ && !keywords_.empty()) {
        std::vector<ScanPosition> positions = dualPointer_->scanAllKeywords();
        
        for (const auto& pos : positions) {
            std::vector<std::string> extracted = dualPointer_->extractBetweenPointers(pos);
            relevant.insert(relevant.end(), extracted.begin(), extracted.end());
            
            // 使用前置截取机制
            if (useFrontExtract_) {
                std::string fragment = dualPointer_->extractRange(pos.backPointer, pos.frontPointer + 5);
                std::vector<std::string> frontParams = frontExtract_->extractFrontParameters(fragment, pos.detectedKeyword);
                relevant.insert(relevant.end(), frontParams.begin(), frontParams.end());
            }
        }
    } else {
        // 简单策略：过滤掉明显的语法元素
        for (const auto& token : tokens_) {
            if (token != "(" && token != ")" && token != "{" && token != "}" && 
                token != "," && token != ";" && token != ":" && !token.empty()) {
                relevant.push_back(token);
            }
        }
    }
    
    std::cout << "提取相关token，数量: " << relevant.size() << std::endl;
    return relevant;
}

void AutoFillScanner::optimizeResults(AutoFillResult& result) {
    if (result.candidates.empty()) return;
    
    std::cout << "优化自动填充结果" << std::endl;
    
    // 去除重复值
    std::sort(result.candidates.begin(), result.candidates.end(),
              [](const ParamCandidate& a, const ParamCandidate& b) {
                  return a.value < b.value;
              });
    
    auto it = std::unique(result.candidates.begin(), result.candidates.end(),
                         [](const ParamCandidate& a, const ParamCandidate& b) {
                             return a.value == b.value;
                         });
    
    result.candidates.erase(it, result.candidates.end());
    
    // 重新计算总体置信度
    if (!result.candidates.empty()) {
        double totalConfidence = 0.0;
        for (const auto& candidate : result.candidates) {
            totalConfidence += candidate.confidence;
        }
        result.overallConfidence = totalConfidence / result.candidates.size();
    }
    
    std::cout << "优化后候选项数量: " << result.candidates.size() 
              << ", 总体置信度: " << result.overallConfidence << std::endl;
}

// ============ Syntax类实现 - 自动填充增强 ============

Syntax& Syntax::debug(const std::string& message) const {
    std::cout << "\n=== Debug: " << message << " ===" << std::endl;
    std::cout << "参数数量: " << args.length() << std::endl;
    
    for (size_t i = 0; i < args.length(); ++i) {
        const auto& arg = args[i];
        std::cout << "参数[" << i << "]: " << arg.getName() 
                  << " (占位符: " << (arg.isPlaceholder() ? "是" : "否")
                  << ", 已绑定: " << (arg.hasBind() ? "是" : "否")
                  << ", 有转换: " << (arg.hasTransform() ? "是" : "否")
                  << ", 期望类型: " << static_cast<int>(arg.getExpectedType())
                  << ", 有候选项: " << (arg.hasAutoFillCandidate() ? "是" : "否") << ")" << std::endl;
        
        if (!arg.getValue().empty()) {
            std::cout << "  值: " << arg.getValue() << std::endl;
        }
        if (!arg.getJSCode().empty()) {
            std::cout << "  JS代码: " << arg.getJSCode() << std::endl;
        }
        if (arg.hasAutoFillCandidate()) {
            const auto& candidate = arg.getAutoFillCandidate();
            std::cout << "  候选项: " << candidate.value 
                      << " (置信度: " << candidate.confidence << ")" << std::endl;
        }
    }
    
    std::cout << "=== Debug End ===" << std::endl;
    return const_cast<Syntax&>(*this);
}

// ============ CHTLJSFunction类实现 - 自动填充集成 ============

CHTLJSFunction::CHTLJSFunction(const std::string& functionName, 
                               const std::vector<std::string>& paramNames)
    : functionName_(functionName), paramNames_(paramNames), fillStrategy_(FillStrategy::AUTO), autoFillEnabled_(false) {
    std::cout << "创建智能CHTLJSFunction: " << functionName_ << std::endl;
}

CHTLJSFunction& CHTLJSFunction::transformKey(const std::string& keyName, const std::string& jsTemplate) {
    if (!syntax_) createSyntax();
    syntax_->args.transform(keyName, jsTemplate);
    return *this;
}

CHTLJSFunction& CHTLJSFunction::matchKeyword(const std::string& keyword, const std::string& value) {
    if (!syntax_) createSyntax();
    syntax_->args.match(keyword, value);
    return *this;
}

CHTLJSFunction& CHTLJSFunction::enableAutoFill(const std::vector<ParamType>& expectedTypes,
                                               const std::vector<std::string>& patterns,
                                               FillStrategy strategy) {
    expectedTypes_ = expectedTypes;
    patterns_ = patterns;
    fillStrategy_ = strategy;
    autoFillEnabled_ = true;
    
    std::cout << "启用自动填充: " << functionName_ << " (策略: " << static_cast<int>(strategy) << ")" << std::endl;
    return *this;
}

CHTLJSFunction& CHTLJSFunction::performAutoFill(const std::vector<std::string>& sourceTokens) {
    if (!syntax_) createSyntax();
    
    if (autoFillEnabled_) {
        syntax_->expectTypes(expectedTypes_);
        syntax_->setPatterns(patterns_);
        syntax_->autoFill(sourceTokens, fillStrategy_);
        syntax_->acceptAutoFill(0.7);
    }
    
    return *this;
}

std::unique_ptr<Syntax> CHTLJSFunction::createSyntax() {
    syntax_ = std::make_unique<Syntax>();
    
    if (paramNames_.empty()) {
        for (int i = 0; i < 5; ++i) {
            syntax_->args.addArg("$", true);
        }
    } else {
        for (const auto& paramName : paramNames_) {
            bool isPlaceholder = (paramName == "$");
            syntax_->args.addArg(paramName, isPlaceholder);
        }
    }
    
    return std::make_unique<Syntax>(*syntax_);
}

std::string CHTLJSFunction::generateCode() {
    if (!syntax_) return "";
    
    std::string code = functionName_ + "({\n";
    
    for (size_t i = 0; i < paramNames_.size() && i < syntax_->args.length(); ++i) {
        if (i > 0) code += ",\n";
        code += "  " + paramNames_[i] + ": ";
        
        const auto& arg = syntax_->args[i];
        if (!arg.getJSCode().empty()) {
            code += arg.getJSCode();
        } else {
            code += "\"" + arg.getValue() + "\"";
        }
    }
    
    code += "\n});";
    return code;
}

// ============ 全局函数实现 ============

std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                     const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    
    std::cout << "智能语法分析: " << pattern.substr(0, 50) << "..." << std::endl;
    
    size_t pos = 0;
    int placeholderCount = 0;
    
    while ((pos = pattern.find('$', pos)) != std::string::npos) {
        syntax->args.addArg("$", true);
        placeholderCount++;
        pos++;
    }
    
    if (placeholderCount == 0) {
        std::regex paramRegex(R"((\w+):\s*\$)");
        std::sregex_iterator iter(pattern.begin(), pattern.end(), paramRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string paramName = iter->str(1);
            syntax->args.addArg(paramName, false);
        }
    }
    
    std::cout << "识别到 " << syntax->args.length() << " 个参数，支持自动填充" << std::endl;
    
    return syntax;
}

std::unique_ptr<AutoFillScanner> createAutoFillScanner() {
    return std::make_unique<AutoFillScanner>();
}

std::unique_ptr<CHTLJSFunction> createSmartCHTLJSFunction(
    const std::string& functionName,
    const std::vector<std::string>& paramNames,
    const std::vector<ParamType>& expectedTypes,
    FillStrategy strategy) {
    
    auto func = std::make_unique<CHTLJSFunction>(functionName, paramNames);
    func->enableAutoFill(expectedTypes, {}, strategy);
    return func;
}

} // namespace CJMOD
} // namespace CHTL