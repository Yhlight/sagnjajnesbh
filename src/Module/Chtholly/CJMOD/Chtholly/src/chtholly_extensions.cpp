// ========================================
// 珂朵莉CJMOD扩展实现 - 使用您的设计
// printMylove和iNeverAway的完整C++实现
// ========================================

#include "../../CHTLJSCompiler/Module/YourCJMODDesign.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace chtl::your_cjmod_design;

/**
 * printMylove扩展 - 图片转ASCII/像素字符
 * 严格按照CHTL语法文档实现
 */
class ChthollyPrintMyloveExtension : public CJMODExtension {
public:
    void initialize() override {
        std::cout << "💖 初始化珂朵莉printMylove扩展" << std::endl;
    }
    
    std::string processCHTLJS(const std::string& chtljsCode) override {
        if (chtljsCode.find("printMylove") == std::string::npos) {
            return chtljsCode;
        }
        
        std::cout << "💖 珂朵莉printMylove开始处理" << std::endl;
        
        // 🚀 使用您的syntaxAnalys设计
        std::string ignoreSymbols = ",:{};()";
        
        const auto keyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
            printMylove({
                url: $,
                mode: $,
                width: $,
                height: $,
                scale: $,
            });
        )", ignoreSymbols);
        
        // 🔗 绑定参数处理器 - 珂朵莉特色处理
        keyword.args.bind("url", [](std::string str) -> std::string {
            // 清理引号
            if ((str.front() == '"' && str.back() == '"') || 
                (str.front() == '\'' && str.back() == '\'')) {
                str = str.substr(1, str.length() - 2);
            }
            
            std::cout << "🖼️ 珂朵莉处理图片URL: " << str << std::endl;
            return "'" + str + "'";
        });
        
        keyword.args.bind("mode", [](std::string str) -> std::string {
            // 清理引号
            if ((str.front() == '"' && str.back() == '"') || 
                (str.front() == '\'' && str.back() == '\'')) {
                str = str.substr(1, str.length() - 2);
            }
            
            if (str == "ASCII" || str == "Pixel") {
                std::cout << "🎨 珂朵莉设置模式: " << str << std::endl;
                return "'" + str + "'";
            } else {
                std::cout << "⚠️ 珂朵莉使用默认ASCII模式" << std::endl;
                return "'ASCII'";
            }
        });
        
        // ✂️ 使用slice功能处理width参数
        keyword.args.slice("width", 0, -1, [](const std::string& slice) -> std::string {
            std::cout << "📏 珂朵莉处理宽度参数: " << slice << std::endl;
            
            if (slice.find("%") != std::string::npos) {
                std::string percent = slice.substr(0, slice.find("%"));
                return "Math.floor(window.innerWidth * " + percent + " / 100)";
            } else if (slice.find("px") != std::string::npos) {
                return slice.substr(0, slice.find("px"));
            } else {
                try {
                    double num = std::stod(slice);
                    return std::to_string(static_cast<int>(num));
                } catch (...) {
                    return "80"; // 珂朵莉默认宽度
                }
            }
        });
        
        // ✂️ 使用slice功能处理height参数
        keyword.args.slice("height", 0, -1, [](const std::string& slice) -> std::string {
            std::cout << "📐 珂朵莉处理高度参数: " << slice << std::endl;
            
            if (slice.find("%") != std::string::npos) {
                std::string percent = slice.substr(0, slice.find("%"));
                return "Math.floor(window.innerHeight * " + percent + " / 100)";
            } else if (slice.find("px") != std::string::npos) {
                return slice.substr(0, slice.find("px"));
            } else {
                try {
                    double num = std::stod(slice);
                    return std::to_string(static_cast<int>(num));
                } catch (...) {
                    return "60"; // 珂朵莉默认高度
                }
            }
        });
        
        keyword.args.bind("scale", [](std::string str) -> std::string {
            try {
                double scale = std::stod(str);
                scale = std::max(0.1, std::min(scale, 5.0)); // 珂朵莉的合理范围
                std::cout << "🔍 珂朵莉设置缩放: " << scale << std::endl;
                return std::to_string(scale);
            } catch (...) {
                std::cout << "🔍 珂朵莉使用默认缩放: 1.0" << std::endl;
                return "1.0";
            }
        });
        
        // 🔍 设置扫描器
        CJMODScanner scanner;
        scanner.scanKeyword(keyword.args[0], [&]() {
            std::cout << "🎯 珂朵莉扫描到printMylove调用" << std::endl;
            
            // 解析实际参数
            auto params = parseActualParameters(chtljsCode);
            for (const auto& [key, value] : params) {
                Keyword kw(key, "parameter");
                kw.value = value;
                keyword.args.match(key, kw);
            }
        });
        
        // 执行扫描
        scanner.executeScan(chtljsCode, keyword);
        
        // 生成珂朵莉特色的JavaScript代码
        std::string finalCode = generateChthollyPrintMyloveCode(keyword);
        
        return replaceFunctionCall(chtljsCode, "printMylove", finalCode);
    }
    
    std::string getExtensionName() const override {
        return "ChthollyPrintMylove";
    }
    
    std::vector<std::string> getSupportedPatterns() const override {
        return {"printMylove"};
    }

private:
    std::unordered_map<std::string, std::string> parseActualParameters(const std::string& code) {
        std::unordered_map<std::string, std::string> params;
        
        size_t start = code.find("printMylove");
        if (start == std::string::npos) return params;
        
        size_t openBrace = code.find('{', start);
        size_t closeBrace = code.find('}', openBrace);
        
        if (openBrace == std::string::npos || closeBrace == std::string::npos) {
            return params;
        }
        
        std::string content = code.substr(openBrace + 1, closeBrace - openBrace - 1);
        
        // 简单解析键值对
        std::stringstream ss(content);
        std::string line;
        
        while (std::getline(ss, line, ',')) {
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);
                
                // 清理空白
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                if (!key.empty() && !value.empty()) {
                    params[key] = value;
                }
            }
        }
        
        return params;
    }
    
    std::string generateChthollyPrintMyloveCode(const SyntaxKeywordObject& keyword) {
        std::stringstream js;
        
        js << "(function() {\n";
        js << "    console.log('💖 珂朵莉的printMylove开始工作');\n";
        js << "    \n";
        js << "    return new Promise((resolve, reject) => {\n";
        js << "        try {\n";
        js << "            const canvas = document.createElement('canvas');\n";
        js << "            const ctx = canvas.getContext('2d');\n";
        js << "            const img = new Image();\n";
        js << "            \n";
        js << "            img.crossOrigin = 'anonymous';\n";
        js << "            \n";
        
        // 获取参数值
        std::string url = keyword.args.getValue("url");
        std::string mode = keyword.args.getValue("mode");
        std::string width = keyword.args.getValue("width");
        std::string height = keyword.args.getValue("height");
        std::string scale = keyword.args.getValue("scale");
        
        js << "            const url = " << (url.empty() ? "''" : url) << ";\n";
        js << "            const mode = " << (mode.empty() ? "'ASCII'" : mode) << ";\n";
        js << "            const width = " << (width.empty() ? "80" : width) << ";\n";
        js << "            const height = " << (height.empty() ? "60" : height) << ";\n";
        js << "            const scale = " << (scale.empty() ? "1.0" : scale) << ";\n";
        js << "            \n";
        
        js << "            img.onload = function() {\n";
        js << "                console.log('🖼️ 珂朵莉成功加载图片');\n";
        js << "                \n";
        js << "                // 设置画布尺寸\n";
        js << "                canvas.width = width || img.width;\n";
        js << "                canvas.height = height || img.height;\n";
        js << "                \n";
        js << "                // 应用缩放\n";
        js << "                const scaledWidth = Math.floor(canvas.width * scale);\n";
        js << "                const scaledHeight = Math.floor(canvas.height * scale);\n";
        js << "                \n";
        js << "                // 绘制图片\n";
        js << "                ctx.drawImage(img, 0, 0, scaledWidth, scaledHeight);\n";
        js << "                \n";
        js << "                // 获取像素数据\n";
        js << "                const imageData = ctx.getImageData(0, 0, scaledWidth, scaledHeight);\n";
        js << "                const pixels = imageData.data;\n";
        js << "                \n";
        js << "                let result = '';\n";
        js << "                \n";
        js << "                if (mode === 'ASCII') {\n";
        js << "                    console.log('🎨 珂朵莉使用ASCII模式');\n";
        js << "                    // 珂朵莉特色ASCII字符集\n";
        js << "                    const chthollyChars = ' .♡♥❤💖💕💗💓💝💘💞💟💋👑🌸🌺🌻🌼🌷🌹🥀💐🌙⭐✨🌟💫⚡🔥💎';\n";
        js << "                    \n";
        js << "                    for (let y = 0; y < scaledHeight; y += 2) {\n";
        js << "                        for (let x = 0; x < scaledWidth; x++) {\n";
        js << "                            const i = (y * scaledWidth + x) * 4;\n";
        js << "                            \n";
        js << "                            // 计算亮度\n";
        js << "                            const r = pixels[i];\n";
        js << "                            const g = pixels[i + 1];\n";
        js << "                            const b = pixels[i + 2];\n";
        js << "                            const brightness = (r * 0.299 + g * 0.587 + b * 0.114);\n";
        js << "                            \n";
        js << "                            // 映射到珂朵莉字符\n";
        js << "                            const charIndex = Math.floor((brightness / 255) * (chthollyChars.length - 1));\n";
        js << "                            result += chthollyChars[charIndex];\n";
        js << "                        }\n";
        js << "                        result += '\\n';\n";
        js << "                    }\n";
        js << "                } else if (mode === 'Pixel') {\n";
        js << "                    console.log('🎨 珂朵莉使用像素模式');\n";
        js << "                    // 珂朵莉特色像素字符\n";
        js << "                    const chthollyPixels = ['💖', '💕', '💗', '💓', '💝', '💘', '💞', '💟', '♡', '♥', ' '];\n";
        js << "                    \n";
        js << "                    for (let y = 0; y < scaledHeight; y++) {\n";
        js << "                        for (let x = 0; x < scaledWidth; x++) {\n";
        js << "                            const i = (y * scaledWidth + x) * 4;\n";
        js << "                            \n";
        js << "                            // 计算颜色强度\n";
        js << "                            const r = pixels[i];\n";
        js << "                            const g = pixels[i + 1];\n";
        js << "                            const b = pixels[i + 2];\n";
        js << "                            const intensity = (r + g + b) / 3;\n";
        js << "                            \n";
        js << "                            // 映射到珂朵莉像素\n";
        js << "                            const pixelIndex = Math.floor((intensity / 255) * (chthollyPixels.length - 1));\n";
        js << "                            result += chthollyPixels[pixelIndex];\n";
        js << "                        }\n";
        js << "                        result += '\\n';\n";
        js << "                    }\n";
        js << "                }\n";
        js << "                \n";
        js << "                console.log('💖 珂朵莉的printMylove完成转换:');\n";
        js << "                console.log(result);\n";
        js << "                \n";
        js << "                // 显示在页面上（珂朵莉特色样式）\n";
        js << "                let outputElement = document.getElementById('chtholly-image-output');\n";
        js << "                if (!outputElement) {\n";
        js << "                    outputElement = document.createElement('pre');\n";
        js << "                    outputElement.id = 'chtholly-image-output';\n";
        js << "                    outputElement.style.cssText = `\n";
        js << "                        font-family: 'Consolas', 'Monaco', monospace;\n";
        js << "                        font-size: 8px;\n";
        js << "                        line-height: 8px;\n";
        js << "                        white-space: pre;\n";
        js << "                        background: linear-gradient(135deg, #FFF8DC, #FFB6C1);\n";
        js << "                        padding: 20px;\n";
        js << "                        border-radius: 12px;\n";
        js << "                        box-shadow: 0 8px 24px rgba(255, 182, 193, 0.3);\n";
        js << "                        margin: 20px;\n";
        js << "                        border: 2px solid #FFB6C1;\n";
        js << "                        color: #8B4513;\n";
        js << "                    `;\n";
        js << "                    document.body.appendChild(outputElement);\n";
        js << "                }\n";
        js << "                \n";
        js << "                outputElement.textContent = result;\n";
        js << "                \n";
        js << "                resolve(result);\n";
        js << "            };\n";
        js << "            \n";
        js << "            img.onerror = function() {\n";
        js << "                console.error('💔 珂朵莉无法加载图片:', url);\n";
        js << "                reject(new Error('珂朵莉图片加载失败'));\n";
        js << "            };\n";
        js << "            \n";
        js << "            img.src = url;\n";
        js << "            console.log('🖼️ 珂朵莉开始加载图片:', url);\n";
        js << "            \n";
        js << "        } catch (error) {\n";
        js << "            console.error('💔 珂朵莉printMylove出错:', error);\n";
        js << "            reject(error);\n";
        js << "        }\n";
        js << "    });\n";
        js << "})()";
        
        return js.str();
    }
    
    std::string replaceFunctionCall(const std::string& code, const std::string& functionName, const std::string& replacement) {
        size_t start = code.find(functionName);
        if (start == std::string::npos) return code;
        
        // 找到完整的函数调用
        size_t parenStart = code.find('(', start);
        if (parenStart == std::string::npos) return code;
        
        int parenCount = 1;
        size_t pos = parenStart + 1;
        
        while (pos < code.length() && parenCount > 0) {
            if (code[pos] == '(') parenCount++;
            else if (code[pos] == ')') parenCount--;
            pos++;
        }
        
        if (parenCount == 0) {
            std::string before = code.substr(0, start);
            std::string after = code.substr(pos);
            return before + replacement + after;
        }
        
        return code;
    }
};

/**
 * iNeverAway扩展 - 虚对象和标记函数系统
 * 严格按照CHTL语法文档实现
 */
class ChthollyINeverAwayExtension : public CJMODExtension {
public:
    void initialize() override {
        std::cout << "✨ 初始化珂朵莉iNeverAway扩展" << std::endl;
    }
    
    std::string processCHTLJS(const std::string& chtljsCode) override {
        if (chtljsCode.find("iNeverAway") == std::string::npos) {
            return chtljsCode;
        }
        
        std::cout << "✨ 珂朵莉iNeverAway开始处理" << std::endl;
        
        // 🚀 使用您的syntaxAnalys设计处理复杂对象语法
        std::string ignoreSymbols = ",:{}<>()";
        
        const auto keyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
            iNeverAway({
                Void<A>: $,
                Void<B>: $,
                Void: $,
                Ax: $,
            });
        )", ignoreSymbols);
        
        // 🚀 使用高级绑定处理函数和对象参数
        keyword.args.bindAdvanced("Void<A>", [](const AdvancedParamValue& param) -> std::string {
            if (param.type == AdvancedParamType::FUNCTION) {
                std::cout << "✨ 珂朵莉处理Void<A>函数" << std::endl;
                return generateChthollyMarkerFunction("Void_A", param.processedValue, "状态A的珂朵莉");
            }
            return param.processedValue;
        });
        
        keyword.args.bindAdvanced("Void<B>", [](const AdvancedParamValue& param) -> std::string {
            if (param.type == AdvancedParamType::FUNCTION) {
                std::cout << "✨ 珂朵莉处理Void<B>函数" << std::endl;
                return generateChthollyMarkerFunction("Void_B", param.processedValue, "状态B的珂朵莉");
            }
            return param.processedValue;
        });
        
        // ✂️ 使用slice功能处理对象参数
        keyword.args.slice("Void", 1, -1, [](const std::string& slice) -> std::string {
            std::cout << "✨ 珂朵莉处理Void对象切片: " << slice << std::endl;
            return generateChthollyMarkerObject("Void", "{" + slice + "}", "珂朵莉的空白状态");
        });
        
        keyword.args.slice("Ax", 1, -1, [](const std::string& slice) -> std::string {
            std::cout << "✨ 珂朵莉处理Ax对象切片: " << slice << std::endl;
            return generateChthollyMarkerObject("Ax", "{" + slice + "}", "珂朵莉的轴心状态");
        });
        
        // 🔍 设置扫描器
        CJMODScanner scanner;
        scanner.scanKeyword(keyword.args[0], [&]() {
            std::cout << "🎯 珂朵莉扫描到iNeverAway调用" << std::endl;
            
            // 解析复杂的参数结构
            auto params = parseINeverAwayParameters(chtljsCode);
            for (const auto& [key, value] : params) {
                Keyword kw(key, "parameter");
                kw.value = value;
                keyword.args.match(key, kw);
            }
        });
        
        // 执行扫描
        scanner.executeScan(chtljsCode, keyword);
        
        // 生成珂朵莉特色的iNeverAway代码
        std::string finalCode = generateChthollyINeverAwayCode(keyword);
        
        return replaceFunctionCall(chtljsCode, "iNeverAway", finalCode);
    }
    
    std::string getExtensionName() const override {
        return "ChthollyINeverAway";
    }
    
    std::vector<std::string> getSupportedPatterns() const override {
        return {"iNeverAway", "vir"};
    }

private:
    static std::string generateChthollyMarkerFunction(const std::string& name, 
                                                     const std::string& func, 
                                                     const std::string& description) {
        std::stringstream js;
        js << "// 🌟 珂朵莉标记函数: " << name << " - " << description << "\n";
        js << "window.__chtl_chtholly_markers = window.__chtl_chtholly_markers || {};\n";
        js << "window.__chtl_chtholly_markers['" << name << "'] = " << func << ";\n";
        js << "function __chtholly_" << name << "() {\n";
        js << "    console.log('✨ 珂朵莉调用标记函数: " << name << "');\n";
        js << "    return window.__chtl_chtholly_markers['" << name << "'].apply(this, arguments);\n";
        js << "}";
        return js.str();
    }
    
    static std::string generateChthollyMarkerObject(const std::string& name, 
                                                   const std::string& obj, 
                                                   const std::string& description) {
        std::stringstream js;
        js << "// 💖 珂朵莉标记对象: " << name << " - " << description << "\n";
        js << "window.__chtl_chtholly_objects = window.__chtl_chtholly_objects || {};\n";
        js << "window.__chtl_chtholly_objects['" << name << "'] = " << obj << ";\n";
        js << "console.log('💖 珂朵莉注册标记对象: " << name << "');";
        return js.str();
    }
    
    std::unordered_map<std::string, std::string> parseINeverAwayParameters(const std::string& code) {
        std::unordered_map<std::string, std::string> params;
        
        size_t start = code.find("iNeverAway");
        if (start == std::string::npos) return params;
        
        size_t openBrace = code.find('{', start);
        size_t closeBrace = findMatchingBrace(code, openBrace);
        
        if (openBrace == std::string::npos || closeBrace == std::string::npos) {
            return params;
        }
        
        std::string content = code.substr(openBrace + 1, closeBrace - openBrace - 1);
        
        // 解析复杂的键值对，支持泛型和嵌套结构
        size_t pos = 0;
        while (pos < content.length()) {
            // 跳过空白
            while (pos < content.length() && std::isspace(content[pos])) pos++;
            if (pos >= content.length()) break;
            
            // 查找键
            size_t keyStart = pos;
            while (pos < content.length() && content[pos] != ':') pos++;
            if (pos >= content.length()) break;
            
            std::string key = content.substr(keyStart, pos - keyStart);
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            
            pos++; // 跳过 ':'
            
            // 查找值
            while (pos < content.length() && std::isspace(content[pos])) pos++;
            size_t valueStart = pos;
            
            // 处理不同类型的值
            if (pos < content.length()) {
                if (content.substr(pos, 8) == "function") {
                    // 函数值
                    pos = findEndOfFunction(content, pos);
                } else if (content[pos] == '{') {
                    // 对象值
                    pos = findMatchingBrace(content, pos) + 1;
                } else {
                    // 简单值
                    while (pos < content.length() && content[pos] != ',' && content[pos] != '}') pos++;
                }
            }
            
            std::string value = content.substr(valueStart, pos - valueStart);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t,") + 1);
            
            if (!key.empty() && !value.empty()) {
                params[key] = value;
            }
            
            // 跳过逗号
            if (pos < content.length() && content[pos] == ',') pos++;
        }
        
        return params;
    }
    
    size_t findMatchingBrace(const std::string& str, size_t start) {
        if (start >= str.length() || str[start] != '{') return std::string::npos;
        
        int braceCount = 1;
        size_t pos = start + 1;
        
        while (pos < str.length() && braceCount > 0) {
            if (str[pos] == '{') braceCount++;
            else if (str[pos] == '}') braceCount--;
            pos++;
        }
        
        return braceCount == 0 ? pos - 1 : std::string::npos;
    }
    
    size_t findEndOfFunction(const std::string& str, size_t start) {
        size_t pos = start;
        
        // 跳过 "function"
        pos += 8;
        
        // 跳过参数列表
        size_t parenStart = str.find('(', pos);
        if (parenStart != std::string::npos) {
            int parenCount = 1;
            pos = parenStart + 1;
            
            while (pos < str.length() && parenCount > 0) {
                if (str[pos] == '(') parenCount++;
                else if (str[pos] == ')') parenCount--;
                pos++;
            }
        }
        
        // 查找函数体
        size_t braceStart = str.find('{', pos);
        if (braceStart != std::string::npos) {
            pos = findMatchingBrace(str, braceStart) + 1;
        }
        
        return pos;
    }
    
    std::string generateChthollyINeverAwayCode(const SyntaxKeywordObject& keyword) {
        std::stringstream js;
        
        js << "(function() {\n";
        js << "    console.log('✨ 珂朵莉的iNeverAway标记系统启动');\n";
        js << "    \n";
        js << "    // 初始化珂朵莉标记系统\n";
        js << "    window.__chtl_chtholly_markers = window.__chtl_chtholly_markers || {};\n";
        js << "    window.__chtl_chtholly_objects = window.__chtl_chtholly_objects || {};\n";
        js << "    \n";
        
        // 生成所有标记函数和对象
        for (const auto& kw : keyword.args.keywords) {
            if (!kw.value.empty()) {
                js << "    // 珂朵莉标记: " << kw.name << "\n";
                js << "    " << kw.value << "\n";
                js << "    \n";
            }
        }
        
        js << "    // 珂朵莉特色功能增强\n";
        js << "    const chthollySystem = {\n";
        js << "        initialized: true,\n";
        js << "        timestamp: Date.now(),\n";
        js << "        version: '1.0.0',\n";
        js << "        creator: '珂朵莉',\n";
        js << "        \n";
        js << "        // 获取标记函数\n";
        js << "        getMarker: function(name) {\n";
        js << "            return window.__chtl_chtholly_markers[name];\n";
        js << "        },\n";
        js << "        \n";
        js << "        // 获取标记对象\n";
        js << "        getObject: function(name) {\n";
        js << "            return window.__chtl_chtholly_objects[name];\n";
        js << "        },\n";
        js << "        \n";
        js << "        // 列出所有标记\n";
        js << "        listMarkers: function() {\n";
        js << "            console.log('✨ 珂朵莉的标记函数:', Object.keys(window.__chtl_chtholly_markers));\n";
        js << "            console.log('💖 珂朵莉的标记对象:', Object.keys(window.__chtl_chtholly_objects));\n";
        js << "        }\n";
        js << "    };\n";
        js << "    \n";
        js << "    console.log('✨ 珂朵莉iNeverAway系统初始化完成');\n";
        js << "    console.log('💖 珂朵莉永远与你同在 - I Never Away');\n";
        js << "    \n";
        js << "    return chthollySystem;\n";
        js << "})()";
        
        return js.str();
    }
    
    std::string replaceFunctionCall(const std::string& code, const std::string& functionName, const std::string& replacement) {
        size_t start = code.find(functionName);
        if (start == std::string::npos) return code;
        
        // 找到完整的函数调用
        size_t parenStart = code.find('(', start);
        if (parenStart == std::string::npos) return code;
        
        int parenCount = 1;
        size_t pos = parenStart + 1;
        
        while (pos < code.length() && parenCount > 0) {
            if (code[pos] == '(') parenCount++;
            else if (code[pos] == ')') parenCount--;
            pos++;
        }
        
        if (parenCount == 0) {
            std::string before = code.substr(0, start);
            std::string after = code.substr(pos);
            return before + replacement + after;
        }
        
        return code;
    }
};

// CJMOD导出函数
extern "C" {
    void initializeChthollyExtensions() {
        std::cout << "🌸 初始化珂朵莉CJMOD扩展系统" << std::endl;
        
        auto& manager = CJMODManager::getInstance();
        
        // 注册珂朵莉的printMylove扩展
        auto printMylove = std::make_unique<ChthollyPrintMyloveExtension>();
        printMylove->initialize();
        manager.registerExtension(std::move(printMylove));
        
        // 注册珂朵莉的iNeverAway扩展
        auto iNeverAway = std::make_unique<ChthollyINeverAwayExtension>();
        iNeverAway->initialize();
        manager.registerExtension(std::move(iNeverAway));
        
        std::cout << "💖 珂朵莉CJMOD扩展系统初始化完成" << std::endl;
        std::cout << "✨ 珂朵莉永远与你同在 - I Never Away" << std::endl;
    }
    
    const char* processChthollyExtensions(const char* chtljsCode) {
        if (!chtljsCode) return nullptr;
        
        std::string code(chtljsCode);
        auto& manager = CJMODManager::getInstance();
        
        std::string processedCode = manager.processCHTLJS(code);
        
        // 返回处理后的代码（需要注意内存管理）
        static std::string result = processedCode;
        return result.c_str();
    }
}