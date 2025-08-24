#pragma once

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// 前向声明
namespace Scanner {
    struct CodeFragment;
    enum class FragmentType;
}

namespace Dispatcher {

/**
 * @brief 代码片段处理结果
 */
struct FragmentProcessingResult {
    std::string processedCode;        // 处理后的代码
    bool success = false;             // 处理是否成功
    std::vector<std::string> errors;  // 错误信息
    std::vector<std::string> warnings; // 警告信息
};

/**
 * @brief JavaScript中间处理器
 * 负责合并CHTL JS转换的JS代码与纯JS代码，处理局部script块的CHTL特征
 */
class JavaScriptIntermediateProcessor {
public:
    /**
     * @brief 构造函数
     */
    JavaScriptIntermediateProcessor();
    
    /**
     * @brief 析构函数
     */
    ~JavaScriptIntermediateProcessor();
    
    /**
     * @brief 处理JavaScript相关片段，合并CHTL JS和纯JS
     * @param chtlJSFragments CHTL JS片段列表
     * @param jsFragments 纯JS片段列表
     * @return 合并处理结果
     */
    FragmentProcessingResult ProcessJavaScriptFragments(
        const std::vector<Scanner::CodeFragment>& chtlJSFragments,
        const std::vector<Scanner::CodeFragment>& jsFragments
    );
    
    /**
     * @brief 处理局部script块中的CHTL特征
     * @param scriptContent script块内容
     * @return 处理结果
     */
    FragmentProcessingResult ProcessLocalScriptCHTLFeatures(const std::string& scriptContent);
    
    /**
     * @brief 转换CHTL JS代码为标准JavaScript
     * @param chtlJSCode CHTL JS代码
     * @return 转换后的JavaScript代码
     */
    std::string ConvertCHTLJSToJavaScript(const std::string& chtlJSCode);
    
    /**
     * @brief 合并多个JavaScript代码片段
     * @param jsCodeFragments JavaScript代码片段列表
     * @return 合并后的JavaScript代码
     */
    std::string MergeJavaScriptFragments(const std::vector<std::string>& jsCodeFragments);
    
    /**
     * @brief 设置调试模式
     * @param enabled 是否启用调试
     */
    void SetDebugMode(bool enabled) { debugMode_ = enabled; }

private:
    /**
     * @brief 处理箭头操作符转换
     * @param code 输入代码
     * @return 转换后的代码
     */
    std::string ProcessArrowOperators(const std::string& code);
    
    /**
     * @brief 处理增强选择器
     * @param code 输入代码
     * @return 处理后的代码
     */
    std::string ProcessEnhancedSelectors(const std::string& code);
    
    /**
     * @brief 处理虚对象语法
     * @param code 输入代码
     * @return 处理后的代码
     */
    std::string ProcessVirtualObjects(const std::string& code);
    
    /**
     * @brief 处理CHTL JS特定方法调用
     * @param code 输入代码
     * @return 处理后的代码
     */
    std::string ProcessCHTLJSMethods(const std::string& code);
    
    /**
     * @brief 验证合并后的JavaScript语法
     * @param jsCode JavaScript代码
     * @return 是否有效
     */
    bool ValidateMergedJavaScript(const std::string& jsCode);
    
    /**
     * @brief 记录调试信息
     * @param message 调试信息
     */
    void LogDebug(const std::string& message);

private:
    bool debugMode_ = false;
};

/**
 * @brief CSS中间处理器
 * 负责处理CSS中的CHTL语法和CHTL JS语法
 */
class CSSIntermediateProcessor {
public:
    /**
     * @brief 构造函数
     */
    CSSIntermediateProcessor();
    
    /**
     * @brief 析构函数
     */
    ~CSSIntermediateProcessor();
    
    /**
     * @brief 处理CSS片段中的CHTL特征
     * @param cssFragments CSS片段列表
     * @return 处理结果
     */
    FragmentProcessingResult ProcessCSSFragments(
        const std::vector<Scanner::CodeFragment>& cssFragments
    );
    
    /**
     * @brief 处理CSS中的CHTL变量模板
     * @param cssCode CSS代码
     * @return 处理后的CSS代码
     */
    std::string ProcessCHTLVariableTemplates(const std::string& cssCode);
    
    /**
     * @brief 处理CSS中的CHTL JS表达式
     * @param cssCode CSS代码
     * @return 处理后的CSS代码
     */
    std::string ProcessCHTLJSExpressions(const std::string& cssCode);
    
    /**
     * @brief 设置调试模式
     * @param enabled 是否启用调试
     */
    void SetDebugMode(bool enabled) { debugMode_ = enabled; }

private:
    /**
     * @brief 记录调试信息
     * @param message 调试信息
     */
    void LogDebug(const std::string& message);

private:
    bool debugMode_ = false;
};

} // namespace Dispatcher
} // namespace CHTL