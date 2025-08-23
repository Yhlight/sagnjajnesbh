#include "CJMOD/SimpleCJMODApi.h"
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

std::string Arg::operator()(const std::string& value) {
    if (hasBind_ && processor_) {
        return processor_(value);
    }
    return value;
}

void Arg::match(const std::string& value) {
    rawValue_ = value;
    if (hasBind_ && processor_) {
        processedValue_ = processor_(value);
    } else {
        processedValue_ = value;
    }
}

// ============ Syntax类实现 ============

void Syntax::addArg(const std::string& name, bool isPlaceholder) {
    args_.emplace_back(name, isPlaceholder);
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

void Syntax::match(const std::vector<std::string>& values) {
    size_t valueIndex = 0;
    for (auto& arg : args_) {
        if (valueIndex < values.size()) {
            arg.match(values[valueIndex]);
            valueIndex++;
        }
    }
}

void Syntax::match(const std::string& name, const std::string& value) {
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
        for (auto& arg : args_) {
            if (arg.getName() == name) {
                arg.match(value);
                return;
            }
        }
    }
}

std::string Syntax::result() const {
    std::string result;
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) result += ", ";
        result += args_[i].getValue();
    }
    return result;
}

// ============ CHTLJSFunction类实现 ============

CHTLJSFunction::CHTLJSFunction(const std::string& functionName, 
                               const std::vector<std::string>& paramNames)
    : functionName_(functionName), paramNames_(paramNames) {
}

std::unique_ptr<Syntax> CHTLJSFunction::createSyntax() {
    syntax_ = std::make_unique<Syntax>();
    
    if (paramNames_.empty()) {
        // 自动生成占位符参数
        // 根据常见的JS函数参数模式推断
        std::vector<std::string> defaultParams = {"$", "$", "$", "$", "$"};
        for (const auto& param : defaultParams) {
            syntax_->addArg(param, true);
        }
    } else {
        // 使用指定的参数名
        for (const auto& paramName : paramNames_) {
            bool isPlaceholder = (paramName == "$");
            syntax_->addArg(paramName, isPlaceholder);
        }
    }
    
    return std::make_unique<Syntax>(*syntax_);
}

void CHTLJSFunction::scanKeyword(const std::string& sourceCode) {
    std::cout << "扫描关键字: " << functionName_ << " 在源代码中..." << std::endl;
    executeDualPointerScan(sourceCode);
}

void CHTLJSFunction::matchValues(const std::vector<std::string>& values) {
    if (!syntax_) createSyntax();
    syntax_->match(values);
}

std::string CHTLJSFunction::generateCode() {
    if (!syntax_) return "";
    
    std::string code = functionName_ + "({\n";
    
    for (size_t i = 0; i < paramNames_.size() && i < syntax_->length(); ++i) {
        if (i > 0) code += ",\n";
        code += "  " + paramNames_[i] + ": " + (*syntax_)[i].getValue();
    }
    
    if (paramNames_.empty()) {
        // 占位符模式，生成简单的参数列表
        for (size_t i = 0; i < syntax_->length(); ++i) {
            if (i > 0) code += ", ";
            code += (*syntax_)[i].getValue();
        }
        code = functionName_ + "(" + code + ")";
    } else {
        code += "\n});";
    }
    
    return code;
}

std::string CHTLJSFunction::process(const std::string& sourceCode, 
                                   const std::vector<std::string>& values) {
    std::cout << "=== CHTLJSFunction 简化流程处理 ===" << std::endl;
    std::cout << "函数名: " << functionName_ << std::endl;
    
    // 1. 自动创建语法
    if (!syntax_) createSyntax();
    
    // 2. 扫描关键字
    scanKeyword(sourceCode);
    
    // 3. 匹配值
    if (!values.empty()) {
        matchValues(values);
         } else {
         // 从源代码中自动提取值（简化实现）
         // extractValuesFromSource(sourceCode);
     }
    
    // 4. 生成代码
    std::string result = generateCode();
    
    std::cout << "生成的代码: " << result << std::endl;
    return result;
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

void CHTLJSFunction::executeDualPointerScan(const std::string& sourceCode) {
    std::cout << "执行双指针扫描..." << std::endl;
    
    // 正确的双指针扫描机制
    size_t frontPtr = 0;    // 前指针：寻找CJMOD语法
    size_t backPtr = 0;     // 后指针：标记已处理位置
    
    // 扫描初始范围，确定不存在语法片段
    const size_t INITIAL_SCAN_RANGE = std::min(static_cast<size_t>(100), sourceCode.length());
    
    std::cout << "初始范围扫描 [0-" << INITIAL_SCAN_RANGE << "]" << std::endl;
    
    bool foundInInitial = false;
    std::string initialRange = sourceCode.substr(0, INITIAL_SCAN_RANGE);
    if (initialRange.find(functionName_) != std::string::npos) {
        foundInInitial = true;
        std::cout << "初始范围发现函数名: " << functionName_ << std::endl;
    }
    
    if (!foundInInitial) {
        // 前指针移动到合适位置
        frontPtr = INITIAL_SCAN_RANGE;
        backPtr = 0;
        std::cout << "前指针移动到: " << frontPtr << ", 后指针保持: " << backPtr << std::endl;
    }
    
    // 前后指针同步向前，前指针寻找关键字
    while (frontPtr < sourceCode.length()) {
        // 前指针检测CJMOD关键字
        if (frontPtr + functionName_.length() <= sourceCode.length() &&
            sourceCode.substr(frontPtr, functionName_.length()) == functionName_) {
            
            std::cout << "前指针在位置 " << frontPtr << " 检测到关键字，通知后指针开始收集" << std::endl;
            
            // 前置截取机制应用
            applyFrontExtract();
            
            // 双指针同步移动
            frontPtr += functionName_.length();
            backPtr = frontPtr;
            
            break;
        }
        
        frontPtr++;
        if (!foundInInitial) {
            backPtr = frontPtr;
        }
    }
}

void CHTLJSFunction::applyFrontExtract() {
    std::cout << "应用前置截取机制..." << std::endl;
    // 前置截取机制的实现
    // 处理 "arg ** function" 这样的语法
}

// 暂时注释掉，避免编译错误
// void CHTLJSFunction::extractValuesFromSource(const std::string& sourceCode) {
//     // 实现将在后续完善
// }

// ============ 全局函数实现 ============

std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                     const std::string& ignoreChars) {
    auto syntax = std::make_unique<Syntax>();
    
    std::cout << "语法分析: " << pattern.substr(0, 50) << "..." << std::endl;
    
    // 智能分析模式，识别占位符和参数
    size_t pos = 0;
    int placeholderCount = 0;
    
    // 寻找占位符 $
    while ((pos = pattern.find('$', pos)) != std::string::npos) {
        syntax->addArg("$", true);
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
            syntax->addArg(paramName, false);
        }
    }
    
    std::cout << "识别到 " << syntax->length() << " 个参数" << std::endl;
    
    return syntax;
}

std::string generateCode(const Syntax& syntax) {
    return syntax.result();
}

std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(
    const std::string& functionName,
    const std::vector<std::string>& paramNames) {
    
    return std::make_unique<CHTLJSFunction>(functionName, paramNames);
}

} // namespace CJMOD
} // namespace CHTL