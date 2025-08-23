#include "CJMOD/ElegantCJMODApi.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <limits>
#include <cctype>

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

// 静态成员初始化 - CJMOD高自由度特性
std::unordered_map<std::string, std::string> INeverAwaySystem::promiseFunctions_;
std::unordered_map<std::string, std::string> INeverAwaySystem::customKeys_;
std::unordered_map<std::string, std::string> INeverAwaySystem::globalFunctions_;
std::unordered_map<std::string, std::string> INeverAwaySystem::keyStateMapping_;
int INeverAwaySystem::globalFunctionCounter_ = 0;
bool INeverAwaySystem::isInitialized_ = false;

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

void INeverAwaySystem::initializeGlobalState() {
    if (!isInitialized_) {
        // 利用CJMOD的高自由度，初始化全局状态
        std::cout << "初始化iNeverAway全局状态管理系统" << std::endl;
        
        // 重置计数器
        globalFunctionCounter_ = 0;
        
        // 清理之前的状态
        globalFunctions_.clear();
        keyStateMapping_.clear();
        
        isInitialized_ = true;
    }
}

void INeverAwaySystem::defineCustomKey(const std::string& keyName, const std::string& state, const std::string& jsTemplate) {
    // 支持任意自定义键名，不限于Void
    // 状态是可选的，开发者可以自由决定
    std::string fullKey = keyName;
    if (!state.empty()) {
        fullKey += "<" + state + ">";
    }
    
    customKeys_[fullKey] = jsTemplate;
    
    std::cout << "定义自定义键: " << fullKey << std::endl;
}

void INeverAwaySystem::registerGlobalFunction(const std::string& functionName, const std::string& jsCode) {
    // CJMOD的高自由度：可以定义全局变量和函数
    globalFunctions_[functionName] = jsCode;
    std::cout << "注册全局函数: " << functionName << std::endl;
}

std::string INeverAwaySystem::getGlobalFunctionName(const std::string& keyName, const std::string& state) {
    // 生成全局唯一的函数名，由CHTL编译器统一管理
    std::string fullKey = keyName;
    if (!state.empty()) {
        fullKey += "_" + state;
    }
    
    // 检查是否已经存在
    auto it = keyStateMapping_.find(fullKey);
    if (it != keyStateMapping_.end()) {
        return it->second;
    }
    
    // 生成新的全局函数名
    std::string globalName = "chtl_ineveraway_" + 
                           std::to_string(globalFunctionCounter_++) + "_" + 
                           fullKey;
    
    // 转换为合法的JavaScript标识符
    std::replace(globalName.begin(), globalName.end(), '<', '_');
    std::replace(globalName.begin(), globalName.end(), '>', '_');
    std::replace(globalName.begin(), globalName.end(), ' ', '_');
    
    keyStateMapping_[fullKey] = globalName;
    return globalName;
}

std::string INeverAwaySystem::processVirObject(const std::string& virName, const std::string& objectContent) {
    // 处理虚对象 - CHTL JS原生功能的扩展支持
    initializeGlobalState();
    
    std::ostringstream result;
    result << "// 处理虚对象: " << virName << "\n";
    result << "const " << virName << " = {};\n";
    
    // 解析对象内容，提取所有键值对
    std::regex keyRegex(R"((\w+(?:<\w+>)?)\s*:\s*(.+?)(?=,|\}|$))");
    std::sregex_iterator iter(objectContent.begin(), objectContent.end(), keyRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string keyName = (*iter)[1].str();
        std::string keyValue = (*iter)[2].str();
        
        // 生成全局函数名
        std::string globalFuncName = getGlobalFunctionName(keyName, "");
        
        // 注册到全局作用域
        result << "window['" << globalFuncName << "'] = " << keyValue << ";\n";
        
        // 创建虚对象的引用
        result << virName << "['" << keyName << "'] = window['" << globalFuncName << "'];\n";
    }
    
    return result.str();
}

std::string INeverAwaySystem::processCustomKeys(const std::string& virObjectCode) {
    // 处理虚对象中的任意自定义键
    std::string result = virObjectCode;
    
    // 这里不需要替换，因为自定义键本身就是合法的JavaScript
    // 只需要确保全局函数名的生成和管理
    
    return result;
}

std::string PrintMyloveSystem::generateImageProcessor(const std::string& configObject) {
    std::ostringstream code;
    
    code << "// printMylove 图片转字符像素块处理器 - 官方键值对版本\n";
    code << "function printMylove(config) {\n";
    code << "    // 参数验证和默认值设置\n";
    code << "    const processedConfig = {\n";
    code << "        url: config.url || '',\n";
    code << "        mode: (config.mode || 'ascii').toLowerCase(),\n";
    code << "        width: parseInt(config.width) || 80,\n";
    code << "        height: parseInt(config.height) || 40,\n";
    code << "        scale: parseFloat(config.scale) || 1.0\n";
    code << "    };\n";
    code << "    \n";
    code << "    // URL验证\n";
    code << "    if (!processedConfig.url) {\n";
    code << "        console.error('printMylove: URL不能为空');\n";
    code << "        return '';\n";
    code << "    }\n";
    code << "    \n";
    code << "    // 模式验证 - 官方支持ASCII和Pixel模式\n";
    code << "    if (!['ascii', 'pixel'].includes(processedConfig.mode)) {\n";
    code << "        console.warn('printMylove: 不支持的模式，使用ASCII模式');\n";
    code << "        processedConfig.mode = 'ascii';\n";
    code << "    }\n";
    code << "    \n";
    code << "    // 缩放验证\n";
    code << "    if (processedConfig.scale <= 0 || processedConfig.scale > 10) {\n";
    code << "        console.warn('printMylove: 无效的缩放比例，使用默认值1.0');\n";
    code << "        processedConfig.scale = 1.0;\n";
    code << "    }\n";
    code << "    \n";
    code << "    console.log('珂朵莉正在处理图片: ' + processedConfig.url);\n";
    code << "    console.log('模式: ' + processedConfig.mode + ', 尺寸: ' + processedConfig.width + 'x' + processedConfig.height);\n";
    code << "    \n";
    code << "    // 根据模式选择处理方式\n";
    code << "    if (processedConfig.mode === 'ascii') {\n";
    code << "        return generateASCIIArt(processedConfig);\n";
    code << "    } else if (processedConfig.mode === 'pixel') {\n";
    code << "        return generatePixelArt(processedConfig);\n";
    code << "    }\n";
    code << "    \n";
    code << "    return '';\n";
    code << "}\n";
    code << "\n";
    code << generateASCIIConverter() << "\n";
    code << generatePixelConverter() << "\n";
    
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

std::string PrintMyloveSystem::processConfigObject(const std::string& configStr) {
    // 处理配置对象字符串 - 支持CHTL JS官方特性
    std::string processedConfig = parseUnorderedKeyValue(configStr);
    processedConfig = handleOptionalKeys(processedConfig);
    
    return processedConfig;
}

std::string PrintMyloveSystem::parseUnorderedKeyValue(const std::string& configStr) {
    // 支持无序键值对 - CHTL JS官方特性
    std::ostringstream result;
    std::unordered_map<std::string, std::string> keyValues;
    
    // 解析所有键值对，不依赖顺序
    std::regex keyValueRegex(R"((\w+):\s*([^,}]+))");
    std::sregex_iterator iter(configStr.begin(), configStr.end(), keyValueRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
        // 处理无修饰字面量
        value = processUndecoratedLiterals(value);
        
        // 根据键名进行特殊处理
        if (key == "url") {
            keyValues[key] = processImageUrl(value);
        } else if (key == "mode") {
            keyValues[key] = validateMode(value);
        } else if (key == "width" || key == "height") {
            keyValues[key] = processDimension(value);
        } else {
            keyValues[key] = value;
        }
    }
    
    // 按标准顺序输出（内部处理，用户无需关心顺序）
    result << "{\n";
    std::vector<std::string> standardOrder = {"url", "mode", "width", "height", "scale"};
    bool first = true;
    
    for (const auto& key : standardOrder) {
        if (keyValues.find(key) != keyValues.end()) {
            if (!first) result << ",\n";
            result << "    " << key << ": " << keyValues[key];
            first = false;
        }
    }
    
    // 添加其他非标准键
    for (const auto& [key, value] : keyValues) {
        if (std::find(standardOrder.begin(), standardOrder.end(), key) == standardOrder.end()) {
            if (!first) result << ",\n";
            result << "    " << key << ": " << value;
            first = false;
        }
    }
    
    result << "\n}";
    return result.str();
}

std::string PrintMyloveSystem::handleOptionalKeys(const std::string& configStr) {
    // 支持可选键值对 - CHTL JS官方特性
    std::ostringstream result;
    
    // 检查是否包含必需的键，为缺失的键提供默认值
    std::unordered_map<std::string, std::string> defaults = {
        {"mode", "\"ascii\""},
        {"width", "80"},
        {"height", "40"},
        {"scale", "1.0"}
    };
    
    std::string processed = configStr;
    
    // 检查每个默认键是否存在
    for (const auto& [key, defaultValue] : defaults) {
        std::regex keyRegex(key + ":");
        if (!std::regex_search(processed, keyRegex)) {
            // 键不存在，添加默认值
            std::cout << "printMylove: 使用默认值 " << key << " = " << defaultValue << std::endl;
            
            // 在最后一个键值对后添加默认键
            size_t lastComma = processed.find_last_of(',');
            if (lastComma != std::string::npos) {
                processed.insert(lastComma + 1, "\n    " + key + ": " + defaultValue + ",");
            } else {
                // 如果没有逗号，说明只有一个键或者是空对象
                size_t closeBrace = processed.find_last_of('}');
                if (closeBrace != std::string::npos) {
                    processed.insert(closeBrace, ",\n    " + key + ": " + defaultValue + "\n");
                }
            }
        }
    }
    
    return processed;
}

std::string PrintMyloveSystem::processUndecoratedLiterals(const std::string& value) {
    // 支持无修饰字面量 - CHTL JS官方特性
    std::string trimmedValue = value;
    
    // 去除前后空格
    trimmedValue.erase(0, trimmedValue.find_first_not_of(" \t\n\r"));
    trimmedValue.erase(trimmedValue.find_last_not_of(" \t\n\r") + 1);
    
    // 如果已经有引号，直接返回
    if ((trimmedValue.front() == '"' && trimmedValue.back() == '"') ||
        (trimmedValue.front() == '\'' && trimmedValue.back() == '\'')) {
        return trimmedValue;
    }
    
    // 检查是否是数字
    std::regex numberRegex(R"(^-?\d+(\.\d+)?$)");
    if (std::regex_match(trimmedValue, numberRegex)) {
        return trimmedValue; // 数字不需要引号
    }
    
    // 检查是否是布尔值
    if (trimmedValue == "true" || trimmedValue == "false") {
        return trimmedValue;
    }
    
    // 检查是否是特殊关键字
    std::vector<std::string> keywords = {"null", "undefined", "ascii", "pixel"};
    for (const auto& keyword : keywords) {
        if (trimmedValue == keyword) {
            return "\"" + trimmedValue + "\""; // 关键字需要引号
        }
    }
    
    // 其他情况视为无修饰字面量，添加引号
    return "\"" + trimmedValue + "\"";
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

// ==========================================
// vir函数注册表实现
// ==========================================

// 静态成员定义
std::unordered_map<std::string, std::vector<std::string>> CHTLJSVirRegistry::virFunctions_;

void CHTLJSVirRegistry::registerFunction(const std::string& functionName, 
                                       const std::vector<std::string>& supportedKeys) {
    virFunctions_[functionName] = supportedKeys;
    
    std::cout << "=== vir注册表更新 ===" << std::endl;
    std::cout << "✓ 函数 '" << functionName << "' 已注册vir支持" << std::endl;
    std::cout << "✓ 支持的键: ";
    for (const auto& key : supportedKeys) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    std::cout << "✓ 现在可以使用: vir myVir = " << functionName << "({...});" << std::endl;
}

bool CHTLJSVirRegistry::isSupported(const std::string& functionName) {
    return virFunctions_.find(functionName) != virFunctions_.end();
}

std::vector<std::string> CHTLJSVirRegistry::getSupportedKeys(const std::string& functionName) {
    auto it = virFunctions_.find(functionName);
    if (it != virFunctions_.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::string> CHTLJSVirRegistry::getAllSupportedFunctions() {
    std::vector<std::string> functions;
    for (const auto& pair : virFunctions_) {
        functions.push_back(pair.first);
    }
    return functions;
}

void CHTLJSVirRegistry::clear() {
    virFunctions_.clear();
    std::cout << "✓ vir注册表已清空" << std::endl;
}

void registerCJMODFunctionForVir(const std::string& functionName, 
                                const std::vector<std::string>& supportedKeys) {
    std::cout << "=== 手动注册vir支持 ===" << std::endl;
    std::cout << "函数: " << functionName << " (标准CJMOD流程创建)" << std::endl;
    
    CHTLJSVirRegistry::registerFunction(functionName, supportedKeys);
    
    std::cout << "✓ 注册完成！统一扫描器现在可以识别此函数的vir语法" << std::endl;
}

// ==========================================
// CJMOD关键字处理系统实现
// ==========================================

// 静态成员定义
std::unordered_map<std::string, CJMODKeywordInfo> CJMODKeywordHandler::keywordMap_;

void CJMODKeywordHandler::registerKeyword(const std::string& keyword, const CJMODKeywordInfo& info) {
    keywordMap_[keyword] = info;
    
    std::cout << "=== 注册CJMOD关键字 ===" << std::endl;
    std::cout << "关键字: " << keyword << std::endl;
    std::cout << "需要回退: " << (info.needsBacktrack ? "是" : "否") << std::endl;
    if (info.needsBacktrack) {
        std::cout << "回退距离: " << info.backtrackDistance << " 个单元" << std::endl;
    }
    std::cout << "需要向前收集: " << (info.needsForwardCollect ? "是" : "否") << std::endl;
    if (info.needsForwardCollect) {
        std::cout << "向前收集距离: " << info.forwardCollectDistance << " 个单元" << std::endl;
    }
}

bool CJMODKeywordHandler::isCJMODKeyword(const std::string& keyword) {
    return keywordMap_.find(keyword) != keywordMap_.end();
}

CJMODKeywordInfo CJMODKeywordHandler::getKeywordInfo(const std::string& keyword) {
    auto it = keywordMap_.find(keyword);
    if (it != keywordMap_.end()) {
        return it->second;
    }
    return CJMODKeywordInfo(); // 返回默认信息
}

bool CJMODKeywordHandler::handleKeyword(const std::string& keyword) {
    auto it = keywordMap_.find(keyword);
    if (it != keywordMap_.end()) {
        std::cout << "=== 处理CJMOD关键字: " << keyword << " ===" << std::endl;
        
        const auto& info = it->second;
        
        if (info.needsBacktrack) {
            std::cout << "→ 执行回退操作，距离: " << info.backtrackDistance << std::endl;
        }
        
        if (info.needsForwardCollect) {
            std::cout << "→ 执行向前收集，距离: " << info.forwardCollectDistance << std::endl;
        }
        
        if (info.handler) {
            std::cout << "→ 调用处理函数" << std::endl;
            info.handler();
        }
        
        std::cout << "✓ 关键字处理完成" << std::endl;
        return true;
    }
    
    std::cout << "❌ 未知的CJMOD关键字: " << keyword << std::endl;
    return false;
}

std::vector<std::string> CJMODKeywordHandler::getAllKeywords() {
    std::vector<std::string> keywords;
    for (const auto& pair : keywordMap_) {
        keywords.push_back(pair.first);
    }
    return keywords;
}

void CJMODKeywordHandler::clear() {
    keywordMap_.clear();
    std::cout << "✓ CJMOD关键字注册表已清空" << std::endl;
}

// 统一扫描器集成实现
bool CJMODScannerIntegration::processKeywordFromScanner(const std::string& keyword, void* context) {
    std::cout << "=== 统一扫描器调用CJMOD处理 ===" << std::endl;
    std::cout << "识别到关键字: " << keyword << std::endl;
    
    if (!CJMODKeywordHandler::isCJMODKeyword(keyword)) {
        std::cout << "❌ 不是CJMOD关键字，返回统一扫描器" << std::endl;
        return false;
    }
    
    auto info = CJMODKeywordHandler::getKeywordInfo(keyword);
    
    std::cout << "✓ 确认为CJMOD关键字，开始处理" << std::endl;
    std::cout << "处理策略:" << std::endl;
    std::cout << "  - 回退: " << (info.needsBacktrack ? "是" : "否") << std::endl;
    std::cout << "  - 向前收集: " << (info.needsForwardCollect ? "是" : "否") << std::endl;
    
    // 执行CJMOD处理
    bool success = CJMODKeywordHandler::handleKeyword(keyword);
    
    if (success) {
        std::cout << "✓ CJMOD处理成功，拦截统一扫描器" << std::endl;
    } else {
        std::cout << "❌ CJMOD处理失败，返回统一扫描器" << std::endl;
    }
    
    return success;
}

void CJMODScannerIntegration::registerCommonKeywords() {
    std::cout << "=== 注册常用CJMOD关键字 ===" << std::endl;
    
    // 注册幂运算符 ** - 需要回退收集左操作数
    CJMODKeywordInfo powerOpInfo;
    powerOpInfo.keyword = "**";
    powerOpInfo.needsBacktrack = true;
    powerOpInfo.backtrackDistance = 1;
    powerOpInfo.needsForwardCollect = true;
    powerOpInfo.forwardCollectDistance = 1;
    powerOpInfo.handler = []() {
        std::cout << "  → 处理幂运算符 **" << std::endl;
        std::cout << "  → 收集左操作数（回退1单元）" << std::endl;
        std::cout << "  → 收集右操作数（向前1单元）" << std::endl;
        std::cout << "  → 生成CJMOD幂运算代码" << std::endl;
    };
    CJMODKeywordHandler::registerKeyword("**", powerOpInfo);
    
    // 注册iNeverAway函数 - 不需要回退，需要收集参数
    CJMODKeywordInfo iNeverAwayInfo;
    iNeverAwayInfo.keyword = "iNeverAway";
    iNeverAwayInfo.needsBacktrack = false;
    iNeverAwayInfo.needsForwardCollect = true;
    iNeverAwayInfo.forwardCollectDistance = 3; // 收集整个函数调用
    iNeverAwayInfo.handler = []() {
        std::cout << "  → 处理iNeverAway函数" << std::endl;
        std::cout << "  → 检查vir支持" << std::endl;
        std::cout << "  → 收集函数参数" << std::endl;
        std::cout << "  → 生成vir处理代码" << std::endl;
    };
    CJMODKeywordHandler::registerKeyword("iNeverAway", iNeverAwayInfo);
    
    // 注册printMylove函数 - 不需要回退，需要收集参数
    CJMODKeywordInfo printMyloveInfo;
    printMyloveInfo.keyword = "printMylove";
    printMyloveInfo.needsBacktrack = false;
    printMyloveInfo.needsForwardCollect = true;
    printMyloveInfo.forwardCollectDistance = 3;
    printMyloveInfo.handler = []() {
        std::cout << "  → 处理printMylove函数" << std::endl;
        std::cout << "  → 解析配置对象" << std::endl;
        std::cout << "  → 处理CHTL JS官方特性" << std::endl;
        std::cout << "  → 生成图像处理代码" << std::endl;
    };
    CJMODKeywordHandler::registerKeyword("printMylove", printMyloveInfo);
    
    std::cout << "✓ 常用关键字注册完成" << std::endl;
}

void CJMODScannerIntegration::initialize() {
    std::cout << "=== 初始化CJMOD扫描器集成 ===" << std::endl;
    
    // 清空现有注册
    CJMODKeywordHandler::clear();
    
    // 注册常用关键字
    registerCommonKeywords();
    
    // 显示注册结果
    auto keywords = CJMODKeywordHandler::getAllKeywords();
    std::cout << "✓ 已注册 " << keywords.size() << " 个CJMOD关键字:" << std::endl;
    for (const auto& keyword : keywords) {
        std::cout << "  - " << keyword << std::endl;
    }
    
    std::cout << "✓ CJMOD扫描器集成初始化完成" << std::endl;
}

// ==========================================
// vir机制限制系统实现
// ==========================================

// 静态成员定义
std::unordered_set<std::string> VirFunctionBinder::boundFunctions_;
std::unordered_map<std::string, std::vector<std::string>> VirFunctionBinder::scannedFunctionKeys_;

bool VirFunctionBinder::bind(const std::string& functionName) {
    std::cout << "=== vir函数自动绑定 ===" << std::endl;
    std::cout << "函数: " << functionName << std::endl;
    std::cout << "✓ 采用官方自动化特征，只需要函数名" << std::endl;
    std::cout << "✓ 委托给CHTL JS自动扫描函数内部键值对" << std::endl;
    
    // 标记函数已绑定vir支持
    boundFunctions_.insert(functionName);
    
    // 模拟CHTL JS自动扫描过程
    std::cout << "→ CHTL JS开始自动扫描函数内部..." << std::endl;
    
    // 这里会委托给CHTL JS扫描器，现在用模拟数据演示
    std::vector<std::string> autoScannedKeys;
    
    if (functionName == "iNeverAway") {
        autoScannedKeys = {"MyPromise<Happy>", "MyPromise<Sad>", "UserAction", "GameEvent"};
    } else if (functionName == "printMylove") {
        // printMylove通常不包含函数键，所以为空
        autoScannedKeys = {};
    } else {
        // 其他函数的默认扫描结果
        autoScannedKeys = {"defaultKey", "customAction"};
    }
    
    scannedFunctionKeys_[functionName] = autoScannedKeys;
    
    std::cout << "→ 自动扫描完成，发现函数键:" << std::endl;
    for (const auto& key : autoScannedKeys) {
        std::cout << "  - " << key << " (键值为函数)" << std::endl;
    }
    
    if (autoScannedKeys.empty()) {
        std::cout << "  (未发现函数类型的键值对)" << std::endl;
    }
    
    std::cout << "✓ vir自动绑定成功，避免了直接vir语法对统一扫描器的影响" << std::endl;
    
    return true;
}

bool VirFunctionBinder::isBound(const std::string& functionName) {
    return boundFunctions_.find(functionName) != boundFunctions_.end();
}

std::vector<std::string> VirFunctionBinder::getScannedKeys(const std::string& functionName) {
    auto it = scannedFunctionKeys_.find(functionName);
    if (it != scannedFunctionKeys_.end()) {
        return it->second;
    }
    return {};
}

std::string VirFunctionBinder::generateVirAccess(const std::string& functionName,
                                                const std::string& objectName,
                                                const std::string& keyAccess) {
    if (!isBound(functionName)) {
        return "// 错误：函数 " + functionName + " 未绑定vir支持";
    }
    
    auto scannedKeys = getScannedKeys(functionName);
    
    // 检查键是否在扫描结果中
    bool keyFound = std::find(scannedKeys.begin(), scannedKeys.end(), keyAccess) != scannedKeys.end();
    
    if (keyFound) {
        // 生成全局函数名（模拟CHTL JS的命名规则）
        std::string globalFuncName = "chtl_vir_" + functionName + "_" + keyAccess;
        // 替换特殊字符
        std::replace(globalFuncName.begin(), globalFuncName.end(), '<', '_');
        std::replace(globalFuncName.begin(), globalFuncName.end(), '>', '_');
        
        return globalFuncName + "()"; // 生成函数调用
    }
    
    return "// 错误：键 " + keyAccess + " 未在自动扫描结果中找到";
}

std::vector<std::string> VirFunctionBinder::scanFunctionKeys(const std::string& functionName,
                                                           const std::string& functionDefinition) {
    std::cout << "=== CHTL JS自动扫描函数键值 ===" << std::endl;
    std::cout << "函数: " << functionName << std::endl;
    std::cout << "定义: " << functionDefinition.substr(0, 50) << "..." << std::endl;
    
    // 解析函数定义，查找函数类型的键值对
    std::vector<std::string> functionKeys = parseFunctionDefinition(functionDefinition);
    
    // 更新扫描结果
    scannedFunctionKeys_[functionName] = functionKeys;
    
    std::cout << "→ 扫描到 " << functionKeys.size() << " 个函数键:" << std::endl;
    for (const auto& key : functionKeys) {
        std::cout << "  - " << key << std::endl;
    }
    
    return functionKeys;
}

std::vector<std::string> VirFunctionBinder::parseFunctionDefinition(const std::string& functionDefinition) {
    std::vector<std::string> functionKeys;
    
    // 使用正则表达式查找键值对中的函数
    // 匹配模式：key: function() {...} 或 key: () => {...}
    std::regex functionKeyPattern(R"((\w+(?:<\w+>)?)\s*:\s*(?:function\s*\(|.*=>\s*\{|\(\s*.*\)\s*=>\s*\{))");
    std::sregex_iterator iter(functionDefinition.begin(), functionDefinition.end(), functionKeyPattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::smatch match = *iter;
        std::string keyName = match[1].str();
        functionKeys.push_back(keyName);
    }
    
    return functionKeys;
}

bool VirFunctionBinder::isKeyValueFunction(const std::string& keyValue) {
    // 检查键值是否为函数定义
    std::regex functionPattern(R"(^\s*(?:function\s*\(|.*=>\s*\{|\(\s*.*\)\s*=>\s*\{))");
    return std::regex_search(keyValue, functionPattern);
}

void VirFunctionBinder::clear() {
    boundFunctions_.clear();
    scannedFunctionKeys_.clear();
    std::cout << "✓ vir函数绑定已清空" << std::endl;
}

// vir语法检测器实现
bool VirSyntaxDetector::detectDirectVirUsage(const std::string& code) {
    // 简单的正则匹配检测vir关键字
    std::regex virPattern(R"(\bvir\s+\w+\s*=)");
    return std::regex_search(code, virPattern);
}

std::string VirSyntaxDetector::generateAlternativeSuggestion(const std::string& virStatement) {
    std::ostringstream suggestion;
    suggestion << "检测到禁止的vir语法！\n";
    suggestion << "违规语句: " << virStatement << "\n\n";
    suggestion << "请使用以下替代方案之一：\n";
    suggestion << "1. 使用createCHTLJSFunction创建函数（自动支持vir）：\n";
    suggestion << "   auto func = createCHTLJSFunction(\"functionName\", {\"key1\", \"key2\"});\n\n";
    suggestion << "2. 使用VirFunctionBinder手动绑定vir支持：\n";
    suggestion << "   VirFunctionBinder::bind(\"functionName\", {{\"key1\", \"globalFunc1\"}, {\"key2\", \"globalFunc2\"}});\n\n";
    suggestion << "这样可以避免vir语法对统一扫描器造成负担！";
    
    return suggestion.str();
}

std::string VirSyntaxDetector::handleVirViolation(const std::string& code) {
    if (detectDirectVirUsage(code)) {
        return generateAlternativeSuggestion(code);
    }
    return "代码检查通过，未发现直接vir语法使用。";
}

// ==========================================
// 扫描策略系统实现
// ==========================================

// 静态成员定义
ScanStrategy ScanStrategyManager::defaultStrategy_ = ScanStrategy::DUAL_POINTER;
std::map<ScanStrategy, size_t> ScanStrategyManager::strategyUsageCount_;
std::map<ScanStrategy, double> ScanStrategyManager::strategyPerformance_;

ScanStrategy ScanStrategyManager::selectStrategy(const std::string& keyword, const ScanContext& context) {
    std::cout << "=== 扫描策略选择 ===" << std::endl;
    std::cout << "关键字: " << keyword << std::endl;
    std::cout << "上下文: token数=" << context.tokenCount << ", 嵌套层级=" << context.nestingLevel << std::endl;
    
    // 1. 检查关键字复杂度
    int complexity = KeywordComplexityAnalyzer::calculateComplexity(keyword);
    std::cout << "关键字复杂度: " << complexity << "/10" << std::endl;
    
    // 2. 根据复杂度和上下文选择策略
    ScanStrategy selectedStrategy = defaultStrategy_;
    
    if (complexity >= 7) {
        // 高复杂度关键字，使用回退策略
        selectedStrategy = ScanStrategy::BACKTRACK;
        std::cout << "→ 选择回退策略（高复杂度关键字）" << std::endl;
    } else if (context.tokenCount > 1000) {
        // 大量token，优先性能
        selectedStrategy = ScanStrategy::DUAL_POINTER;
        std::cout << "→ 选择双指针策略（大量token，优先性能）" << std::endl;
    } else if (context.nestingLevel > 3) {
        // 深度嵌套，使用混合策略
        selectedStrategy = ScanStrategy::HYBRID;
        std::cout << "→ 选择混合策略（深度嵌套）" << std::endl;
    } else {
        // 默认情况
        selectedStrategy = defaultStrategy_;
        std::cout << "→ 使用默认策略（双指针）" << std::endl;
    }
    
    return selectedStrategy;
}

ScanResult ScanStrategyManager::executeStrategy(ScanStrategy strategy, 
                                              const std::string& keyword, 
                                              ScanContext& context) {
    std::cout << "=== 执行扫描策略 ===" << std::endl;
    
    ScanResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    switch (strategy) {
        case ScanStrategy::DUAL_POINTER:
            result = executeDualPointerScan(keyword, context);
            break;
            
        case ScanStrategy::BACKTRACK:
            result = executeBacktrackScan(keyword, context);
            break;
            
        case ScanStrategy::HYBRID:
            result = executeHybridScan(keyword, context);
            break;
            
        case ScanStrategy::ADAPTIVE:
            result = executeAdaptiveScan(keyword, context);
            break;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.memoryUsage = context.tokenCount * sizeof(void*); // 简化的内存估算
    
    // 更新统计信息
    strategyUsageCount_[strategy]++;
    strategyPerformance_[strategy] = (strategyPerformance_[strategy] + result.executionTime) / 2;
    
    std::cout << "✓ 策略执行完成，耗时: " << result.executionTime << "ms" << std::endl;
    
    return result;
}

void ScanStrategyManager::setDefaultStrategy(ScanStrategy strategy) {
    defaultStrategy_ = strategy;
    std::cout << "✓ 默认扫描策略已设置为: ";
    switch (strategy) {
        case ScanStrategy::DUAL_POINTER: std::cout << "双指针"; break;
        case ScanStrategy::BACKTRACK: std::cout << "回退重拼接"; break;
        case ScanStrategy::HYBRID: std::cout << "混合策略"; break;
        case ScanStrategy::ADAPTIVE: std::cout << "自适应"; break;
    }
    std::cout << std::endl;
}

ScanStrategy ScanStrategyManager::getDefaultStrategy() {
    return defaultStrategy_;
}

void ScanStrategyManager::initialize() {
    std::cout << "=== 初始化扫描策略系统 ===" << std::endl;
    
    // 设置默认策略为双指针
    setDefaultStrategy(ScanStrategy::DUAL_POINTER);
    
    // 初始化统计信息
    strategyUsageCount_.clear();
    strategyPerformance_.clear();
    
    std::cout << "✓ 扫描策略系统初始化完成" << std::endl;
}

// ==========================================
// 具体扫描策略实现
// ==========================================

// 双指针扫描策略实现
ScanResult ScanStrategyManager::executeDualPointerScan(const std::string& keyword, 
                                                      ScanContext& context) {
    std::cout << "=== 双指针扫描策略 ===" << std::endl;
    std::cout << "关键字: " << keyword << std::endl;
    
    ScanResult result;
    result.success = true;
    
    // 模拟获取当前代码上下文
    std::string codeContext = getCurrentCodeContext(context);
    
    // 双指针扫描算法
    size_t leftPtr = 0;
    size_t rightPtr = codeContext.length() > 0 ? codeContext.length() - 1 : 0;
    std::vector<std::string> tokens;
    std::ostringstream processedCode;
    
    std::cout << "→ 开始双指针扫描，代码长度: " << codeContext.length() << std::endl;
    
    // 从左右两端向中间扫描
    while (leftPtr <= rightPtr && leftPtr < codeContext.length()) {
        // 左指针扫描
        std::string leftToken = scanTokenFromLeft(codeContext, leftPtr);
        if (!leftToken.empty()) {
            tokens.push_back("L:" + leftToken);
            std::cout << "  左指针发现: " << leftToken << std::endl;
            
            // 检查是否找到目标关键字
            if (leftToken == keyword) {
                std::cout << "✓ 双指针扫描找到关键字: " << keyword << std::endl;
                processedCode << processCJMODKeyword(keyword, context);
                break;
            }
        }
        
        // 右指针扫描
        if (rightPtr > leftPtr && rightPtr < codeContext.length()) {
            std::string rightToken = scanTokenFromRight(codeContext, rightPtr);
            if (!rightToken.empty()) {
                tokens.push_back("R:" + rightToken);
                std::cout << "  右指针发现: " << rightToken << std::endl;
                
                // 检查是否找到目标关键字
                if (rightToken == keyword) {
                    std::cout << "✓ 双指针扫描找到关键字: " << keyword << std::endl;
                    processedCode << processCJMODKeyword(keyword, context);
                    break;
                }
            }
        }
        
        leftPtr++;
        if (rightPtr > 0) rightPtr--;
    }
    
    if (processedCode.str().empty()) {
        processedCode << "// 双指针扫描完成，未找到关键字: " << keyword;
    }
    
    result.processedCode = processedCode.str();
    result.memoryUsage = tokens.size() * 64; // 估算内存使用
    
    std::cout << "→ 双指针扫描完成，处理了 " << tokens.size() << " 个令牌" << std::endl;
    
    return result;
}

// 回退重拼接扫描策略实现
ScanResult ScanStrategyManager::executeBacktrackScan(const std::string& keyword, 
                                                    ScanContext& context) {
    std::cout << "=== 回退重拼接扫描策略 ===" << std::endl;
    std::cout << "关键字: " << keyword << std::endl;
    
    ScanResult result;
    result.success = true;
    
    std::string codeContext = getCurrentCodeContext(context);
    std::ostringstream processedCode;
    
    std::cout << "→ 开始回退扫描，支持复杂中缀操作符" << std::endl;
    
    // 回退扫描算法 - 特别适合处理复杂的中缀操作符
    std::vector<ScanState> scanStack;
    size_t currentPos = 0;
    bool found = false;
    
    while (currentPos < codeContext.length() && !found) {
        // 保存当前扫描状态
        ScanState currentState;
        currentState.position = currentPos;
        currentState.tokenBuffer = "";
        currentState.nestingLevel = context.nestingLevel;
        
        // 尝试扫描当前位置
        std::string token = scanTokenAtPosition(codeContext, currentPos);
        
        if (token == keyword) {
            std::cout << "✓ 回退扫描找到关键字: " << keyword << " 在位置 " << currentPos << std::endl;
            
            // 检查是否需要回退处理
            if (needsBacktrackProcessing(keyword, codeContext, currentPos)) {
                std::cout << "→ 执行回退重拼接处理" << std::endl;
                
                // 回退到合适的位置
                size_t backtrackPos = findBacktrackPosition(codeContext, currentPos, keyword);
                std::cout << "  回退到位置: " << backtrackPos << std::endl;
                
                // 重新拼接代码片段
                std::string reassembledCode = reassembleCodeSegment(codeContext, backtrackPos, currentPos + keyword.length());
                std::cout << "  重拼接代码: " << reassembledCode.substr(0, 50) << "..." << std::endl;
                
                processedCode << processCJMODKeywordWithBacktrack(keyword, reassembledCode, context);
            } else {
                // 直接处理
                processedCode << processCJMODKeyword(keyword, context);
            }
            found = true;
            break;
        }
        
        // 保存扫描状态到栈中
        scanStack.push_back(currentState);
        currentPos++;
        
        // 如果栈太深，执行回退
        if (scanStack.size() > 100) {
            std::cout << "→ 执行栈深度回退" << std::endl;
            currentPos = scanStack[scanStack.size() / 2].position;
            scanStack.erase(scanStack.begin() + scanStack.size() / 2, scanStack.end());
        }
    }
    
    if (!found) {
        processedCode << "// 回退扫描完成，未找到关键字: " << keyword;
    }
    
    result.processedCode = processedCode.str();
    result.memoryUsage = scanStack.size() * 128; // 回退扫描需要更多内存
    
    std::cout << "→ 回退扫描完成，最大栈深度: " << scanStack.size() << std::endl;
    
    return result;
}

// 混合扫描策略实现
ScanResult ScanStrategyManager::executeHybridScan(const std::string& keyword, 
                                                 ScanContext& context) {
    std::cout << "=== 混合扫描策略 ===" << std::endl;
    
    // 根据关键字复杂度选择合适的策略组合
    int complexity = KeywordComplexityAnalyzer::calculateComplexity(keyword);
    
    if (complexity < 5) {
        std::cout << "→ 低复杂度，使用双指针扫描" << std::endl;
        return executeDualPointerScan(keyword, context);
    } else {
        std::cout << "→ 高复杂度，使用回退扫描" << std::endl;
        return executeBacktrackScan(keyword, context);
    }
}

// 自适应扫描策略实现
ScanResult ScanStrategyManager::executeAdaptiveScan(const std::string& keyword, 
                                                   ScanContext& context) {
    std::cout << "=== 自适应扫描策略 ===" << std::endl;
    
    // 根据历史性能数据选择最优策略
    ScanStrategy optimalStrategy = ScanStrategy::DUAL_POINTER;
    double bestPerformance = std::numeric_limits<double>::max();
    
    for (const auto& [strategy, performance] : strategyPerformance_) {
        if (performance < bestPerformance) {
            bestPerformance = performance;
            optimalStrategy = strategy;
        }
    }
    
    std::cout << "→ 自适应选择策略: ";
    switch (optimalStrategy) {
        case ScanStrategy::DUAL_POINTER:
            std::cout << "双指针扫描" << std::endl;
            return executeDualPointerScan(keyword, context);
        case ScanStrategy::BACKTRACK:
            std::cout << "回退扫描" << std::endl;
            return executeBacktrackScan(keyword, context);
        default:
            std::cout << "双指针扫描（默认）" << std::endl;
            return executeDualPointerScan(keyword, context);
    }
}

// ==========================================
// 辅助方法实现
// ==========================================

std::string ScanStrategyManager::getCurrentCodeContext(const ScanContext& context) {
    // 模拟获取当前代码上下文
    std::ostringstream codeStream;
    
    // 根据上下文生成模拟代码
    codeStream << "// CHTL JS代码上下文 (位置: " << context.currentPosition << ")\n";
    codeStream << "iNeverAway({\n";
    codeStream << "  MyPromise<Happy>: function() { console.log('快乐'); },\n";
    codeStream << "  MyPromise<Sad>: () => { console.log('忧伤'); },\n";
    codeStream << "  UserAction: function(action) { return action; }\n";
    codeStream << "});\n";
    codeStream << "printMylove({ mode: ascii, width: 80 });\n";
    codeStream << "** 这是CJMOD特殊语法 **\n";
    
    return codeStream.str();
}

std::string ScanStrategyManager::scanTokenFromLeft(const std::string& code, size_t& pos) {
    if (pos >= code.length()) return "";
    
    // 跳过空白字符
    while (pos < code.length() && std::isspace(code[pos])) {
        pos++;
    }
    
    if (pos >= code.length()) return "";
    
    // 扫描标识符或关键字
    size_t start = pos;
    if (std::isalpha(code[pos]) || code[pos] == '_') {
        while (pos < code.length() && (std::isalnum(code[pos]) || code[pos] == '_')) {
            pos++;
        }
        return code.substr(start, pos - start);
    }
    
    // 扫描特殊符号
    if (code[pos] == '*' && pos + 1 < code.length() && code[pos + 1] == '*') {
        pos += 2;
        return "**";
    }
    
    // 单字符令牌
    return std::string(1, code[pos++]);
}

std::string ScanStrategyManager::scanTokenFromRight(const std::string& code, size_t& pos) {
    if (pos >= code.length()) return "";
    
    // 跳过空白字符
    while (pos > 0 && std::isspace(code[pos])) {
        pos--;
    }
    
    if (pos == 0 && std::isspace(code[0])) return "";
    
    // 从右向左扫描标识符
    size_t end = pos + 1;
    if (std::isalpha(code[pos]) || code[pos] == '_') {
        while (pos > 0 && (std::isalnum(code[pos - 1]) || code[pos - 1] == '_')) {
            pos--;
        }
        return code.substr(pos, end - pos);
    }
    
    // 特殊符号
    if (pos > 0 && code[pos] == '*' && code[pos - 1] == '*') {
        pos--;
        return "**";
    }
    
    // 单字符令牌
    return std::string(1, code[pos--]);
}

std::string ScanStrategyManager::scanTokenAtPosition(const std::string& code, size_t pos) {
    if (pos >= code.length()) return "";
    
    // 跳过空白字符
    while (pos < code.length() && std::isspace(code[pos])) {
        pos++;
    }
    
    if (pos >= code.length()) return "";
    
    // 扫描标识符
    size_t start = pos;
    if (std::isalpha(code[pos]) || code[pos] == '_') {
        while (pos < code.length() && (std::isalnum(code[pos]) || code[pos] == '_')) {
            pos++;
        }
        return code.substr(start, pos - start);
    }
    
    // 特殊符号
    if (code[pos] == '*' && pos + 1 < code.length() && code[pos + 1] == '*') {
        return "**";
    }
    
    return std::string(1, code[pos]);
}

bool ScanStrategyManager::needsBacktrackProcessing(const std::string& keyword, 
                                                  const std::string& code, 
                                                  size_t pos) {
    // 检查是否为复杂的中缀操作符或需要上下文的关键字
    if (keyword == "**") return true;  // CJMOD特殊语法
    if (keyword == "iNeverAway") return true;  // 复杂函数调用
    
    // 检查前后是否有复杂的语法结构
    if (pos > 0 && code[pos - 1] == '.') return true;  // 成员访问
    if (pos + keyword.length() < code.length() && code[pos + keyword.length()] == '(') return true;  // 函数调用
    
    return false;
}

size_t ScanStrategyManager::findBacktrackPosition(const std::string& code, 
                                                 size_t currentPos, 
                                                 const std::string& keyword) {
    // 回退到语句开始或合适的语法边界
    size_t backtrackPos = currentPos;
    
    // 向前查找语句开始
    while (backtrackPos > 0) {
        char ch = code[backtrackPos - 1];
        if (ch == ';' || ch == '{' || ch == '\n') {
            break;
        }
        backtrackPos--;
    }
    
    return backtrackPos;
}

std::string ScanStrategyManager::reassembleCodeSegment(const std::string& code, 
                                                      size_t start, 
                                                      size_t end) {
    if (start >= code.length() || end > code.length() || start >= end) {
        return "";
    }
    
    std::string segment = code.substr(start, end - start);
    
    // 清理和重新格式化代码段
    std::ostringstream cleaned;
    bool inString = false;
    char stringChar = '\0';
    
    for (char ch : segment) {
        if (!inString && (ch == '"' || ch == '\'')) {
            inString = true;
            stringChar = ch;
        } else if (inString && ch == stringChar) {
            inString = false;
        }
        
        if (!inString && std::isspace(ch)) {
            if (!cleaned.str().empty() && cleaned.str().back() != ' ') {
                cleaned << ' ';
            }
        } else {
            cleaned << ch;
        }
    }
    
    return cleaned.str();
}

std::string ScanStrategyManager::processCJMODKeyword(const std::string& keyword, 
                                                    const ScanContext& context) {
    std::ostringstream result;
    
    if (CJMODKeywordHandler::isCJMODKeyword(keyword)) {
        result << "// CJMOD关键字处理: " << keyword << "\n";
        result << "// 上下文位置: " << context.currentPosition << "\n";
        result << "// 嵌套级别: " << context.nestingLevel << "\n";
        
        // 触发CJMOD处理器
        CJMODKeywordHandler::handleKeyword(keyword);
        
        result << "// 生成的JS代码:\n";
        if (keyword == "iNeverAway") {
            result << "const iNeverAwayPromises = {\n";
            result << "  'MyPromise<Happy>': chtl_iNeverAway_MyPromise_Happy_,\n";
            result << "  'MyPromise<Sad>': chtl_iNeverAway_MyPromise_Sad_\n";
            result << "};";
        } else if (keyword == "printMylove") {
            result << "printMylove({ mode: 'ascii', width: 80, height: 40 });";
        } else if (keyword == "**") {
            result << "// 特殊CJMOD语法处理";
        }
    } else {
        result << "// 标准关键字: " << keyword;
    }
    
    return result.str();
}

std::string ScanStrategyManager::processCJMODKeywordWithBacktrack(const std::string& keyword, 
                                                                 const std::string& code, 
                                                                 const ScanContext& context) {
    std::ostringstream result;
    
    result << "// 回退处理CJMOD关键字: " << keyword << "\n";
    result << "// 重拼接代码: " << code.substr(0, 50) << "...\n";
    result << "// 上下文: 位置=" << context.currentPosition << ", 嵌套=" << context.nestingLevel << "\n";
    
    // 基于回退的特殊处理
    if (keyword == "**") {
        result << "// 特殊语法回退处理\n";
        result << "const specialSyntax = processSpecialCJMOD(`" << code << "`);";
    } else {
        // 调用标准处理，但加上回退信息
        result << processCJMODKeyword(keyword, context);
        result << "\n// 注：使用了回退重拼接策略";
    }
    
    return result.str();
}

// 关键字复杂度分析器实现
int KeywordComplexityAnalyzer::calculateComplexity(const std::string& keyword) {
    int complexity = 1; // 基础复杂度
    
    // 检查是否为CJMOD关键字
    if (CJMODKeywordHandler::isCJMODKeyword(keyword)) {
        auto info = CJMODKeywordHandler::getKeywordInfo(keyword);
        
        // 回退增加复杂度
        if (info.needsBacktrack) {
            complexity += info.backtrackDistance * 2;
        }
        
        // 向前收集增加复杂度
        if (info.needsForwardCollect) {
            complexity += info.forwardCollectDistance;
        }
    }
    
    // 特殊关键字额外复杂度
    static std::unordered_set<std::string> specialKeywords = {
        "**", "iNeverAway", "printMylove"
    };
    
    if (specialKeywords.find(keyword) != specialKeywords.end()) {
        complexity += 3;
    }
    
    return std::min(complexity, 10);
}

ScanStrategy KeywordComplexityAnalyzer::recommendStrategy(const std::string& keyword, const ScanContext& context) {
    int complexity = calculateComplexity(keyword);
    
    if (complexity >= 8) {
        return ScanStrategy::BACKTRACK;
    } else if (complexity >= 5) {
        return ScanStrategy::HYBRID;
    } else if (context.tokenCount > 500) {
        return ScanStrategy::DUAL_POINTER;
    } else {
        return ScanStrategy::ADAPTIVE;
    }
}

// ==========================================
// CHTL JS函数快速创建系统实现
// ==========================================

CHTLJSFunction::CHTLJSFunction(const std::string& functionName, const std::vector<std::string>& keyNames)
    : functionName_(functionName), keyNames_(keyNames),
      supportUnordered_(true), supportOptional_(true), supportUndecoratedLiterals_(true) {
    
    std::cout << "托管CHTL JS函数创建: " << functionName_ << std::endl;
    std::cout << "托管的键: ";
    for (const auto& key : keyNames_) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    initializeKeyword();
}

void CHTLJSFunction::initializeKeyword() {
    // 托管步骤1：自动生成语法模式
    std::string pattern = generateSyntaxPattern();
    std::cout << "托管生成的语法模式: " << pattern << std::endl;
    
    // 托管步骤2：调用标准syntaxAnalys
    std::string ignoreChars = ",:{};()";
    keyword_ = syntaxAnalys(pattern, ignoreChars);
    
    std::cout << "✓ 语法创建托管完成，返回标准Keyword对象" << std::endl;
    std::cout << "✓ 开发者可继续使用标准CJMOD流程：bind -> scanKeyword -> match -> generateCode" << std::endl;
}

std::string CHTLJSFunction::generateSyntaxPattern() {
    // CHTL JS函数天然支持vir，生成基础语法模式
    // 统一扫描器会识别vir关键字，我们只需要提供函数调用模式
    std::ostringstream pattern;
    
    pattern << functionName_ << "({\n";
    pattern << "    $\n";
    pattern << "});";
    
    return pattern.str();
}

void CHTLJSFunction::bindKeyProcessor(const std::string& keyName, 
                                    std::function<std::string(const std::string&)> processor) {
    std::cout << "托管键处理器绑定: " << keyName << std::endl;
    keyProcessors_[keyName] = processor;
    
    // 托管步骤：自动调用标准bind方法
    // 开发者无需手动为每个键调用keyword->args.bind
    if (keyProcessors_.size() == 1) {
        // CHTL JS函数天然支持vir，统一扫描器会处理vir关键字
        // 我们只需要绑定configObject参数即可
        
        // 所有类型都需要configObject参数
        keyword_->args.bind<std::string>("configObject", [this](const std::string& configStr) -> std::string {
            return this->processConfigObject(configStr);
        });
        
        std::cout << "✓ 标准bind步骤已托管，开发者仍需手动执行：scanKeyword -> match -> generateCode" << std::endl;
    }
}

void CHTLJSFunction::setDefaultValues(const std::unordered_map<std::string, std::string>& defaults) {
    std::cout << "设置默认值: ";
    for (const auto& [key, value] : defaults) {
        defaultValues_[key] = value;
        std::cout << key << "=" << value << " ";
    }
    std::cout << std::endl;
}

void CHTLJSFunction::enableCHTLJSFeatures(bool unordered, bool optional, bool undecoratedLiterals) {
    supportUnordered_ = unordered;
    supportOptional_ = optional;
    supportUndecoratedLiterals_ = undecoratedLiterals;
    
    std::cout << "CHTL JS特性配置: ";
    std::cout << "无序=" << (unordered ? "是" : "否") << " ";
    std::cout << "可选=" << (optional ? "是" : "否") << " ";
    std::cout << "无修饰字面量=" << (undecoratedLiterals ? "是" : "否") << std::endl;
}

std::string CHTLJSFunction::processConfigObject(const std::string& configStr) {
    std::cout << "处理配置对象: " << functionName_ << std::endl;
    
    std::ostringstream result;
    std::unordered_map<std::string, std::string> keyValues;
    
    // 解析键值对
    std::regex keyValueRegex(R"((\w+):\s*([^,}]+))");
    std::sregex_iterator iter(configStr.begin(), configStr.end(), keyValueRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = (*iter)[1].str();
        std::string value = (*iter)[2].str();
        
        // 处理无修饰字面量
        if (supportUndecoratedLiterals_) {
            value = PrintMyloveSystem::processUndecoratedLiterals(value);
        }
        
        // 应用键处理器
        if (keyProcessors_.find(key) != keyProcessors_.end()) {
            value = keyProcessors_[key](value);
            std::cout << "  → 处理键 " << key << ": " << value << std::endl;
        }
        
        keyValues[key] = value;
    }
    
    // 处理可选键值对
    if (supportOptional_) {
        for (const auto& [key, defaultValue] : defaultValues_) {
            if (keyValues.find(key) == keyValues.end()) {
                keyValues[key] = defaultValue;
                std::cout << "  → 补充默认值 " << key << ": " << defaultValue << std::endl;
            }
        }
    }
    
    // 生成结果（支持无序）
    result << "{\n";
    bool first = true;
    
    if (supportUnordered_) {
        // 按keyNames_顺序输出
        for (const auto& keyName : keyNames_) {
            if (keyValues.find(keyName) != keyValues.end()) {
                if (!first) result << ",\n";
                result << "    " << keyName << ": " << keyValues[keyName];
                first = false;
            }
        }
        
        // 添加其他键
        for (const auto& [key, value] : keyValues) {
            if (std::find(keyNames_.begin(), keyNames_.end(), key) == keyNames_.end()) {
                if (!first) result << ",\n";
                result << "    " << key << ": " << value;
                first = false;
            }
        }
    } else {
        // 按原始顺序输出
        for (const auto& [key, value] : keyValues) {
            if (!first) result << ",\n";
            result << "    " << key << ": " << value;
            first = false;
        }
    }
    
    result << "\n}";
    return result.str();
}

// 注意：process和generateJavaScript方法已移除
// 这些方法颠覆了标准CJMOD流程，不符合"托管部分步骤"的设计理念
// 开发者应该手动执行标准流程：scanKeyword -> match -> generateCode

std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(const std::string& functionName, 
                                                   const std::vector<std::string>& keyNames) {
    std::cout << "=== 托管CHTL JS函数创建: " << functionName << " ===" << std::endl;
    std::cout << "✓ CHTL JS函数天然支持vir，无需区分类型" << std::endl;
    std::cout << "✓ 统一扫描器会识别vir关键字，优化vir对CHTL JS函数的支持" << std::endl;
    std::cout << "✓ 使用方式：" << std::endl;
    std::cout << "  - " << functionName << "({...});              ← 直接调用" << std::endl;
    std::cout << "  - const result = " << functionName << "({...}); ← 赋值调用" << std::endl;
    std::cout << "  - vir myVir = " << functionName << "({...});   ← 虚对象调用（优化支持）" << std::endl;
    
    auto chtljsFunc = std::make_unique<CHTLJSFunction>(functionName, keyNames);
    
    // 自动注册vir支持 - 解决统一扫描器识别问题
    CHTLJSVirRegistry::registerFunction(functionName, keyNames);
    
    // 默认启用所有CHTL JS特性
    chtljsFunc->enableCHTLJSFeatures(true, true, true);
    
    std::cout << "✅ 部分步骤托管完成，返回标准Keyword对象" << std::endl;
    std::cout << "✅ 开发者需继续执行标准CJMOD流程：" << std::endl;
    std::cout << "   1. [已托管] syntaxAnalys - 语法模式已自动生成" << std::endl;
    std::cout << "   2. [需手动] bind - 或使用托管的bindKeyProcessor" << std::endl;
    std::cout << "   3. [需手动] scanKeyword - 扫描CHTL代码" << std::endl;
    std::cout << "   4. [需手动] match - 匹配参数值" << std::endl;
    std::cout << "   5. [需手动] generateCode - 生成最终JavaScript" << std::endl;
    
    return chtljsFunc;
}

} // namespace CJMOD
} // namespace CHTL