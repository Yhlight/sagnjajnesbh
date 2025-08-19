#include "ProfessionalCJMODInterface.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cctype>

namespace chtl {
namespace cjmod {

// 全局扫描器实例
ProfessionalScanner professionalScanner;

// ========================================
// AdvancedArg实现
// ========================================

AdvancedArg::AdvancedArg(const std::string& content, const std::string& name)
    : content(content), name(name), isPlaceholder(content == "$"), 
      placeholderIndex(-1), isResultCached(false) {
    
    if (name.empty()) {
        resolvedName = (content == "$") ? "placeholder" : content;
    } else {
        resolvedName = name;
    }
}

void AdvancedArg::bind(std::function<std::string(const std::string&)> processor) {
    bindProcessor = processor;
}

void AdvancedArg::bind(std::function<std::string()> processor) {
    bindProcessor = [processor](const std::string&) { return processor(); };
}

void AdvancedArg::transform(const std::string& template_str) {
    transformTemplate = template_str;
}

void AdvancedArg::transform(std::function<std::string(const std::string&)> transformer) {
    transformProcessor = transformer;
}

void AdvancedArg::transformIf(std::function<bool(const std::string&)> condition,
                              std::function<std::string(const std::string&)> transformer) {
    conditionalTransforms.emplace_back(condition, transformer);
}

void AdvancedArg::setValidator(std::function<bool(const std::string&)> validatorFunc) {
    validator = validatorFunc;
}

std::string AdvancedArg::match(const std::string& value) {
    if (isResultCached) {
        return cachedResult;
    }
    
    std::string result = value;
    
    // 1. 数据类型推断
    inferDataType(result);
    
    // 2. 验证（如果设置了验证器）
    if (validator && !validator(result)) {
        return "/* 验证失败: " + resolvedName + " */";
    }
    
    // 3. 应用bind预处理
    if (bindProcessor) {
        result = bindProcessor(result);
    }
    
    // 4. 应用条件转换
    result = applyConditionalTransforms(result);
    
    // 5. 应用主要转换
    if (transformProcessor) {
        result = transformProcessor(result);
    } else if (!transformTemplate.empty()) {
        result = applyTemplate(transformTemplate, result);
    }
    
    // 6. 缓存结果
    cachedResult = result;
    isResultCached = true;
    
    return result;
}

void AdvancedArg::clearCache() {
    isResultCached = false;
    cachedResult.clear();
}

void AdvancedArg::inferDataType(const std::string& value) {
    if (isObject(value)) {
        dataType = "object";
    } else if (isFunction(value)) {
        dataType = "function";
    } else if (isCHTLJSSelector(value)) {
        dataType = "selector";
    } else if (isNumber(value)) {
        dataType = "number";
    } else if (isString(value)) {
        dataType = "string";
    } else {
        dataType = "unknown";
    }
}

std::string AdvancedArg::applyTemplate(const std::string& template_str, const std::string& value) {
    std::string result = template_str;
    
    // 支持多种模板占位符
    std::vector<std::pair<std::string, std::string>> replacements = {
        {"${VALUE}", value},
        {"$VALUE", value},
        {"${ARG}", value},
        {"${NAME}", resolvedName},
        {"${TYPE}", dataType}
    };
    
    for (const auto& [placeholder, replacement] : replacements) {
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), replacement);
            pos += replacement.length();
        }
    }
    
    return result;
}

std::string AdvancedArg::applyConditionalTransforms(const std::string& value) {
    for (const auto& [condition, transformer] : conditionalTransforms) {
        if (condition(value)) {
            return transformer(value);
        }
    }
    return value;
}

// ========================================
// ProfessionalArgsManager实现
// ========================================

AdvancedArg& ProfessionalArgsManager::operator[](int index) {
    static std::unique_ptr<AdvancedArg> emptyArg = std::make_unique<AdvancedArg>("", "");
    
    if (index >= 0 && index < args.size()) {
        return *args[index];
    }
    return *emptyArg;
}

const AdvancedArg& ProfessionalArgsManager::operator[](int index) const {
    static std::unique_ptr<AdvancedArg> emptyArg = std::make_unique<AdvancedArg>("", "");
    
    if (index >= 0 && index < args.size()) {
        return *args[index];
    }
    return *emptyArg;
}

void ProfessionalArgsManager::addArg(const std::string& content, const std::string& name) {
    auto arg = std::make_unique<AdvancedArg>(content, name);
    
    if (arg->isPlaceholder) {
        placeholderCount++;
        arg->placeholderIndex = placeholderCount - 1;
    } else {
        namedArgCount++;
    }
    
    args.push_back(std::move(arg));
    totalArgs++;
    
    updateStatistics();
}

AdvancedArg* ProfessionalArgsManager::findArg(const std::string& name) {
    for (auto& arg : args) {
        if (arg->name == name || arg->resolvedName == name || arg->content == name) {
            return arg.get();
        }
    }
    return nullptr;
}

const AdvancedArg* ProfessionalArgsManager::findArg(const std::string& name) const {
    for (const auto& arg : args) {
        if (arg->name == name || arg->resolvedName == name || arg->content == name) {
            return arg.get();
        }
    }
    return nullptr;
}

void ProfessionalArgsManager::bind(const std::string& name, std::function<std::string(const std::string&)> processor) {
    if (name == "$") {
        // 占位符自动计数绑定
        for (auto& arg : args) {
            if (arg->isPlaceholder && !arg->bindProcessor) {
                arg->bind(processor);
                return;
            }
        }
    } else {
        auto* arg = findArg(name);
        if (arg) {
            arg->bind(processor);
        }
    }
}

void ProfessionalArgsManager::bind(int index, std::function<std::string(const std::string&)> processor) {
    if (index >= 0 && index < args.size()) {
        args[index]->bind(processor);
    }
}

void ProfessionalArgsManager::transform(const std::string& name, const std::string& template_str) {
    auto* arg = findArg(name);
    if (arg) {
        arg->transform(template_str);
    }
}

void ProfessionalArgsManager::transform(const std::string& name, std::function<std::string(const std::string&)> transformer) {
    auto* arg = findArg(name);
    if (arg) {
        arg->transform(transformer);
    }
}

void ProfessionalArgsManager::transform(int index, std::function<std::string(const std::string&)> transformer) {
    if (index >= 0 && index < args.size()) {
        args[index]->transform(transformer);
    }
}

void ProfessionalArgsManager::match(const std::string& name, const std::string& value) {
    auto* arg = findArg(name);
    if (arg) {
        finalResults[name] = arg->match(value);
    }
}

void ProfessionalArgsManager::match(int index, const std::string& value) {
    if (index >= 0 && index < args.size()) {
        std::string name = args[index]->resolvedName;
        finalResults[name] = args[index]->match(value);
    }
}

void ProfessionalArgsManager::slice(const std::string& name, int start, int length, 
                                   std::function<std::string(const std::string&)> processor) {
    auto it = finalResults.find(name);
    if (it != finalResults.end()) {
        std::string content = it->second;
        std::string sliced = chtl::cjmod::slice(content, start, length);
        finalResults[name] = processor(sliced);
    }
}

void ProfessionalArgsManager::sliceFunctionBody(const std::string& name, 
                                                std::function<std::string(const std::string&)> processor) {
    auto it = finalResults.find(name);
    if (it != finalResults.end()) {
        std::string sliced = sliceFunction(it->second);
        finalResults[name] = processor(sliced);
    }
}

std::string ProfessionalArgsManager::result() {
    std::string code = "";
    
    // 按处理顺序生成代码
    if (!processingOrder.empty()) {
        for (const auto& name : processingOrder) {
            auto it = finalResults.find(name);
            if (it != finalResults.end()) {
                code += it->second + "\n";
            }
        }
    } else {
        // 默认顺序
        for (const auto& arg : args) {
            std::string name = arg->resolvedName;
            auto it = finalResults.find(name);
            if (it != finalResults.end()) {
                code += it->second + "\n";
            }
        }
    }
    
    return code;
}

void ProfessionalArgsManager::updateStatistics() {
    // 更新统计信息
    placeholderCount = 0;
    namedArgCount = 0;
    
    for (const auto& arg : args) {
        if (arg->isPlaceholder) {
            placeholderCount++;
        } else {
            namedArgCount++;
        }
    }
}

// ========================================
// ProfessionalScanner实现
// ========================================

ProfessionalScanner::ProfessionalScanner() {
    // 初始化
}

void ProfessionalScanner::initialize(std::shared_ptr<chtl::scanner::CHTLUnifiedScanner> scanner) {
    unifiedScanner = scanner;
}

void ProfessionalScanner::scanKeyword(const std::string& keyword, std::function<void()> callback) {
    keywordCallbacks[keyword] = callback;
}

void ProfessionalScanner::scanKeyword(const AdvancedArg& keywordArg, std::function<void()> callback) {
    keywordCallbacks[keywordArg.content] = callback;
}

std::string ProfessionalScanner::peekKeyword(int offset) {
    if (currentTokenIndex + offset >= 0 && currentTokenIndex + offset < tokenHistory.size()) {
        return tokenHistory[currentTokenIndex + offset];
    }
    
    // 如果没有历史记录，返回模拟值
    return "token" + std::to_string(offset);
}

void ProfessionalScanner::policyChangeBegin(const std::string& trigger, Policy policy) {
    policyStack.push(currentPolicy);
    currentPolicy = policy;
    policyTrigger = trigger;
    collectedContent = "";
}

std::string ProfessionalScanner::policyChangeEnd(const std::string& endTrigger, Policy newPolicy) {
    std::string result = collectedContent;
    
    if (!policyStack.empty()) {
        currentPolicy = policyStack.top();
        policyStack.pop();
    } else {
        currentPolicy = newPolicy;
    }
    
    collectedContent = "";
    return result;
}

// ========================================
// ProfessionalKeyword实现
// ========================================

ProfessionalKeyword::ProfessionalKeyword(const std::string& pattern, const std::string& ignoreChars)
    : originalPattern(pattern), ignoreChars(ignoreChars) {
    
    parsePattern();
    extractMetadata();
}

void ProfessionalKeyword::parsePattern() {
    // 解析模式，提取函数名和参数
    std::regex functionRegex(R"((\w+)\s*\()");
    std::smatch match;
    
    if (std::regex_search(originalPattern, match, functionRegex)) {
        functionName = match[1].str();
    }
    
    // 标记化模式
    auto parseResult = ProfessionalSyntaxAnalyzer::analyzePattern(originalPattern, ignoreChars);
    
    for (const auto& token : parseResult.tokens) {
        if (!token.isIgnored) {
            args.addArg(token.content, token.content);
        }
    }
}

void ProfessionalKeyword::extractMetadata() {
    // 从模式中提取元数据
    if (originalPattern.find("printMylove") != std::string::npos) {
        setMetadata("description", "珂朵莉图片转字符画扩展");
        setMetadata("category", "graphics");
        setMetadata("author", "Chtholly Team");
    } else if (originalPattern.find("iNeverAway") != std::string::npos) {
        setMetadata("description", "珂朵莉标记函数扩展");
        setMetadata("category", "utility");
        setMetadata("author", "Chtholly Team");
    }
}

void ProfessionalKeyword::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string ProfessionalKeyword::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

std::string ProfessionalKeyword::generateCode() {
    return ProfessionalCodeGenerator::generateCompleteCode(*this, generationOptions);
}

// ========================================
// ProfessionalSyntaxAnalyzer实现
// ========================================

ProfessionalSyntaxAnalyzer::ParseResult ProfessionalSyntaxAnalyzer::analyzePattern(
    const std::string& pattern, const std::string& ignoreChars) {
    
    ParseResult result;
    result.tokens = tokenize(pattern, ignoreChars);
    result.isValid = validateSyntax(pattern);
    result.signature = extractFunctionSignature(pattern);
    
    // 提取函数名
    std::regex functionRegex(R"((\w+)\s*\()");
    std::smatch match;
    if (std::regex_search(pattern, match, functionRegex)) {
        result.functionName = match[1].str();
    }
    
    return result;
}

std::vector<ProfessionalSyntaxAnalyzer::Token> ProfessionalSyntaxAnalyzer::tokenize(
    const std::string& pattern, const std::string& ignoreChars) {
    
    std::vector<Token> tokens;
    std::string current = "";
    int position = 0;
    
    for (size_t i = 0; i < pattern.length(); i++) {
        char c = pattern[i];
        
        if (std::isspace(c)) {
            if (!current.empty()) {
                Token token;
                token.content = current;
                token.type = classifyToken(current);
                token.position = position;
                token.length = current.length();
                token.isIgnored = false;
                tokens.push_back(token);
                
                current = "";
                position = i + 1;
            }
        } else if (ignoreChars.find(c) != std::string::npos) {
            if (!current.empty()) {
                Token token;
                token.content = current;
                token.type = classifyToken(current);
                token.position = position;
                token.length = current.length();
                token.isIgnored = false;
                tokens.push_back(token);
                
                current = "";
            }
            position = i + 1;
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        Token token;
        token.content = current;
        token.type = classifyToken(current);
        token.position = position;
        token.length = current.length();
        token.isIgnored = false;
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string ProfessionalSyntaxAnalyzer::classifyToken(const std::string& content) {
    if (content == "$") return "PLACEHOLDER";
    if (content == "**") return "WILDCARD";
    if (std::regex_match(content, std::regex(R"(\w+)"))) return "IDENTIFIER";
    if (std::regex_match(content, std::regex(R"([{}()\[\],;:])"))) return "PUNCTUATION";
    return "LITERAL";
}

// ========================================
// ProfessionalCodeGenerator实现
// ========================================

std::string ProfessionalCodeGenerator::generateCompleteCode(const ProfessionalKeyword& keyword, 
                                                           const GenerationOptions& options) {
    std::string code = "";
    
    // 1. 生成文档注释
    code += generateDocumentation(keyword);
    
    // 2. 生成参数处理代码
    code += keyword.args.result();
    
    // 3. 生成类型检查（如果启用）
    if (options.enableErrorHandling) {
        code += generateTypeChecking(keyword.args);
    }
    
    // 4. 生成核心实现
    if (keyword.functionName == "printMylove") {
        code += generatePrintMyloveImplementation();
    } else if (keyword.functionName == "iNeverAway") {
        code += generateINeverAwayImplementation();
    }
    
    // 5. 生成函数包装器
    std::vector<std::string> params;
    for (const auto& arg : keyword.args.args) {
        params.push_back(arg->resolvedName);
    }
    
    code = generateFunctionWrapper(keyword.functionName, params, code);
    
    // 6. 性能优化（如果启用）
    if (options.enablePerformanceOptimization) {
        code += generatePerformanceOptimizations(keyword.functionName);
    }
    
    return formatCode(code);
}

std::string ProfessionalCodeGenerator::generatePrintMyloveImplementation() {
    return R"(
// printMylove核心实现 - 珂朵莉图片转字符画
function printMyloveCore(imageUrl, mode, width, height, scale) {
    return new Promise((resolve, reject) => {
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d');
        
        const img = new Image();
        img.crossOrigin = 'anonymous';
        
        img.onload = function() {
            try {
                // 设置画布尺寸
                canvas.width = width * (scale || 1.0);
                canvas.height = height * (scale || 1.0);
                
                // 绘制图片
                ctx.drawImage(img, 0, 0, canvas.width, canvas.height);
                
                // 获取像素数据
                const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
                const pixels = imageData.data;
                
                let output = '';
                
                if (mode === 'ascii') {
                    // ASCII字符画 - 使用符号
                    const chars = ['.', ':', '*', '#', '@'];
                    for (let y = 0; y < canvas.height; y += 2) { // 2倍采样提高性能
                        for (let x = 0; x < canvas.width; x++) {
                            const i = (y * canvas.width + x) * 4;
                            const brightness = (pixels[i] + pixels[i+1] + pixels[i+2]) / 3;
                            const charIndex = Math.floor((1 - brightness / 255) * (chars.length - 1));
                            output += chars[charIndex];
                        }
                        output += '\n';
                    }
                } else if (mode === 'pixel') {
                    // Pixel像素画 - 使用像素块
                    const chars = ['█', '▓', '▒', '░'];
                    for (let y = 0; y < canvas.height; y += 2) {
                        for (let x = 0; x < canvas.width; x++) {
                            const i = (y * canvas.width + x) * 4;
                            const brightness = (pixels[i] + pixels[i+1] + pixels[i+2]) / 3;
                            const charIndex = Math.floor((1 - brightness / 255) * (chars.length - 1));
                            output += chars[charIndex];
                        }
                        output += '\n';
                    }
                }
                
                // 输出到控制台
                console.log('🌸 珂朵莉的图片转字符画：');
                console.log(output);
                
                resolve(output);
            } catch (error) {
                reject(error);
            }
        };
        
        img.onerror = function() {
            reject(new Error('图片加载失败: ' + imageUrl));
        };
        
        img.src = imageUrl;
    });
}
)";
}

std::string ProfessionalCodeGenerator::generateINeverAwayImplementation() {
    return R"(
// iNeverAway核心实现 - 珂朵莉标记函数系统
const iNeverAwayRegistry = new Map();
let iNeverAwayCounter = 0;

function iNeverAwayCore(functionMap) {
    const registryId = 'iNeverAway_' + (++iNeverAwayCounter);
    const functionRefs = {};
    
    // 处理函数映射
    for (const [key, value] of Object.entries(functionMap)) {
        if (typeof value === 'function') {
            // 注册函数
            const functionId = registryId + '_' + key;
            iNeverAwayRegistry.set(functionId, value);
            functionRefs[key] = functionId;
        } else if (typeof value === 'object') {
            // 注册对象标记
            const objectId = registryId + '_' + key;
            iNeverAwayRegistry.set(objectId, value);
            functionRefs[key] = objectId;
        }
    }
    
    // 返回虚对象接口
    return new Proxy({}, {
        get(target, prop) {
            const fullKey = registryId + '_' + prop;
            if (iNeverAwayRegistry.has(fullKey)) {
                const item = iNeverAwayRegistry.get(fullKey);
                if (typeof item === 'function') {
                    return item;
                } else {
                    return item;
                }
            }
            return undefined;
        }
    });
}
)";
}

std::string ProfessionalCodeGenerator::generateDocumentation(const ProfessionalKeyword& keyword) {
    std::string doc = "/**\n";
    doc += " * " + keyword.getMetadata("description") + "\n";
    doc += " * @function " + keyword.functionName + "\n";
    doc += " * @category " + keyword.getMetadata("category") + "\n";
    doc += " * @author " + keyword.getMetadata("author") + "\n";
    
    // 参数文档
    for (const auto& arg : keyword.args.args) {
        doc += " * @param {" + arg->dataType + "} " + arg->resolvedName + "\n";
    }
    
    doc += " */\n";
    return doc;
}

std::string ProfessionalCodeGenerator::formatCode(const std::string& code, int indentLevel) {
    std::istringstream stream(code);
    std::string line;
    std::string formatted = "";
    std::string indent(indentLevel * 2, ' ');
    
    while (std::getline(stream, line)) {
        formatted += indent + line + "\n";
    }
    
    return formatted;
}

// ========================================
// 全局函数实现
// ========================================

ProfessionalKeyword syntaxAnalys(const std::string& pattern, const std::string& ignoreChars) {
    return ProfessionalKeyword(pattern, ignoreChars);
}

std::string generateCode(const ProfessionalKeyword& keyword) {
    return keyword.args.result();
}

// ========================================
// 辅助函数实现
// ========================================

bool isObject(const std::string& value) {
    std::string trimmed = value;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return !trimmed.empty() && trimmed.front() == '{' && trimmed.back() == '}';
}

bool isFunction(const std::string& value) {
    return std::regex_search(value, std::regex(R"(\bfunction\b|=>|\(\s*\)\s*\{)"));
}

bool isArray(const std::string& value) {
    std::string trimmed = value;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\n\r"));
    trimmed.erase(trimmed.find_last_not_of(" \t\n\r") + 1);
    
    return !trimmed.empty() && trimmed.front() == '[' && trimmed.back() == ']';
}

bool isString(const std::string& value) {
    return !value.empty() && 
           ((value.front() == '"' && value.back() == '"') ||
            (value.front() == '\'' && value.back() == '\''));
}

bool isNumber(const std::string& value) {
    if (value.empty()) return false;
    
    bool hasDecimal = false;
    for (size_t i = 0; i < value.length(); i++) {
        char c = value[i];
        if (i == 0 && (c == '+' || c == '-')) continue;
        if (c == '.' && !hasDecimal) {
            hasDecimal = true;
            continue;
        }
        if (!std::isdigit(c)) return false;
    }
    return true;
}

bool isCHTLJSSelector(const std::string& value) {
    return value.find("{{") != std::string::npos && value.find("}}") != std::string::npos;
}

std::string slice(const std::string& content, int start, int length) {
    if (start < 0) start = content.length() + start;
    if (start < 0) start = 0;
    if (start >= content.length()) return "";
    
    if (length == -1) {
        return content.substr(start);
    }
    
    if (start + length > content.length()) {
        length = content.length() - start;
    }
    
    return content.substr(start, length);
}

std::string sliceFunction(const std::string& functionCode, int start, int length) {
    // 查找函数体
    size_t bodyStart = functionCode.find('{');
    size_t bodyEnd = functionCode.rfind('}');
    
    if (bodyStart == std::string::npos || bodyEnd == std::string::npos) {
        return slice(functionCode, start, length);
    }
    
    std::string functionBody = functionCode.substr(bodyStart + 1, bodyEnd - bodyStart - 1);
    return slice(functionBody, start, length);
}

std::string sliceObject(const std::string& objectCode, const std::vector<std::string>& properties) {
    std::string result = "{\n";
    
    for (const auto& prop : properties) {
        std::regex propRegex(prop + R"(\s*:\s*([^,}]+))");
        std::smatch match;
        
        if (std::regex_search(objectCode, match, propRegex)) {
            result += "  " + prop + ": " + match[1].str() + ",\n";
        }
    }
    
    if (result.back() == '\n' && result[result.length()-2] == ',') {
        result = result.substr(0, result.length()-2) + "\n";
    }
    
    result += "}";
    return result;
}

std::string sliceArray(const std::string& arrayCode, int start, int count) {
    // 解析数组元素
    std::vector<std::string> elements;
    std::string current = "";
    int braceLevel = 0;
    bool inString = false;
    char stringChar = '\0';
    
    for (char c : arrayCode) {
        if (!inString && (c == '"' || c == '\'')) {
            inString = true;
            stringChar = c;
        } else if (inString && c == stringChar) {
            inString = false;
        } else if (!inString) {
            if (c == '{' || c == '[') braceLevel++;
            else if (c == '}' || c == ']') braceLevel--;
            else if (c == ',' && braceLevel == 0) {
                elements.push_back(current);
                current = "";
                continue;
            }
        }
        current += c;
    }
    
    if (!current.empty()) {
        elements.push_back(current);
    }
    
    // 切片
    std::string result = "[";
    for (int i = start; i < start + count && i < elements.size(); i++) {
        if (i > start) result += ", ";
        result += elements[i];
    }
    result += "]";
    
    return result;
}

} // namespace cjmod
} // namespace chtl