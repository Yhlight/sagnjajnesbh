#pragma once
#include <string>
<parameter name="vector">
#include <unordered_map>
#include <unordered_set>
#include <optional>

namespace chtl {
namespace chtljs {

/**
 * 无序和可选键值对处理器
 * 扩展CHTL JS支持无修饰字面量、无序键值对、可选键值对
 */
class UnorderedOptionalProcessor {
public:
    struct KeyValuePair {
        std::string key;
        std::string value;
        bool isOptional = false;        // 是否为可选键值对
        bool hasUndecoratedLiteral = false; // 是否包含无修饰字面量
        int originalOrder = 0;          // 原始顺序
    };
    
    struct ProcessedObject {
        std::vector<KeyValuePair> pairs;
        bool supportsUnordered = true;   // 是否支持无序
        bool supportsOptional = true;    // 是否支持可选
        std::string processedCode;       // 处理后的JavaScript代码
    };
    
    UnorderedOptionalProcessor();
    ~UnorderedOptionalProcessor();
    
    // === 无修饰字面量处理 ===
    
    /**
     * 处理无修饰字面量
     * 在CHTL JS中支持无引号的字符串字面量
     */
    std::string processUndecoratedLiterals(const std::string& code);
    
    /**
     * 检测无修饰字面量
     * 识别没有引号包围的字符串
     */
    bool isUndecoratedLiteral(const std::string& value);
    
    /**
     * 转换无修饰字面量为标准JavaScript
     * color: red → color: "red"
     */
    std::string convertUndecoratedLiteral(const std::string& literal);
    
    // === 无序键值对处理 ===
    
    /**
     * 处理无序键值对
     * 允许键值对以任意顺序出现，不影响功能
     */
    ProcessedObject processUnorderedKeyValues(const std::string& objectCode);
    
    /**
     * 重排序键值对
     * 根据重要性和依赖关系重新排序
     */
    std::vector<KeyValuePair> reorderKeyValuePairs(const std::vector<KeyValuePair>& pairs);
    
    /**
     * 分析键值对依赖
     * 确定哪些键值对有依赖关系
     */
    std::unordered_map<std::string, std::vector<std::string>> analyzeKeyDependencies(const std::vector<KeyValuePair>& pairs);
    
    // === 可选键值对处理 ===
    
    /**
     * 处理可选键值对
     * 支持键值对的可选性，缺失时使用默认值
     */
    ProcessedObject processOptionalKeyValues(const std::string& objectCode);
    
    /**
     * 检测可选键值对
     * 识别可以省略的键值对
     */
    bool isOptionalKey(const std::string& key, const std::string& context);
    
    /**
     * 提供默认值
     * 为缺失的可选键值对提供合理的默认值
     */
    std::string getDefaultValue(const std::string& key, const std::string& context);
    
    /**
     * 验证必需键值对
     * 确保所有必需的键值对都存在
     */
    bool validateRequiredKeys(const std::vector<KeyValuePair>& pairs, const std::string& context);
    
    // === 上下文感知处理 ===
    
    /**
     * 根据上下文处理键值对
     * listen、delegate、animate等不同上下文有不同的处理规则
     */
    ProcessedObject processContextAwareKeyValues(const std::string& objectCode, const std::string& context);
    
    /**
     * 获取上下文的键值对规则
     * 不同CHTL JS函数有不同的键值对规则
     */
    struct ContextRules {
        std::vector<std::string> requiredKeys;    // 必需键
        std::vector<std::string> optionalKeys;    // 可选键
        std::unordered_map<std::string, std::string> defaultValues; // 默认值
        bool allowsUnordered = true;              // 是否允许无序
        std::vector<std::string> keyOrder;        // 推荐顺序
    };
    
    ContextRules getContextRules(const std::string& context);
    
    // === 智能解析 ===
    
    /**
     * 智能解析对象内容
     * 处理嵌套、注释、复杂结构
     */
    std::vector<KeyValuePair> parseObjectContent(const std::string& content);
    
    /**
     * 处理嵌套对象
     * 支持多层嵌套的键值对结构
     */
    std::string processNestedObjects(const std::string& content);
    
    /**
     * 智能分割键值对
     * 考虑嵌套结构和字符串内容的智能分割
     */
    std::vector<std::string> smartSplitKeyValuePairs(const std::string& content);
    
    // === 代码生成 ===
    
    /**
     * 生成优化的JavaScript对象
     * 根据处理结果生成最优的JavaScript代码
     */
    std::string generateOptimizedObject(const ProcessedObject& processed);
    
    /**
     * 生成默认值注入代码
     * 为缺失的可选键值对注入默认值
     */
    std::string generateDefaultValueInjection(const std::vector<KeyValuePair>& pairs, const ContextRules& rules);
    
    /**
     * 生成验证代码
     * 运行时验证必需键值对的存在性
     */
    std::string generateValidationCode(const ContextRules& rules);
    
    // === 辅助方法 ===
    
    /**
     * 清理和标准化键名
     */
    std::string normalizeKey(const std::string& key);
    
    /**
     * 检测键值对格式
     */
    bool isValidKeyValueFormat(const std::string& pair);
    
    /**
     * 提取键和值
     */
    std::pair<std::string, std::string> extractKeyValue(const std::string& pair);
    
    // === 错误处理 ===
    void addError(const std::string& message);
    void addWarning(const std::string& message);
    void clearErrors();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;
    std::vector<std::string> getWarnings() const;

private:
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    // 上下文规则缓存
    std::unordered_map<std::string, ContextRules> contextRulesCache_;
    
    // 无修饰字面量识别
    std::unordered_set<std::string> commonUndecoratedValues_;
    
    void initializeCommonUndecoratedValues();
    void initializeContextRules();
};

} // namespace chtljs
} // namespace chtl