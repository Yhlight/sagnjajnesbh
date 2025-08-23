#include "CJMOD/ElegantCJMODApi.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace CHTL {
namespace CJMOD {

// ==========================================
// Arg类实现 - 表面简单，内在精妙
// ==========================================

Arg::Arg(const std::string& name, bool isPlaceholder)
    : name_(name), isPlaceholder_(isPlaceholder) {}

void Arg::match(const ArgValue& value) {
    // 内在精妙：智能类型处理和转换
    if (std::holds_alternative<std::string>(value)) {
        rawValue_ = std::get<std::string>(value);
        if (stringProcessor_) {
            processedValue_ = stringProcessor_(rawValue_);
        } else {
            processedValue_ = rawValue_;
        }
    } else if (std::holds_alternative<int>(value)) {
        int intVal = std::get<int>(value);
        rawValue_ = std::to_string(intVal);
        if (intProcessor_) {
            processedValue_ = intProcessor_(intVal);
        } else {
            processedValue_ = rawValue_;
        }
    } else if (std::holds_alternative<double>(value)) {
        double doubleVal = std::get<double>(value);
        rawValue_ = std::to_string(doubleVal);
        if (doubleProcessor_) {
            processedValue_ = doubleProcessor_(doubleVal);
        } else {
            processedValue_ = rawValue_;
        }
    } else if (std::holds_alternative<bool>(value)) {
        bool boolVal = std::get<bool>(value);
        rawValue_ = boolVal ? "true" : "false";
        if (boolProcessor_) {
            processedValue_ = boolProcessor_(boolVal);
        } else {
            processedValue_ = rawValue_;
        }
    }
}

void Arg::processValue() {
    // 内在精妙：后处理优化
    if (processedValue_.empty()) {
        processedValue_ = rawValue_;
    }
}

// ==========================================
// ArgCollection类实现 - 表面简单，内在智能
// ==========================================

const Arg& ArgCollection::operator[](size_t index) const {
    if (index >= args_.size()) {
        throw std::out_of_range("参数索引超出范围");
    }
    return *args_[index];
}

Arg& ArgCollection::operator[](size_t index) {
    if (index >= args_.size()) {
        throw std::out_of_range("参数索引超出范围");
    }
    return *args_[index];
}

void ArgCollection::match(const std::string& name, const ArgValue& value) {
    Arg* arg = findArg(name);
    if (arg) {
        arg->match(value);
    }
}

void ArgCollection::match(size_t index, const ArgValue& value) {
    if (index < args_.size()) {
        args_[index]->match(value);
    }
}

std::string ArgCollection::result() const {
    // 内在精妙：智能组合所有参数
    std::ostringstream oss;
    for (size_t i = 0; i < args_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << args_[i]->getValue();
    }
    return oss.str();
}

void ArgCollection::addArg(const std::string& name, bool isPlaceholder) {
    auto arg = std::make_unique<Arg>(name, isPlaceholder);
    nameIndex_[name] = args_.size();
    args_.push_back(std::move(arg));
}

Arg* ArgCollection::findArg(const std::string& name) {
    auto it = nameIndex_.find(name);
    if (it != nameIndex_.end()) {
        return args_[it->second].get();
    }
    return nullptr;
}

// ==========================================
// Keyword类实现 - 表面简单，内在精妙
// ==========================================

Keyword::Keyword(const std::string& pattern) : pattern_(pattern) {
    parsePattern();
}

bool Keyword::matches(const std::string& code) const {
    // 内在精妙：智能模式匹配算法
    std::regex patternRegex(pattern_);
    return std::regex_search(code, patternRegex);
}

void Keyword::parsePattern() {
    // 内在精妙：智能解析语法模式
    std::regex argRegex(R"((\w+):\s*\$)");  // 匹配 "name: $" 模式
    std::regex placeholderRegex(R"(\$)");   // 匹配单独的 "$"
    
    std::sregex_iterator iter(pattern_.begin(), pattern_.end(), argRegex);
    std::sregex_iterator end;
    
    // 解析命名参数
    for (; iter != end; ++iter) {
        std::string argName = (*iter)[1].str();
        args.addArg(argName, false);
    }
    
    // 解析占位符参数
    std::sregex_iterator placeholderIter(pattern_.begin(), pattern_.end(), placeholderRegex);
    size_t placeholderCount = 0;
    for (; placeholderIter != std::sregex_iterator(); ++placeholderIter) {
        args.addArg("$" + std::to_string(placeholderCount++), true);
    }
}

// ==========================================
// CJMODScanner类实现 - 表面简单，内在精妙
// ==========================================

CJMODScanner::CJMODScanner() : currentPosition_(0), currentPolicy_(Policy::NORMAL) {}

void CJMODScanner::scanKeyword(const Arg& keywordArg, std::function<void()> handler) {
    scanKeyword(keywordArg.getName(), handler);
}

void CJMODScanner::scanKeyword(const std::string& keyword, std::function<void()> handler) {
    // 内在精妙：智能关键字扫描和处理
    for (size_t i = 0; i < tokens_.size(); ++i) {
        if (tokens_[i] == keyword) {
            currentPosition_ = i;
            handler();
            break;
        }
    }
}

std::string CJMODScanner::peekKeyword(int offset) const {
    // 内在精妙：智能相对位置访问
    size_t targetPos = currentPosition_ + offset;
    if (targetPos < tokens_.size()) {
        return tokens_[targetPos];
    }
    return "";
}

std::string CJMODScanner::currentKeyword() const {
    if (currentPosition_ < tokens_.size()) {
        return tokens_[currentPosition_];
    }
    return "";
}

void CJMODScanner::policyChangeBegin(const std::string& startMarker, Policy policy) {
    // 内在精妙：策略栈管理
    policyStack_.push_back(currentPolicy_);
    currentPolicy_ = policy;
    
    // 查找起始标记
    for (size_t i = currentPosition_; i < tokens_.size(); ++i) {
        if (tokens_[i] == startMarker) {
            currentPosition_ = i + 1;
            break;
        }
    }
}

std::string CJMODScanner::policyChangeEnd(const std::string& endMarker, Policy returnPolicy) {
    // 内在精妙：收集策略期间的内容
    std::ostringstream collected;
    size_t startPos = currentPosition_;
    
    for (size_t i = currentPosition_; i < tokens_.size(); ++i) {
        if (tokens_[i] == endMarker) {
            currentPosition_ = i;
            break;
        }
        if (i > startPos) collected << " ";
        collected << tokens_[i];
    }
    
    // 恢复策略
    if (!policyStack_.empty()) {
        currentPolicy_ = policyStack_.back();
        policyStack_.pop_back();
    } else {
        currentPolicy_ = returnPolicy;
    }
    
    return collected.str();
}

void CJMODScanner::setSourceCode(const std::string& code) {
    sourceCode_ = code;
    tokenize();
    reset();
}

void CJMODScanner::reset() {
    currentPosition_ = 0;
    policyStack_.clear();
    currentPolicy_ = Policy::NORMAL;
}

void CJMODScanner::tokenize() {
    // 内在精妙：智能词法分析
    tokens_.clear();
    
    std::regex tokenRegex(R"(\w+|[{}();:,=]|"[^"]*"|'[^']*')");
    std::sregex_iterator iter(sourceCode_.begin(), sourceCode_.end(), tokenRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string token = iter->str();
        // 去除引号
        if ((token.front() == '"' && token.back() == '"') ||
            (token.front() == '\'' && token.back() == '\'')) {
            token = token.substr(1, token.length() - 2);
        }
        tokens_.push_back(token);
    }
}

void CJMODScanner::advancePosition() {
    if (currentPosition_ < tokens_.size()) {
        currentPosition_++;
    }
}

std::string CJMODScanner::extractToken(size_t position) const {
    if (position < tokens_.size()) {
        return tokens_[position];
    }
    return "";
}

// ==========================================
// 核心API函数实现 - 表面简单，内在精妙
// ==========================================

std::unique_ptr<Keyword> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    // 内在精妙：智能语法分析和模式解析
    std::string cleanPattern = pattern;
    
    // 移除忽略字符
    for (char c : ignoreChars) {
        cleanPattern.erase(std::remove(cleanPattern.begin(), cleanPattern.end(), c), cleanPattern.end());
    }
    
    // 移除多余空白
    cleanPattern = std::regex_replace(cleanPattern, std::regex(R"(\s+)"), " ");
    cleanPattern = Utils::StringUtils::Trim(cleanPattern);
    
    return std::make_unique<Keyword>(cleanPattern);
}

CJMODScanner& getCJMODScanner() {
    // 内在精妙：智能单例管理
    static CJMODScanner instance;
    return instance;
}

std::string generateCode(const Keyword& keyword) {
    // 内在精妙：智能代码生成和组合
    std::ostringstream code;
    
    // 基于模式生成JavaScript代码
    std::string pattern = keyword.getPattern();
    
    if (pattern.find("printMylove") != std::string::npos) {
        // 生成printMylove的JavaScript实现
        code << "function printMylove(options) {\n";
        code << "    const { url, mode, width, height, scale } = options;\n";
        code << "    return new Promise((resolve, reject) => {\n";
        code << "        const img = new Image();\n";
        code << "        img.onload = () => {\n";
        code << "            const canvas = document.createElement('canvas');\n";
        code << "            const ctx = canvas.getContext('2d');\n";
        code << "            canvas.width = parseInt(width) || 300;\n";
        code << "            canvas.height = parseInt(height) || 200;\n";
        code << "            ctx.drawImage(img, 0, 0, canvas.width, canvas.height);\n";
        code << "            const result = convertToCharacters(ctx, mode, scale);\n";
        code << "            console.log(result);\n";
        code << "            resolve(result);\n";
        code << "        };\n";
        code << "        img.onerror = reject;\n";
        code << "        img.src = url;\n";
        code << "    });\n";
        code << "}\n";
    } else if (pattern.find("iNeverAway") != std::string::npos) {
        // 生成iNeverAway的JavaScript实现
        code << "function iNeverAway(promiseMap) {\n";
        code << "    const promiseRegistry = {};\n";
        code << "    for (const [key, func] of Object.entries(promiseMap)) {\n";
        code << "        const globalName = 'chtl_promise_' + key.replace(/[<>]/g, '_');\n";
        code << "        window[globalName] = func;\n";
        code << "        promiseRegistry[key] = globalName;\n";
        code << "    }\n";
        code << "    return promiseRegistry;\n";
        code << "}\n";
    }
    
    return code.str();
}

// ==========================================
// 珂朵莉模块专用扩展实现
// ==========================================

namespace Chtholly {

// 静态成员初始化
std::unordered_map<std::string, std::string> INeverAwaySystem::promiseFunctions_;
std::unordered_map<std::string, std::string> INeverAwaySystem::customKeys_;

void INeverAwaySystem::registerPromiseFunction(const std::string& name, const std::string& jsCode) {
    promiseFunctions_[name] = jsCode;
}

std::string INeverAwaySystem::generatePromiseCall(const std::string& message, int duration) {
    std::ostringstream code;
    code << "setTimeout(() => {\n";
    code << "    console.log('" << message << "');\n";
    code << "    console.log('珂朵莉永远在大家心中 ❀');\n";
    code << "}, " << (duration * 1000) << ");\n";
    return code.str();
}

void INeverAwaySystem::defineCustomKey(const std::string& keyName, const std::string& keyType, const std::string& jsTemplate) {
    // 内在精妙：支持自定义键的状态区分
    std::string fullKey = keyName;
    if (!keyType.empty()) {
        fullKey += "<" + keyType + ">";
    }
    customKeys_[fullKey] = jsTemplate;
}

std::string INeverAwaySystem::processCustomKeys(const std::string& virObjectCode) {
    // 内在精妙：处理虚对象中的自定义键
    std::string result = virObjectCode;
    
    for (const auto& [key, jsTemplate] : customKeys_) {
        std::string placeholder = key + ":";
        size_t pos = result.find(placeholder);
        if (pos != std::string::npos) {
            // 替换自定义键为对应的JavaScript模板
            result.replace(pos, placeholder.length(), jsTemplate + ":");
        }
    }
    
    return result;
}

std::string PrintMyloveSystem::generateImageProcessor(const std::string& url, const std::string& mode, 
                                                    const std::string& width, const std::string& height, 
                                                    double scale) {
    std::ostringstream code;
    
    code << "printMylove({\n";
    code << "    url: " << processImageUrl(url) << ",\n";
    code << "    mode: " << validateMode(mode) << ",\n";
    code << "    width: " << processDimension(width) << ",\n";
    code << "    height: " << processDimension(height) << ",\n";
    code << "    scale: " << scale << "\n";
    code << "});\n";
    
    return code.str();
}

std::string PrintMyloveSystem::processImageUrl(const std::string& url) {
    if (url.find("http") == 0) {
        return "\"" + url + "\"";
    } else {
        return "\"./" + url + "\"";
    }
}

std::string PrintMyloveSystem::validateMode(const std::string& mode) {
    if (mode == "ASCII" || mode == "Pixel") {
        return "\"" + mode + "\"";
    }
    return "\"ASCII\"";  // 默认模式
}

std::string PrintMyloveSystem::processDimension(const std::string& dimension) {
    std::regex cssUnitRegex(R"(\d+(\.\d+)?(px|%|em|rem|vw|vh))");
    if (std::regex_match(dimension, cssUnitRegex)) {
        return "\"" + dimension + "\"";
    } else if (std::all_of(dimension.begin(), dimension.end(), ::isdigit)) {
        return "\"" + dimension + "px\"";
    } else {
        return "\"300px\"";  // 默认值
    }
}

std::string PrintMyloveSystem::generateASCIIConverter() {
    return R"(
function convertToASCII(ctx, scale) {
    const chars = '@%#*+=-:. ';
    const imageData = ctx.getImageData(0, 0, ctx.canvas.width, ctx.canvas.height);
    let result = '';
    
    for (let y = 0; y < imageData.height; y += Math.ceil(2 * scale)) {
        for (let x = 0; x < imageData.width; x += Math.ceil(scale)) {
            const offset = (y * imageData.width + x) * 4;
            const r = imageData.data[offset];
            const g = imageData.data[offset + 1];
            const b = imageData.data[offset + 2];
            const brightness = (r + g + b) / 3;
            const charIndex = Math.floor((brightness / 255) * (chars.length - 1));
            result += chars[charIndex];
        }
        result += '\n';
    }
    
    return result;
}
)";
}

std::string PrintMyloveSystem::generatePixelConverter() {
    return R"(
function convertToPixel(ctx, scale) {
    const imageData = ctx.getImageData(0, 0, ctx.canvas.width, ctx.canvas.height);
    let result = '';
    
    for (let y = 0; y < imageData.height; y += Math.ceil(2 * scale)) {
        for (let x = 0; x < imageData.width; x += Math.ceil(scale)) {
            const offset = (y * imageData.width + x) * 4;
            const r = imageData.data[offset];
            const g = imageData.data[offset + 1];
            const b = imageData.data[offset + 2];
            result += `█`;  // 使用像素块字符
        }
        result += '\n';
    }
    
    return result;
}
)";
}

} // namespace Chtholly

} // namespace CJMOD
} // namespace CHTL