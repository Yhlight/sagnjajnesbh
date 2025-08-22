#pragma once

#include <string>
#include <unordered_map>

namespace CHTL {
namespace Comments {

/**
 * @brief 编程语言上下文枚举
 */
enum class ProgrammingContext {
    HTML,           // HTML上下文
    CSS,            // CSS上下文
    JAVASCRIPT,     // JavaScript上下文
    CHTL,           // CHTL上下文
    CHTL_JS,        // CHTL JS上下文
    UNKNOWN         // 未知上下文
};

/**
 * @brief 上下文注释生成器
 */
class ContextualCommentGenerator {
public:
    /**
     * @brief 构造函数
     */
    ContextualCommentGenerator();
    
    /**
     * @brief 根据上下文生成注释
     * @param comment 注释内容
     * @param context 编程语言上下文
     * @return 生成的注释字符串
     */
    std::string GenerateComment(const std::string& comment, ProgrammingContext context) const;
    
    /**
     * @brief 检测当前上下文
     * @param surroundingCode 周围的代码
     * @return 检测到的上下文
     */
    ProgrammingContext DetectContext(const std::string& surroundingCode) const;
    
    /**
     * @brief 处理"--"注释
     * @param comment 注释内容（去掉--前缀）
     * @param context 当前上下文
     * @return 生成的注释
     */
    std::string ProcessDashComment(const std::string& comment, ProgrammingContext context) const;

private:
    /**
     * @brief 生成HTML注释
     * @param comment 注释内容
     * @return HTML注释
     */
    std::string GenerateHTMLComment(const std::string& comment) const;
    
    /**
     * @brief 生成CSS注释
     * @param comment 注释内容
     * @return CSS注释
     */
    std::string GenerateCSSComment(const std::string& comment) const;
    
    /**
     * @brief 生成JavaScript注释
     * @param comment 注释内容
     * @return JavaScript注释
     */
    std::string GenerateJavaScriptComment(const std::string& comment) const;
    
    /**
     * @brief 生成CHTL注释
     * @param comment 注释内容
     * @return CHTL注释
     */
    std::string GenerateCHTLComment(const std::string& comment) const;
    
    /**
     * @brief 生成CHTL JS注释
     * @param comment 注释内容
     * @return CHTL JS注释
     */
    std::string GenerateCHTLJSComment(const std::string& comment) const;
};

/**
 * @brief [Origin]任意位置支持管理器
 */
class OriginAnywhereManager {
public:
    /**
     * @brief 构造函数
     */
    OriginAnywhereManager();
    
    /**
     * @brief 检查[Origin]是否可以在当前位置使用
     * @param context 当前上下文
     * @param surroundingCode 周围代码
     * @return 是否可以使用
     */
    bool CanUseOriginHere(const std::string& context, const std::string& surroundingCode) const;
    
    /**
     * @brief 验证[Origin]使用的合理性
     * @param originType 原始嵌入类型
     * @param context 当前上下文
     * @return 验证结果和建议
     */
    std::pair<bool, std::string> ValidateOriginUsage(const std::string& originType, 
                                                     const std::string& context) const;
    
    /**
     * @brief 获取[Origin]使用建议
     * @param context 当前上下文
     * @return 使用建议
     */
    std::string GetOriginUsageSuggestion(const std::string& context) const;

private:
    /**
     * @brief 检查上下文兼容性
     * @param originType 原始嵌入类型
     * @param context 当前上下文
     * @return 是否兼容
     */
    bool IsContextCompatible(const std::string& originType, const std::string& context) const;
};

} // namespace Comments
} // namespace CHTL