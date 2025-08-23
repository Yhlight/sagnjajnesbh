#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

// 前向声明
class CJMODScanner;

/**
 * @brief 扫描策略枚举
 */
enum class Policy {
    NORMAL,     // 常规
    COLLECT,    // 收集
    SKIP        // 跳过
};

/**
 * @brief Arg类 - 按照您的原始设计
 * 简单直接的参数处理
 */
class Arg {
public:
    /**
     * @brief 构造函数
     */
    Arg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief bind方法 - 绑定值处理函数
     * 按照您的设计：简单的函数绑定，不使用复杂模板
     */
    void bind(const std::function<std::string(const std::string&)>& func);
    
    /**
     * @brief match方法 - 匹配并处理值
     * 按照您的设计：直接传值并调用绑定的函数
     */
    void match(const std::string& value);
    
    /**
     * @brief transform方法 - 转换为JS代码
     * 按照您的设计：简单的字符串替换
     */
    const std::string& transform(const std::string& jsCode);
    
    /**
     * @brief 获取参数名称
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief 是否为占位符
     */
    bool isPlaceholder() const { return isPlaceholder_; }
    
    /**
     * @brief 获取转换后的值
     */
    const std::string& getValue() const { return transformedValue_; }

private:
    std::string name_;                          // 参数名称
    bool isPlaceholder_;                        // 是否为占位符($)
    std::string value_;                         // 原始值
    std::string transformedValue_;              // 转换后的JS代码
    std::function<std::string(const std::string&)> bindFunction_;  // 绑定的处理函数
};

/**
 * @brief Args集合 - 简化的参数集合管理
 * 按照您的设计：简单的vector + 便捷访问方法
 */
class Args {
public:
    Args() = default;
    
    /**
     * @brief 添加参数
     */
    void addArg(const std::string& name, bool isPlaceholder = false);
    
    /**
     * @brief 索引访问 - keyword.args[0]
     */
    Arg& operator[](size_t index);
    const Arg& operator[](size_t index) const;
    
    /**
     * @brief 名称访问便捷方法
     */
    Arg* find(const std::string& name);
    
    /**
     * @brief bind方法 - 按名称绑定
     * keyword.args.bind("url", [](std::string str) { return str; });
     */
    void bind(const std::string& name, const std::function<std::string(const std::string&)>& func);
    
    /**
     * @brief match方法 - 按名称匹配
     * keyword.args.match("url", peekKeyword(1));
     */
    void match(const std::string& name, const std::string& value);
    
    /**
     * @brief result方法 - 组合所有转换后的JS代码
     */
    std::string result() const;
    
    /**
     * @brief 获取参数数量
     */
    size_t length() const { return args_.size(); }

private:
    std::vector<Arg> args_;
    std::unordered_map<std::string, size_t> nameToIndex_;
    int placeholderCount_ = 0;  // 占位符计数，用于重复绑定
};

/**
 * @brief Syntax类 - 按照您的原始设计
 * 简单的语法分析结果容器
 */
class Syntax {
public:
    /**
     * @brief 构造函数
     */
    Syntax() = default;
    
    /**
     * @brief 获取参数集合
     */
    Args& getArgs() { return args_; }
    const Args& getArgs() const { return args_; }
    
    /**
     * @brief 便捷访问：keyword.args
     */
    Args args;

private:
    Args args_;
};

/**
 * @brief CJMODScanner类 - 按照您的原始设计
 * 简单的扫描器接口
 */
class CJMODScanner {
public:
    CJMODScanner();
    
    /**
     * @brief scanKeyword - 核心扫描接口
     * scanner.scanKeyword(keyword.args[0], []() { ... });
     */
    void scanKeyword(const Arg& arg, std::function<void()> handler);
    void scanKeyword(const std::string& keyword, std::function<void()> handler);
    
    /**
     * @brief peekKeyword - 查看偏移关键字
     * peekKeyword(-1), peekKeyword(1)
     */
    std::string peekKeyword(int offset) const;
    
    /**
     * @brief policy系列 - 策略改变
     * scanner.policyChangeBegin(":", Policy::COLLECT);
     */
    void policyChangeBegin(const std::string& trigger, Policy policy);
    std::string policyChangeEnd(const std::string& trigger, Policy policy);
    
    /**
     * @brief 辅助函数
     * 按照您的设计：简单的判断函数
     */
    bool isObject(const std::string& content);
    bool isFunction(const std::string& content);

private:
    std::unordered_map<std::string, std::function<void()>> keywordHandlers_;
    std::vector<std::string> currentTokens_;
    size_t currentPosition_;
    Policy currentPolicy_;
    std::string collectBuffer_;
};

/**
 * @brief 全局函数 - 按照您的原始设计
 * 简单直接的函数接口
 */

/**
 * @brief syntaxAnalys - 语法分析核心函数
 * const keyword = syntaxAnalys("printMylove({ url: $, mode: $ });", ",:{};()");
 */
std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern, const std::string& ignoreChars = "");

/**
 * @brief generateCode - 生成JS代码
 * generateCode(keyword);
 */
std::string generateCode(const Syntax& syntax);

} // namespace CJMOD
} // namespace CHTL