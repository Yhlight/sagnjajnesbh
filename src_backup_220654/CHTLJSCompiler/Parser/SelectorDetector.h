#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

namespace chtl {
namespace chtljs {

/**
 * 选择器信息
 */
struct SelectorInfo {
    std::string type;           // "class", "id", "tag", "complex"
    std::string value;          // 选择器值（不包含.或#）
    std::string full_selector;  // 完整选择器（包含.或#）
    size_t position;           // 在代码中的位置
    bool needs_auto_addition;  // 是否需要自动添加
    
    SelectorInfo(const std::string& t = "", const std::string& v = "", 
                const std::string& full = "", size_t pos = 0)
        : type(t), value(v), full_selector(full), position(pos), needs_auto_addition(false) {}
};

/**
 * 自动添加规则
 */
struct AutoAdditionRule {
    std::string target_type;    // "style", "script"
    std::string selector_type;  // "class", "id"
    int priority;              // 优先级（数字越大优先级越高）
    bool enabled;              // 是否启用
    
    AutoAdditionRule(const std::string& target = "", const std::string& selector = "", 
                    int prio = 0, bool enable = true)
        : target_type(target), selector_type(selector), priority(prio), enabled(enable) {}
};

/**
 * 选择器检测器
 * 检测CHTL JS代码中的选择器并决定自动添加逻辑
 */
class SelectorDetector {
public:
    SelectorDetector();
    ~SelectorDetector() = default;
    
    // 主检测接口
    std::vector<SelectorInfo> detectSelectors(const std::string& code);
    std::vector<SelectorInfo> detectCHTLJSSelectors(const std::string& code);
    
    // 选择器解析
    SelectorInfo parseSelector(const std::string& selector_content);
    bool isValidSelector(const std::string& selector);
    std::string extractSelectorValue(const std::string& selector);
    
    // 自动添加决策
    struct AutoAdditionDecision {
        bool should_add_style;
        bool should_add_script;
        std::string style_selector;    // 要添加到style的选择器
        std::string script_selector;   // 要添加到script的选择器
        std::string reason;           // 决策原因
        
        AutoAdditionDecision() : should_add_style(false), should_add_script(false) {}
    };
    
    AutoAdditionDecision makeAutoAdditionDecision(const std::vector<SelectorInfo>& selectors,
                                                  const std::string& existing_code);
    
    // 现有块检测
    bool hasExistingStyleBlock(const std::string& code);
    bool hasExistingScriptBlock(const std::string& code);
    std::vector<std::string> getExistingClassSelectors(const std::string& code);
    std::vector<std::string> getExistingIdSelectors(const std::string& code);
    
    // 优先级处理
    SelectorInfo selectBestSelectorForStyle(const std::vector<SelectorInfo>& selectors);
    SelectorInfo selectBestSelectorForScript(const std::vector<SelectorInfo>& selectors);
    
    // 代码生成
    std::string generateAutoStyleBlock(const std::string& selector);
    std::string generateAutoScriptBlock(const std::string& selector);
    std::string insertAutoBlocks(const std::string& original_code, const AutoAdditionDecision& decision);
    
    // 配置管理
    void setAutoAdditionRule(const AutoAdditionRule& rule);
    std::vector<AutoAdditionRule> getAutoAdditionRules() const;
    void enableAutoAddition(bool enable) { auto_addition_enabled_ = enable; }
    bool isAutoAdditionEnabled() const { return auto_addition_enabled_; }
    
    // 调试和统计
    struct DetectionStatistics {
        size_t total_selectors;
        size_t class_selectors;
        size_t id_selectors;
        size_t complex_selectors;
        size_t auto_added_styles;
        size_t auto_added_scripts;
        
        DetectionStatistics() : total_selectors(0), class_selectors(0), id_selectors(0),
                               complex_selectors(0), auto_added_styles(0), auto_added_scripts(0) {}
    };
    
    DetectionStatistics getStatistics() const { return stats_; }
    void resetStatistics() { stats_ = DetectionStatistics{}; }
    void printDetectionInfo() const;
    
private:
    std::vector<AutoAdditionRule> rules_;
    bool auto_addition_enabled_;
    mutable DetectionStatistics stats_;
    
    // 内部检测方法
    std::vector<size_t> findSelectorPositions(const std::string& code);
    SelectorInfo analyzeSelectorAtPosition(const std::string& code, size_t position);
    
    // 选择器类型判断
    bool isClassSelector(const std::string& selector);
    bool isIdSelector(const std::string& selector);
    bool isTagSelector(const std::string& selector);
    bool isComplexSelector(const std::string& selector);
    
    // 代码分析
    bool hasStyleBlockWithSelector(const std::string& code, const std::string& selector);
    bool hasScriptBlockWithSelector(const std::string& code, const std::string& selector);
    std::string findBestInsertionPoint(const std::string& code, const std::string& block_type);
    
    // 优先级计算
    int calculateSelectorPriority(const SelectorInfo& selector);
    bool shouldPreferId(const std::vector<SelectorInfo>& selectors);
    
    // 统计更新
    void updateStatistics(const SelectorInfo& selector);
    void updateAutoAdditionStats(const AutoAdditionDecision& decision);
};

} // namespace chtljs
} // namespace chtl