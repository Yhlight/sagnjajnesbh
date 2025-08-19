// ========================================
// CJMOD开发接口 - 标准化的CJMOD开发API
// 这是开发者创建CJMOD时必须遵循的接口规范
// ========================================

#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace chtl {
namespace cjmod {

/**
 * CJMOD语法匹配结果
 */
struct SyntaxMatch {
    bool matched = false;               // 是否匹配成功
    std::string syntaxName;             // 语法名称
    std::vector<std::string> parameters; // 提取的参数
    std::unordered_map<std::string, std::string> namedParams; // 命名参数
    int startPosition = 0;              // 匹配开始位置
    int endPosition = 0;                // 匹配结束位置
};

/**
 * CJMOD上下文信息
 * 提供给C++扩展的编译上下文
 */
struct CJMODContext {
    std::string currentFile;            // 当前编译的文件路径
    std::string moduleName;             // 当前模块名
    int lineNumber = 0;                 // 当前行号
    int columnNumber = 0;               // 当前列号
    std::vector<std::string> importedModules; // 已导入的模块
    std::unordered_map<std::string, std::string> variables; // 可用变量
};

/**
 * CJMOD错误信息
 */
struct CJMODError {
    std::string message;                // 错误消息
    std::string context;                // 错误上下文
    int lineNumber = 0;                 // 错误行号
    int columnNumber = 0;               // 错误列号
    std::string severity = "error";     // 错误级别: "info", "warning", "error"
};

/**
 * CJMOD基础接口类
 * 所有CJMOD扩展都必须继承这个接口
 */
class ICJMODExtension {
public:
    virtual ~ICJMODExtension() = default;
    
    /**
     * 获取扩展名称
     */
    virtual std::string getExtensionName() const = 0;
    
    /**
     * 获取扩展版本
     */
    virtual std::string getExtensionVersion() const = 0;
    
    /**
     * 获取支持的语法列表
     */
    virtual std::vector<std::string> getSupportedSyntax() const = 0;
    
    /**
     * 检查是否支持某个语法
     */
    virtual bool supportsSyntax(const std::string& syntaxName) const = 0;
    
    /**
     * 解析CHTL JS语法
     * 输入：CHTL JS代码片段
     * 输出：语法匹配结果
     */
    virtual SyntaxMatch parseSyntax(const std::string& chtljs_code) const = 0;
    
    /**
     * 生成JavaScript代码
     * 输入：语法匹配结果 + 编译上下文
     * 输出：对应的JavaScript代码
     */
    virtual std::string generateJavaScript(const SyntaxMatch& match, const CJMODContext& context) const = 0;
    
    /**
     * 验证语法参数
     * 在生成代码前验证参数的有效性
     */
    virtual bool validateParameters(const SyntaxMatch& match, std::vector<CJMODError>& errors) const = 0;
    
    /**
     * 获取语法文档
     * 返回语法的使用文档和示例
     */
    virtual std::string getSyntaxDocumentation(const std::string& syntaxName) const = 0;
    
    /**
     * 初始化扩展
     * 在加载CJMOD时调用
     */
    virtual bool initialize(const CJMODContext& context) = 0;
    
    /**
     * 清理扩展
     * 在卸载CJMOD时调用
     */
    virtual void cleanup() = 0;
};

/**
 * CJMOD辅助工具类
 * 提供常用的开发辅助函数
 */
class CJMODHelper {
public:
    /**
     * 解析CHTL JS选择器
     * {{#id}} → "document.getElementById('id')"
     * {{.class}} → "document.querySelector('.class')"
     * {{element}} → "document.querySelector('element')"
     */
    static std::string parseSelector(const std::string& selector);
    
    /**
     * 解析函数参数
     * "function(a, b) { ... }" → 提取参数列表和函数体
     */
    static std::vector<std::string> parseFunctionParameters(const std::string& functionCode);
    static std::string parseFunctionBody(const std::string& functionCode);
    
    /**
     * 生成安全的JavaScript标识符
     */
    static std::string generateSafeIdentifier(const std::string& name);
    
    /**
     * 转义JavaScript字符串
     */
    static std::string escapeJavaScriptString(const std::string& str);
    
    /**
     * 验证JavaScript语法
     */
    static bool isValidJavaScript(const std::string& jsCode);
    
    /**
     * 生成唯一ID
     */
    static std::string generateUniqueId(const std::string& prefix = "cjmod");
    
    /**
     * 解析JSON格式的参数
     */
    static std::unordered_map<std::string, std::string> parseJsonParameters(const std::string& jsonStr);
    
    /**
     * 格式化JavaScript代码
     */
    static std::string formatJavaScript(const std::string& jsCode, int indentLevel = 0);
};

/**
 * CJMOD注册宏
 * 简化CJMOD扩展的注册过程
 */
#define CJMOD_REGISTER_EXTENSION(className) \
    extern "C" ICJMODExtension* createCJMODExtension() { \
        return new className(); \
    } \
    extern "C" void destroyCJMODExtension(ICJMODExtension* extension) { \
        delete extension; \
    } \
    extern "C" const char* getCJMODExtensionName() { \
        static className instance; \
        return instance.getExtensionName().c_str(); \
    }

/**
 * 简化的CJMOD语法处理宏
 * 用于快速创建简单的语法扩展
 */
#define CJMOD_SIMPLE_SYNTAX(syntaxName, pattern, generator) \
    class syntaxName##Extension : public ICJMODExtension { \
    public: \
        std::string getExtensionName() const override { return #syntaxName; } \
        std::string getExtensionVersion() const override { return "1.0.0"; } \
        std::vector<std::string> getSupportedSyntax() const override { return {#syntaxName}; } \
        bool supportsSyntax(const std::string& name) const override { return name == #syntaxName; } \
        SyntaxMatch parseSyntax(const std::string& code) const override { \
            /* 实现解析逻辑 */ \
        } \
        std::string generateJavaScript(const SyntaxMatch& match, const CJMODContext& context) const override { \
            return generator(match, context); \
        } \
        /* 其他必需方法的默认实现... */ \
    };

} // namespace cjmod
} // namespace chtl