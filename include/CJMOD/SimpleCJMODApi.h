#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 前向声明
class Syntax;
class Arg;
class CHTLJSFunction;

/**
 * @brief Arg类 - 参数处理核心
 * 设计理念：使用简单，功能强大
 * 
 * 核心特性：
 * - operator()重载：直接处理值
 * - 智能bind：支持任意类型处理器
 * - 自动类型转换：字符串、数字、布尔值
 */
class Arg {
public:
    Arg(const std::string& name = "", bool isPlaceholder = false);
    
    /**
     * @brief operator()重载 - 核心功能
     * 使用方式：arg("value") 或 keyword.args[0]("value")
     */
    std::string operator()(const std::string& value);
    
    /**
     * @brief bind处理器 - 智能类型处理
     * 支持：bind<string>, bind<int>, bind<double>, bind<bool>
     */
    template<typename T>
    void bind(const std::function<std::string(T)>& processor) {
        processor_ = [processor](const std::string& value) -> std::string {
            if constexpr (std::is_same_v<T, std::string>) {
                return processor(value);
            } else if constexpr (std::is_same_v<T, int>) {
                try { return processor(std::stoi(value)); }
                catch (...) { return processor(0); }
            } else if constexpr (std::is_same_v<T, double>) {
                try { return processor(std::stod(value)); }
                catch (...) { return processor(0.0); }
            } else if constexpr (std::is_same_v<T, bool>) {
                return processor(value == "true" || value == "1");
            }
            return value;
        };
        hasBind_ = true;
    }
    
    /**
     * @brief 匹配值
     */
    void match(const std::string& value);
    
    /**
     * @brief 获取处理后的值
     */
    const std::string& getValue() const { return processedValue_; }
    
    /**
     * @brief 访问器
     */
    const std::string& getName() const { return name_; }
    bool isPlaceholder() const { return isPlaceholder_; }
    bool hasBind() const { return hasBind_; }

private:
    std::string name_;
    bool isPlaceholder_;
    bool hasBind_;
    std::string rawValue_;
    std::string processedValue_;
    std::function<std::string(const std::string&)> processor_;
};

/**
 * @brief Syntax类 - 语法分析结果
 * 设计理念：简洁直观，强大易用
 * 
 * 核心特性：
 * - 智能参数管理：自动识别占位符
 * - 链式绑定：支持连续bind操作
 * - 批量处理：一次性处理所有参数
 */
class Syntax {
public:
    Syntax() = default;
    
    /**
     * @brief 添加参数（内部使用）
     */
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 索引访问：syntax.args[0]
     */
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    /**
     * @brief 智能绑定 - 占位符自动计数
     * 使用方式：
     * syntax.bind<string>([](const string& s) { return process(s); });
     */
    template<typename T>
    Syntax& bind(const std::function<std::string(T)>& processor) {
        // 找到下一个未绑定的占位符
        for (auto& arg : args_) {
            if (arg.isPlaceholder() && !arg.hasBind()) {
                arg.bind(processor);
                break;
            }
        }
        return *this;  // 支持链式调用
    }
    
    /**
     * @brief 命名绑定
     * 使用方式：syntax.bind("url", processor);
     */
    template<typename T>
    Syntax& bind(const std::string& name, const std::function<std::string(T)>& processor) {
        for (auto& arg : args_) {
            if (arg.getName() == name) {
                arg.bind(processor);
                break;
            }
        }
        return *this;  // 支持链式调用
    }
    
    /**
     * @brief 批量匹配值
     */
    void match(const std::vector<std::string>& values);
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief 生成结果
     */
    std::string result() const;
    
    /**
     * @brief 参数数量
     */
    size_t length() const { return args_.size(); }
    
    /**
     * @brief 获取所有参数
     */
    const std::vector<Arg>& getArgs() const { return args_; }

private:
    std::vector<Arg> args_;
};

/**
 * @brief CHTLJSFunction类 - 简化流程的核心
 * 设计理念：一个类搞定所有事情
 * 
 * 核心特性：
 * - 自动语法生成：根据函数名和参数自动生成
 * - 托管流程：自动化syntaxAnalys、bind、match、generate
 * - 双指针扫描：内置正确的扫描机制
 * - 前置截取：自动处理复杂语法
 */
class CHTLJSFunction {
public:
    /**
     * @brief 构造函数 - 极简设计
     * @param functionName JS函数名
     * @param paramNames 参数名列表
     */
    CHTLJSFunction(const std::string& functionName, 
                   const std::vector<std::string>& paramNames = {});
    
    /**
     * @brief 简化流程：一步到位的处理器绑定
     * 使用方式：
     * func.bind<string>([](const string& url) { return "\"" + url + "\""; })
     *     .bind<int>([](int width) { return to_string(width); });
     */
    template<typename T>
    CHTLJSFunction& bind(const std::function<std::string(T)>& processor) {
        if (!syntax_) createSyntax();
        syntax_->bind(processor);
        return *this;  // 支持链式调用
    }
    
    /**
     * @brief 命名绑定
     */
    template<typename T>
    CHTLJSFunction& bind(const std::string& paramName, 
                        const std::function<std::string(T)>& processor) {
        if (!syntax_) createSyntax();
        syntax_->bind(paramName, processor);
        return *this;
    }
    
    /**
     * @brief 标准流程：手动控制每个步骤
     */
    std::unique_ptr<Syntax> createSyntax();
    void scanKeyword(const std::string& sourceCode);
    void matchValues(const std::vector<std::string>& values);
    std::string generateCode();
    
    /**
     * @brief 简化流程：一键处理
     * @param sourceCode 源代码
     * @param values 参数值列表
     * @return 生成的JS代码
     */
    std::string process(const std::string& sourceCode, 
                       const std::vector<std::string>& values = {});
    
    /**
     * @brief 获取函数信息
     */
    const std::string& getFunctionName() const { return functionName_; }
    const std::vector<std::string>& getParamNames() const { return paramNames_; }
    Syntax* getSyntax() const { return syntax_.get(); }

private:
    std::string functionName_;
    std::vector<std::string> paramNames_;
    std::unique_ptr<Syntax> syntax_;
    
    // 内部辅助方法
    std::string generateSyntaxPattern();
    void executeDualPointerScan(const std::string& sourceCode);
    void applyFrontExtract();
};

/**
 * @brief 全局函数 - 标准流程入口
 */

/**
 * @brief syntaxAnalys - 语法分析入口
 * 使用方式：auto syntax = syntaxAnalys("func({ url: $, width: $ });");
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, 
                                    const std::string& ignoreChars = ",:{};()");

/**
 * @brief generateCode - 代码生成
 * 使用方式：string code = generateCode(*syntax);
 */
std::string generateCode(const Syntax& syntax);

/**
 * @brief 简化创建函数
 * 使用方式：auto func = createCHTLJSFunction("printMylove", {"url", "mode", "width"});
 */
std::unique_ptr<CHTLJSFunction> createCHTLJSFunction(
    const std::string& functionName,
    const std::vector<std::string>& paramNames = {});

} // namespace CJMOD
} // namespace CHTL