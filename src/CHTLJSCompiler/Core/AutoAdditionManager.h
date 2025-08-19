#pragma once
#include "../Parser/SelectorDetector.h"
#include <string>
#include <memory>

namespace chtl {
namespace chtljs {

/**
 * 自动添加管理器
 * 统一管理局部style/script的自动添加功能
 */
class AutoAdditionManager {
public:
    AutoAdditionManager();
    ~AutoAdditionManager() = default;
    
    // 主处理接口
    std::string processCodeWithAutoAddition(const std::string& original_code);
    
    // 分步处理
    std::vector<SelectorInfo> analyzeSelectors(const std::string& code);
    SelectorDetector::AutoAdditionDecision makeDecision(const std::vector<SelectorInfo>& selectors, 
                                                        const std::string& code);
    std::string applyAutoAddition(const std::string& code, const SelectorDetector::AutoAdditionDecision& decision);
    
    // 配置管理
    void enableAutoAddition(bool enable);
    bool isAutoAdditionEnabled() const;
    void setDebugMode(bool debug) { debug_mode_ = debug; }
    bool isDebugMode() const { return debug_mode_; }
    
    // 统计和报告
    struct ProcessingReport {
        bool changes_made;
        std::string original_code;
        std::string processed_code;
        std::vector<SelectorInfo> detected_selectors;
        SelectorDetector::AutoAdditionDecision decision;
        std::vector<std::string> messages;
        
        ProcessingReport() : changes_made(false) {}
    };
    
    ProcessingReport getLastProcessingReport() const { return last_report_; }
    void printProcessingReport() const;
    
    // 验证功能
    bool validateAutoAddition(const std::string& original_code, const std::string& processed_code);
    std::vector<std::string> getValidationErrors() const;
    
private:
    std::unique_ptr<SelectorDetector> selector_detector_;
    bool debug_mode_;
    ProcessingReport last_report_;
    
    // 内部处理方法
    void logMessage(const std::string& message);
    void generateProcessingReport(const std::string& original_code,
                                 const std::string& processed_code,
                                 const std::vector<SelectorInfo>& selectors,
                                 const SelectorDetector::AutoAdditionDecision& decision);
    
    // 验证辅助
    bool hasValidSyntax(const std::string& code);
    bool preservesOriginalLogic(const std::string& original, const std::string& processed);
};

} // namespace chtljs
} // namespace chtl