// ========================================
// 您的CJMOD设计 - 完整实现
// syntaxAnalys + args.bind + scanner + match + result
// ========================================

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <iostream>

namespace chtl {
namespace your_cjmod_design {

/**
 * 关键字类 - 存储单个关键字和其值
 */
class Keyword {
public:
    std::string name;           // 关键字名称
    std::string value;          // 关键字值（通过match填充）
    std::string type;           // 关键字类型
    int position;               // 在源码中的位置
    
    Keyword() : position(0) {}
    Keyword(const std::string& n, const std::string& t = "auto", int p = 0) 
        : name(n), type(t), position(p) {}
};

/**
 * 参数管理器 - 您的args设计
 */
class ArgsManager {
public:
    std::vector<Keyword> keywords;          // 所有关键字
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> bindFunctions; // 绑定的处理函数
    
    /**
     * 绑定采集函数 - args.bind("url", lambda)
     */
    void bind(const std::string& keywordName, std::function<std::string(const std::string&)> processor) {
        bindFunctions[keywordName] = processor;
        std::cout << "🔗 绑定关键字: " << keywordName << std::endl;
    }
    
    /**
     * 匹配参数 - args.match("url", peekKeyword(1))
     * 如果匹配到参数是这个时，就执行参数的绑定函数
     */
    void match(const std::string& keywordName, const Keyword& keywordValue) {
        std::cout << "🎯 匹配参数: " << keywordName << " = " << keywordValue.value << std::endl;
        
        // 查找对应的关键字
        auto it = std::find_if(keywords.begin(), keywords.end(),
            [&keywordName](const Keyword& kw) {
                return kw.name == keywordName;
            });
        
        if (it != keywords.end()) {
            // 执行绑定的处理函数
            if (bindFunctions.count(keywordName)) {
                it->value = bindFunctions[keywordName](keywordValue.value);
                std::cout << "✅ 参数处理完成: " << keywordName << " -> " << it->value << std::endl;
            } else {
                it->value = keywordValue.value;
                std::cout << "📝 直接赋值: " << keywordName << " = " << keywordValue.value << std::endl;
            }
        }
    }
    
    /**
     * 按索引访问关键字 - args[0]
     */
    Keyword& operator[](size_t index) {
        if (index < keywords.size()) {
            return keywords[index];
        }
        
        // 如果索引超出范围，扩展数组
        while (keywords.size() <= index) {
            keywords.emplace_back("keyword" + std::to_string(keywords.size()));
        }
        
        return keywords[index];
    }
    
    /**
     * 获取关键字值
     */
    std::string getValue(const std::string& keywordName) const {
        auto it = std::find_if(keywords.begin(), keywords.end(),
            [&keywordName](const Keyword& kw) {
                return kw.name == keywordName;
            });
        
        return it != keywords.end() ? it->value : "";
    }
    
    /**
     * 获取所有关键字名称
     */
    std::vector<std::string> getAllKeywordNames() const {
        std::vector<std::string> names;
        for (const auto& kw : keywords) {
            names.push_back(kw.name);
        }
        return names;
    }
};

/**
 * 语法关键字对象 - syntaxAnalys返回的对象
 */
class SyntaxKeywordObject {
public:
    std::string originalPattern;        // 原始输入字符串
    std::string ignoreSymbols;          // 不进行收集的内容
    ArgsManager args;                   // 维护着关键字的args
    
    SyntaxKeywordObject() = default;
    SyntaxKeywordObject(const std::string& pattern, const std::string& ignore) 
        : originalPattern(pattern), ignoreSymbols(ignore) {}
    
    /**
     * result()方法 - 把参数拼接起来，返回字符串
     */
    std::string result() const {
        std::cout << "🔧 开始拼接参数生成最终代码..." << std::endl;
        
        std::stringstream js;
        js << "(() => {\n";
        js << "    // 🚀 由您的CJMOD设计生成的代码\n";
        
        // 获取函数名
        std::string functionName = extractFunctionName();
        js << "    console.log('🎯 执行函数: " << functionName << "');\n";
        js << "    \n";
        
        // 拼接所有参数
        for (const auto& keyword : args.keywords) {
            if (!keyword.value.empty()) {
                js << "    const " << keyword.name << " = " << keyword.value << ";\n";
            }
        }
        
        js << "    \n";
        js << generateFunctionImplementation(functionName);
        js << "\n})()";
        
        std::string result = js.str();
        std::cout << "✅ 参数拼接完成，生成代码长度: " << result.length() << " 字符" << std::endl;
        
        return result;
    }

private:
    std::string extractFunctionName() const {
        // 从原始模式中提取函数名
        size_t funcStart = originalPattern.find_first_not_of(" \t\n");
        if (funcStart == std::string::npos) return "unknown";
        
        size_t funcEnd = originalPattern.find('(', funcStart);
        if (funcEnd == std::string::npos) return "unknown";
        
        return originalPattern.substr(funcStart, funcEnd - funcStart);
    }
    
    std::string generateFunctionImplementation(const std::string& functionName) const {
        if (functionName == "printMylove") {
            return R"(    // printMylove 完整实现
    return new Promise((resolve) => {
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d');
        const img = new Image();
        
        img.crossOrigin = 'anonymous';
        
        img.onload = function() {
            try {
                // 设置画布尺寸
                canvas.width = width || img.width;
                canvas.height = height || img.height;
                
                // 应用缩放
                const scaledWidth = Math.floor(canvas.width * (scale || 1));
                const scaledHeight = Math.floor(canvas.height * (scale || 1));
                
                // 绘制图片
                ctx.drawImage(img, 0, 0, scaledWidth, scaledHeight);
                
                // 获取像素数据
                const imageData = ctx.getImageData(0, 0, scaledWidth, scaledHeight);
                const pixels = imageData.data;
                
                let result = '';
                
                if (mode === 'ASCII') {
                    // ASCII字符映射
                    const asciiChars = ' .\'`^",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$';
                    
                    for (let y = 0; y < scaledHeight; y += 2) { // 每2行取一行，因为字符高度比宽度大
                        for (let x = 0; x < scaledWidth; x++) {
                            const i = (y * scaledWidth + x) * 4;
                            
                            // 计算灰度值
                            const gray = Math.floor((pixels[i] + pixels[i+1] + pixels[i+2]) / 3);
                            
                            // 映射到ASCII字符
                            const charIndex = Math.floor((gray / 255) * (asciiChars.length - 1));
                            result += asciiChars[charIndex];
                        }
                        result += '\n';
                    }
                } else if (mode === 'Pixel') {
                    // 像素块模式
                    const pixelChars = ['█', '▓', '▒', '░', ' '];
                    
                    for (let y = 0; y < scaledHeight; y++) {
                        for (let x = 0; x < scaledWidth; x++) {
                            const i = (y * scaledWidth + x) * 4;
                            
                            // 计算亮度
                            const brightness = (pixels[i] + pixels[i+1] + pixels[i+2]) / 3;
                            const charIndex = Math.floor((brightness / 255) * (pixelChars.length - 1));
                            
                            result += pixelChars[charIndex];
                        }
                        result += '\n';
                    }
                }
                
                console.log('🎨 图片转换完成:');
                console.log(result);
                
                // 显示在页面上
                const outputElement = document.getElementById('image-output') || document.body;
                if (outputElement) {
                    outputElement.textContent = result;
                    outputElement.style.fontFamily = 'Courier New, monospace';
                    outputElement.style.fontSize = '8px';
                    outputElement.style.lineHeight = '8px';
                    outputElement.style.whiteSpace = 'pre';
                }
                
                resolve(result);
                
            } catch (error) {
                console.error('❌ 图片处理错误:', error);
                resolve('处理失败: ' + error.message);
            }
        };
        
        img.onerror = function() {
            console.error('❌ 图片加载失败:', url);
            resolve('图片加载失败');
        };
        
        img.src = url;
        console.log('🖼️ 开始加载图片:', url);
    });)";
        } else {
            return "    console.log('🚀 " + functionName + " 执行');";
        }
    }
};

/**
 * 扫描器 - 您的scanner设计
 */
class CJMODScanner {
public:
    std::vector<Keyword> scannedKeywords;   // 扫描到的关键字
    std::unordered_map<std::string, std::function<void()>> scanCallbacks; // 扫描回调
    
    /**
     * 扫描关键字 - scanner.scanKeyword(keyword.args[0], callback)
     */
    void scanKeyword(const Keyword& targetKeyword, std::function<void()> callback) {
        scanCallbacks[targetKeyword.name] = callback;
        std::cout << "🔍 注册扫描器: " << targetKeyword.name << std::endl;
    }
    
    /**
     * 查看关键字 - peekKeyword(offset)
     */
    Keyword peekKeyword(int offset) {
        int currentPos = static_cast<int>(scannedKeywords.size()) - 1;
        int targetPos = currentPos + offset;
        
        if (targetPos >= 0 && targetPos < static_cast<int>(scannedKeywords.size())) {
            Keyword result = scannedKeywords[targetPos];
            std::cout << "👀 peekKeyword(" << offset << ") = " << result.name << ":" << result.value << std::endl;
            return result;
        }
        
        std::cout << "👀 peekKeyword(" << offset << ") = <空>" << std::endl;
        return Keyword("", "empty");
    }
    
    /**
     * 执行扫描过程
     */
    void executeScan(const std::string& chtljsCode, SyntaxKeywordObject& keywordObj) {
        std::cout << "🔍 开始扫描CHTL JS代码..." << std::endl;
        
        // 构建扫描的关键字列表
        buildScannedKeywords(chtljsCode, keywordObj);
        
        // 执行扫描回调
        for (const auto& keyword : scannedKeywords) {
            if (scanCallbacks.count(keyword.name)) {
                std::cout << "🎯 执行扫描回调: " << keyword.name << std::endl;
                scanCallbacks[keyword.name]();
            }
        }
        
        std::cout << "✅ 扫描完成" << std::endl;
    }

private:
    void buildScannedKeywords(const std::string& code, const SyntaxKeywordObject& keywordObj) {
        scannedKeywords.clear();
        
        // 从CHTL JS代码中提取实际的关键字和值
        // 这里需要智能解析代码，提取参数值
        
        // 简化实现：查找函数调用和参数
        std::string functionName = extractFunctionName(code);
        if (!functionName.empty()) {
            scannedKeywords.emplace_back(functionName, "function", 0);
        }
        
        // 提取对象参数中的键值对
        size_t objStart = code.find('{');
        size_t objEnd = code.find_last_of('}');
        
        if (objStart != std::string::npos && objEnd != std::string::npos && objEnd > objStart) {
            std::string objContent = code.substr(objStart + 1, objEnd - objStart - 1);
            parseObjectContent(objContent, keywordObj.ignoreSymbols);
        }
        
        std::cout << "📚 扫描到关键字数量: " << scannedKeywords.size() << std::endl;
    }
    
    std::string extractFunctionName(const std::string& code) {
        size_t funcStart = code.find_first_not_of(" \t\n");
        if (funcStart == std::string::npos) return "";
        
        size_t funcEnd = code.find('(', funcStart);
        if (funcEnd == std::string::npos) return "";
        
        return code.substr(funcStart, funcEnd - funcStart);
    }
    
    void parseObjectContent(const std::string& content, const std::string& ignoreSymbols) {
        std::string cleanContent = content;
        
        // 移除忽略的符号
        for (char c : ignoreSymbols) {
            if (c != ':') { // 保留冒号，因为需要它来分离键值对
                cleanContent.erase(std::remove(cleanContent.begin(), cleanContent.end(), c), 
                                 cleanContent.end());
            }
        }
        
        // 按行分割
        std::stringstream ss(cleanContent);
        std::string line;
        
        while (std::getline(ss, line)) {
            // 查找键值对
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // 清理空白字符
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                // 移除注释
                size_t commentPos = value.find("//");
                if (commentPos != std::string::npos) {
                    value = value.substr(0, commentPos);
                    value.erase(value.find_last_not_of(" \t") + 1);
                }
                
                if (!key.empty() && !value.empty()) {
                    scannedKeywords.emplace_back(key, "parameter", scannedKeywords.size());
                    scannedKeywords.back().value = value;
                    std::cout << "📌 提取键值对: " << key << " = " << value << std::endl;
                }
            }
        }
    }
};

/**
 * 语法分析器 - 您的syntaxAnalys函数
 */
class YourSyntaxAnalyzer {
public:
    /**
     * 语法分析函数 - syntaxAnalys(字符串, 不收集的内容)
     * 返回一个对象，内部维护着args
     */
    static SyntaxKeywordObject syntaxAnalys(const std::string& inputString, const std::string& ignoreContent = "") {
        std::cout << "🔍 syntaxAnalys 开始分析..." << std::endl;
        std::cout << "📝 输入字符串长度: " << inputString.length() << " 字符" << std::endl;
        std::cout << "🚫 忽略内容: " << ignoreContent << std::endl;
        
        SyntaxKeywordObject keywordObj(inputString, ignoreContent);
        
        // 分析字符串，提取关键字
        analyzeStringToKeywords(inputString, ignoreContent, keywordObj);
        
        std::cout << "✅ 分析完成，提取关键字数量: " << keywordObj.args.keywords.size() << std::endl;
        
        return keywordObj;
    }

private:
    /**
     * 将输入字符串分成多个Keyword
     */
    static void analyzeStringToKeywords(const std::string& inputString, 
                                      const std::string& ignoreContent,
                                      SyntaxKeywordObject& keywordObj) {
        std::string cleanString = inputString;
        
        // 移除忽略的内容
        for (char c : ignoreContent) {
            if (c != ':') { // 保留冒号用于键值对分离
                cleanString.erase(std::remove(cleanString.begin(), cleanString.end(), c), 
                                cleanString.end());
            }
        }
        
        // 提取函数名
        std::string functionName = extractFunctionName(cleanString);
        if (!functionName.empty()) {
            keywordObj.args.keywords.emplace_back(functionName, "function", 0);
            std::cout << "🎯 发现函数: " << functionName << std::endl;
        }
        
        // 查找所有的 $ 占位符及其前面的参数名
        size_t pos = 0;
        while ((pos = cleanString.find('$', pos)) != std::string::npos) {
            // 向前查找参数名
            size_t colonPos = cleanString.find_last_of(':', pos);
            if (colonPos != std::string::npos) {
                // 查找冒号前的参数名
                size_t nameStart = colonPos;
                while (nameStart > 0 && (std::isalnum(cleanString[nameStart-1]) || cleanString[nameStart-1] == '_')) {
                    nameStart--;
                }
                
                std::string paramName = cleanString.substr(nameStart, colonPos - nameStart);
                
                // 清理参数名
                paramName.erase(0, paramName.find_first_not_of(" \t\n"));
                paramName.erase(paramName.find_last_not_of(" \t\n") + 1);
                
                if (!paramName.empty()) {
                    keywordObj.args.keywords.emplace_back(paramName, "parameter", pos);
                    std::cout << "📌 发现参数: " << paramName << std::endl;
                }
            }
            
            pos++;
        }
    }
    
    static std::string extractFunctionName(const std::string& str) {
        size_t funcStart = str.find_first_not_of(" \t\n");
        if (funcStart == std::string::npos) return "";
        
        size_t funcEnd = str.find('(', funcStart);
        if (funcEnd == std::string::npos) return "";
        
        return str.substr(funcStart, funcEnd - funcStart);
    }
};

/**
 * 代码生成器 - 您的generateCode函数
 */
class YourCodeGenerator {
public:
    /**
     * 生成代码 - generateCode(keyword)
     * 返回最终的结果
     */
    static std::string generateCode(const SyntaxKeywordObject& keyword) {
        std::cout << "🔧 generateCode 开始生成最终代码..." << std::endl;
        
        std::string result = keyword.result();
        
        std::cout << "✅ 最终代码生成完成" << std::endl;
        
        return result;
    }
};

} // namespace your_cjmod_design
} // namespace chtl