#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "CHTL/AST/CHTLASTNodes.h"

namespace CHTL {
namespace Generator {

/**
 * @brief 选择器类型
 */
enum class SelectorType {
    CLASS_SELECTOR,     // 类选择器 .class
    ID_SELECTOR,        // ID选择器 #id
    ELEMENT_SELECTOR    // 元素选择器 element
};

/**
 * @brief 引用上下文类型
 */
enum class ReferenceContext {
    STYLE_BLOCK,        // 样式块中的&引用
    SCRIPT_BLOCK        // 脚本块中的{{&}}引用
};

/**
 * @brief 自动化配置
 */
struct AutomationConfig {
    bool disableStyleAutoAddClass = false;     // 禁止局部样式块自动添加类选择器
    bool disableStyleAutoAddId = false;        // 禁止局部样式块自动添加id选择器
    bool disableScriptAutoAddClass = true;     // 禁止局部脚本自动添加类选择器
    bool disableScriptAutoAddId = true;        // 禁止局部脚本自动添加id选择器
};

/**
 * @brief 选择器信息
 */
struct SelectorInfo {
    SelectorType type;
    std::string name;
    bool isFirst;           // 是否为第一个
    bool isUsed;            // 是否已使用
    
    SelectorInfo() : type(SelectorType::CLASS_SELECTOR), isFirst(false), isUsed(false) {}
};

/**
 * @brief 选择器自动化管理器
 * 实现选择器自动化与引用规则，支持配置控制
 */
class SelectorAutomation {
public:
    SelectorAutomation();
    
    /**
     * @brief 设置自动化配置
     * @param config 配置对象
     */
    void SetConfig(const AutomationConfig& config);
    
    /**
     * @brief 从上下文变量更新配置
     * @param contextVariables 上下文变量
     */
    void UpdateConfigFromContext(const std::unordered_map<std::string, std::string>& contextVariables);
    
    /**
     * @brief 分析样式块中的选择器
     * @param styleBlock 样式块节点
     * @return 选择器信息列表
     */
    std::vector<SelectorInfo> AnalyzeStyleBlockSelectors(std::shared_ptr<AST::StyleBlockNode> styleBlock);
    
    /**
     * @brief 分析脚本块中的选择器使用
     * @param scriptBlock 脚本块节点
     * @return 选择器信息列表
     */
    std::vector<SelectorInfo> AnalyzeScriptBlockSelectors(std::shared_ptr<AST::ScriptBlockNode> scriptBlock);
    
    /**
     * @brief 处理样式块的引用选择器&
     * @param styleBlock 样式块节点
     * @param element 父元素节点
     * @return 优先选择的选择器（class优先）
     */
    std::string ProcessStyleReference(std::shared_ptr<AST::StyleBlockNode> styleBlock,
                                     std::shared_ptr<AST::ElementNode> element);
    
    /**
     * @brief 处理脚本块的引用选择器{{&}}
     * @param scriptBlock 脚本块节点
     * @param element 父元素节点
     * @return 优先选择的选择器（id优先）
     */
    std::string ProcessScriptReference(std::shared_ptr<AST::ScriptBlockNode> scriptBlock,
                                      std::shared_ptr<AST::ElementNode> element);
    
    /**
     * @brief 自动添加缺失的class属性
     * @param element 元素节点
     * @param firstClassSelector 第一个类选择器名称
     * @return 是否添加成功
     */
    bool AutoAddClassAttribute(std::shared_ptr<AST::ElementNode> element, 
                              const std::string& firstClassSelector);
    
    /**
     * @brief 自动添加缺失的id属性
     * @param element 元素节点
     * @param firstIdSelector 第一个ID选择器名称
     * @return 是否添加成功
     */
    bool AutoAddIdAttribute(std::shared_ptr<AST::ElementNode> element, 
                           const std::string& firstIdSelector);
    
    /**
     * @brief 检查脚本块选择器自动化条件
     * @param element 元素节点
     * @param styleBlock 对应的样式块（如果有）
     * @return 是否满足自动化条件
     */
    bool CheckScriptSelectorAutomation(std::shared_ptr<AST::ElementNode> element,
                                      std::shared_ptr<AST::StyleBlockNode> styleBlock);

private:
    AutomationConfig config_;
    
    /**
     * @brief 提取选择器名称（去掉.或#前缀）
     * @param selector 选择器字符串
     * @param type 选择器类型
     * @return 选择器名称
     */
    std::string ExtractSelectorName(const std::string& selector, SelectorType type);
    
    /**
     * @brief 检查元素是否有指定属性
     * @param element 元素节点
     * @param attributeName 属性名
     * @return 是否有该属性
     */
    bool ElementHasAttribute(std::shared_ptr<AST::ElementNode> element, 
                            const std::string& attributeName);
    
    /**
     * @brief 获取元素的属性值
     * @param element 元素节点
     * @param attributeName 属性名
     * @return 属性值
     */
    std::string GetElementAttribute(std::shared_ptr<AST::ElementNode> element, 
                                   const std::string& attributeName);
    
    /**
     * @brief 设置元素属性
     * @param element 元素节点
     * @param attributeName 属性名
     * @param attributeValue 属性值
     */
    void SetElementAttribute(std::shared_ptr<AST::ElementNode> element, 
                            const std::string& attributeName, 
                            const std::string& attributeValue);
    
    /**
     * @brief 检查是否为类选择器
     * @param selector 选择器字符串
     * @return 是否为类选择器
     */
    bool IsClassSelector(const std::string& selector);
    
    /**
     * @brief 检查是否为ID选择器
     * @param selector 选择器字符串
     * @return 是否为ID选择器
     */
    bool IsIdSelector(const std::string& selector);
    
    /**
     * @brief 从AST节点中提取选择器
     * @param node AST节点
     * @param selectors 选择器列表（输出参数）
     */
    void ExtractSelectorsFromNode(std::shared_ptr<AST::ASTNode> node, 
                                 std::vector<SelectorInfo>& selectors);
    
    /**
     * @brief 从CHTL JS表达式中提取选择器引用
     * @param chtljsNode CHTL JS节点
     * @param selectors 选择器列表（输出参数）
     */
    void ExtractSelectorsFromCHTLJS(std::shared_ptr<AST::CHTLJSNode> chtljsNode,
                                   std::vector<SelectorInfo>& selectors);
};

} // namespace Generator
} // namespace CHTL