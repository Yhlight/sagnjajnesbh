// ========================================
// CHTL JS处理器 - 严格按照CHTL语法文档
// 实现所有CHTL JS特性
// ========================================

#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <sstream>

namespace chtl {
namespace chtljs {

/**
 * 增强选择器处理器 - 按照语法文档实现
 * 支持: {{CSS选择器}}, {{.class}}, {{#id}}, {{tag}}, {{selector[index]}}
 */
class EnhancedSelectorProcessor {
public:
    /**
     * 处理增强选择器语法
     */
    static std::string processEnhancedSelectors(const std::string& code);
    
private:
    static std::string processSingleSelector(const std::string& selector);
    static bool isValidSelector(const std::string& selector);
    static std::string generateSelectorCode(const std::string& selector);
};

/**
 * 箭头操作符处理器 - 按照语法文档实现
 * -> 与 . 完全等价，用于明确使用CHTL JS语法
 */
class ArrowOperatorProcessor {
public:
    /**
     * 处理箭头操作符语法
     */
    static std::string processArrowOperator(const std::string& code);
    
private:
    static bool isInStringLiteral(const std::string& code, size_t pos);
    static bool isInComment(const std::string& code, size_t pos);
};

/**
 * Listen增强监听器 - 按照语法文档实现
 * 支持所有JS事件的快捷绑定
 */
class ListenProcessor {
public:
    /**
     * 处理listen语法
     */
    static std::string processListen(const std::string& code);
    
    /**
     * 获取支持的所有JS事件
     */
    static std::vector<std::string> getAllJSEvents();
    
private:
    static std::string generateListenCode(const std::string& selector, const std::string& events);
    static std::string parseEventObject(const std::string& eventObj);
    static bool isValidEvent(const std::string& eventName);
};

/**
 * Delegate事件委托处理器 - 按照语法文档实现
 * 全局注册表管理，避免重复绑定
 */
class DelegateProcessor {
public:
    /**
     * 处理delegate语法
     */
    static std::string processDelegate(const std::string& code);
    
    /**
     * 初始化全局委托注册表
     */
    static std::string generateDelegateRegistry();
    
private:
    static std::string generateDelegateCode(const std::string& parentSelector, const std::string& config);
    static std::string parseDelegateConfig(const std::string& config);
    static std::unordered_set<std::string> registeredDelegates;
};

/**
 * Animate动画处理器 - 按照语法文档实现
 * 封装requestAnimationFrame，支持缓动函数和关键帧
 */
class AnimateProcessor {
public:
    /**
     * 处理animate语法
     */
    static std::string processAnimate(const std::string& code);
    
private:
    static std::string generateAnimateCode(const std::string& config);
    static std::string parseAnimateConfig(const std::string& config);
    static std::string generateEasingFunction(const std::string& easing);
    static std::string generateKeyframes(const std::string& keyframes);
};

/**
 * Vir虚对象处理器 - 按照语法文档实现
 * 编译期语法糖，创建全局函数引用
 */
class VirProcessor {
public:
    /**
     * 处理vir语法
     */
    static std::string processVir(const std::string& code);
    
    /**
     * 解析虚对象调用
     */
    static std::string processVirCall(const std::string& code);
    
private:
    static std::string generateVirObject(const std::string& name, const std::string& definition);
    static std::string generateVirCallCode(const std::string& name, const std::string& method);
    static std::unordered_map<std::string, std::unordered_map<std::string, std::string>> virObjects;
};

/**
 * 局部Script处理器 - 按照语法文档实现
 * 自动添加到高优先级全局script，不全局污染
 */
class LocalScriptProcessor {
public:
    /**
     * 处理局部script块
     */
    static std::string processLocalScript(const std::string& code, const std::string& elementContext);
    
    /**
     * 生成全局script合并代码
     */
    static std::string generateGlobalScriptMerge();
    
    /**
     * 添加局部script到全局队列
     */
    static void addLocalScript(const std::string& script, int priority = 100);
    
private:
    static std::vector<std::pair<std::string, int>> localScripts;
    static std::string wrapLocalScript(const std::string& script);
};

/**
 * CHTL JS主处理器 - 协调所有子处理器
 */
class CHTLJSProcessor {
public:
    /**
     * 处理完整的CHTL JS代码
     */
    static std::string processCHTLJS(const std::string& code);
    
    /**
     * 初始化CHTL JS处理器
     */
    static void initialize();
    
    /**
     * 检查是否为CHTL JS语法
     */
    static bool isCHTLJSSyntax(const std::string& code);
    
private:
    static bool initialized;
    static std::vector<std::function<std::string(const std::string&)>> processors;
    
    static void registerProcessors();
    static std::string applyAllProcessors(const std::string& code);
};

/**
 * 自动添加管理器 - 按照语法文档实现
 * 处理缺什么自动添加什么的逻辑
 */
class AutoAdditionManager {
public:
    /**
     * 处理自动添加逻辑
     */
    static std::string processAutoAddition(const std::string& code, const std::string& elementInfo);
    
    /**
     * 检查元素是否需要自动添加class/id
     */
    static bool needsAutoAddition(const std::string& elementInfo, const std::string& selectorUsage);
    
    /**
     * 生成自动添加的class/id
     */
    static std::string generateAutoAddition(const std::string& selectorType, const std::string& selectorName);
    
private:
    static std::string extractSelectorUsage(const std::string& code);
    static bool hasExplicitClassOrId(const std::string& elementInfo);
};

} // namespace chtljs
} // namespace chtl