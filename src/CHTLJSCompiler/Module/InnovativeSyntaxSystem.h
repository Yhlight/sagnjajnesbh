// ========================================
// 创新语法系统 - 基于您的完整设计
// syntaxAnalys + lambda处理器 + 扫描器接口
// ========================================

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <stack>

namespace chtl {
namespace innovative_cjmod {

/**
 * 关键字值类型
 */
struct KeywordValue {
    std::string value;
    std::string type;       // "number", "string", "selector", "function", "object"
    int position;
    
    KeywordValue() : position(0) {}
    KeywordValue(const std::string& v, const std::string& t, int p = 0) 
        : value(v), type(t), position(p) {}
    
    // 自动类型转换
    operator int() const { 
        try { return std::stoi(value); } catch(...) { return 0; }
    }
    operator double() const { 
        try { return std::stod(value); } catch(...) { return 0.0; }
    }
    operator std::string() const { 
        return value; 
    }
};

/**
 * 语法关键字 - 您的核心设计
 */
class SyntaxKeyword {
public:
    std::string pattern;                    // "arg ** arg2"
    std::string syntaxName;                 // 从CHTL JS代码中提取的语法名
    std::vector<KeywordValue> parsedArgs;   // 解析出的实际参数
    
    // 参数处理函数数组 - 您的设计！
    std::vector<std::function<std::string(const KeywordValue&)>> argProcessors;
    
    // 扫描器回调函数 - 您的扩展设计！
    std::function<void()> scannerCallback;
    
    SyntaxKeyword() = default;
    SyntaxKeyword(const std::string& pat) : pattern(pat) {}
    
    /**
     * 参数代理类 - 支持您的 keyword.arg[0] = lambda 语法
     */
    class ArgProxy {
    public:
        ArgProxy(SyntaxKeyword& kw, size_t idx) : keyword_(kw), index_(idx) {}
        
        // 支持各种lambda类型
        ArgProxy& operator=(std::function<std::string(const KeywordValue&)> processor) {
            ensureSize();
            keyword_.argProcessors[index_] = processor;
            return *this;
        }
        
        ArgProxy& operator=(std::function<std::string(int)> processor) {
            ensureSize();
            keyword_.argProcessors[index_] = [processor](const KeywordValue& val) {
                return processor(static_cast<int>(val));
            };
            return *this;
        }
        
        ArgProxy& operator=(std::function<std::string(const std::string&)> processor) {
            ensureSize();
            keyword_.argProcessors[index_] = [processor](const KeywordValue& val) {
                return processor(static_cast<std::string>(val));
            };
            return *this;
        }
        
        ArgProxy& operator=(std::function<std::string()> processor) {
            ensureSize();
            keyword_.argProcessors[index_] = [processor](const KeywordValue&) {
                return processor();
            };
            return *this;
        }
        
        // 支持直接返回 ** 的特殊语法
        ArgProxy& operator=(const std::string& directReturn) {
            ensureSize();
            keyword_.argProcessors[index_] = [directReturn](const KeywordValue&) {
                return directReturn;
            };
            return *this;
        }
        
    private:
        void ensureSize() {
            while (keyword_.argProcessors.size() <= index_) {
                keyword_.argProcessors.push_back([](const KeywordValue& val) { 
                    return val.value; 
                });
            }
        }
        
        SyntaxKeyword& keyword_;
        size_t index_;
    };
    
    /**
     * 参数访问 - keyword.arg[index]
     */
    ArgProxy arg(size_t index) {
        return ArgProxy(*this, index);
    }
    
    // 重载[]操作符支持 keyword.arg[0] 语法
    ArgProxy operator[](size_t index) {
        return arg(index);
    }
    
    /**
     * 生成结果 - keyword.result()
     */
    std::string result() const {
        std::stringstream js;
        js << "(() => {\n";
        
        // 处理每个参数
        for (size_t i = 0; i < parsedArgs.size() && i < argProcessors.size(); ++i) {
            std::string processedValue = argProcessors[i](parsedArgs[i]);
            js << "    const arg" << i << " = " << processedValue << ";\n";
        }
        
        js << "    \n";
        js << "    // 执行" << syntaxName << "逻辑\n";
        js << generateSyntaxLogic();
        js << "\n})()";
        
        return js.str();
    }

private:
    std::string generateSyntaxLogic() const {
        // 根据语法名称生成对应逻辑
        if (syntaxName == "particleEffect") {
            return "    return createAdvancedParticleSystem(arg0, arg1, arg2);";
        } else if (syntaxName == "smartValidate") {
            return "    return performIntelligentValidation(arg0, arg1, arg2);";
        } else if (syntaxName == "aiChat") {
            return "    return processAIConversation(arg0, arg1, arg2);";
        } else {
            return "    console.log('🚀 " + syntaxName + "执行:', arg0, arg1, arg2);\n    return {success: true, args: [arg0, arg1, arg2]};";
        }
    }
};

/**
 * 扫描器接口 - 您的scanner.scanKeyword设计
 */
class CJMODScanner {
public:
    /**
     * 当前扫描位置的关键字栈
     */
    std::vector<KeywordValue> keywordStack;
    
    /**
     * 当前扫描的语法关键字
     */
    SyntaxKeyword* currentKeyword = nullptr;
    
    /**
     * 扫描关键字 - 您的接口设计
     * scanner.scanKeyword(keyword.arg[1], []() { ... });
     */
    void scanKeyword(const SyntaxKeyword::ArgProxy& argProxy, std::function<void()> callback) {
        // 注册扫描回调
        scanCallbacks_[&argProxy] = callback;
    }
    
    /**
     * 查看相对位置的关键字 - 您的peekKeyword设计
     * peekKeyword(-1) 查看前一个关键字
     * peekKeyword(1) 查看后一个关键字
     */
    KeywordValue peekKeyword(int offset) {
        int currentPos = static_cast<int>(keywordStack.size()) - 1;
        int targetPos = currentPos + offset;
        
        if (targetPos >= 0 && targetPos < static_cast<int>(keywordStack.size())) {
            return keywordStack[targetPos];
        }
        
        return KeywordValue("", "unknown");
    }
    
    /**
     * 执行扫描过程
     */
    void executeScan(SyntaxKeyword& keyword, const std::string& chtljsCode) {
        currentKeyword = &keyword;
        
        // 解析代码，构建关键字栈
        buildKeywordStack(chtljsCode);
        
        // 执行所有注册的扫描回调
        for (auto& pair : scanCallbacks_) {
            try {
                pair.second(); // 执行回调函数
            } catch (const std::exception& e) {
                std::cerr << "❌ 扫描回调执行错误: " << e.what() << std::endl;
            }
        }
    }
    
    /**
     * 清理扫描状态
     */
    void clearScanState() {
        keywordStack.clear();
        scanCallbacks_.clear();
        currentKeyword = nullptr;
    }

private:
    std::unordered_map<const SyntaxKeyword::ArgProxy*, std::function<void()>> scanCallbacks_;
    
    /**
     * 构建关键字栈
     */
    void buildKeywordStack(const std::string& code) {
        keywordStack.clear();
        
        // 智能分词和关键字识别
        std::vector<std::string> tokens = tokenize(code);
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            const std::string& token = tokens[i];
            
            // 识别不同类型的关键字
            if (isSelector(token)) {
                keywordStack.emplace_back(token, "selector", i);
            } else if (isNumber(token)) {
                keywordStack.emplace_back(token, "number", i);
            } else if (isString(token)) {
                keywordStack.emplace_back(token, "string", i);
            } else if (isFunction(token)) {
                keywordStack.emplace_back(token, "function", i);
            } else if (isObject(token)) {
                keywordStack.emplace_back(token, "object", i);
            } else {
                keywordStack.emplace_back(token, "identifier", i);
            }
        }
    }
    
    std::vector<std::string> tokenize(const std::string& code) {
        std::vector<std::string> tokens;
        std::string current;
        bool inString = false;
        char stringChar = 0;
        int depth = 0;
        
        for (size_t i = 0; i < code.length(); ++i) {
            char c = code[i];
            
            if (!inString) {
                if (c == '"' || c == '\'') {
                    inString = true;
                    stringChar = c;
                    current += c;
                } else if (c == '(' || c == '{' || c == '[') {
                    current += c;
                    depth++;
                } else if (c == ')' || c == '}' || c == ']') {
                    current += c;
                    depth--;
                } else if ((c == ',' || std::isspace(c)) && depth == 0) {
                    if (!current.empty()) {
                        tokens.push_back(trim(current));
                        current.clear();
                    }
                } else {
                    current += c;
                }
            } else {
                current += c;
                if (c == stringChar && (i == 0 || code[i-1] != '\\')) {
                    inString = false;
                }
            }
        }
        
        if (!current.empty()) {
            tokens.push_back(trim(current));
        }
        
        return tokens;
    }
    
    bool isSelector(const std::string& token) {
        return token.length() > 4 && token.substr(0, 2) == "{{" && token.substr(token.length()-2) == "}}";
    }
    
    bool isNumber(const std::string& token) {
        try {
            std::stod(token);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool isString(const std::string& token) {
        return token.length() >= 2 && 
               ((token[0] == '"' && token[token.length()-1] == '"') ||
                (token[0] == '\'' && token[token.length()-1] == '\''));
    }
    
    bool isFunction(const std::string& token) {
        return token.find("function") != std::string::npos || 
               (token.find("(") != std::string::npos && token.find(")") != std::string::npos);
    }
    
    bool isObject(const std::string& token) {
        return token.length() >= 2 && token[0] == '{' && token[token.length()-1] == '}';
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
};

/**
 * 语法分析器 - 您的syntaxAnalys函数
 */
class InnovativeSyntaxAnalyzer {
public:
    /**
     * 语法分析主函数 - 您的设计
     */
    static SyntaxKeyword syntaxAnalys(const std::string& pattern) {
        SyntaxKeyword keyword(pattern);
        parsePattern(pattern, keyword);
        return keyword;
    }
    
    /**
     * 应用语法到CHTL JS代码
     */
    static bool applySyntaxToCode(SyntaxKeyword& keyword, const std::string& chtljsCode, CJMODScanner& scanner) {
        // 从CHTL JS代码中提取语法名称
        extractSyntaxName(chtljsCode, keyword);
        
        // 解析参数
        extractArguments(chtljsCode, keyword);
        
        // 执行扫描器处理
        scanner.executeScan(keyword, chtljsCode);
        
        return true;
    }

private:
    static void parsePattern(const std::string& pattern, SyntaxKeyword& keyword) {
        // 解析模式字符串
        std::vector<std::string> tokens = tokenizePattern(pattern);
        
        for (const auto& token : tokens) {
            KeywordValue placeholder;
            placeholder.value = token;
            placeholder.type = inferType(token);
            keyword.parsedArgs.push_back(placeholder);
        }
    }
    
    static std::vector<std::string> tokenizePattern(const std::string& pattern) {
        std::vector<std::string> tokens;
        std::stringstream ss(pattern);
        std::string token;
        
        while (ss >> token) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    static std::string inferType(const std::string& token) {
        if (token == "**") return "wildcard";
        if (token.find("arg") == 0) return "auto";
        if (token == "selector") return "selector";
        if (token == "number") return "number";
        if (token == "string") return "string";
        if (token == "function") return "function";
        if (token == "object") return "object";
        return "unknown";
    }
    
    static void extractSyntaxName(const std::string& code, SyntaxKeyword& keyword) {
        // 提取函数名作为语法名称
        size_t parenPos = code.find('(');
        if (parenPos != std::string::npos) {
            size_t start = code.find_last_of(" \t\n", parenPos);
            start = (start == std::string::npos) ? 0 : start + 1;
            keyword.syntaxName = code.substr(start, parenPos - start);
        }
    }
    
    static void extractArguments(const std::string& code, SyntaxKeyword& keyword) {
        // 提取实际参数并更新parsedArgs
        size_t start = code.find('(');
        size_t end = code.find_last_of(')');
        
        if (start != std::string::npos && end != std::string::npos && end > start) {
            std::string paramStr = code.substr(start + 1, end - start - 1);
            std::vector<std::string> params = parseParameters(paramStr);
            
            // 更新parsedArgs
            for (size_t i = 0; i < params.size() && i < keyword.parsedArgs.size(); ++i) {
                keyword.parsedArgs[i].value = params[i];
                keyword.parsedArgs[i].position = i;
            }
        }
    }
    
    static std::vector<std::string> parseParameters(const std::string& paramStr) {
        std::vector<std::string> params;
        std::string current;
        int depth = 0;
        bool inString = false;
        char stringChar = 0;
        
        for (char c : paramStr) {
            if (!inString) {
                if (c == '"' || c == '\'') {
                    inString = true;
                    stringChar = c;
                } else if (c == '(' || c == '{' || c == '[') {
                    depth++;
                } else if (c == ')' || c == '}' || c == ']') {
                    depth--;
                } else if (c == ',' && depth == 0) {
                    if (!current.empty()) {
                        params.push_back(trim(current));
                        current.clear();
                    }
                    continue;
                }
            } else {
                if (c == stringChar) {
                    inString = false;
                }
            }
            
            current += c;
        }
        
        if (!current.empty()) {
            params.push_back(trim(current));
        }
        
        return params;
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
};

/**
 * 代码生成器 - 您的generateCode函数
 */
class InnovativeCodeGenerator {
public:
    /**
     * 生成代码 - generateCode(keyword)
     */
    static std::string generateCode(const SyntaxKeyword& keyword) {
        return keyword.result();
    }
    
    /**
     * 带优化的代码生成
     */
    static std::string generateOptimizedCode(const SyntaxKeyword& keyword) {
        std::string baseCode = keyword.result();
        
        // 代码优化
        baseCode = optimizeJavaScript(baseCode);
        
        // 添加错误处理
        baseCode = addErrorHandling(baseCode, keyword.syntaxName);
        
        return baseCode;
    }

private:
    static std::string optimizeJavaScript(const std::string& code) {
        // 简单的代码优化
        std::string optimized = code;
        
        // 移除多余的空行
        std::regex multipleNewlines(R"(\n\s*\n\s*\n)");
        optimized = std::regex_replace(optimized, multipleNewlines, "\n\n");
        
        return optimized;
    }
    
    static std::string addErrorHandling(const std::string& code, const std::string& syntaxName) {
        std::stringstream wrapped;
        wrapped << "try {\n";
        wrapped << "    " << code << "\n";
        wrapped << "} catch (error) {\n";
        wrapped << "    console.error('❌ CJMOD " << syntaxName << " 执行错误:', error);\n";
        wrapped << "    return { error: error.message, syntax: '" << syntaxName << "' };\n";
        wrapped << "}";
        
        return wrapped.str();
    }
};

} // namespace innovative_cjmod
} // namespace chtl