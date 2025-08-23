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

} // namespace CJMOD
} // namespace CHTL