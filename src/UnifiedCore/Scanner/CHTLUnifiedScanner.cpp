#include "CHTLUnifiedScanner.h"
#include <iostream>
#include <algorithm>
#include <regex>
#include <sstream>

namespace chtl {

CHTLUnifiedScanner::CHTLUnifiedScanner(const ScannerConfig& config) : config_(config) {
    initializeKeywords();
    resetStatistics();
}

CHTLUnifiedScanner::~CHTLUnifiedScanner() = default;

void CHTLUnifiedScanner::initializeKeywords() {
    initializeCHTLKeywords();
    initializeCHTLJSKeywords();
}

void CHTLUnifiedScanner::initializeCHTLKeywords() {
    chtl_keywords_ = {
        "text", "style", "script", "Template", "Custom", "Origin", "Import", "Namespace",
        "Configuration", "Info", "Export", "@Style", "@Element", "@Var", "@Html", 
        "@JavaScript", "@Chtl", "@CJmod", "inherit", "delete", "insert", "after", 
        "before", "replace", "from", "as", "except"
    };
}

void CHTLUnifiedScanner::initializeCHTLJSKeywords() {
    chtl_js_keywords_ = {
        "listen", "delegate", "animate", "vir", "->", "{{", "}}"
    };
}

std::vector<CodeSlice> CHTLUnifiedScanner::scanSource(const std::string& source_code) {
    std::vector<CodeSlice> slices;
    size_t position = 0;
    
    while (position < source_code.length()) {
        // 1. 读取下一个代码片段
        CodeSlice slice = readNextSlice(source_code, position);
        
        // 2. 检查片段完整性
        std::string next_preview = getCompletionPreview(source_code, position, 100);
        
        // 3. 判断是否需要扩增
        while (!isSliceComplete(slice, next_preview) && slice.content.length() < config_.max_slice_size) {
            slice = expandSlice(slice, source_code, config_.expansion_step);
            next_preview = getCompletionPreview(source_code, slice.end_pos, 100);
            stats_.expanded_slices++;
        }
        
        // 4. 执行最小单元切割
        std::vector<MinimalUnit> units = performMinimalUnitCutting(slice);
        
        // 5. 上下文感知聚合
        if (config_.enable_aggregation) {
            std::vector<CodeSlice> aggregated_slices = performContextAwareAggregation(units);
            slices.insert(slices.end(), aggregated_slices.begin(), aggregated_slices.end());
        } else {
            // 直接转换为切片
            for (const auto& unit : units) {
                CodeSlice unit_slice(unit.content, unit.type, unit.start_pos, unit.end_pos);
                slices.push_back(unit_slice);
            }
        }
        
        // 更新统计信息
        updateStatistics(slice, slice.needs_expansion, config_.enable_aggregation);
        
        // 移动到下一个位置
        position = slice.end_pos;
    }
    
    stats_.total_slices = slices.size();
    return slices;
}

CodeSlice CHTLUnifiedScanner::readNextSlice(const std::string& source, size_t& position) {
    if (position >= source.length()) {
        return CodeSlice();
    }
    
    size_t slice_size = std::min(config_.initial_slice_size, source.length() - position);
    size_t end_pos = position + slice_size;
    
    std::string content = source.substr(position, slice_size);
    FragmentType type = identifyFragmentType(content);
    
    CodeSlice slice(content, type, position, end_pos);
    slice.is_complete = checkFragmentCompleteness(content, type);
    
    return slice;
}

bool CHTLUnifiedScanner::isSliceComplete(const CodeSlice& current_slice, const std::string& next_preview) {
    // 检查当前切片是否可能与下一个片段组成完整的CHTL或CHTL JS代码片段
    
    if (next_preview.empty()) {
        return true; // 没有更多内容，当前切片完整
    }
    
    // 检查CHTL语法完整性
    if (current_slice.type == FragmentType::CHTL || current_slice.type == FragmentType::MIXED) {
        if (canFormCompleteFragment(current_slice.content, next_preview, FragmentType::CHTL)) {
            return false; // 可以组成更完整的CHTL片段
        }
    }
    
    // 检查CHTL JS语法完整性
    if (current_slice.type == FragmentType::CHTL_JS || current_slice.type == FragmentType::MIXED) {
        if (canFormCompleteFragment(current_slice.content, next_preview, FragmentType::CHTL_JS)) {
            return false; // 可以组成更完整的CHTL JS片段
        }
    }
    
    // 检查嵌套结构完整性
    if (calculateNestingLevel(current_slice.content, current_slice.content.length() - 1) > 0) {
        return false; // 嵌套结构未闭合
    }
    
    return true;
}

bool CHTLUnifiedScanner::canFormCompleteFragment(const std::string& current, const std::string& next, FragmentType type) {
    std::string combined = current + next.substr(0, std::min(next.length(), size_t(50)));
    
    switch (type) {
        case FragmentType::CHTL:
            // 检查CHTL语法的完整性
            if (current.find('[') != std::string::npos && current.find(']') == std::string::npos) {
                return next.find(']') != std::string::npos; // 寻找闭合括号
            }
            if (current.find('{') != std::string::npos) {
                int brace_count = 0;
                for (char c : current) {
                    if (c == '{') brace_count++;
                    if (c == '}') brace_count--;
                }
                if (brace_count > 0) {
                    return next.find('}') != std::string::npos; // 寻找闭合花括号
                }
            }
            break;
            
        case FragmentType::CHTL_JS:
            // 检查CHTL JS语法的完整性
            if (current.find("{{") != std::string::npos && current.find("}}") == std::string::npos) {
                return next.find("}}") != std::string::npos; // 寻找CHTL JS选择器闭合
            }
            // 检查是否以选择器结尾，下一个片段以->开始（表示链式调用）
            if (current.find("}}") != std::string::npos && next.find("->") == 0) {
                return true; // 可以组成链式调用
            }
            if (current.find("->") != std::string::npos) {
                // 检查链式调用是否完整
                return !next.empty() && (std::isalpha(next[0]) || next[0] == '_');
            }
            break;
            
        default:
            break;
    }
    
    return false;
}

CodeSlice CHTLUnifiedScanner::expandSlice(const CodeSlice& slice, const std::string& source, size_t expansion_size) {
    size_t new_end = std::min(slice.end_pos + expansion_size, source.length());
    size_t new_size = new_end - slice.start_pos;
    
    std::string new_content = source.substr(slice.start_pos, new_size);
    FragmentType new_type = identifyFragmentType(new_content);
    
    CodeSlice expanded_slice(new_content, new_type, slice.start_pos, new_end, slice.line, slice.column);
    expanded_slice.needs_expansion = true;
    expanded_slice.is_complete = checkFragmentCompleteness(new_content, new_type);
    
    return expanded_slice;
}

bool CHTLUnifiedScanner::checkFragmentCompleteness(const std::string& fragment, FragmentType type) {
    switch (type) {
        case FragmentType::CHTL:
            return ScannerUtils::isBalanced(fragment, '{', '}') && 
                   ScannerUtils::isBalanced(fragment, '[', ']');
            
        case FragmentType::CHTL_JS:
            // 检查{{}}选择器是否闭合
            if (fragment.find("{{") != std::string::npos) {
                return fragment.find("}}") != std::string::npos;
            }
            return true;
            
        case FragmentType::CSS:
            return ScannerUtils::isBalanced(fragment, '{', '}');
            
        case FragmentType::JAVASCRIPT:
            return ScannerUtils::isBalanced(fragment, '{', '}') && 
                   ScannerUtils::isBalanced(fragment, '(', ')');
            
        default:
            return true;
    }
}

std::vector<MinimalUnit> CHTLUnifiedScanner::performMinimalUnitCutting(const CodeSlice& slice) {
    std::vector<MinimalUnit> units;
    
    switch (slice.type) {
        case FragmentType::CHTL:
            units = cutCHTLUnits(slice.content, slice.start_pos);
            break;
        case FragmentType::CHTL_JS:
            units = cutCHTLJSUnits(slice.content, slice.start_pos);
            break;
        case FragmentType::CSS:
            units = cutCSSUnits(slice.content, slice.start_pos);
            break;
        case FragmentType::JAVASCRIPT:
            units = cutJavaScriptUnits(slice.content, slice.start_pos);
            break;
        case FragmentType::MIXED:
            // 混合类型需要更复杂的处理
            units = cutCHTLUnits(slice.content, slice.start_pos);
            break;
        default:
            // 默认作为单个单元
            units.emplace_back(slice.content, slice.type, slice.start_pos, slice.end_pos);
            break;
    }
    
    return units;
}

std::vector<MinimalUnit> CHTLUnifiedScanner::cutCHTLJSUnits(const std::string& content, size_t base_pos) {
    std::vector<MinimalUnit> units;
    size_t pos = 0;
    
    while (pos < content.length()) {
        // 查找CHTL JS特殊语法
        
        // 1. 检查{{}}选择器
        if (pos + 1 < content.length() && content.substr(pos, 2) == "{{") {
            size_t end_pos = content.find("}}", pos + 2);
            if (end_pos != std::string::npos) {
                end_pos += 2; // 包含}}
                std::string selector_content = content.substr(pos, end_pos - pos);
                units.emplace_back(selector_content, FragmentType::CHTL_JS, base_pos + pos, base_pos + end_pos);
                pos = end_pos;
                continue;
            }
        }
        
        // 2. 检查->操作符
        if (pos + 1 < content.length() && content.substr(pos, 2) == "->") {
            // 查找->后的标识符
            size_t arrow_start = pos;
            pos += 2;
            
            // 跳过空白
            while (pos < content.length() && ScannerUtils::isWhitespace(content[pos])) {
                pos++;
            }
            
            // 查找标识符结束
            size_t identifier_start = pos;
            while (pos < content.length() && (std::isalnum(content[pos]) || content[pos] == '_')) {
                pos++;
            }
            
            if (pos > identifier_start) {
                std::string arrow_unit = content.substr(arrow_start, pos - arrow_start);
                units.emplace_back(arrow_unit, FragmentType::CHTL_JS, base_pos + arrow_start, base_pos + pos);
                continue;
            }
        }
        
        // 3. 查找其他CHTL JS关键字
        bool found_keyword = false;
        for (const std::string& keyword : chtl_js_keywords_) {
            if (pos + keyword.length() <= content.length() && 
                content.substr(pos, keyword.length()) == keyword) {
                
                // 确保是完整的关键字（不是其他标识符的一部分）
                bool is_word_boundary = true;
                if (pos > 0 && (std::isalnum(content[pos - 1]) || content[pos - 1] == '_')) {
                    is_word_boundary = false;
                }
                if (pos + keyword.length() < content.length() && 
                    (std::isalnum(content[pos + keyword.length()]) || content[pos + keyword.length()] == '_')) {
                    is_word_boundary = false;
                }
                
                if (is_word_boundary) {
                    units.emplace_back(keyword, FragmentType::CHTL_JS, base_pos + pos, base_pos + pos + keyword.length());
                    pos += keyword.length();
                    found_keyword = true;
                    break;
                }
            }
        }
        
        if (!found_keyword) {
            // 4. 处理普通字符
            size_t start = pos;
            while (pos < content.length() && 
                   content.substr(pos, 2) != "{{" && 
                   content.substr(pos, 2) != "->" &&
                   !ScannerUtils::isCHTLJSKeyword(std::string(1, content[pos]))) {
                pos++;
            }
            
            if (pos > start) {
                std::string regular_content = content.substr(start, pos - start);
                units.emplace_back(regular_content, FragmentType::JAVASCRIPT, base_pos + start, base_pos + pos);
            }
        }
    }
    
    return units;
}

std::vector<MinimalUnit> CHTLUnifiedScanner::cutCHTLUnits(const std::string& content, size_t base_pos) {
    std::vector<MinimalUnit> units;
    size_t pos = 0;
    
    while (pos < content.length()) {
        // 查找CHTL特殊语法
        
        // 1. 检查[关键字]语法
        if (content[pos] == '[') {
            size_t end_pos = content.find(']', pos + 1);
            if (end_pos != std::string::npos) {
                end_pos++; // 包含]
                std::string bracket_content = content.substr(pos, end_pos - pos);
                units.emplace_back(bracket_content, FragmentType::CHTL, base_pos + pos, base_pos + end_pos);
                pos = end_pos;
                continue;
            }
        }
        
        // 2. 检查@关键字
        if (content[pos] == '@') {
            size_t start = pos;
            pos++; // 跳过@
            
            // 查找标识符
            while (pos < content.length() && (std::isalnum(content[pos]) || content[pos] == '_')) {
                pos++;
            }
            
            if (pos > start + 1) {
                std::string at_keyword = content.substr(start, pos - start);
                units.emplace_back(at_keyword, FragmentType::CHTL, base_pos + start, base_pos + pos);
                continue;
            }
        }
        
        // 3. 检查CHTL关键字
        bool found_keyword = false;
        for (const std::string& keyword : chtl_keywords_) {
            if (pos + keyword.length() <= content.length() && 
                content.substr(pos, keyword.length()) == keyword) {
                
                // 确保是完整的关键字
                bool is_word_boundary = true;
                if (pos > 0 && (std::isalnum(content[pos - 1]) || content[pos - 1] == '_')) {
                    is_word_boundary = false;
                }
                if (pos + keyword.length() < content.length() && 
                    (std::isalnum(content[pos + keyword.length()]) || content[pos + keyword.length()] == '_')) {
                    is_word_boundary = false;
                }
                
                if (is_word_boundary) {
                    units.emplace_back(keyword, FragmentType::CHTL, base_pos + pos, base_pos + pos + keyword.length());
                    pos += keyword.length();
                    found_keyword = true;
                    break;
                }
            }
        }
        
        if (!found_keyword) {
            // 4. 处理花括号块
            if (content[pos] == '{') {
                size_t start = pos;
                int brace_count = 1;
                pos++; // 跳过开始的{
                
                while (pos < content.length() && brace_count > 0) {
                    if (content[pos] == '{') brace_count++;
                    if (content[pos] == '}') brace_count--;
                    pos++;
                }
                
                std::string block_content = content.substr(start, pos - start);
                FragmentType block_type = identifyFragmentType(block_content);
                units.emplace_back(block_content, block_type, base_pos + start, base_pos + pos);
                continue;
            }
            
            // 5. 处理普通内容
            size_t start = pos;
            while (pos < content.length() && 
                   content[pos] != '[' && content[pos] != '@' && content[pos] != '{' &&
                   !ScannerUtils::isCHTLKeyword(std::string(1, content[pos]))) {
                pos++;
            }
            
            if (pos > start) {
                std::string regular_content = content.substr(start, pos - start);
                FragmentType content_type = analyzeContentType(regular_content);
                units.emplace_back(regular_content, content_type, base_pos + start, base_pos + pos);
            }
        }
    }
    
    return units;
}

std::vector<MinimalUnit> CHTLUnifiedScanner::cutCSSUnits(const std::string& content, size_t base_pos) {
    std::vector<MinimalUnit> units;
    // CSS单元切割的简化实现
    units.emplace_back(content, FragmentType::CSS, base_pos, base_pos + content.length());
    return units;
}

std::vector<MinimalUnit> CHTLUnifiedScanner::cutJavaScriptUnits(const std::string& content, size_t base_pos) {
    std::vector<MinimalUnit> units;
    // JavaScript单元切割的简化实现
    units.emplace_back(content, FragmentType::JAVASCRIPT, base_pos, base_pos + content.length());
    return units;
}

std::vector<CodeSlice> CHTLUnifiedScanner::performContextAwareAggregation(const std::vector<MinimalUnit>& units) {
    std::vector<CodeSlice> aggregated_slices;
    
    if (units.empty()) {
        return aggregated_slices;
    }
    
    std::vector<MinimalUnit> current_group;
    FragmentType current_type = units[0].type;
    
    for (const auto& unit : units) {
        // 检查是否可以与当前组聚合
        if (unit.type == current_type && shouldAggregateUnits(current_group.empty() ? unit : current_group.back(), unit)) {
            current_group.push_back(unit);
        } else {
            // 完成当前组的聚合
            if (!current_group.empty()) {
                CodeSlice aggregated = aggregateUnits(current_group, current_type);
                aggregated_slices.push_back(aggregated);
                stats_.aggregated_units += current_group.size();
            }
            
            // 开始新组
            current_group.clear();
            current_group.push_back(unit);
            current_type = unit.type;
        }
    }
    
    // 处理最后一组
    if (!current_group.empty()) {
        CodeSlice aggregated = aggregateUnits(current_group, current_type);
        aggregated_slices.push_back(aggregated);
        stats_.aggregated_units += current_group.size();
    }
    
    return aggregated_slices;
}

bool CHTLUnifiedScanner::shouldAggregateUnits(const MinimalUnit& unit1, const MinimalUnit& unit2) {
    // 基本聚合条件
    if (unit1.type != unit2.type) {
        return false;
    }
    
    // 检查是否相邻
    if (unit2.start_pos != unit1.end_pos) {
        return false;
    }
    
    // 检查聚合大小限制
    size_t combined_size = (unit1.end_pos - unit1.start_pos) + (unit2.end_pos - unit2.start_pos);
    if (combined_size > config_.max_aggregation_size) {
        return false;
    }
    
    // 类型特定的聚合规则
    switch (unit1.type) {
        case FragmentType::CHTL:
            return canAggregateCHTLUnits(unit1, unit2);
        case FragmentType::CHTL_JS:
            return canAggregateCHTLJSUnits(unit1, unit2);
        case FragmentType::CSS:
        case FragmentType::JAVASCRIPT:
            return true; // CSS和JS可以自由聚合
        default:
            return false;
    }
}

bool CHTLUnifiedScanner::canAggregateCHTLUnits(const MinimalUnit& unit1, const MinimalUnit& unit2) {
    // CHTL单元聚合规则：相同类型的CHTL语法可以聚合
    return unit1.can_aggregate && unit2.can_aggregate;
}

bool CHTLUnifiedScanner::canAggregateCHTLJSUnits(const MinimalUnit& unit1, const MinimalUnit& unit2) {
    // CHTL JS单元聚合规则：链式调用可以聚合
    if (unit1.content.find("->") != std::string::npos || unit2.content.find("->") != std::string::npos) {
        return true; // 链式调用应该聚合
    }
    
    return unit1.can_aggregate && unit2.can_aggregate;
}

CodeSlice CHTLUnifiedScanner::aggregateUnits(const std::vector<MinimalUnit>& units, FragmentType target_type) {
    if (units.empty()) {
        return CodeSlice();
    }
    
    std::ostringstream content_stream;
    size_t start_pos = units[0].start_pos;
    size_t end_pos = units.back().end_pos;
    
    for (const auto& unit : units) {
        content_stream << unit.content;
    }
    
    return CodeSlice(content_stream.str(), target_type, start_pos, end_pos);
}

FragmentType CHTLUnifiedScanner::identifyFragmentType(const std::string& content) {
    bool has_chtl = containsCHTLSyntax(content);
    bool has_chtl_js = containsCHTLJSSyntax(content);
    bool has_css = containsCSSSyntax(content);
    bool has_js = containsJavaScriptSyntax(content);
    
    // 优先级判断
    if (has_chtl && has_chtl_js) {
        return FragmentType::MIXED;
    } else if (has_chtl) {
        return FragmentType::CHTL;
    } else if (has_chtl_js) {
        return FragmentType::CHTL_JS;
    } else if (has_css) {
        return FragmentType::CSS;
    } else if (has_js) {
        return FragmentType::JAVASCRIPT;
    }
    
    return FragmentType::UNKNOWN;
}

bool CHTLUnifiedScanner::containsCHTLSyntax(const std::string& content) {
    // 检查CHTL关键字
    for (const std::string& keyword : chtl_keywords_) {
        if (content.find(keyword) != std::string::npos) {
            return true;
        }
    }
    
    // 检查CHTL特有的语法模式
    return content.find('[') != std::string::npos ||  // [Template], [Custom]等
           content.find('@') != std::string::npos;    // @Style, @Element等
}

bool CHTLUnifiedScanner::containsCHTLJSSyntax(const std::string& content) {
    // 检查CHTL JS特有语法
    // 注意：无修饰字面量不作为单独判断条件！
    return content.find("{{") != std::string::npos ||  // {{选择器}}
           content.find("->") != std::string::npos ||  // ->操作符
           content.find("listen") != std::string::npos ||
           content.find("delegate") != std::string::npos ||
           content.find("animate") != std::string::npos ||
           content.find("vir") != std::string::npos;
    
    // 重要：无修饰字面量不包含在此判断中，因为它们可能出现在任何地方
    // 无修饰字面量的识别依赖于上下文，而不是内容特征
}

bool CHTLUnifiedScanner::containsCSSSyntax(const std::string& content) {
    // 简化的CSS语法检测
    return content.find(':') != std::string::npos &&   // CSS属性
           content.find(';') != std::string::npos;     // CSS分号
}

bool CHTLUnifiedScanner::containsJavaScriptSyntax(const std::string& content) {
    // 简化的JavaScript语法检测
    return content.find("function") != std::string::npos ||
           content.find("var") != std::string::npos ||
           content.find("let") != std::string::npos ||
           content.find("const") != std::string::npos ||
           content.find("=>") != std::string::npos;
}

FragmentType CHTLUnifiedScanner::analyzeContentType(const std::string& content) {
    return identifyFragmentType(content);
}

std::string CHTLUnifiedScanner::getCompletionPreview(const std::string& source, size_t position, size_t preview_size) {
    if (position >= source.length()) {
        return "";
    }
    
    size_t actual_size = std::min(preview_size, source.length() - position);
    return source.substr(position, actual_size);
}

int CHTLUnifiedScanner::calculateNestingLevel(const std::string& content, size_t position) {
    int level = 0;
    
    for (size_t i = 0; i <= position && i < content.length(); ++i) {
        if (content[i] == '{') level++;
        if (content[i] == '}') level--;
    }
    
    return level;
}

std::vector<std::pair<size_t, size_t>> CHTLUnifiedScanner::findNestedBlocks(const std::string& content) {
    std::vector<std::pair<size_t, size_t>> blocks;
    std::vector<size_t> open_positions;
    
    for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] == '{') {
            open_positions.push_back(i);
        } else if (content[i] == '}' && !open_positions.empty()) {
            size_t start = open_positions.back();
            open_positions.pop_back();
            blocks.emplace_back(start, i);
        }
    }
    
    return blocks;
}

void CHTLUnifiedScanner::setConfig(const ScannerConfig& config) {
    config_ = config;
}

ScannerConfig CHTLUnifiedScanner::getConfig() const {
    return config_;
}

CHTLUnifiedScanner::ScannerStatistics CHTLUnifiedScanner::getStatistics() const {
    return stats_;
}

void CHTLUnifiedScanner::resetStatistics() {
    stats_ = ScannerStatistics{};
}

void CHTLUnifiedScanner::updateStatistics(const CodeSlice& slice, bool was_expanded, bool was_aggregated) {
    switch (slice.type) {
        case FragmentType::CHTL:
            stats_.chtl_fragments++;
            break;
        case FragmentType::CHTL_JS:
            stats_.chtl_js_fragments++;
            break;
        case FragmentType::CSS:
            stats_.css_fragments++;
            break;
        case FragmentType::JAVASCRIPT:
            stats_.js_fragments++;
            break;
        default:
            break;
    }
    
    if (was_expanded) {
        stats_.expanded_slices++;
    }
    
    stats_.average_slice_size = (stats_.average_slice_size * (stats_.total_slices - 1) + slice.content.length()) / stats_.total_slices;
}

void CHTLUnifiedScanner::setError(const std::string& error) {
    last_error_ = error;
    std::cerr << "CHTLUnifiedScanner错误: " << error << std::endl;
}

void CHTLUnifiedScanner::clearErrors() {
    last_error_.clear();
}

bool CHTLUnifiedScanner::isUndecoratedLiteralContext(const std::string& content, size_t position) {
    // 检查当前位置是否在允许无修饰字面量的上下文中
    
    // 检查是否在text块中
    size_t text_pos = content.rfind("text", position);
    if (text_pos != std::string::npos) {
        size_t brace_pos = content.find('{', text_pos);
        if (brace_pos != std::string::npos && brace_pos < position) {
            // 在text块内部
            return true;
        }
    }
    
    // 检查是否在CSS属性值中
    size_t colon_pos = content.rfind(':', position);
    if (colon_pos != std::string::npos) {
        size_t semicolon_pos = content.find(';', position);
        if (semicolon_pos != std::string::npos && colon_pos < position && position < semicolon_pos) {
            // 在CSS属性值中
            return true;
        }
    }
    
    return false;
}

std::vector<std::pair<size_t, size_t>> CHTLUnifiedScanner::findUndecoratedLiterals(const std::string& content, const std::string& context) {
    std::vector<std::pair<size_t, size_t>> literals;
    
    if (context != "text" && context != "css_value") {
        return literals; // 只在特定上下文中查找
    }
    
    size_t pos = 0;
    while (pos < content.length()) {
        // 跳过空白
        while (pos < content.length() && std::isspace(content[pos])) {
            pos++;
        }
        
        if (pos >= content.length()) break;
        
        // 检查是否为无修饰字面量的开始
        if (isUndecoratedLiteralContext(content, pos)) {
            size_t start = pos;
            
            // 查找字面量的结束
            while (pos < content.length() && 
                   content[pos] != ';' && content[pos] != '}' && content[pos] != '{' &&
                   content[pos] != '\n') {
                pos++;
            }
            
            if (pos > start) {
                literals.emplace_back(start, pos);
            }
        } else {
            pos++;
        }
    }
    
    return literals;
}

bool CHTLUnifiedScanner::shouldTreatAsUndecoratedLiteral(const std::string& content, size_t start, size_t end, const std::string& context) {
    if (start >= end || end > content.length()) {
        return false;
    }
    
    std::string literal = content.substr(start, end - start);
    
    // 去除首尾空白
    size_t first_non_space = literal.find_first_not_of(" \t");
    size_t last_non_space = literal.find_last_not_of(" \t");
    
    if (first_non_space == std::string::npos) {
        return false; // 只有空白
    }
    
    literal = literal.substr(first_non_space, last_non_space - first_non_space + 1);
    
    // 检查是否包含引号（如果有引号就不是无修饰字面量）
    if (literal.find('"') != std::string::npos || literal.find('\'') != std::string::npos) {
        return false;
    }
    
    // 检查是否为CHTL关键字（关键字不应该被当作字面量）
    static const std::unordered_set<std::string> chtl_keywords = {
        "Template", "Custom", "Origin", "Import", "Namespace", "text", "style", "script",
        "listen", "delegate", "animate", "vir", "inherit", "delete", "from", "as"
    };
    
    if (chtl_keywords.find(literal) != chtl_keywords.end()) {
        return false;
    }
    
    // 在指定上下文中，可以作为无修饰字面量
    return context == "text" || context == "css_value" || context == "attribute";
}

// ScannerUtils命名空间实现
namespace ScannerUtils {

bool isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool isSignificantChar(char c) {
    return !isWhitespace(c);
}

std::vector<std::pair<char, char>> getBracePairs() {
    return {
        {'{', '}'},
        {'[', ']'},
        {'(', ')'},
        {'<', '>'}
    };
}

bool isBalanced(const std::string& content, char open_char, char close_char) {
    int count = 0;
    bool in_string = false;
    char string_char = '\0';
    
    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        
        // 处理字符串
        if (!in_string && (c == '"' || c == '\'')) {
            in_string = true;
            string_char = c;
            continue;
        }
        
        if (in_string) {
            if (c == string_char && (i == 0 || content[i-1] != '\\')) {
                in_string = false;
                string_char = '\0';
            }
            continue;
        }
        
        // 计算括号
        if (c == open_char) count++;
        if (c == close_char) count--;
    }
    
    return count == 0;
}

bool isCHTLKeyword(const std::string& word) {
    static const std::unordered_set<std::string> keywords = {
        "text", "style", "script", "Template", "Custom", "Origin", "Import", 
        "Namespace", "inherit", "delete", "insert", "from", "as", "except"
    };
    
    return keywords.find(word) != keywords.end();
}

bool isCHTLJSKeyword(const std::string& word) {
    static const std::unordered_set<std::string> keywords = {
        "listen", "delegate", "animate", "vir", "->", "{{", "}}"
    };
    
    return keywords.find(word) != keywords.end();
}

std::pair<size_t, size_t> findStringSpan(const std::string& content, size_t start_pos) {
    if (start_pos >= content.length()) {
        return {std::string::npos, std::string::npos};
    }
    
    char quote_char = content[start_pos];
    if (quote_char != '"' && quote_char != '\'') {
        return {std::string::npos, std::string::npos};
    }
    
    for (size_t i = start_pos + 1; i < content.length(); ++i) {
        if (content[i] == quote_char && (i == 0 || content[i-1] != '\\')) {
            return {start_pos, i + 1};
        }
    }
    
    return {start_pos, std::string::npos}; // 未闭合的字符串
}

std::pair<size_t, size_t> findCommentSpan(const std::string& content, size_t start_pos) {
    if (start_pos + 1 >= content.length()) {
        return {std::string::npos, std::string::npos};
    }
    
    if (content.substr(start_pos, 2) == "//") {
        // 单行注释
        size_t end = content.find('\n', start_pos);
        return {start_pos, end == std::string::npos ? content.length() : end};
    }
    
    if (content.substr(start_pos, 2) == "/*") {
        // 多行注释
        size_t end = content.find("*/", start_pos + 2);
        return {start_pos, end == std::string::npos ? content.length() : end + 2};
    }
    
    if (content.substr(start_pos, 2) == "--") {
        // 生成器注释
        size_t end = content.find('\n', start_pos);
        return {start_pos, end == std::string::npos ? content.length() : end};
    }
    
    return {std::string::npos, std::string::npos};
}

std::string optimizeSliceContent(const std::string& content) {
    return trimSliceContent(content);
}

std::string trimSliceContent(const std::string& content) {
    size_t start = content.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = content.find_last_not_of(" \t\n\r");
    return content.substr(start, end - start + 1);
}

bool isEmptySlice(const std::string& content) {
    return trimSliceContent(content).empty();
}

} // namespace ScannerUtils

} // namespace chtl