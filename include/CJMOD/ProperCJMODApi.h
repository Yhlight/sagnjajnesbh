#pragma once

#include "Scanner/CHTLUnifiedScanner.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

namespace CHTL {
namespace CJMOD {

/**
 * @brief 正确的CJMOD API - 基于CHTL优秀架构设计
 * 
 * 核心原理：
 * 1. 统一扫描器负责代码片段切割和分类（不做词法分析）
 * 2. CJMOD API接收CodeFragment，使用peekKeyword提取参数
 * 3. 专注于参数处理和JS代码生成
 * 
 * 工作流程：
 * 源代码 → CHTLUnifiedScanner → CodeFragment → CJMOD API → 标准JS
 */

// ============ 全局注册表（语法文档1425行：允许全局变量管理状态） ============
extern std::unordered_map<std::string, std::string> g_virtualObjectRegistry;
extern std::unordered_map<std::string, std::string> g_globalFunctionRegistry;
extern std::unordered_map<std::string, std::vector<std::string>> g_eventDelegateRegistry;

/**
 * @brief Arg类 - 参数处理核心
 * 基于统一扫描器的CodeFragment进行参数提取
 */
class Arg {
public:
    Arg();
    explicit Arg(const std::string& name);
    
    // 从CodeFragment中提取参数值（使用peekKeyword技术）
    void extractFromFragment(const Scanner::CodeFragment& fragment, int offset = 0);
    
    // 参数值访问
    const std::string& getName() const { return name_; }
    const std::string& getValue() const { return value_; }
    const std::string& getType() const { return type_; }
    
    // 参数验证
    bool isValid() const { return !value_.empty(); }
    bool isString() const { return type_ == "string"; }
    bool isNumber() const { return type_ == "number"; }
    bool isObject() const { return type_ == "object"; }
    bool isFunction() const { return type_ == "function"; }
    
private:
    std::string name_;
    std::string value_;
    std::string type_;
    
    // peekKeyword核心实现
    std::string peekKeyword(const Scanner::CodeFragment& fragment, int offset);
    std::string extractBackwardParameter(const std::string& content, size_t pos, int steps);
    std::string extractForwardParameter(const std::string& content, size_t pos, int steps);
    void analyzeType(const std::string& value);
};

/**
 * @brief Syntax类 - 语法分析和绑定
 * 处理CHTL JS语法模式，与统一扫描器协作
 */
class Syntax {
public:
    Syntax();
    explicit Syntax(const std::string& pattern);
    
    // 语法分析（基于CodeFragment，不重新做词法分析）
    void analyzeFromFragment(const Scanner::CodeFragment& fragment);
    
    // 参数绑定
    void bind(const std::string& paramName, std::function<std::string(const std::string&)> processor);
    
    // 代码转换
    std::string transform(const std::string& targetTemplate);
    
    // 获取分析结果
    const std::vector<Arg>& getArgs() const { return args_; }
    const std::string& getPattern() const { return pattern_; }
    
private:
    std::string pattern_;
    std::vector<Arg> args_;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> processors_;
    
    // 从CodeFragment中解析参数
    void parseArgsFromFragment(const Scanner::CodeFragment& fragment);
};

/**
 * @brief CHTLJSFunction类 - CHTL JS函数处理核心
 * 与统一扫描器完美协作，处理CHTL JS语法扩展
 */
class CHTLJSFunction {
public:
    CHTLJSFunction();
    explicit CHTLJSFunction(const std::string& functionName);
    
    // ============ 核心工作流程（与统一扫描器协作） ============
    
    /**
     * @brief 处理来自统一扫描器的代码片段
     * 这是CJMOD API的核心入口点
     */
    std::string processFragment(const Scanner::CodeFragment& fragment);
    
    /**
     * @brief 注册CHTL JS关键字到统一扫描器
     * 让统一扫描器知道要寻找哪些关键字
     */
    void registerKeyword(const std::string& keyword, Scanner::CHTLUnifiedScanner& scanner);
    
    // ============ 原始API工作流程 ============
    
    // 1. syntaxAnalys - 语法分析
    std::unique_ptr<Syntax> syntaxAnalys(const std::string& pattern);
    
    // 2. bind - 参数绑定
    void bind(const std::string& paramName, std::function<std::string(const std::string&)> processor);
    
    // 3. transform - 代码转换
    std::string transform(const std::string& targetTemplate);
    
    // 4. scanKeyword - 设置扫描关键字（告诉统一扫描器要找什么）
    void scanKeyword(const std::string& keyword);
    
    // 5. match - 匹配参数值（从CodeFragment中提取）
    void match(const Scanner::CodeFragment& fragment);
    
    // 6. result - 获取处理结果
    const std::string& result() const { return generatedCode_; }
    
    // 7. generateCode - 生成最终JS代码
    std::string generateCode();
    
    // ============ 辅助方法 ============
    
    const std::string& getFunctionName() const { return functionName_; }
    bool isProcessed() const { return processed_; }
    
    // 虚对象支持（委托给CHTL JS）
    void registerVirtualObject(const std::string& virName, const std::string& sourceFunction);
    std::string delegateVirtualCall(const std::string& virName, const std::string& methodName);
    
protected:
    std::string functionName_;
    std::string generatedCode_;
    bool processed_;
    
    std::unique_ptr<Syntax> syntax_;
    std::vector<std::string> registeredKeywords_;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> processors_;
    
    // 与统一扫描器协作的核心方法
    void processWithUnifiedScanner(const Scanner::CodeFragment& fragment);
};

// ============ 简化流程 - createCHTLJSFunction自动集成原始API流程 ============

/**
 * @brief 简化的CHTL JS函数创建器
 * 自动集成原始API的7步流程
 */
class SimplifiedCHTLJSFunction : public CHTLJSFunction {
public:
    SimplifiedCHTLJSFunction(const std::string& functionName);
    
    /**
     * @brief 自动处理 - 集成完整的原始API流程
     * @param fragment 来自统一扫描器的代码片段
     * @param pattern 语法模式
     * @param processors 参数处理器映射
     * @return 生成的JS代码
     */
    std::string autoProcess(const Scanner::CodeFragment& fragment, 
                           const std::string& pattern,
                           const std::unordered_map<std::string, std::function<std::string(const std::string&)>>& processors = {});
};

// ============ 全局便利函数 ============

/**
 * @brief 创建CJMOD处理器（标准流程）
 */
std::unique_ptr<CHTLJSFunction> createCJMODProcessor(const std::string& functionName = "");

/**
 * @brief 创建CHTL JS函数（简化流程）
 */
std::unique_ptr<SimplifiedCHTLJSFunction> createCHTLJSFunction(const std::string& functionName);

/**
 * @brief 注册CJMOD扩展到统一扫描器
 * @param scanner 统一扫描器实例
 * @param keywords 要注册的关键字列表
 */
void registerCJMODExtensions(Scanner::CHTLUnifiedScanner& scanner, const std::vector<std::string>& keywords);

/**
 * @brief 处理CJMOD代码片段的统一入口
 * @param fragment 来自统一扫描器的代码片段
 * @param functionName CJMOD函数名
 * @return 生成的JS代码
 */
std::string processCJMODFragment(const Scanner::CodeFragment& fragment, const std::string& functionName);

// ============ 虚对象委托（委托给CHTL JS，不重新实现） ============
namespace VirtualObjectDelegate {
    void registerVirtualObject(const std::string& virName, const std::string& sourceFunction);
    std::string delegateVirtualCall(const std::string& virName, const std::string& methodName);
    bool isVirtualObjectRegistered(const std::string& virName);
    void clearVirtualObjectRegistry();
}

} // namespace CJMOD
} // namespace CHTL