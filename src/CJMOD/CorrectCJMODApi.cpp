#include "CJMOD/CorrectCJMODApi.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// ============ DualPointerScanner 实现 ============

DualPointerScanner::DualPointerScanner(const std::string& source, 
                                     const std::vector<std::string>& keywords)
    : source_(source), keywords_(keywords), frontPtr_(0), backPtr_(0), isCollecting_(false) {
}

DualPointerScanner::ScanResult DualPointerScanner::scan() {
    ScanResult result;
    
    std::cout << "=== 正确的双指针扫描机制开始 ===" << std::endl;
    std::cout << "源代码长度: " << source_.length() << std::endl;
    std::cout << "扫描关键字: ";
    for (const auto& kw : keywords_) {
        std::cout << "\"" << kw << "\" ";
    }
    std::cout << std::endl;
    
    // 第一步：前指针和后指针都从位置0开始
    frontPtr_ = 0;
    backPtr_ = 0;
    isCollecting_ = false;
    
    // 第二步：扫描初始范围，确定不存在语法片段
    const size_t INITIAL_SCAN_RANGE = std::min(static_cast<size_t>(100), source_.length());
    if (!scanInitialRange(0, INITIAL_SCAN_RANGE)) {
        std::cout << "初始范围扫描：未发现CJMOD语法片段" << std::endl;
        
        // 第三步：前指针移动到合适位置
        frontPtr_ = INITIAL_SCAN_RANGE;
        backPtr_ = 0;  // 后指针保持在0位置
        
        std::cout << "前指针移动到位置: " << frontPtr_ << ", 后指针保持在: " << backPtr_ << std::endl;
    }
    
    // 第四步：前后指针同步向前，前指针寻找关键字
    while (frontPtr_ < source_.length()) {
        // 前指针检测CJMOD关键字
        if (detectKeywordAtFrontPtr()) {
            std::cout << "前指针在位置 " << frontPtr_ << " 检测到关键字，通知后指针开始收集" << std::endl;
            
            // 输出前面的普通代码（后指针到前指针之间）
            if (backPtr_ < frontPtr_) {
                std::string normalCode = source_.substr(backPtr_, frontPtr_ - backPtr_);
                result.normalCodeSegments.push_back(normalCode);
                std::cout << "添加普通代码片段: \"" << normalCode.substr(0, 30) << "...\"" << std::endl;
            }
            
            // 前指针通知后指针进行收集
            startCollection();
            
            // 收集CJMOD代码片段
            std::string cjmodCode = endCollection();
            result.cjmodCodeSegments.push_back(cjmodCode);
            result.cjmodPositions.push_back(frontPtr_);
            
            std::cout << "收集到CJMOD片段: \"" << cjmodCode.substr(0, 30) << "...\"" << std::endl;
            
            // 双指针同步移动到收集结束位置
            movePtrsSynchronously();
        } else {
            // 前指针向前移动
            frontPtr_++;
            if (!isCollecting_) {
                backPtr_ = frontPtr_;  // 后指针跟随（非收集模式）
            }
        }
    }
    
    // 处理剩余的普通代码
    if (backPtr_ < source_.length()) {
        std::string remainingCode = source_.substr(backPtr_);
        result.normalCodeSegments.push_back(remainingCode);
        std::cout << "添加剩余普通代码: \"" << remainingCode.substr(0, 30) << "...\"" << std::endl;
    }
    
    std::cout << "=== 双指针扫描完成 ===" << std::endl;
    std::cout << "普通代码片段数: " << result.normalCodeSegments.size() << std::endl;
    std::cout << "CJMOD代码片段数: " << result.cjmodCodeSegments.size() << std::endl;
    
    return result;
}

bool DualPointerScanner::scanInitialRange(size_t start, size_t end) {
    std::string range = source_.substr(start, end - start);
    for (const auto& keyword : keywords_) {
        if (range.find(keyword) != std::string::npos) {
            std::cout << "初始范围发现关键字: " << keyword << std::endl;
            return true;
        }
    }
    return false;
}

bool DualPointerScanner::detectKeywordAtFrontPtr() {
    for (const auto& keyword : keywords_) {
        if (frontPtr_ + keyword.length() <= source_.length()) {
            if (source_.substr(frontPtr_, keyword.length()) == keyword) {
                return true;
            }
        }
    }
    return false;
}

void DualPointerScanner::movePtrsSynchronously() {
    // 实现双指针同步移动逻辑
    // 这里简化为移动到下一个可能的位置
    frontPtr_++;
    backPtr_ = frontPtr_;
    isCollecting_ = false;
    
    std::cout << "双指针同步移动到位置: " << frontPtr_ << std::endl;
}

void DualPointerScanner::startCollection() {
    isCollecting_ = true;
    std::cout << "开始收集模式" << std::endl;
}

std::string DualPointerScanner::endCollection() {
    // 简化的收集结束逻辑
    // 在实际实现中，这里需要根据语法规则确定收集范围
    size_t collectEnd = frontPtr_ + 50;  // 示例：收集50个字符
    collectEnd = std::min(collectEnd, source_.length());
    
    std::string collected = source_.substr(frontPtr_, collectEnd - frontPtr_);
    frontPtr_ = collectEnd;
    backPtr_ = collectEnd;
    isCollecting_ = false;
    
    std::cout << "收集结束，双指针移动到位置: " << frontPtr_ << std::endl;
    return collected;
}

// ============ FrontExtractMechanism 实现 ============

FrontExtractMechanism::FrontExtractMechanism(const std::string& triggerKeyword, 
                                           const std::string& extractPattern)
    : triggerKeyword_(triggerKeyword), extractPattern_(extractPattern) {
}

FrontExtractMechanism::ExtractResult FrontExtractMechanism::extract(const std::string& codeSegment) {
    ExtractResult result;
    result.hasExtraction = false;
    
    std::cout << "=== 前置截取机制开始 ===" << std::endl;
    std::cout << "触发关键字: \"" << triggerKeyword_ << "\"" << std::endl;
    std::cout << "截取模式: \"" << extractPattern_ << "\"" << std::endl;
    std::cout << "代码片段: \"" << codeSegment.substr(0, 50) << "...\"" << std::endl;
    
    // 寻找触发关键字位置
    size_t triggerPos = findTriggerPosition(codeSegment);
    
    if (triggerPos != std::string::npos) {
        std::cout << "找到触发关键字在位置: " << triggerPos << std::endl;
        
        // 从触发位置向前截取匹配模式
        std::string frontPart = extractFrontPattern(codeSegment, triggerPos);
        
        if (!frontPart.empty()) {
            result.frontPart = frontPart;
            result.remainingPart = codeSegment.substr(frontPart.length());
            result.hasExtraction = true;
            
            std::cout << "成功截取前置部分: \"" << frontPart << "\"" << std::endl;
            std::cout << "剩余部分: \"" << result.remainingPart.substr(0, 30) << "...\"" << std::endl;
        } else {
            std::cout << "未找到匹配的前置模式" << std::endl;
            result.remainingPart = codeSegment;
        }
    } else {
        std::cout << "未找到触发关键字" << std::endl;
        result.remainingPart = codeSegment;
    }
    
    std::cout << "=== 前置截取机制完成 ===" << std::endl;
    return result;
}

size_t FrontExtractMechanism::findTriggerPosition(const std::string& segment) {
    return segment.find(triggerKeyword_);
}

std::string FrontExtractMechanism::extractFrontPattern(const std::string& segment, size_t triggerPos) {
    // 示例实现：从触发位置向前查找模式
    // 对于 "arg ** arg" 的情况，从 "**" 位置向前找 "arg"
    
    if (triggerPos == 0) return "";
    
    // 向前搜索extractPattern_
    std::string beforeTrigger = segment.substr(0, triggerPos);
    
    // 从右向左查找最后一个匹配的extractPattern_
    size_t patternPos = beforeTrigger.rfind(extractPattern_);
    
    if (patternPos != std::string::npos) {
        // 找到了模式，截取从模式开始到触发关键字的部分
        return segment.substr(patternPos, triggerPos + triggerKeyword_.length() - patternPos);
    }
    
    return "";
}

// ============ Arg类实现 ============

Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder), hasBind_(false) {
}

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

const std::string& Arg::transform(const std::string& jsCode) {
    // 简化实现
    return transformedValue_;
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

void Args::match(const std::string& name, const std::string& value) {
    if (name == "$") {
        // 占位符匹配：找到下一个未匹配的占位符
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && arg.getValue().empty()) {
                arg.match(value);
                break;
            }
        }
    } else {
        Arg* arg = findArg(name);
        if (arg) {
            arg->match(value);
        }
    }
}

void Args::transform(const std::string& name, const std::string& jsTemplate) {
    // 简化实现
}

std::string Args::result() const {
    std::string result;
    for (const auto& arg : args_) {
        if (!result.empty()) result += ", ";
        result += arg.getValue();
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

// ============ CHTLJSFunction类实现 ============

CHTLJSFunction::CHTLJSFunction(const std::string& functionName, 
                             const std::vector<std::string>& keyNames,
                             bool useFrontExtract)
    : functionName_(functionName), keyNames_(keyNames), useFrontExtract_(useFrontExtract) {
    
    std::cout << "创建CHTLJSFunction: " << functionName_ << std::endl;
    std::cout << "参数键名: ";
    for (const auto& key : keyNames_) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    std::cout << "前置截取: " << (useFrontExtract_ ? "启用" : "禁用") << std::endl;
}

void CHTLJSFunction::setFrontExtractRule(const std::string& triggerKeyword, 
                                        const std::string& extractPattern) {
    if (useFrontExtract_) {
        frontExtractor_ = std::make_unique<FrontExtractMechanism>(triggerKeyword, extractPattern);
        std::cout << "设置前置截取规则: " << triggerKeyword << " -> " << extractPattern << std::endl;
    }
}

std::unique_ptr<Syntax> CHTLJSFunction::createSyntax() {
    auto syntax = std::make_unique<Syntax>();
    
    // 根据keyNames_创建参数
    for (const auto& keyName : keyNames_) {
        bool isPlaceholder = (keyName == "$");
        syntax->args.addArg(keyName, isPlaceholder);
    }
    
    return syntax;
}

std::string CHTLJSFunction::processWithDualPointer(const std::string& sourceCode) {
    std::cout << "=== 使用双指针扫描处理源代码 ===" << std::endl;
    
    // 创建双指针扫描器
    std::vector<std::string> keywords = {functionName_, "**", "arg"};  // 示例关键字
    scanner_ = std::make_unique<DualPointerScanner>(sourceCode, keywords);
    
    // 执行扫描
    auto scanResult = scanner_->scan();
    
    // 处理扫描结果
    std::string processedCode;
    size_t cjmodIndex = 0;
    
    for (size_t i = 0; i < scanResult.normalCodeSegments.size(); ++i) {
        // 添加普通代码片段
        processedCode += scanResult.normalCodeSegments[i];
        
        // 如果有对应的CJMOD片段，进行处理
        if (cjmodIndex < scanResult.cjmodCodeSegments.size()) {
            std::string cjmodCode = scanResult.cjmodCodeSegments[cjmodIndex];
            
            // 应用前置截取机制
            if (useFrontExtract_ && frontExtractor_) {
                auto extractResult = frontExtractor_->extract(cjmodCode);
                if (extractResult.hasExtraction) {
                    // 前置部分不发送给编译器，只处理剩余部分
                    cjmodCode = extractResult.remainingPart;
                    std::cout << "前置截取应用，处理剩余部分: \"" << cjmodCode.substr(0, 30) << "...\"" << std::endl;
                }
            }
            
            // 生成处理后的JavaScript代码
            processedCode += "/* CJMOD处理: " + cjmodCode + " */";
            
            cjmodIndex++;
        }
    }
    
    return processedCode;
}

std::string CHTLJSFunction::generateCode(const Syntax& syntax) {
    std::string code = functionName_ + "({\n";
    
    for (size_t i = 0; i < keyNames_.size() && i < syntax.args.length(); ++i) {
        if (i > 0) code += ",\n";
        code += "  " + keyNames_[i] + ": " + syntax.args[i].getValue();
    }
    
    code += "\n});";
    return code;
}

std::string CHTLJSFunction::generateSyntaxPattern() {
    std::string pattern = functionName_ + "({\n";
    for (size_t i = 0; i < keyNames_.size(); ++i) {
        if (i > 0) pattern += ",\n";
        pattern += "  " + keyNames_[i] + ": $";
    }
    pattern += "\n});";
    return pattern;
}

// ============ 全局函数实现 ============

std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    
    std::cout << "语法分析: " << pattern.substr(0, 50) << "..." << std::endl;
    std::cout << "忽略字符: " << ignoreChars << std::endl;
    
    // 简化的语法分析实现
    // 在实际实现中，这里需要完整的词法分析和语法分析
    
    // 寻找占位符$
    size_t pos = 0;
    int placeholderCount = 0;
    
    while ((pos = pattern.find('$', pos)) != std::string::npos) {
        syntax->args.addArg("$", true);
        placeholderCount++;
        pos++;
    }
    
    std::cout << "识别到 " << placeholderCount << " 个占位符" << std::endl;
    
    return syntax;
}

std::string generateCode(const Syntax& syntax) {
    return syntax.result();
}

void configureDualPointerScanning(const std::vector<std::string>& keywords) {
    std::cout << "配置双指针扫描关键字: ";
    for (const auto& kw : keywords) {
        std::cout << kw << " ";
    }
    std::cout << std::endl;
}

void enableFrontExtractMechanism(const std::string& triggerKeyword, 
                                const std::string& extractPattern) {
    std::cout << "启用前置截取机制: " << triggerKeyword << " -> " << extractPattern << std::endl;
}

} // namespace CJMOD
} // namespace CHTL