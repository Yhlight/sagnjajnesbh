#pragma once

#include "CHTL/Constraints/CHTLConstraintValidator.h"
#include "CHTL/Core/CHTLToken.h"
#include <vector>
#include <memory>

namespace CHTL {
namespace Constraints {

/**
 * @brief except约束解析器
 * 
 * 语法文档第1062-1097行：
 * - 精确约束：except span, [Custom] @Element Box;
 * - 类型约束：except @Html; except [Template] @Var; except [Custom];
 * - 全局约束：在命名空间内使用except [Template];
 */
class ExceptConstraintParser {
public:
    /**
     * @brief 构造函数
     * @param tokens Token流
     */
    ExceptConstraintParser(const std::vector<Core::CHTLToken>& tokens);
    
    /**
     * @brief 解析except约束语句
     * @param startPos 开始位置
     * @return 解析的约束规则列表
     */
    std::vector<ConstraintRule> ParseExceptConstraints(size_t& startPos);
    
    /**
     * @brief 检查当前位置是否为except关键字
     * @param pos 位置
     * @return 是否为except关键字
     */
    bool IsExceptKeyword(size_t pos) const;

private:
    /**
     * @brief 解析精确约束
     * @param pos 当前位置
     * @return 约束规则
     */
    ConstraintRule ParsePreciseConstraint(size_t& pos);
    
    /**
     * @brief 解析类型约束
     * @param pos 当前位置
     * @return 约束规则
     */
    ConstraintRule ParseTypeConstraint(size_t& pos);
    
    /**
     * @brief 解析约束目标
     * @param pos 当前位置
     * @param target 输出的约束目标
     * @param specificName 输出的特定名称
     * @return 是否解析成功
     */
    bool ParseConstraintTarget(size_t& pos, ConstraintTarget& target, std::string& specificName);
    
    /**
     * @brief 跳过空白和注释
     * @param pos 位置引用
     */
    void SkipWhitespaceAndComments(size_t& pos);
    
    /**
     * @brief 检查是否为指定类型的Token
     * @param pos 位置
     * @param type Token类型
     * @return 是否匹配
     */
    bool IsTokenType(size_t pos, Core::TokenType type) const;
    
    /**
     * @brief 获取Token值
     * @param pos 位置
     * @return Token值
     */
    std::string GetTokenValue(size_t pos) const;
    
    /**
     * @brief 消费指定类型的Token
     * @param pos 位置引用
     * @param type 期望的Token类型
     * @return 是否成功消费
     */
    bool ConsumeToken(size_t& pos, Core::TokenType type);
    
    /**
     * @brief 解析标识符
     * @param pos 位置引用
     * @return 标识符字符串
     */
    std::string ParseIdentifier(size_t& pos);
    
    /**
     * @brief 解析类型约束目标
     * @param pos 位置引用
     * @param target 输出的约束目标
     * @param specificName 输出的特定名称
     * @return 是否解析成功
     */
    bool ParseTypeConstraintTarget(size_t& pos, ConstraintTarget& target, std::string& specificName);
    
    /**
     * @brief 解析@类型约束目标
     * @param pos 位置引用
     * @param target 输出的约束目标
     * @param specificName 输出的特定名称
     * @return 是否解析成功
     */
    bool ParseAtTypeConstraintTarget(size_t& pos, ConstraintTarget& target, std::string& specificName);

private:
    const std::vector<Core::CHTLToken>& tokens_;    // Token流
    mutable std::string lastError_;                 // 最后的错误信息
};

/**
 * @brief except约束集成器
 * 
 * 将解析的except约束集成到约束验证器中
 */
class ExceptConstraintIntegrator {
public:
    /**
     * @brief 构造函数
     * @param validator 约束验证器
     */
    ExceptConstraintIntegrator(CHTLConstraintValidator& validator);
    
    /**
     * @brief 处理except约束语句
     * @param tokens Token流
     * @param context 语法上下文
     * @param startPos 开始位置
     * @return 是否处理成功
     */
    bool ProcessExceptConstraints(const std::vector<Core::CHTLToken>& tokens,
                                 SyntaxContext context,
                                 size_t startPos);
    
    /**
     * @brief 应用约束规则到验证器
     * @param context 语法上下文
     * @param rules 约束规则列表
     */
    void ApplyConstraintRules(SyntaxContext context, const std::vector<ConstraintRule>& rules);

private:
    CHTLConstraintValidator& validator_;    // 约束验证器引用
};

} // namespace Constraints
} // namespace CHTL