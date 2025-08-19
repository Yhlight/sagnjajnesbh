// ========================================
// CJMOD语法分析器 - 基于您的创新设计
// 使用syntaxAnalys + lambda处理函数的方式
// ========================================

#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>

namespace chtl {
namespace cjmod {

/**
 * 语法参数类型
 */
enum class ArgType {
    NUMBER,         // 数字
    STRING,         // 字符串
    SELECTOR,       // {{选择器}}
    FUNCTION,       // 函数
    OBJECT,         // 对象字面量
    ARRAY,          // 数组
    BOOLEAN,        // 布尔值
    WILDCARD,       // ** 通配符
    AUTO            // 自动推断
};

/**
 * 语法参数值
 */
struct ArgValue {
    ArgType type;
    std::string rawValue;       // 原始值
    std::string processedValue; // 处理后的值
    int position;               // 在代码中的位置
    
    ArgValue() : type(ArgType::AUTO), position(0) {}
    ArgValue(ArgType t, const std::string& raw, int pos = 0) 
        : type(t), rawValue(raw), position(pos) {}
};

/**
 * 语法关键字分析结果
 */
class SyntaxKeyword {
public:
    std::string pattern;                    // 原始模式 "arg ** arg2"
    std::string syntaxName;                 // 语法名称
    std::vector<ArgValue> args;             // 解析出的参数
    
    // 参数处理函数 - 您的创新设计！
    std::vector<std::function<std::string(const ArgValue&)>> argProcessors;
    
    SyntaxKeyword() = default;
    SyntaxKeyword(const std::string& pat) : pattern(pat) {}
    
    /**
     * 设置参数处理器 - 您的接口设计
     * keyword.arg[0] = [](const ArgValue& val) { return process(val); };
     */
    class ArgProxy {
    public:
        ArgProxy(SyntaxKeyword& kw, size_t idx) : keyword_(kw), index_(idx) {}
        
        // 重载赋值操作符，支持lambda函数赋值
        ArgProxy& operator=(std::function<std::string(const ArgValue&)> processor) {
            // 确保vector足够大
            while (keyword_.argProcessors.size() <= index_) {
                keyword_.argProcessors.push_back([](const ArgValue& val) { 
                    return val.rawValue; // 默认处理器
                });
            }
            keyword_.argProcessors[index_] = processor;
            return *this;
        }
        
        // 支持不同类型的lambda
        ArgProxy& operator=(std::function<std::string(int)> processor) {
            return *this = [processor](const ArgValue& val) {
                try {
                    int num = std::stoi(val.rawValue);
                    return processor(num);
                } catch (...) {
                    return val.rawValue;
                }
            };
        }
        
        ArgProxy& operator=(std::function<std::string(const std::string&)> processor) {
            return *this = [processor](const ArgValue& val) {
                return processor(val.rawValue);
            };
        }
        
        ArgProxy& operator=(std::function<std::string()> processor) {
            return *this = [processor](const ArgValue&) {
                return processor();
            };
        }
        
    private:
        SyntaxKeyword& keyword_;
        size_t index_;
    };
    
    /**
     * 参数访问接口 - 您的设计
     * keyword.arg[0] = lambda函数
     */
    ArgProxy operator[](size_t index) {
        return ArgProxy(*this, index);
    }
    
    /**
     * 生成最终的JavaScript代码
     */
    std::string result() const {
        std::string jsCode = "(() => {\n";
        
        // 处理每个参数
        for (size_t i = 0; i < args.size() && i < argProcessors.size(); ++i) {
            std::string processedValue = argProcessors[i](args[i]);
            jsCode += "    const arg" + std::to_string(i) + " = " + processedValue + ";\n";
        }
        
        jsCode += "    \n";
        jsCode += "    // 生成的业务逻辑\n";
        jsCode += generateBusinessLogic();
        jsCode += "\n})()";
        
        return jsCode;
    }

private:
    std::string generateBusinessLogic() const {
        // 根据语法名称生成对应的业务逻辑
        if (syntaxName == "particleEffect") {
            return "    return createParticleEffect(arg0, arg1, arg2);";
        } else if (syntaxName == "smoothMove") {
            return "    return applySmoothMovement(arg0, arg1, arg2);";
        } else {
            return "    console.log('🚀 " + syntaxName + " 执行', arg0, arg1, arg2);";
        }
    }
    
    friend class SyntaxAnalyzer;
};

/**
 * 语法分析器 - 实现您的syntaxAnalys函数
 */
class SyntaxAnalyzer {
public:
    /**
     * 语法分析主函数 - 您的接口设计
     * const keyword = syntaxAnalys("arg ** arg2");
     */
    static SyntaxKeyword syntaxAnalys(const std::string& pattern) {
        SyntaxKeyword keyword(pattern);
        
        // 解析模式
        parsePattern(pattern, keyword);
        
        return keyword;
    }
    
    /**
     * 应用到实际的CHTL JS代码
     */
    static bool applySyntax(SyntaxKeyword& keyword, const std::string& chtljsCode) {
        return matchAndExtractArgs(keyword, chtljsCode);
    }
    
    /**
     * 生成最终代码 - 您的generateCode函数
     */
    static std::string generateCode(const SyntaxKeyword& keyword) {
        return keyword.result();
    }

private:
    /**
     * 解析模式字符串
     * "arg ** arg2" → 识别参数位置和类型
     */
    static void parsePattern(const std::string& pattern, SyntaxKeyword& keyword) {
        std::vector<std::string> tokens = tokenizePattern(pattern);
        
        for (size_t i = 0; i < tokens.size(); ++i) {
            const std::string& token = tokens[i];
            
            if (token == "**") {
                // 通配符参数
                keyword.args.emplace_back(ArgType::WILDCARD, "**", i);
            } else if (token.find("arg") == 0) {
                // 具名参数
                keyword.args.emplace_back(ArgType::AUTO, token, i);
            } else if (token == "selector") {
                // 选择器参数
                keyword.args.emplace_back(ArgType::SELECTOR, token, i);
            } else if (token == "number") {
                // 数字参数
                keyword.args.emplace_back(ArgType::NUMBER, token, i);
            } else if (token == "string") {
                // 字符串参数
                keyword.args.emplace_back(ArgType::STRING, token, i);
            } else if (token == "function") {
                // 函数参数
                keyword.args.emplace_back(ArgType::FUNCTION, token, i);
            } else if (token == "object") {
                // 对象参数
                keyword.args.emplace_back(ArgType::OBJECT, token, i);
            }
        }
    }
    
    /**
     * 模式字符串分词
     */
    static std::vector<std::string> tokenizePattern(const std::string& pattern) {
        std::vector<std::string> tokens;
        std::string current;
        
        for (size_t i = 0; i < pattern.length(); ++i) {
            char c = pattern[i];
            
            if (std::isspace(c)) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
            } else if (c == '*' && i + 1 < pattern.length() && pattern[i + 1] == '*') {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                tokens.push_back("**");
                i++; // 跳过第二个*
            } else {
                current += c;
            }
        }
        
        if (!current.empty()) {
            tokens.push_back(current);
        }
        
        return tokens;
    }
    
    /**
     * 匹配CHTL JS代码并提取参数
     */
    static bool matchAndExtractArgs(SyntaxKeyword& keyword, const std::string& chtljsCode) {
        // 这里实现更智能的匹配算法
        // 不仅仅是正则表达式，而是基于模式的智能匹配
        
        // 简化示例：假设我们有一个函数调用
        // particleEffect({{#container}}, {count: 100}, callback)
        
        size_t funcStart = chtljsCode.find(keyword.syntaxName + "(");
        if (funcStart == std::string::npos) {
            return false;
        }
        
        // 提取参数部分
        size_t paramStart = funcStart + keyword.syntaxName.length() + 1;
        size_t paramEnd = findMatchingParen(chtljsCode, paramStart - 1);
        
        if (paramEnd == std::string::npos) {
            return false;
        }
        
        std::string paramStr = chtljsCode.substr(paramStart, paramEnd - paramStart);
        
        // 智能参数解析
        std::vector<std::string> extractedParams = parseParameters(paramStr);
        
        // 将提取的参数赋值给keyword.args
        for (size_t i = 0; i < extractedParams.size() && i < keyword.args.size(); ++i) {
            keyword.args[i].rawValue = extractedParams[i];
            keyword.args[i].processedValue = extractedParams[i];
        }
        
        return true;
    }
    
    static size_t findMatchingParen(const std::string& code, size_t start) {
        int depth = 1;
        for (size_t i = start + 1; i < code.length(); ++i) {
            if (code[i] == '(') depth++;
            else if (code[i] == ')') depth--;
            
            if (depth == 0) return i;
        }
        return std::string::npos;
    }
    
    static std::vector<std::string> parseParameters(const std::string& paramStr) {
        std::vector<std::string> params;
        std::string current;
        int depth = 0;
        bool inString = false;
        char stringChar = 0;
        
        for (size_t i = 0; i < paramStr.length(); ++i) {
            char c = paramStr[i];
            
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
                        // 去除前后空白
                        current.erase(0, current.find_first_not_of(" \t"));
                        current.erase(current.find_last_not_of(" \t") + 1);
                        params.push_back(current);
                        current.clear();
                    }
                    continue;
                }
            } else {
                if (c == stringChar && (i == 0 || paramStr[i-1] != '\\')) {
                    inString = false;
                }
            }
            
            current += c;
        }
        
        if (!current.empty()) {
            current.erase(0, current.find_first_not_of(" \t"));
            current.erase(current.find_last_not_of(" \t") + 1);
            params.push_back(current);
        }
        
        return params;
    }
};

/**
 * 代码生成器 - 您的generateCode函数
 */
class CodeGenerator {
public:
    /**
     * 生成最终的JavaScript代码
     */
    static std::string generateCode(const SyntaxKeyword& keyword) {
        return keyword.result();
    }
    
    /**
     * 带上下文的代码生成
     */
    static std::string generateCodeWithContext(const SyntaxKeyword& keyword, 
                                              const std::string& context) {
        std::string baseCode = keyword.result();
        
        // 根据上下文优化代码
        if (context == "event_handler") {
            return wrapInEventHandler(baseCode);
        } else if (context == "animation_block") {
            return wrapInAnimationFrame(baseCode);
        }
        
        return baseCode;
    }

private:
    static std::string wrapInEventHandler(const std::string& code) {
        return "try {\n    " + code + "\n} catch (error) {\n    console.error('CJMOD事件处理错误:', error);\n}";
    }
    
    static std::string wrapInAnimationFrame(const std::string& code) {
        return "requestAnimationFrame(() => {\n    " + code + "\n});";
    }
};

} // namespace cjmod
} // namespace chtl