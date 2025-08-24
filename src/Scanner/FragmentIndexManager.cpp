#include "Scanner/CHTLUnifiedScanner.h"
#include "Utils/StringUtils.h"
#include "Utils/ErrorHandler.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {
namespace Scanner {

FragmentIndexManager::FragmentIndexManager() : nextFragmentId_(1) {
}

void FragmentIndexManager::BuildIndex(std::vector<CodeFragment>& fragments) {
    Utils::ErrorHandler::GetInstance().LogInfo("开始构建片段索引，共 " + std::to_string(fragments.size()) + " 个片段");
    
    // 第一阶段：分配ID和基本信息
    for (size_t i = 0; i < fragments.size(); ++i) {
        UpdateFragmentIndex(fragments[i], i);
    }
    
    // 第二阶段：分析上下文和依赖关系
    for (size_t i = 0; i < fragments.size(); ++i) {
        fragments[i].context = AnalyzeFragmentContext(fragments[i], fragments);
        ValidateFragmentIntegrity(fragments[i]);
        AnalyzeKeywords(fragments[i]);
        
        // 分析与相邻片段的合并可能性
        if (i > 0) {
            fragments[i].canMergeWithPrev = CanMergeFragments(fragments[i-1], fragments[i]);
        }
        if (i < fragments.size() - 1) {
            fragments[i].canMergeWithNext = CanMergeFragments(fragments[i], fragments[i+1]);
        }
        
        // 计算合并优先级
        fragments[i].mergeOrder = CalculateMergeOrder(fragments[i], fragments);
    }
    
    // 第三阶段：建立层次关系
    for (size_t i = 0; i < fragments.size(); ++i) {
        // 简单的层次关系检测：基于位置包含关系
        for (size_t j = 0; j < fragments.size(); ++j) {
            if (i != j && 
                fragments[j].startPos >= fragments[i].startPos &&
                fragments[j].endPos <= fragments[i].endPos &&
                fragments[j].endPos - fragments[j].startPos < fragments[i].endPos - fragments[i].startPos) {
                SetParentChild(fragments[i].fragmentId, fragments[j].fragmentId);
            }
        }
    }
    
    Utils::ErrorHandler::GetInstance().LogInfo("片段索引构建完成");
}

void FragmentIndexManager::UpdateFragmentIndex(CodeFragment& fragment, size_t sequenceIndex) {
    fragment.fragmentId = nextFragmentId_++;
    fragment.sequenceIndex = sequenceIndex;
    
    // 初始化为不完整状态，后续会验证
    fragment.integrity = FragmentIntegrity::INCOMPLETE;
    fragment.context = FragmentContext::GLOBAL_SCOPE;
    fragment.mergeOrder = static_cast<int>(sequenceIndex);
}

void FragmentIndexManager::AddDependency(size_t fragmentId, size_t dependsOnId) {
    dependencyGraph_[fragmentId].push_back(dependsOnId);
    dependentGraph_[dependsOnId].push_back(fragmentId);
}

void FragmentIndexManager::RemoveDependency(size_t fragmentId, size_t dependsOnId) {
    auto& deps = dependencyGraph_[fragmentId];
    deps.erase(std::remove(deps.begin(), deps.end(), dependsOnId), deps.end());
    
    auto& dependents = dependentGraph_[dependsOnId];
    dependents.erase(std::remove(dependents.begin(), dependents.end(), fragmentId), dependents.end());
}

std::vector<size_t> FragmentIndexManager::GetDependencies(size_t fragmentId) const {
    auto it = dependencyGraph_.find(fragmentId);
    return (it != dependencyGraph_.end()) ? it->second : std::vector<size_t>();
}

std::vector<size_t> FragmentIndexManager::GetDependents(size_t fragmentId) const {
    auto it = dependentGraph_.find(fragmentId);
    return (it != dependentGraph_.end()) ? it->second : std::vector<size_t>();
}

void FragmentIndexManager::SetParentChild(size_t parentId, size_t childId) {
    parentMap_[childId] = parentId;
    childrenMap_[parentId].push_back(childId);
}

void FragmentIndexManager::RemoveParentChild(size_t parentId, size_t childId) {
    parentMap_.erase(childId);
    
    auto& children = childrenMap_[parentId];
    children.erase(std::remove(children.begin(), children.end(), childId), children.end());
}

std::vector<size_t> FragmentIndexManager::GetChildren(size_t fragmentId) const {
    auto it = childrenMap_.find(fragmentId);
    return (it != childrenMap_.end()) ? it->second : std::vector<size_t>();
}

size_t FragmentIndexManager::GetParent(size_t fragmentId) const {
    auto it = parentMap_.find(fragmentId);
    return (it != parentMap_.end()) ? it->second : 0;
}

std::vector<size_t> FragmentIndexManager::GetOptimalMergeOrder(const std::vector<CodeFragment>& fragments) const {
    std::vector<size_t> order;
    
    // 创建片段ID到索引的映射
    std::unordered_map<size_t, size_t> idToIndex;
    for (size_t i = 0; i < fragments.size(); ++i) {
        idToIndex[fragments[i].fragmentId] = i;
    }
    
    // 按合并优先级排序
    std::vector<std::pair<int, size_t>> priorityOrder;
    for (const auto& fragment : fragments) {
        priorityOrder.emplace_back(fragment.mergeOrder, fragment.fragmentId);
    }
    
    std::sort(priorityOrder.begin(), priorityOrder.end());
    
    for (const auto& pair : priorityOrder) {
        order.push_back(pair.second);
    }
    
    return order;
}

bool FragmentIndexManager::CanMergeFragments(const CodeFragment& fragment1, const CodeFragment& fragment2) const {
    // 基本合并规则
    if (fragment1.type != fragment2.type) {
        return false;
    }
    
    // 位置连续性检查
    if (fragment2.startPos != fragment1.endPos) {
        return false;
    }
    
    // 完整性检查
    if (fragment1.integrity == FragmentIntegrity::INCOMPLETE || 
        fragment2.integrity == FragmentIntegrity::INCOMPLETE) {
        return false;
    }
    
    // 上下文兼容性检查
    if (fragment1.context != fragment2.context) {
        return false;
    }
    
    return true;
}

void FragmentIndexManager::ValidateFragmentIntegrity(CodeFragment& fragment) {
    if (IsFragmentComplete(fragment)) {
        fragment.integrity = FragmentIntegrity::COMPLETE;
    } else if (HasBalancedBraces(fragment.content) && HasBalancedParentheses(fragment.content)) {
        fragment.integrity = FragmentIntegrity::PARTIAL;
    } else {
        fragment.integrity = FragmentIntegrity::INCOMPLETE;
    }
}

std::vector<size_t> FragmentIndexManager::FindIncompleteFragments(const std::vector<CodeFragment>& fragments) const {
    std::vector<size_t> incompleteIds;
    
    for (const auto& fragment : fragments) {
        if (fragment.integrity == FragmentIntegrity::INCOMPLETE) {
            incompleteIds.push_back(fragment.fragmentId);
        }
    }
    
    return incompleteIds;
}

FragmentContext FragmentIndexManager::AnalyzeFragmentContext(const CodeFragment& fragment, 
                                                           const std::vector<CodeFragment>& allFragments) const {
    // 基于内容和位置分析上下文
    std::string content = Utils::StringUtils::Trim(fragment.content);
    
    if (content.find("style") != std::string::npos && content.find("{") != std::string::npos) {
        return FragmentContext::STYLE_BLOCK;
    }
    
    if (content.find("script") != std::string::npos && content.find("{") != std::string::npos) {
        return FragmentContext::SCRIPT_BLOCK;
    }
    
    if (content.find("html") != std::string::npos || content.find("div") != std::string::npos ||
        content.find("span") != std::string::npos || content.find("text") != std::string::npos) {
        return FragmentContext::HTML_ELEMENT;
    }
    
    if (content.find("[Template]") != std::string::npos || content.find("@") != std::string::npos) {
        return FragmentContext::TEMPLATE_BLOCK;
    }
    
    if (content.find("[Custom]") != std::string::npos) {
        return FragmentContext::CUSTOM_BLOCK;
    }
    
    if (content.find("function") != std::string::npos || content.find("=>") != std::string::npos) {
        return FragmentContext::FUNCTION_BODY;
    }
    
    return FragmentContext::GLOBAL_SCOPE;
}

std::vector<size_t> FragmentIndexManager::FindFragmentsByType(const std::vector<CodeFragment>& fragments, 
                                                            FragmentType type) const {
    std::vector<size_t> matchingIds;
    
    for (const auto& fragment : fragments) {
        if (fragment.type == type) {
            matchingIds.push_back(fragment.fragmentId);
        }
    }
    
    return matchingIds;
}

std::vector<size_t> FragmentIndexManager::FindFragmentsByContext(const std::vector<CodeFragment>& fragments, 
                                                               FragmentContext context) const {
    std::vector<size_t> matchingIds;
    
    for (const auto& fragment : fragments) {
        if (fragment.context == context) {
            matchingIds.push_back(fragment.fragmentId);
        }
    }
    
    return matchingIds;
}

std::vector<size_t> FragmentIndexManager::FindFragmentsByKeyword(const std::vector<CodeFragment>& fragments, 
                                                               const std::string& keyword) const {
    std::vector<size_t> matchingIds;
    
    for (const auto& fragment : fragments) {
        if (fragment.containedKeywords.find(keyword) != fragment.containedKeywords.end()) {
            matchingIds.push_back(fragment.fragmentId);
        }
    }
    
    return matchingIds;
}

void FragmentIndexManager::PrintIndexStatistics(const std::vector<CodeFragment>& fragments) const {
    std::ostringstream stats;
    
    stats << "\n=== 片段索引统计 ===\n";
    stats << "总片段数: " << fragments.size() << "\n";
    
    // 按类型统计
    std::unordered_map<FragmentType, int> typeCount;
    std::unordered_map<FragmentContext, int> contextCount;
    std::unordered_map<FragmentIntegrity, int> integrityCount;
    
    for (const auto& fragment : fragments) {
        typeCount[fragment.type]++;
        contextCount[fragment.context]++;
        integrityCount[fragment.integrity]++;
    }
    
    stats << "\n按类型分布:\n";
    stats << "  CHTL: " << typeCount[FragmentType::CHTL] << "\n";
    stats << "  CHTL_JS: " << typeCount[FragmentType::CHTL_JS] << "\n";
    stats << "  CSS: " << typeCount[FragmentType::CSS] << "\n";
    stats << "  JS: " << typeCount[FragmentType::JS] << "\n";
    stats << "  Unknown: " << typeCount[FragmentType::Unknown] << "\n";
    
    stats << "\n按完整性分布:\n";
    stats << "  完整: " << integrityCount[FragmentIntegrity::COMPLETE] << "\n";
    stats << "  部分: " << integrityCount[FragmentIntegrity::PARTIAL] << "\n";
    stats << "  不完整: " << integrityCount[FragmentIntegrity::INCOMPLETE] << "\n";
    stats << "  已合并: " << integrityCount[FragmentIntegrity::MERGED] << "\n";
    
    stats << "\n依赖关系统计:\n";
    stats << "  总依赖数: " << dependencyGraph_.size() << "\n";
    stats << "  层次关系数: " << parentMap_.size() << "\n";
    
    Utils::ErrorHandler::GetInstance().LogInfo(stats.str());
}

// 私有辅助方法实现

bool FragmentIndexManager::IsFragmentComplete(const CodeFragment& fragment) const {
    std::string content = Utils::StringUtils::Trim(fragment.content);
    
    // 检查基本的语法完整性
    if (content.empty()) {
        return false;
    }
    
    // 检查括号平衡
    if (!HasBalancedBraces(content) || !HasBalancedParentheses(content)) {
        return false;
    }
    
    // 检查是否有明显的截断标志
    if (content.back() == ',' || content.back() == '+' || content.back() == '-') {
        return false;
    }
    
    return true;
}

int FragmentIndexManager::CalculateMergeOrder(const CodeFragment& fragment, 
                                            const std::vector<CodeFragment>& allFragments) const {
    int order = static_cast<int>(fragment.sequenceIndex);
    
    // 根据类型调整优先级
    switch (fragment.type) {
        case FragmentType::CHTL:
            order += 0;  // 最高优先级
            break;
        case FragmentType::CSS:
            order += 1000;
            break;
        case FragmentType::CHTL_JS:
            order += 2000;
            break;
        case FragmentType::JS:
            order += 3000;
            break;
        default:
            order += 9000;  // 最低优先级
            break;
    }
    
    // 根据完整性调整
    switch (fragment.integrity) {
        case FragmentIntegrity::COMPLETE:
            order += 0;
            break;
        case FragmentIntegrity::PARTIAL:
            order += 100;
            break;
        case FragmentIntegrity::INCOMPLETE:
            order += 200;
            break;
        default:
            order += 300;
            break;
    }
    
    return order;
}

void FragmentIndexManager::AnalyzeKeywords(CodeFragment& fragment) {
    std::string content = fragment.content;
    
    // CHTL关键字
    std::vector<std::string> chtlKeywords = {
        "html", "head", "body", "div", "span", "text", "style", "script",
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]",
        "@Style", "@Element", "@Var", "@Html", "@JavaScript"
    };
    
    // CHTL JS关键字
    std::vector<std::string> chtljsKeywords = {
        "vir", "function", "var", "let", "const", "if", "else", "for", "while"
    };
    
    // CSS关键字
    std::vector<std::string> cssKeywords = {
        "color", "background", "margin", "padding", "border", "width", "height",
        "display", "position", "float", "clear", "font", "text-align"
    };
    
    // 检查所有关键字
    for (const auto& keyword : chtlKeywords) {
        if (content.find(keyword) != std::string::npos) {
            fragment.containedKeywords.insert(keyword);
        }
    }
    
    for (const auto& keyword : chtljsKeywords) {
        if (content.find(keyword) != std::string::npos) {
            fragment.containedKeywords.insert(keyword);
        }
    }
    
    for (const auto& keyword : cssKeywords) {
        if (content.find(keyword) != std::string::npos) {
            fragment.containedKeywords.insert(keyword);
        }
    }
}

bool FragmentIndexManager::HasBalancedBraces(const std::string& content) const {
    int count = 0;
    for (char c : content) {
        if (c == '{') count++;
        else if (c == '}') count--;
        if (count < 0) return false;
    }
    return count == 0;
}

bool FragmentIndexManager::HasBalancedParentheses(const std::string& content) const {
    int count = 0;
    for (char c : content) {
        if (c == '(') count++;
        else if (c == ')') count--;
        if (count < 0) return false;
    }
    return count == 0;
}

} // namespace Scanner
} // namespace CHTL