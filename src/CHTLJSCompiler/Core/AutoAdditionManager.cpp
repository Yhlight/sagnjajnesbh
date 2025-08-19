#include "AutoAdditionManager.h"
#include <iostream>
#include <sstream>

namespace chtl {
namespace chtljs {

AutoAdditionManager::AutoAdditionManager() : debug_mode_(false) {
    selector_detector_ = std::make_unique<SelectorDetector>();
}

std::string AutoAdditionManager::processCodeWithAutoAddition(const std::string& original_code) {
    if (debug_mode_) {
        std::cout << "开始处理自动添加..." << std::endl;
    }
    
    // 1. 分析选择器
    auto selectors = analyzeSelectors(original_code);
    
    if (debug_mode_) {
        std::cout << "检测到 " << selectors.size() << " 个选择器" << std::endl;
    }
    
    // 2. 制定决策
    auto decision = makeDecision(selectors, original_code);
    
    if (debug_mode_) {
        std::cout << "决策结果: " << decision.reason << std::endl;
        std::cout << "添加style: " << (decision.should_add_style ? "是" : "否") << std::endl;
        std::cout << "添加script: " << (decision.should_add_script ? "是" : "否") << std::endl;
    }
    
    // 3. 应用自动添加
    std::string processed_code = applyAutoAddition(original_code, decision);
    
    // 4. 生成处理报告
    generateProcessingReport(original_code, processed_code, selectors, decision);
    
    return processed_code;
}

std::vector<SelectorInfo> AutoAdditionManager::analyzeSelectors(const std::string& code) {
    return selector_detector_->detectSelectors(code);
}

SelectorDetector::AutoAdditionDecision AutoAdditionManager::makeDecision(
    const std::vector<SelectorInfo>& selectors, const std::string& code) {
    return selector_detector_->makeAutoAdditionDecision(selectors, code);
}

std::string AutoAdditionManager::applyAutoAddition(const std::string& code, 
                                                   const SelectorDetector::AutoAdditionDecision& decision) {
    if (!decision.should_add_style && !decision.should_add_script) {
        return code; // 无需修改
    }
    
    return selector_detector_->insertAutoBlocks(code, decision);
}

void AutoAdditionManager::enableAutoAddition(bool enable) {
    selector_detector_->enableAutoAddition(enable);
}

bool AutoAdditionManager::isAutoAdditionEnabled() const {
    return selector_detector_->isAutoAdditionEnabled();
}

void AutoAdditionManager::generateProcessingReport(const std::string& original_code,
                                                   const std::string& processed_code,
                                                   const std::vector<SelectorInfo>& selectors,
                                                   const SelectorDetector::AutoAdditionDecision& decision) {
    last_report_.original_code = original_code;
    last_report_.processed_code = processed_code;
    last_report_.detected_selectors = selectors;
    last_report_.decision = decision;
    last_report_.changes_made = (original_code != processed_code);
    
    last_report_.messages.clear();
    
    if (last_report_.changes_made) {
        if (decision.should_add_style) {
            last_report_.messages.push_back("自动添加了style块，选择器: " + decision.style_selector);
        }
        if (decision.should_add_script) {
            last_report_.messages.push_back("自动添加了script块，选择器: " + decision.script_selector);
        }
    } else {
        last_report_.messages.push_back("无需自动添加");
    }
    
    last_report_.messages.push_back("决策原因: " + decision.reason);
}

void AutoAdditionManager::printProcessingReport() const {
    std::cout << "=== 自动添加处理报告 ===" << std::endl;
    std::cout << "是否有变更: " << (last_report_.changes_made ? "是" : "否") << std::endl;
    std::cout << "检测到的选择器数量: " << last_report_.detected_selectors.size() << std::endl;
    
    std::cout << "检测到的选择器:" << std::endl;
    for (const auto& selector : last_report_.detected_selectors) {
        std::cout << "  " << selector.type << ": " << selector.full_selector << std::endl;
    }
    
    std::cout << "处理消息:" << std::endl;
    for (const auto& message : last_report_.messages) {
        std::cout << "  - " << message << std::endl;
    }
    
    if (last_report_.changes_made) {
        std::cout << "\n处理后的代码:" << std::endl;
        std::cout << last_report_.processed_code << std::endl;
    }
}

bool AutoAdditionManager::validateAutoAddition(const std::string& original_code, const std::string& processed_code) {
    // 基本验证：处理后的代码应该包含原始代码的所有内容
    if (processed_code.find(original_code.substr(0, 100)) == std::string::npos) {
        return false;
    }
    
    // 验证语法正确性
    return hasValidSyntax(processed_code);
}

bool AutoAdditionManager::hasValidSyntax(const std::string& code) {
    // 简化的语法验证：检查括号匹配
    int brace_count = 0;
    bool in_string = false;
    char string_char = '\0';
    
    for (char c : code) {
        if (!in_string && (c == '"' || c == '\'')) {
            in_string = true;
            string_char = c;
        } else if (in_string && c == string_char) {
            in_string = false;
        } else if (!in_string) {
            if (c == '{') brace_count++;
            else if (c == '}') brace_count--;
        }
    }
    
    return brace_count == 0; // 括号应该匹配
}

void AutoAdditionManager::logMessage(const std::string& message) {
    if (debug_mode_) {
        std::cout << "AutoAdditionManager: " << message << std::endl;
    }
}

} // namespace chtljs
} // namespace chtl