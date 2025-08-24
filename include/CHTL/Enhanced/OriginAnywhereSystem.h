#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace CHTL {
namespace Enhanced {

/**
 * @brief 编程语言上下文类型 - 增强版
 */
enum class ContextType {
    CHTL_ROOT,          // CHTL根上下文
    CHTL_ELEMENT,       // CHTL元素内部
    CHTL_STYLE,         // CHTL样式块
    CHTL_SCRIPT,        // CHTL脚本块
    HTML_ELEMENT,       // HTML元素
    CSS_BLOCK,          // CSS代码块
    JAVASCRIPT_BLOCK,   // JavaScript代码块
    CHTL_JS_BLOCK,      // CHTL JS代码块
    CUSTOM_BLOCK,       // 自定义代码块
    TEMPLATE_BLOCK,     // 模板块
    CONFIGURATION_BLOCK,// 配置块
    IMPORT_BLOCK,       // 导入块
    NAMESPACE_BLOCK,    // 命名空间块
    ORIGIN_BLOCK,       // Origin原始嵌入块内部
    MIXED_CONTEXT,      // 混合上下文
    UNKNOWN_CONTEXT     // 未知上下文
};

/**
 * @brief Origin类型定义
 */
enum class OriginType {
    HTML,           // @Html
    STYLE,          // @Style  
    JAVASCRIPT,     // @JavaScript
    VUE,            // @Vue
    REACT,          // @React
    SVELTE,         // @Svelte
    MARKDOWN,       // @Markdown
    XML,            // @XML
    JSON,           // @JSON
    YAML,           // @YAML
    CUSTOM_TYPE     // 自定义类型
};

/**
 * @brief Origin使用位置信息
 */
struct OriginPosition {
    size_t line;
    size_t column;
    ContextType context;
    std::string surroundingCode;
    std::string parentElement;
    int nestingLevel;
    
    OriginPosition() : line(0), column(0), context(ContextType::UNKNOWN_CONTEXT), nestingLevel(0) {}
};

/**
 * @brief 增强的Origin任意位置管理器
 * 核心理念：[Origin]原始嵌入节点任意地方都可以使用，不应该被束缚，问题由开发者自己控制
 */
class EnhancedOriginManager {
public:
    EnhancedOriginManager();
    
    /**
     * @brief 检查Origin是否可以在指定位置使用
     * @param position Origin使用位置信息
     * @param originType Origin类型
     * @return 始终返回true，因为Origin可以在任意位置使用
     */
    bool canUseOriginAnywhere(const OriginPosition& position, OriginType originType) const;
    
    /**
     * @brief 获取Origin在当前位置的使用建议
     * @param position Origin使用位置信息
     * @param originType Origin类型
     * @return 使用建议和最佳实践
     */
    std::string getOriginUsageAdvice(const OriginPosition& position, OriginType originType) const;
    
    /**
     * @brief 验证Origin使用的语义合理性（仅警告，不阻止）
     * @param position Origin使用位置信息
     * @param originType Origin类型
     * @return {是否语义合理, 建议信息}
     */
    std::pair<bool, std::string> validateOriginSemantics(const OriginPosition& position, 
                                                         OriginType originType) const;
    
    /**
     * @brief 获取Origin在不同上下文中的最佳实践
     * @param context 当前上下文
     * @return 最佳实践建议
     */
    std::vector<std::string> getBestPractices(ContextType context) const;
    
    /**
     * @brief 注册自定义Origin类型
     * @param typeName 自定义类型名称（如"@CustomType"）
     * @param description 类型描述
     * @param compatibleContexts 兼容的上下文列表
     */
    void registerCustomOriginType(const std::string& typeName,
                                 const std::string& description,
                                 const std::vector<ContextType>& compatibleContexts);
    
    /**
     * @brief 获取所有支持的Origin类型
     * @return Origin类型列表及其描述
     */
    std::unordered_map<std::string, std::string> getSupportedOriginTypes() const;

private:
    std::unordered_map<std::string, std::string> customOriginTypes_;
    std::unordered_map<std::string, std::vector<ContextType>> customTypeContexts_;
    
    std::string getContextName(ContextType context) const;
    std::string getOriginTypeName(OriginType type) const;
    bool isSemanticallySensible(ContextType context, OriginType originType) const;
};

/**
 * @brief 增强的上下文注释生成器
 * 核心理念："--"注释会根据上下文生成不同编程语言的注释
 */
class EnhancedContextualCommentGenerator {
public:
    EnhancedContextualCommentGenerator();
    
    /**
     * @brief 智能检测当前上下文类型
     * @param surroundingCode 周围的代码
     * @param cursorPosition 光标位置
     * @return 检测到的上下文类型
     */
    ContextType detectCurrentContext(const std::string& surroundingCode, 
                                   size_t cursorPosition = 0) const;
    
    /**
     * @brief 处理"--"注释，根据上下文生成对应语言的注释
     * @param commentContent 注释内容（去掉"--"前缀）
     * @param context 当前上下文类型
     * @param isMultiLine 是否为多行注释
     * @return 生成的注释字符串
     */
    std::string processDashComment(const std::string& commentContent,
                                 ContextType context,
                                 bool isMultiLine = false) const;
    
    /**
     * @brief 智能处理"--"注释（自动检测上下文）
     * @param commentContent 注释内容
     * @param surroundingCode 周围代码
     * @param cursorPosition 光标位置
     * @return 生成的注释字符串
     */
    std::string smartProcessDashComment(const std::string& commentContent,
                                      const std::string& surroundingCode,
                                      size_t cursorPosition = 0) const;
    
    /**
     * @brief 获取指定上下文的注释示例
     * @param context 上下文类型
     * @return 注释示例
     */
    std::string getCommentExample(ContextType context) const;
    
    /**
     * @brief 获取所有支持的上下文及其注释格式
     * @return {上下文名称: {单行格式, 多行格式}}
     */
    std::unordered_map<std::string, std::pair<std::string, std::string>> getSupportedCommentFormats() const;

private:
    /**
     * @brief 生成HTML注释
     */
    std::string generateHTMLComment(const std::string& content, bool isMultiLine) const;
    
    /**
     * @brief 生成CSS注释
     */
    std::string generateCSSComment(const std::string& content, bool isMultiLine) const;
    
    /**
     * @brief 生成JavaScript注释
     */
    std::string generateJavaScriptComment(const std::string& content, bool isMultiLine) const;
    
    /**
     * @brief 生成CHTL注释
     */
    std::string generateCHTLComment(const std::string& content, bool isMultiLine) const;
    
    /**
     * @brief 生成CHTL JS注释
     */
    std::string generateCHTLJSComment(const std::string& content, bool isMultiLine) const;
    
    /**
     * @brief 生成自定义格式注释
     */
    std::string generateCustomComment(const std::string& content, 
                                    ContextType context, 
                                    bool isMultiLine) const;
    
    /**
     * @brief 检测是否在Origin块内部
     */
    bool isInsideOriginBlock(const std::string& surroundingCode, size_t position) const;
    
    /**
     * @brief 检测是否在样式块内部
     */
    bool isInsideStyleBlock(const std::string& surroundingCode, size_t position) const;
    
    /**
     * @brief 检测是否在脚本块内部
     */
    bool isInsideScriptBlock(const std::string& surroundingCode, size_t position) const;
    
    /**
     * @brief 检测是否在HTML元素内部
     */
    bool isInsideHTMLElement(const std::string& surroundingCode, size_t position) const;
    
    /**
     * @brief 检测是否在CHTL JS块内部
     */
    bool isInsideCHTLJSBlock(const std::string& surroundingCode, size_t position) const;
};

/**
 * @brief 统一的增强功能管理器
 */
class EnhancedFeatureManager {
public:
    EnhancedFeatureManager();
    
    /**
     * @brief 获取Origin管理器
     */
    EnhancedOriginManager& getOriginManager() { return *originManager_; }
    
    /**
     * @brief 获取注释生成器
     */
    EnhancedContextualCommentGenerator& getCommentGenerator() { return *commentGenerator_; }
    
    /**
     * @brief 处理完整的CHTL代码片段
     * @param code CHTL代码
     * @return 处理后的代码
     */
    std::string processCode(const std::string& code) const;
    
    /**
     * @brief 验证代码中的Origin使用
     * @param code CHTL代码
     * @return 验证报告
     */
    std::vector<std::string> validateOriginUsage(const std::string& code) const;
    
    /**
     * @brief 转换代码中的"--"注释
     * @param code CHTL代码
     * @return 转换后的代码
     */
    std::string transformDashComments(const std::string& code) const;

private:
    std::unique_ptr<EnhancedOriginManager> originManager_;
    std::unique_ptr<EnhancedContextualCommentGenerator> commentGenerator_;
};

/**
 * @brief 全局获取增强功能管理器
 */
EnhancedFeatureManager& getEnhancedFeatureManager();

} // namespace Enhanced
} // namespace CHTL