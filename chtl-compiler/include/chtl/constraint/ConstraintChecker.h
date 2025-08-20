#pragma once

#include <memory>
#include <string>
#include <vector>
#include "chtl/constraint/ConstraintSystem.h"

namespace chtl {

// 前向声明
namespace ast {
    class ASTNode;
    class DocumentNode;
}

namespace parser {
    class CHTLParser;
}

namespace constraint {

/**
 * 约束检查器
 * 集成约束系统到编译器流程中
 */
class ConstraintChecker {
public:
    ConstraintChecker();
    ~ConstraintChecker();
    
    /**
     * 检查AST树的约束
     * @param document AST文档根节点
     * @return 是否通过所有约束检查
     */
    bool CheckDocument(ast::DocumentNode* document);
    
    /**
     * 检查单个AST节点
     * @param node AST节点
     * @param context 当前上下文
     * @return 是否通过约束检查
     */
    bool CheckNode(ast::ASTNode* node, ContextType context = ContextType::TOP_LEVEL);
    
    /**
     * 获取所有约束错误
     */
    const std::vector<std::string>& GetErrors() const { return m_Validator->GetErrors(); }
    
    /**
     * 清除错误
     */
    void ClearErrors() { m_Validator->ClearErrors(); }
    
    /**
     * 设置严格模式
     * 严格模式下，约束违反会立即报错
     */
    void SetStrictMode(bool strict);
    
    /**
     * 获取约束管理器
     */
    ConstraintManager& GetManager() { return *m_Manager; }
    const ConstraintManager& GetManager() const { return *m_Manager; }
    
    /**
     * 注册到解析器
     * 使解析器在构建AST时进行实时约束检查
     */
    void RegisterToParser(parser::CHTLParser* parser);
    
    /**
     * 执行约束修复
     * 尝试自动修复一些简单的约束违反
     * @return 修复的问题数量
     */
    int AutoFix(ast::ASTNode* node);
    
    /**
     * 获取约束建议
     * 根据当前上下文，提供可用的语法元素建议
     */
    std::vector<SyntaxElement> GetSuggestions(ContextType context) const;
    
    /**
     * 验证except约束
     * 检查except语句定义的约束
     */
    bool ValidateExceptConstraints(ast::ASTNode* node);
    
private:
    std::unique_ptr<ConstraintManager> m_Manager;
    std::unique_ptr<ConstraintValidator> m_Validator;
    
    // 递归检查辅助方法
    bool CheckNodeRecursive(ast::ASTNode* node, ContextType context);
    
    // 确定子节点的上下文
    ContextType DetermineChildContext(ast::ASTNode* parent, 
                                     ast::ASTNode* child,
                                     ContextType currentContext) const;
    
    // 修复辅助方法
    bool TryFixNode(ast::ASTNode* node, ContextType context);
    
    // 处理特殊节点类型
    bool HandleStyleBlock(ast::ASTNode* node, ContextType context);
    bool HandleScriptBlock(ast::ASTNode* node, ContextType context);
    bool HandleNamespace(ast::ASTNode* node, ContextType context);
};

/**
 * 约束报告生成器
 * 生成详细的约束检查报告
 */
class ConstraintReporter {
public:
    ConstraintReporter();
    ~ConstraintReporter();
    
    /**
     * 生成约束检查报告
     * @param checker 约束检查器
     * @param outputFile 输出文件路径（可选）
     */
    std::string GenerateReport(const ConstraintChecker& checker,
                              const std::string& outputFile = "");
    
    /**
     * 生成上下文使用指南
     */
    std::string GenerateContextGuide() const;
    
    /**
     * 生成约束规则总结
     */
    std::string GenerateRuleSummary(const ConstraintManager& manager) const;
    
private:
    // 格式化错误信息
    std::string FormatError(const std::string& error) const;
    
    // 生成统计信息
    std::string GenerateStatistics(const std::vector<std::string>& errors) const;
};

/**
 * 约束配置
 * 从配置文件加载自定义约束规则
 */
class ConstraintConfig {
public:
    ConstraintConfig();
    ~ConstraintConfig();
    
    /**
     * 从文件加载配置
     * @param configFile 配置文件路径
     * @return 是否加载成功
     */
    bool LoadFromFile(const std::string& configFile);
    
    /**
     * 应用配置到约束管理器
     */
    void ApplyTo(ConstraintManager& manager) const;
    
    /**
     * 保存当前配置
     */
    bool SaveToFile(const std::string& configFile) const;
    
private:
    struct RuleConfig {
        ContextType context;
        std::vector<SyntaxElement> allowed;
        std::vector<SyntaxElement> forbidden;
    };
    
    std::vector<RuleConfig> m_Rules;
    
    // 解析配置
    bool ParseConfig(const std::string& content);
};

} // namespace constraint
} // namespace chtl