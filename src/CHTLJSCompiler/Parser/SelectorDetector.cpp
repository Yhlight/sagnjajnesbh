#include "SelectorDetector.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace chtljs {

SelectorDetector::SelectorDetector() : auto_addition_enabled_(true) {
    // 初始化默认规则
    
    // 局部style添加规则：优先添加第一个类选择器
    rules_.emplace_back("style", "class", 1, true);
    
    // 局部script添加规则：优先添加第一个ID选择器
    rules_.emplace_back("script", "id", 2, true);  // ID优先级更高
    
    resetStatistics();
}

std::vector<SelectorInfo> SelectorDetector::detectSelectors(const std::string& code) {
    std::vector<SelectorInfo> selectors;
    
    // 检测CHTL JS选择器 {{...}}
    auto chtljs_selectors = detectCHTLJSSelectors(code);
    selectors.insert(selectors.end(), chtljs_selectors.begin(), chtljs_selectors.end());
    
    // 更新统计
    for (const auto& selector : selectors) {
        updateStatistics(selector);
    }
    
    stats_.total_selectors = selectors.size();
    
    return selectors;
}

std::vector<SelectorInfo> SelectorDetector::detectCHTLJSSelectors(const std::string& code) {
    std::vector<SelectorInfo> selectors;
    
    // 使用正则表达式查找所有 {{...}} 选择器
    std::regex selector_pattern(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator iter(code.begin(), code.end(), selector_pattern);
    std::sregex_iterator end;
    
    while (iter != end) {
        std::smatch match = *iter;
        std::string selector_content = match[1].str();
        size_t position = match.position();
        
        SelectorInfo selector = parseSelector(selector_content);
        selector.position = position;
        selector.full_selector = match[0].str(); // 包含{{}}
        
        if (isValidSelector(selector_content)) {
            selectors.push_back(selector);
        }
        
        ++iter;
    }
    
    return selectors;
}

SelectorInfo SelectorDetector::parseSelector(const std::string& selector_content) {
    SelectorInfo info;
    
    // 去除首尾空白
    std::string trimmed = selector_content;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    if (trimmed.empty()) {
        info.type = "unknown";
        return info;
    }
    
    // 判断选择器类型
    if (isClassSelector(trimmed)) {
        info.type = "class";
        info.value = trimmed.substr(1); // 去除.
        info.full_selector = trimmed;
    } else if (isIdSelector(trimmed)) {
        info.type = "id";
        info.value = trimmed.substr(1); // 去除#
        info.full_selector = trimmed;
    } else if (isTagSelector(trimmed)) {
        info.type = "tag";
        info.value = trimmed;
        info.full_selector = trimmed;
    } else {
        info.type = "complex";
        info.value = trimmed;
        info.full_selector = trimmed;
    }
    
    return info;
}

bool SelectorDetector::isValidSelector(const std::string& selector) {
    if (selector.empty()) return false;
    
    // 基本选择器验证
    return isClassSelector(selector) || 
           isIdSelector(selector) || 
           isTagSelector(selector) ||
           isComplexSelector(selector);
}

bool SelectorDetector::isClassSelector(const std::string& selector) {
    return !selector.empty() && selector[0] == '.' && selector.length() > 1;
}

bool SelectorDetector::isIdSelector(const std::string& selector) {
    return !selector.empty() && selector[0] == '#' && selector.length() > 1;
}

bool SelectorDetector::isTagSelector(const std::string& selector) {
    if (selector.empty()) return false;
    
    // 标签选择器：字母开头，只包含字母数字和连字符
    if (!std::isalpha(selector[0])) return false;
    
    for (char c : selector) {
        if (!std::isalnum(c) && c != '-') return false;
    }
    
    return true;
}

bool SelectorDetector::isComplexSelector(const std::string& selector) {
    // 复杂选择器：包含空格、>、+、~等
    return selector.find(' ') != std::string::npos ||
           selector.find('>') != std::string::npos ||
           selector.find('+') != std::string::npos ||
           selector.find('~') != std::string::npos ||
           selector.find('[') != std::string::npos;
}

SelectorDetector::AutoAdditionDecision SelectorDetector::makeAutoAdditionDecision(
    const std::vector<SelectorInfo>& selectors, const std::string& existing_code) {
    
    AutoAdditionDecision decision;
    
    if (!auto_addition_enabled_ || selectors.empty()) {
        decision.reason = "自动添加未启用或无选择器";
        return decision;
    }
    
    // 检查现有块
    bool has_style = hasExistingStyleBlock(existing_code);
    bool has_script = hasExistingScriptBlock(existing_code);
    
    // 如果已经有style和script块，不需要自动添加
    if (has_style && has_script) {
        decision.reason = "已存在style和script块";
        return decision;
    }
    
    // 查找类选择器和ID选择器
    std::vector<SelectorInfo> class_selectors;
    std::vector<SelectorInfo> id_selectors;
    
    for (const auto& selector : selectors) {
        if (selector.type == "class") {
            class_selectors.push_back(selector);
        } else if (selector.type == "id") {
            id_selectors.push_back(selector);
        }
    }
    
    // 决策逻辑
    
    // 1. 局部style自动添加：如果没有style块，添加第一个类选择器
    if (!has_style && !class_selectors.empty()) {
        decision.should_add_style = true;
        decision.style_selector = "." + class_selectors[0].value;
        stats_.auto_added_styles++;
    }
    
    // 2. 局部script自动添加：如果没有script块，添加第一个ID选择器
    if (!has_script && !id_selectors.empty()) {
        decision.should_add_script = true;
        decision.script_selector = "#" + id_selectors[0].value;
        stats_.auto_added_scripts++;
    }
    
    // 3. ID优先级规则：如果同时有class和id，优先使用id
    if (!id_selectors.empty() && !class_selectors.empty()) {
        // 如果同时有class和id选择器，根据ID优先原则调整
        if (decision.should_add_style) {
            decision.style_selector = "#" + id_selectors[0].value; // style也使用ID
        }
        if (decision.should_add_script) {
            decision.script_selector = "#" + id_selectors[0].value; // script使用ID
        }
        decision.reason = "ID优先级规则：class和id同时存在，优先使用id（性能更高）";
    }
    
    // 4. 已存在优先规则
    if (has_style && decision.should_add_script) {
        // 如果style已存在，检查其使用的选择器
        auto existing_classes = getExistingClassSelectors(existing_code);
        auto existing_ids = getExistingIdSelectors(existing_code);
        
        if (!existing_ids.empty()) {
            // 已有ID选择器，script使用同样的ID
            decision.script_selector = "#" + existing_ids[0];
            decision.reason = "使用已存在的ID选择器";
        } else if (!existing_classes.empty() && !id_selectors.empty()) {
            // 已有class但检测到ID，使用ID（性能更高）
            decision.script_selector = "#" + id_selectors[0].value;
            decision.reason = "性能优化：使用ID选择器替代class";
        }
    }
    
    if (decision.reason.empty()) {
        decision.reason = "标准自动添加规则";
    }
    
    return decision;
}

bool SelectorDetector::hasExistingStyleBlock(const std::string& code) {
    // 查找局部style块
    std::regex style_pattern(R"(\bstyle\s*\{)");
    return std::regex_search(code, style_pattern);
}

bool SelectorDetector::hasExistingScriptBlock(const std::string& code) {
    // 查找局部script块
    std::regex script_pattern(R"(\bscript\s*\{)");
    return std::regex_search(code, script_pattern);
}

std::vector<std::string> SelectorDetector::getExistingClassSelectors(const std::string& code) {
    std::vector<std::string> class_selectors;
    
    // 在style块中查找类选择器
    std::regex class_pattern(R"(\.([a-zA-Z][a-zA-Z0-9_-]*))");
    std::sregex_iterator iter(code.begin(), code.end(), class_pattern);
    std::sregex_iterator end;
    
    while (iter != end) {
        class_selectors.push_back((*iter)[1].str());
        ++iter;
    }
    
    return class_selectors;
}

std::vector<std::string> SelectorDetector::getExistingIdSelectors(const std::string& code) {
    std::vector<std::string> id_selectors;
    
    // 在style块中查找ID选择器
    std::regex id_pattern(R"(#([a-zA-Z][a-zA-Z0-9_-]*))");
    std::sregex_iterator iter(code.begin(), code.end(), id_pattern);
    std::sregex_iterator end;
    
    while (iter != end) {
        id_selectors.push_back((*iter)[1].str());
        ++iter;
    }
    
    return id_selectors;
}

SelectorInfo SelectorDetector::selectBestSelectorForStyle(const std::vector<SelectorInfo>& selectors) {
    // 局部style优先选择第一个类选择器
    for (const auto& selector : selectors) {
        if (selector.type == "class") {
            return selector;
        }
    }
    
    // 如果没有类选择器，返回空
    return SelectorInfo();
}

SelectorInfo SelectorDetector::selectBestSelectorForScript(const std::vector<SelectorInfo>& selectors) {
    // 局部script优先选择第一个ID选择器
    for (const auto& selector : selectors) {
        if (selector.type == "id") {
            return selector;
        }
    }
    
    // 如果没有ID选择器，返回空
    return SelectorInfo();
}

std::string SelectorDetector::generateAutoStyleBlock(const std::string& selector) {
    std::ostringstream style_block;
    
    style_block << "\n    style {\n";
    style_block << "        " << selector << " {\n";
    style_block << "            /* 自动添加的样式块 */\n";
    style_block << "        }\n";
    style_block << "    }";
    
    return style_block.str();
}

std::string SelectorDetector::generateAutoScriptBlock(const std::string& selector) {
    std::ostringstream script_block;
    
    script_block << "\n    script {\n";
    script_block << "        // 自动添加的脚本块，基于选择器: " << selector << "\n";
    script_block << "        const element = document.querySelector('" << selector << "');\n";
    script_block << "        if (element) {\n";
    script_block << "            // 在这里添加针对 " << selector << " 的脚本逻辑\n";
    script_block << "        }\n";
    script_block << "    }";
    
    return script_block.str();
}

std::string SelectorDetector::insertAutoBlocks(const std::string& original_code, const AutoAdditionDecision& decision) {
    std::string result = original_code;
    
    // 查找插入点（通常在元素的结束}之前）
    size_t insert_pos = result.rfind('}');
    if (insert_pos == std::string::npos) {
        return result; // 没有找到合适的插入点
    }
    
    std::string blocks_to_insert;
    
    // 按优先级插入：先style后script
    if (decision.should_add_style) {
        blocks_to_insert += generateAutoStyleBlock(decision.style_selector);
    }
    
    if (decision.should_add_script) {
        blocks_to_insert += generateAutoScriptBlock(decision.script_selector);
    }
    
    if (!blocks_to_insert.empty()) {
        result.insert(insert_pos, blocks_to_insert + "\n");
    }
    
    return result;
}

int SelectorDetector::calculateSelectorPriority(const SelectorInfo& selector) {
    // ID选择器优先级最高
    if (selector.type == "id") return 100;
    
    // 类选择器次之
    if (selector.type == "class") return 50;
    
    // 标签选择器较低
    if (selector.type == "tag") return 10;
    
    // 复杂选择器最低
    return 1;
}

bool SelectorDetector::shouldPreferId(const std::vector<SelectorInfo>& selectors) {
    // 如果有ID选择器，总是优先使用
    for (const auto& selector : selectors) {
        if (selector.type == "id") {
            return true;
        }
    }
    
    return false;
}

void SelectorDetector::updateStatistics(const SelectorInfo& selector) {
    if (selector.type == "class") {
        stats_.class_selectors++;
    } else if (selector.type == "id") {
        stats_.id_selectors++;
    } else if (selector.type == "complex") {
        stats_.complex_selectors++;
    }
}

void SelectorDetector::updateAutoAdditionStats(const AutoAdditionDecision& decision) {
    if (decision.should_add_style) {
        stats_.auto_added_styles++;
    }
    if (decision.should_add_script) {
        stats_.auto_added_scripts++;
    }
}

void SelectorDetector::printDetectionInfo() const {
    std::cout << "=== 选择器检测统计 ===" << std::endl;
    std::cout << "总选择器数: " << stats_.total_selectors << std::endl;
    std::cout << "类选择器数: " << stats_.class_selectors << std::endl;
    std::cout << "ID选择器数: " << stats_.id_selectors << std::endl;
    std::cout << "复杂选择器数: " << stats_.complex_selectors << std::endl;
    std::cout << "自动添加style数: " << stats_.auto_added_styles << std::endl;
    std::cout << "自动添加script数: " << stats_.auto_added_scripts << std::endl;
    std::cout << "自动添加功能: " << (auto_addition_enabled_ ? "启用" : "禁用") << std::endl;
}

std::string SelectorDetector::extractSelectorValue(const std::string& selector) {
    if (selector.empty()) return "";
    
    if (selector[0] == '.' || selector[0] == '#') {
        return selector.substr(1);
    }
    
    return selector;
}

void SelectorDetector::setAutoAdditionRule(const AutoAdditionRule& rule) {
    // 查找是否已存在相同规则
    for (auto& existing_rule : rules_) {
        if (existing_rule.target_type == rule.target_type && 
            existing_rule.selector_type == rule.selector_type) {
            existing_rule = rule; // 更新现有规则
            return;
        }
    }
    
    // 添加新规则
    rules_.push_back(rule);
    
    // 按优先级排序
    std::sort(rules_.begin(), rules_.end(), 
        [](const AutoAdditionRule& a, const AutoAdditionRule& b) {
            return a.priority > b.priority;
        });
}

std::vector<AutoAdditionRule> SelectorDetector::getAutoAdditionRules() const {
    return rules_;
}

} // namespace chtljs
} // namespace chtl