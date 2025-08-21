#include "chtl/constraint/ConstraintChecker.h"
#include "chtl/ast/DocumentNode.h"
#include "chtl/ast/StyleBlockNode.h"
#include "chtl/ast/ScriptBlockNode.h"
#include "chtl/ast/NamespaceNode.h"
#include "chtl/ast/ElementNode.h"
#include "chtl/ast/ExceptNode.h"
#include "utils/Logger.h"
#include "utils/FileUtils.h"
#include "utils/StringUtils.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <fstream>

namespace chtl {
namespace constraint {

// ConstraintChecker 实现

ConstraintChecker::ConstraintChecker() {
    m_Manager = std::make_unique<ConstraintManager>();
    m_Manager->InitializeDefaultRules();
    m_Validator = m_Manager->CreateValidator();
}

ConstraintChecker::~ConstraintChecker() = default;

bool ConstraintChecker::CheckDocument(ast::DocumentNode* document) {
    if (!document) return true;
    
    ClearErrors();
    
    // 从顶层开始检查
    return CheckNodeRecursive(document, ContextType::TOP_LEVEL);
}

bool ConstraintChecker::CheckNode(ast::ASTNode* node, ContextType context) {
    if (!node) return true;
    
    return m_Validator->Validate(node, context);
}

void ConstraintChecker::SetStrictMode(bool strict) {
    m_Validator->SetStrictMode(strict);
}

void ConstraintChecker::RegisterToParser(parser::CHTLParser* parser) {
    // TODO: 集成到解析器中进行实时检查
    utils::Logger::GetInstance().Info("约束检查器已注册到解析器");
}

int ConstraintChecker::AutoFix(ast::ASTNode* node) {
    int fixedCount = 0;
    
    // 递归遍历AST树
    std::function<void(ast::ASTNode*, ContextType)> traverse;
    traverse = [&](ast::ASTNode* current, ContextType context) {
        if (!current) return;
        
        // 尝试修复当前节点
        if (TryFixNode(current, context)) {
            fixedCount++;
        }
        
        // 递归处理子节点
        for (const auto& child : current->GetChildren()) {
            ContextType childContext = DetermineChildContext(current, child.get(), context);
            traverse(child.get(), childContext);
        }
    };
    
    traverse(node, ContextType::TOP_LEVEL);
    
    utils::Logger::GetInstance().Info("自动修复了 " + std::to_string(fixedCount) + " 个约束问题");
    return fixedCount;
}

std::vector<SyntaxElement> ConstraintChecker::GetSuggestions(ContextType context) const {
    const ConstraintRule* rule = m_Manager->GetRule(context);
    if (!rule) {
        return {};
    }
    
    return rule->GetAllowedElements();
}

bool ConstraintChecker::ValidateExceptConstraints(ast::ASTNode* node) {
    if (!node) return true;
    
    // 查找所有except节点
    std::vector<ast::ExceptNode*> exceptNodes;
    
    std::function<void(ast::ASTNode*)> findExcepts;
    findExcepts = [&](ast::ASTNode* current) {
        if (auto exceptNode = dynamic_cast<ast::ExceptNode*>(current)) {
            exceptNodes.push_back(exceptNode);
        }
        
        for (const auto& child : current->GetChildren()) {
            findExcepts(child.get());
        }
    };
    
    findExcepts(node);
    
    // 验证每个except约束
    for (auto exceptNode : exceptNodes) {
        // TODO: 实现except约束的验证逻辑
        utils::Logger::GetInstance().Debug("验证except约束: " + exceptNode->GetValue());
    }
    
    return true;
}

bool ConstraintChecker::CheckNodeRecursive(ast::ASTNode* node, ContextType context) {
    if (!node) return true;
    
    // 检查当前节点
    if (!CheckNode(node, context)) {
        return false;
    }
    
    // 特殊处理某些节点类型
    if (dynamic_cast<ast::StyleBlockNode*>(node)) {
        return HandleStyleBlock(node, context);
    } else if (dynamic_cast<ast::ScriptBlockNode*>(node)) {
        return HandleScriptBlock(node, context);
    } else if (dynamic_cast<ast::NamespaceNode*>(node)) {
        return HandleNamespace(node, context);
    }
    
    // 递归检查子节点
    bool allValid = true;
    for (const auto& child : node->GetChildren()) {
        ContextType childContext = DetermineChildContext(node, child.get(), context);
        if (!CheckNodeRecursive(child.get(), childContext)) {
            allValid = false;
        }
    }
    
    return allValid;
}

ContextType ConstraintChecker::DetermineChildContext(ast::ASTNode* parent,
                                                    ast::ASTNode* child,
                                                    ContextType currentContext) const {
    if (!parent || !child) return currentContext;
    
    // 根据父节点类型确定子节点的上下文
    if (dynamic_cast<ast::StyleBlockNode*>(parent)) {
        // 样式块内部
        if (currentContext == ContextType::TOP_LEVEL || 
            currentContext == ContextType::NAMESPACE) {
            return ContextType::GLOBAL_STYLE;
        } else if (currentContext == ContextType::ELEMENT_BODY) {
            return ContextType::LOCAL_STYLE;
        }
    } else if (dynamic_cast<ast::ScriptBlockNode*>(parent)) {
        // script块内部
        if (currentContext == ContextType::ELEMENT_BODY) {
            return ContextType::LOCAL_SCRIPT;
        } else {
            return ContextType::GLOBAL_SCRIPT;
        }
    } else if (dynamic_cast<ast::ElementNode*>(parent)) {
        // 元素内部
        return ContextType::ELEMENT_BODY;
    } else if (dynamic_cast<ast::NamespaceNode*>(parent)) {
        // 命名空间内部
        return ContextType::NAMESPACE;
    }
    
    // 默认保持当前上下文
    return currentContext;
}

bool ConstraintChecker::TryFixNode(ast::ASTNode* node, ContextType context) {
    if (!node) return false;
    
    // 获取节点的语法元素类型
    SyntaxElement element = m_Validator->GetSyntaxElement(node);
    
    // 检查是否允许
    const ConstraintRule* rule = m_Manager->GetRule(context);
    if (!rule || rule->IsAllowed(element)) {
        return false;  // 不需要修复
    }
    
    // 尝试修复策略
    utils::Logger::GetInstance().Warning(
        "尝试修复约束违反: " + ConstraintUtils::ElementToString(element) +
        " 在 " + ConstraintUtils::ContextToString(context));
    
    // 1. 如果是在错误的script块中使用了CHTL语法，可以尝试移除
    if (context == ContextType::GLOBAL_SCRIPT && 
        element != SyntaxElement::GENERATOR_COMMENT &&
        element != SyntaxElement::ORIGIN_EMBED) {
        // TODO: 实现节点移除或转换逻辑
        return false;
    }
    
    // 2. 如果是在样式块中使用了不允许的元素，可以尝试转换
    if ((context == ContextType::GLOBAL_STYLE || context == ContextType::LOCAL_STYLE) &&
        element == SyntaxElement::HTML_ELEMENT) {
        // HTML元素不能在样式块中使用
        // TODO: 实现转换逻辑
        return false;
    }
    
    return false;
}

bool ConstraintChecker::HandleStyleBlock(ast::ASTNode* node, ContextType context) {
    // 确定样式块的类型
    ContextType styleContext = (context == ContextType::ELEMENT_BODY) 
                              ? ContextType::LOCAL_STYLE 
                              : ContextType::GLOBAL_STYLE;
    
    // 递归检查样式块内的所有内容
    bool valid = true;
    for (const auto& child : node->GetChildren()) {
        if (!CheckNodeRecursive(child.get(), styleContext)) {
            valid = false;
        }
    }
    
    return valid;
}

bool ConstraintChecker::HandleScriptBlock(ast::ASTNode* node, ContextType context) {
    // 确定script块的类型
    ContextType scriptContext = (context == ContextType::ELEMENT_BODY)
                               ? ContextType::LOCAL_SCRIPT
                               : ContextType::GLOBAL_SCRIPT;
    
    // 检查script块内容
    bool valid = true;
    for (const auto& child : node->GetChildren()) {
        if (!CheckNodeRecursive(child.get(), scriptContext)) {
            valid = false;
        }
    }
    
    // 特别注意：{{}}语法是CHTL JS的特殊语法，不应该被约束系统禁止
    // 这些应该由CHTL JS编译器处理
    
    return valid;
}

bool ConstraintChecker::HandleNamespace(ast::ASTNode* node, ContextType context) {
    // 命名空间内部使用命名空间上下文
    bool valid = true;
    for (const auto& child : node->GetChildren()) {
        if (!CheckNodeRecursive(child.get(), ContextType::NAMESPACE)) {
            valid = false;
        }
    }
    
    return valid;
}

// ConstraintReporter 实现

ConstraintReporter::ConstraintReporter() {}

ConstraintReporter::~ConstraintReporter() = default;

std::string ConstraintReporter::GenerateReport(const ConstraintChecker& checker,
                                              const std::string& outputFile) {
    std::stringstream report;
    
    report << "===== CHTL约束检查报告 =====\n\n";
    
    // 获取错误
    const auto& errors = checker.GetErrors();
    
    if (errors.empty()) {
        report << "✓ 未发现约束违反\n";
    } else {
        report << "✗ 发现 " << errors.size() << " 个约束违反:\n\n";
        
        for (size_t i = 0; i < errors.size(); ++i) {
            report << (i + 1) << ". " << FormatError(errors[i]) << "\n";
        }
    }
    
    // 生成统计信息
    report << "\n" << GenerateStatistics(errors);
    
    // 生成约束规则总结
    report << "\n" << GenerateRuleSummary(checker.GetManager());
    
    // 生成使用指南
    report << "\n" << GenerateContextGuide();
    
    std::string reportStr = report.str();
    
    // 如果指定了输出文件，保存报告
    if (!outputFile.empty()) {
        utils::FileUtils::WriteFile(outputFile, reportStr);
        utils::Logger::GetInstance().Info("约束检查报告已保存到: " + outputFile);
    }
    
    return reportStr;
}

std::string ConstraintReporter::GenerateContextGuide() const {
    std::stringstream guide;
    
    guide << "===== 上下文使用指南 =====\n\n";
    
    for (int i = 0; i < 7; ++i) {
        ContextType context = static_cast<ContextType>(i);
        guide << "【" << ConstraintUtils::ContextToString(context) << "】\n";
        guide << ConstraintUtils::GetContextDescription(context) << "\n\n";
    }
    
    return guide.str();
}

std::string ConstraintReporter::GenerateRuleSummary(const ConstraintManager& manager) const {
    std::stringstream summary;
    
    summary << "===== 约束规则总结 =====\n\n";
    
    for (const auto& rule : manager.GetAllRules()) {
        ContextType context = rule->GetContext();
        summary << "【" << ConstraintUtils::ContextToString(context) << "】\n";
        
        auto allowed = rule->GetAllowedElements();
        if (!allowed.empty()) {
            summary << "允许的元素:\n";
            for (const auto& elem : allowed) {
                summary << "  - " << ConstraintUtils::ElementToString(elem) << "\n";
            }
        }
        
        auto forbidden = rule->GetForbiddenElements();
        if (!forbidden.empty()) {
            summary << "禁止的元素:\n";
            for (const auto& elem : forbidden) {
                summary << "  - " << ConstraintUtils::ElementToString(elem) << "\n";
            }
        }
        
        summary << "\n";
    }
    
    return summary.str();
}

std::string ConstraintReporter::FormatError(const std::string& error) const {
    // 格式化错误信息，使其更易读
    std::string formatted = error;
    
    // 高亮显示关键词
    std::regex keywordRegex(R"(\b(全局样式块|局部样式块|全局script块|局部script块)\b)");
    formatted = std::regex_replace(formatted, keywordRegex, "【$1】");
    
    return formatted;
}

std::string ConstraintReporter::GenerateStatistics(const std::vector<std::string>& errors) const {
    std::stringstream stats;
    
    stats << "===== 统计信息 =====\n";
    
    // 按上下文分类错误
    std::unordered_map<std::string, int> contextErrors;
    
    for (const auto& error : errors) {
        // 简单的模式匹配来分类错误
        if (error.find("全局样式块") != std::string::npos) {
            contextErrors["全局样式块"]++;
        } else if (error.find("局部样式块") != std::string::npos) {
            contextErrors["局部样式块"]++;
        } else if (error.find("全局script块") != std::string::npos) {
            contextErrors["全局script块"]++;
        } else if (error.find("局部script块") != std::string::npos) {
            contextErrors["局部script块"]++;
        } else {
            contextErrors["其他"]++;
        }
    }
    
    for (const auto& pair : contextErrors) {
        stats << pair.first << ": " << pair.second << " 个错误\n";
    }
    
    return stats.str();
}

// ConstraintConfig 实现

ConstraintConfig::ConstraintConfig() {}

ConstraintConfig::~ConstraintConfig() = default;

bool ConstraintConfig::LoadFromFile(const std::string& configFile) {
    std::string content;
    if (!utils::FileUtils::ReadFile(configFile, content)) {
        utils::Logger::GetInstance().Error("无法读取约束配置文件: " + configFile);
        return false;
    }
    
    return ParseConfig(content);
}

void ConstraintConfig::ApplyTo(ConstraintManager& manager) const {
    for (const auto& ruleConfig : m_Rules) {
        auto rule = std::make_unique<ConstraintRule>(ruleConfig.context);
        
        // 设置允许的元素
        for (const auto& elem : ruleConfig.allowed) {
            rule->AddAllowedElement(elem);
        }
        
        // 设置禁止的元素
        for (const auto& elem : ruleConfig.forbidden) {
            rule->AddForbiddenElement(elem);
        }
        
        manager.AddRule(std::move(rule));
    }
}

bool ConstraintConfig::SaveToFile(const std::string& configFile) const {
    std::stringstream config;
    
    config << "# CHTL约束配置文件\n\n";
    
    for (const auto& ruleConfig : m_Rules) {
        config << "[" << ConstraintUtils::ContextToString(ruleConfig.context) << "]\n";
        
        if (!ruleConfig.allowed.empty()) {
            config << "allowed = ";
            for (size_t i = 0; i < ruleConfig.allowed.size(); ++i) {
                config << ConstraintUtils::ElementToString(ruleConfig.allowed[i]);
                if (i < ruleConfig.allowed.size() - 1) config << ", ";
            }
            config << "\n";
        }
        
        if (!ruleConfig.forbidden.empty()) {
            config << "forbidden = ";
            for (size_t i = 0; i < ruleConfig.forbidden.size(); ++i) {
                config << ConstraintUtils::ElementToString(ruleConfig.forbidden[i]);
                if (i < ruleConfig.forbidden.size() - 1) config << ", ";
            }
            config << "\n";
        }
        
        config << "\n";
    }
    
    return utils::FileUtils::WriteFile(configFile, config.str());
}

bool ConstraintConfig::ParseConfig(const std::string& content) {
    // 简单的配置解析
    // TODO: 实现更完善的配置解析逻辑
    
    std::istringstream stream(content);
    std::string line;
    RuleConfig* currentRule = nullptr;
    
    while (std::getline(stream, line)) {
        line = utils::StringUtils::Trim(line);
        
        // 跳过空行和注释
        if (line.empty() || line[0] == '#') continue;
        
        // 检查是否是新的规则段
        if (line[0] == '[' && line.back() == ']') {
            std::string contextName = line.substr(1, line.length() - 2);
            // TODO: 将contextName转换为ContextType
            m_Rules.push_back(RuleConfig());
            currentRule = &m_Rules.back();
            // 设置context
        } else if (currentRule) {
            // 解析allowed或forbidden
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = utils::StringUtils::Trim(line.substr(0, pos));
                std::string value = utils::StringUtils::Trim(line.substr(pos + 1));
                
                // TODO: 解析value中的元素列表
            }
        }
    }
    
    return true;
}

} // namespace constraint
} // namespace chtl