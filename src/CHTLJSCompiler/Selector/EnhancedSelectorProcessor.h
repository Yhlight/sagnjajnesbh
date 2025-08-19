#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace chtl {
namespace chtljs {

/**
 * 增强选择器处理器
 * 严格按照CHTL语法文档实现 {{CSS选择器}} 的完整功能
 */
class EnhancedSelectorProcessor {
public:
    struct SelectorInfo {
        std::string originalSelector;  // 原始的{{选择器}}
        std::string selectorType;      // tag, class, id, complex, indexed
        std::string targetElement;     // 目标元素名
        std::string className;         // 类名（如果有）
        std::string idName;           // ID名（如果有）
        int index = -1;               // 索引（如果有）
        bool isComplex = false;       // 是否为复合选择器
        std::string jsCode;           // 生成的JavaScript代码
    };
    
    EnhancedSelectorProcessor();
    ~EnhancedSelectorProcessor();
    
    // === 按语法文档：增强选择器处理 ===
    
    /**
     * 处理增强选择器
     * 按语法文档：{{CSS选择器}} 转换为DOM对象
     */
    std::string processEnhancedSelector(const std::string& selector);
    
    /**
     * 解析选择器类型
     * 按语法文档支持的选择器类型：
     * - {{box}} : 先判断tag，然后查找类名/id (id优先)
     * - {{.box}} : 查找类名为box的元素
     * - {{#box}} : 查找id为box的元素  
     * - {{button}} : 所有button元素
     * - {{.box button}} : 类名为box的元素的所有button后代
     * - {{button[0]}} : 第一个button元素
     */
    SelectorInfo parseSelector(const std::string& selector);
    
    /**
     * 生成对应的JavaScript代码
     * 按语法文档：创建DOM对象的JavaScript实现
     */
    std::string generateJavaScriptCode(const SelectorInfo& selectorInfo);
    
    // === 选择器类型处理 ===
    
    /**
     * 处理标签选择器：{{button}}
     * 按语法文档：所有的button元素
     */
    std::string processTagSelector(const std::string& tagName);
    
    /**
     * 处理类选择器：{{.box}}
     * 按语法文档：查找类名为box的元素
     */
    std::string processClassSelector(const std::string& className);
    
    /**
     * 处理ID选择器：{{#box}}
     * 按语法文档：查找id为box的元素
     */
    std::string processIdSelector(const std::string& idName);
    
    /**
     * 处理混合选择器：{{box}}
     * 按语法文档：先判断是否为tag，然后查找类名/id (id优先)
     */
    std::string processMixedSelector(const std::string& name);
    
    /**
     * 处理复合选择器：{{.box button}}
     * 按语法文档：查找类名为box的元素的所有button后代
     */
    std::string processComplexSelector(const std::string& complexSelector);
    
    /**
     * 处理索引选择器：{{button[0]}}
     * 按语法文档：精确访问第一个button元素
     */
    std::string processIndexedSelector(const std::string& element, int index);
    
    // === 辅助方法 ===
    
    /**
     * 验证选择器格式
     * 按语法文档：检查选择器是否符合支持的格式
     */
    bool isValidSelector(const std::string& selector);
    
    /**
     * 检查是否为HTML标签
     */
    bool isHtmlTag(const std::string& tagName);
    
    /**
     * 解析复合选择器
     * 例如：".box button" → {".box", "button"}
     */
    std::vector<std::string> parseComplexSelector(const std::string& selector);
    
    /**
     * 提取索引
     * 例如：{{button[0]}} → {"button", 0}
     */
    std::pair<std::string, int> extractIndex(const std::string& selector);
    
    /**
     * 清理选择器字符串
     * 移除 {{ 和 }} 包装
     */
    std::string cleanSelector(const std::string& selector);
    
    // === 错误处理 ===
    void addError(const std::string& message);
    void clearErrors();
    bool hasErrors() const;
    std::vector<std::string> getErrors() const;

private:
    std::vector<std::string> errors_;
    std::unordered_map<std::string, SelectorInfo> selectorCache_; // 选择器缓存
    
    // HTML标签列表（按语法文档）
    std::vector<std::string> htmlTags_;
    
    void initializeHtmlTags();
};

} // namespace chtljs
} // namespace chtl