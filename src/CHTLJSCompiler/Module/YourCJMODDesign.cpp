// ========================================
// 您的CJMOD设计 - 核心实现
// 严格按照CHTL语法文档和您的设计规范
// ========================================

#include "YourCJMODDesign.h"

namespace chtl {
namespace your_cjmod_design {

/**
 * CJMOD扩展接口 - 按照CHTL语法文档实现
 */
class CJMODExtension {
public:
    virtual ~CJMODExtension() = default;
    
    /**
     * 初始化CJMOD扩展
     */
    virtual void initialize() = 0;
    
    /**
     * 处理CHTL JS语法
     */
    virtual std::string processCHTLJS(const std::string& chtljsCode) = 0;
    
    /**
     * 获取扩展名称
     */
    virtual std::string getExtensionName() const = 0;
    
    /**
     * 获取支持的语法模式
     */
    virtual std::vector<std::string> getSupportedPatterns() const = 0;
};

/**
 * CJMOD管理器 - 管理所有CJMOD扩展
 */
class CJMODManager {
private:
    static CJMODManager* instance;
    std::unordered_map<std::string, std::unique_ptr<CJMODExtension>> extensions;
    std::unordered_map<std::string, std::string> patternToExtension;
    
    CJMODManager() = default;
    
public:
    static CJMODManager& getInstance() {
        if (!instance) {
            instance = new CJMODManager();
        }
        return *instance;
    }
    
    /**
     * 注册CJMOD扩展
     */
    void registerExtension(std::unique_ptr<CJMODExtension> extension) {
        std::string name = extension->getExtensionName();
        std::vector<std::string> patterns = extension->getSupportedPatterns();
        
        // 注册扩展
        extensions[name] = std::move(extension);
        
        // 建立模式到扩展的映射
        for (const auto& pattern : patterns) {
            patternToExtension[pattern] = name;
        }
        
        std::cout << "🔌 注册CJMOD扩展: " << name << std::endl;
    }
    
    /**
     * 处理CHTL JS代码
     */
    std::string processCHTLJS(const std::string& chtljsCode) {
        std::cout << "🔍 CJMOD管理器开始处理CHTL JS代码" << std::endl;
        
        std::string processedCode = chtljsCode;
        
        // 遍历所有注册的扩展，查找匹配的模式
        for (const auto& [pattern, extensionName] : patternToExtension) {
            if (processedCode.find(pattern) != std::string::npos) {
                std::cout << "🎯 找到匹配模式: " << pattern << " -> " << extensionName << std::endl;
                
                auto& extension = extensions[extensionName];
                processedCode = extension->processCHTLJS(processedCode);
            }
        }
        
        return processedCode;
    }
    
    /**
     * 获取所有扩展信息
     */
    std::vector<std::string> getExtensionList() const {
        std::vector<std::string> list;
        for (const auto& [name, extension] : extensions) {
            list.push_back(name);
        }
        return list;
    }
};

// 静态成员初始化
CJMODManager* CJMODManager::instance = nullptr;

/**
 * printMylove CJMOD扩展实现 - 按照CHTL语法文档
 */
class PrintMyloveExtension : public CJMODExtension {
public:
    void initialize() override {
        std::cout << "🖼️ 初始化printMylove扩展" << std::endl;
    }
    
    std::string processCHTLJS(const std::string& chtljsCode) override {
        std::cout << "🎨 printMylove开始处理CHTL JS代码" << std::endl;
        
        if (chtljsCode.find("printMylove") == std::string::npos) {
            return chtljsCode;
        }
        
        // 🚀 使用您的设计处理printMylove语法
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
        
        // 🔗 绑定参数处理器
        keyword.args.bind("url", [](std::string str) -> std::string {
            // 处理URL参数
            if (str.front() == '"' && str.back() == '"') {
                str = str.substr(1, str.length() - 2);
            }
            if (str.front() == '\'' && str.back() == '\'') {
                str = str.substr(1, str.length() - 2);
            }
            
            if (str.find("http") == 0) {
                return "'" + str + "'";
            } else if (str.find("data:") == 0) {
                return "'" + str + "'";
            } else {
                return "'" + str + "'";
            }
        });
        
        keyword.args.bind("mode", [](std::string str) -> std::string {
            // 清理引号
            if (str.front() == '"' && str.back() == '"') {
                str = str.substr(1, str.length() - 2);
            }
            if (str.front() == '\'' && str.back() == '\'') {
                str = str.substr(1, str.length() - 2);
            }
            
            if (str == "ASCII" || str == "Pixel") {
                return "'" + str + "'";
            } else {
                return "'ASCII'";
            }
        });
        
        keyword.args.bind("width", [](std::string str) -> std::string {
            // 处理CSS单位
            if (str.find("%") != std::string::npos) {
                std::string percent = str.substr(0, str.find("%"));
                return "Math.floor(window.innerWidth * " + percent + " / 100)";
            } else if (str.find("px") != std::string::npos) {
                return str.substr(0, str.find("px"));
            } else {
                try {
                    double num = std::stod(str);
                    return std::to_string(static_cast<int>(num));
                } catch (...) {
                    return "100";
                }
            }
        });
        
        keyword.args.bind("height", [](std::string str) -> std::string {
            // 与width相同的处理逻辑
            if (str.find("%") != std::string::npos) {
                std::string percent = str.substr(0, str.find("%"));
                return "Math.floor(window.innerHeight * " + percent + " / 100)";
            } else if (str.find("px") != std::string::npos) {
                return str.substr(0, str.find("px"));
            } else {
                try {
                    double num = std::stod(str);
                    return std::to_string(static_cast<int>(num));
                } catch (...) {
                    return "100";
                }
            }
        });
        
        keyword.args.bind("scale", [](std::string str) -> std::string {
            try {
                double scale = std::stod(str);
                scale = std::max(0.1, std::min(scale, 10.0));
                return std::to_string(scale);
            } catch (...) {
                return "1.0";
            }
        });
        
        // 🔍 设置扫描器
        CJMODScanner scanner;
        scanner.scanKeyword(keyword.args[0], [&]() {
            std::cout << "🎯 扫描到printMylove函数" << std::endl;
            
            // 从实际代码中提取参数值
            std::string extractedCode = extractPrintMyloveCall(chtljsCode);
            parseAndMatchParameters(extractedCode, keyword, scanner);
        });
        
        // 执行扫描
        scanner.executeScan(chtljsCode, keyword);
        
        // 生成最终代码
        std::string finalCode = YourCodeGenerator::generateCode(keyword);
        
        // 替换原始的printMylove调用
        return replacePrintMyloveCall(chtljsCode, finalCode);
    }
    
    std::string getExtensionName() const override {
        return "PrintMylove";
    }
    
    std::vector<std::string> getSupportedPatterns() const override {
        return {"printMylove"};
    }

private:
    std::string extractPrintMyloveCall(const std::string& code) {
        size_t start = code.find("printMylove");
        if (start == std::string::npos) return "";
        
        size_t openBrace = code.find('{', start);
        if (openBrace == std::string::npos) return "";
        
        int braceCount = 1;
        size_t pos = openBrace + 1;
        
        while (pos < code.length() && braceCount > 0) {
            if (code[pos] == '{') braceCount++;
            else if (code[pos] == '}') braceCount--;
            pos++;
        }
        
        if (braceCount == 0) {
            return code.substr(start, pos - start);
        }
        
        return "";
    }
    
    void parseAndMatchParameters(const std::string& extractedCode, 
                                const SyntaxKeywordObject& keyword, 
                                CJMODScanner& scanner) {
        // 解析提取的代码，匹配参数
        std::unordered_map<std::string, std::string> params = parseParameters(extractedCode);
        
        for (const auto& [key, value] : params) {
            Keyword kw(key, "parameter");
            kw.value = value;
            keyword.args.match(key, kw);
        }
    }
    
    std::unordered_map<std::string, std::string> parseParameters(const std::string& code) {
        std::unordered_map<std::string, std::string> params;
        
        size_t braceStart = code.find('{');
        size_t braceEnd = code.find_last_of('}');
        
        if (braceStart == std::string::npos || braceEnd == std::string::npos) {
            return params;
        }
        
        std::string content = code.substr(braceStart + 1, braceEnd - braceStart - 1);
        
        // 简单的键值对解析
        std::stringstream ss(content);
        std::string line;
        
        while (std::getline(ss, line, ',')) {
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
                    params[key] = value;
                }
            }
        }
        
        return params;
    }
    
    std::string replacePrintMyloveCall(const std::string& originalCode, const std::string& generatedCode) {
        size_t start = originalCode.find("printMylove");
        if (start == std::string::npos) return originalCode;
        
        // 找到完整的函数调用
        size_t end = start;
        int parenCount = 0;
        bool foundStart = false;
        
        for (size_t i = start; i < originalCode.length(); i++) {
            if (originalCode[i] == '(') {
                if (!foundStart) foundStart = true;
                parenCount++;
            } else if (originalCode[i] == ')') {
                parenCount--;
                if (parenCount == 0 && foundStart) {
                    end = i + 1;
                    break;
                }
            }
        }
        
        if (end > start) {
            std::string before = originalCode.substr(0, start);
            std::string after = originalCode.substr(end);
            return before + generatedCode + after;
        }
        
        return originalCode;
    }
};

/**
 * iNeverAway CJMOD扩展实现 - 按照CHTL语法文档
 */
class INeverAwayExtension : public CJMODExtension {
public:
    void initialize() override {
        std::cout << "🌟 初始化iNeverAway扩展" << std::endl;
    }
    
    std::string processCHTLJS(const std::string& chtljsCode) override {
        std::cout << "✨ iNeverAway开始处理CHTL JS代码" << std::endl;
        
        if (chtljsCode.find("iNeverAway") == std::string::npos) {
            return chtljsCode;
        }
        
        // 🚀 使用您的设计处理iNeverAway语法
        std::string ignoreSymbols = ",:{}<>()";
        
        const auto keyword = YourSyntaxAnalyzer::syntaxAnalys(R"(
            iNeverAway({
                Void<A>: $,
                Void<B>: $,
                Void: $,
                Ax: $,
            });
        )", ignoreSymbols);
        
        // 🚀 使用高级绑定处理函数参数
        keyword.args.bindAdvanced("Void<A>", [](const AdvancedParamValue& param) -> std::string {
            if (param.type == AdvancedParamType::FUNCTION) {
                std::cout << "🎯 处理Void<A>函数" << std::endl;
                return generateMarkerFunction("Void_A", param.processedValue);
            }
            return param.processedValue;
        });
        
        keyword.args.bindAdvanced("Void<B>", [](const AdvancedParamValue& param) -> std::string {
            if (param.type == AdvancedParamType::FUNCTION) {
                std::cout << "🎯 处理Void<B>函数" << std::endl;
                return generateMarkerFunction("Void_B", param.processedValue);
            }
            return param.processedValue;
        });
        
        keyword.args.bindAdvanced("Void", [](const AdvancedParamValue& param) -> std::string {
            if (param.type == AdvancedParamType::OBJECT) {
                std::cout << "📦 处理Void对象" << std::endl;
                return generateMarkerObject("Void", param.processedValue);
            }
            return param.processedValue;
        });
        
        keyword.args.bindAdvanced("Ax", [](const AdvancedParamValue& param) -> std::string {
            if (param.type == AdvancedParamType::OBJECT) {
                std::cout << "📦 处理Ax对象" << std::endl;
                return generateMarkerObject("Ax", param.processedValue);
            }
            return param.processedValue;
        });
        
        // 🔍 设置扫描器
        CJMODScanner scanner;
        scanner.scanKeyword(keyword.args[0], [&]() {
            std::cout << "🎯 扫描到iNeverAway函数" << std::endl;
            
            // 从实际代码中提取参数值
            std::string extractedCode = extractINeverAwayCall(chtljsCode);
            parseAndMatchINeverAwayParameters(extractedCode, keyword, scanner);
        });
        
        // 执行扫描
        scanner.executeScan(chtljsCode, keyword);
        
        // 生成最终代码
        std::string finalCode = generateINeverAwayCode(keyword);
        
        // 替换原始的iNeverAway调用
        return replaceINeverAwayCall(chtljsCode, finalCode);
    }
    
    std::string getExtensionName() const override {
        return "INeverAway";
    }
    
    std::vector<std::string> getSupportedPatterns() const override {
        return {"iNeverAway", "vir"};
    }

private:
    static std::string generateMarkerFunction(const std::string& name, const std::string& func) {
        std::stringstream js;
        js << "window.__chtl_marker_functions = window.__chtl_marker_functions || {};\n";
        js << "window.__chtl_marker_functions['" << name << "'] = " << func << ";\n";
        js << "function " << name << "() { return window.__chtl_marker_functions['" << name << "'].apply(this, arguments); }";
        return js.str();
    }
    
    static std::string generateMarkerObject(const std::string& name, const std::string& obj) {
        std::stringstream js;
        js << "window.__chtl_marker_objects = window.__chtl_marker_objects || {};\n";
        js << "window.__chtl_marker_objects['" << name << "'] = " << obj << ";";
        return js.str();
    }
    
    std::string extractINeverAwayCall(const std::string& code) {
        size_t start = code.find("iNeverAway");
        if (start == std::string::npos) return "";
        
        size_t openBrace = code.find('{', start);
        if (openBrace == std::string::npos) return "";
        
        int braceCount = 1;
        size_t pos = openBrace + 1;
        
        while (pos < code.length() && braceCount > 0) {
            if (code[pos] == '{') braceCount++;
            else if (code[pos] == '}') braceCount--;
            pos++;
        }
        
        if (braceCount == 0) {
            return code.substr(start, pos - start);
        }
        
        return "";
    }
    
    void parseAndMatchINeverAwayParameters(const std::string& extractedCode, 
                                          const SyntaxKeywordObject& keyword, 
                                          CJMODScanner& scanner) {
        // 解析iNeverAway的复杂参数结构
        std::unordered_map<std::string, std::string> params = parseINeverAwayParameters(extractedCode);
        
        for (const auto& [key, value] : params) {
            Keyword kw(key, "parameter");
            kw.value = value;
            keyword.args.match(key, kw);
        }
    }
    
    std::unordered_map<std::string, std::string> parseINeverAwayParameters(const std::string& code) {
        std::unordered_map<std::string, std::string> params;
        
        size_t braceStart = code.find('{');
        size_t braceEnd = code.find_last_of('}');
        
        if (braceStart == std::string::npos || braceEnd == std::string::npos) {
            return params;
        }
        
        std::string content = code.substr(braceStart + 1, braceEnd - braceStart - 1);
        
        // 解析复杂的键值对，支持泛型语法 Void<A>
        size_t pos = 0;
        while (pos < content.length()) {
            // 跳过空白字符
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
            
            // 处理函数或对象值
            if (pos < content.length() && (content[pos] == 'f' || content[pos] == '{')) {
                if (content.substr(pos, 8) == "function") {
                    // 函数值
                    int parenCount = 0;
                    int braceCount = 0;
                    bool inFunction = true;
                    
                    while (pos < content.length() && inFunction) {
                        if (content[pos] == '(') parenCount++;
                        else if (content[pos] == ')') parenCount--;
                        else if (content[pos] == '{') braceCount++;
                        else if (content[pos] == '}') {
                            braceCount--;
                            if (braceCount == 0) {
                                pos++;
                                break;
                            }
                        }
                        pos++;
                    }
                } else if (content[pos] == '{') {
                    // 对象值
                    int braceCount = 1;
                    pos++;
                    
                    while (pos < content.length() && braceCount > 0) {
                        if (content[pos] == '{') braceCount++;
                        else if (content[pos] == '}') braceCount--;
                        pos++;
                    }
                }
            } else {
                // 简单值，查找到逗号或结束
                while (pos < content.length() && content[pos] != ',' && content[pos] != '}') pos++;
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
    
    std::string generateINeverAwayCode(const SyntaxKeywordObject& keyword) {
        std::stringstream js;
        js << "(function() {\n";
        js << "    // 🌟 iNeverAway标记函数系统\n";
        js << "    console.log('🌟 初始化iNeverAway标记函数');\n";
        js << "    \n";
        
        // 生成所有标记函数和对象
        for (const auto& kw : keyword.args.keywords) {
            if (!kw.value.empty()) {
                js << "    // " << kw.name << "\n";
                js << "    " << kw.value << "\n";
                js << "    \n";
            }
        }
        
        js << "    return {\n";
        js << "        initialized: true,\n";
        js << "        timestamp: Date.now()\n";
        js << "    };\n";
        js << "})()";
        
        return js.str();
    }
    
    std::string replaceINeverAwayCall(const std::string& originalCode, const std::string& generatedCode) {
        size_t start = originalCode.find("iNeverAway");
        if (start == std::string::npos) return originalCode;
        
        // 找到完整的函数调用
        size_t end = start;
        int parenCount = 0;
        bool foundStart = false;
        
        for (size_t i = start; i < originalCode.length(); i++) {
            if (originalCode[i] == '(') {
                if (!foundStart) foundStart = true;
                parenCount++;
            } else if (originalCode[i] == ')') {
                parenCount--;
                if (parenCount == 0 && foundStart) {
                    end = i + 1;
                    break;
                }
            }
        }
        
        if (end > start) {
            std::string before = originalCode.substr(0, start);
            std::string after = originalCode.substr(end);
            return before + generatedCode + after;
        }
        
        return originalCode;
    }
};

/**
 * CJMOD系统初始化器
 */
class CJMODSystemInitializer {
public:
    static void initializeSystem() {
        std::cout << "🚀 初始化CJMOD系统" << std::endl;
        
        auto& manager = CJMODManager::getInstance();
        
        // 注册Chtholly模块的CJMOD扩展
        auto printMylove = std::make_unique<PrintMyloveExtension>();
        printMylove->initialize();
        manager.registerExtension(std::move(printMylove));
        
        auto iNeverAway = std::make_unique<INeverAwayExtension>();
        iNeverAway->initialize();
        manager.registerExtension(std::move(iNeverAway));
        
        std::cout << "✅ CJMOD系统初始化完成" << std::endl;
        
        // 显示已注册的扩展
        auto extensions = manager.getExtensionList();
        std::cout << "📦 已注册的CJMOD扩展: ";
        for (const auto& ext : extensions) {
            std::cout << ext << " ";
        }
        std::cout << std::endl;
    }
};

} // namespace your_cjmod_design
} // namespace chtl