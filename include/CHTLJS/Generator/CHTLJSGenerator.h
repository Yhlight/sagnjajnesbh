#pragma once

#include "CHTLJS/AST/CHTLJSASTNodes.h"
#include "CHTLJS/AST/CHTLJSASTVisitor.h"
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {
namespace CHTLJS {
namespace Generator {

/**
 * @brief CHTL JS生成器配置
 */
struct GeneratorConfig {
    bool prettyPrint;           // 美化输出
    bool minify;                // 压缩输出
    bool includeComments;       // 包含注释
    bool enableDebug;           // 启用调试
    bool generateSourceMap;     // 生成源映射
    int indentSize;             // 缩进大小
    std::string indentChar;     // 缩进字符
    
    GeneratorConfig() : prettyPrint(true), minify(false), includeComments(false),
                       enableDebug(false), generateSourceMap(false),
                       indentSize(2), indentChar("  ") {}
};

/**
 * @brief CHTL JS生成上下文
 */
struct GenerationContext {
    std::unordered_map<std::string, std::string> virtualObjects;    // 虚对象映射
    std::unordered_map<std::string, std::string> globalFunctions;   // 全局函数映射
    std::unordered_set<std::string> usedSelectors;                  // 已使用的选择器
    std::vector<std::string> eventDelegates;                        // 事件委托注册
    std::vector<std::string> animations;                            // 动画定义
    std::string currentNamespace;                                   // 当前命名空间
    int functionCounter;                                            // 函数计数器
    int selectorCounter;                                            // 选择器计数器
    
    GenerationContext() : functionCounter(0), selectorCounter(0) {}
};

/**
 * @brief CHTL JS代码生成器
 * 
 * 将CHTL JS AST转换为优化的JavaScript代码，支持：
 * - 增强选择器转换为DOM查询
 * - 虚对象转换为全局函数
 * - 监听器转换为addEventListener
 * - 事件委托转换为事件委托模式
 * - 动画转换为requestAnimationFrame
 * - iNeverAway转换为状态重载函数
 */
class CHTLJSGenerator : public AST::BaseASTVisitor {
public:
    /**
     * @brief 构造函数
     * @param config 生成器配置
     */
    explicit CHTLJSGenerator(const GeneratorConfig& config = GeneratorConfig());
    
    /**
     * @brief 生成JavaScript代码
     * @param ast AST根节点
     * @return 生成的JavaScript代码
     */
    std::string Generate(AST::ASTNodePtr ast);
    
    /**
     * @brief 重置生成器状态
     */
    void Reset();
    
    /**
     * @brief 获取生成统计信息
     */
    std::string GetStatistics() const;
    
    // AST访问者方法重写
    void VisitRootNode(AST::RootNode& node) override;
    void VisitIdentifierNode(AST::IdentifierNode& node) override;
    void VisitLiteralNode(AST::LiteralNode& node) override;
    void VisitEnhancedSelectorNode(AST::EnhancedSelectorNode& node) override;
    void VisitVirtualObjectNode(AST::VirtualObjectNode& node) override;
    void VisitListenBlockNode(AST::ListenBlockNode& node) override;
    void VisitDelegateBlockNode(AST::DelegateBlockNode& node) override;
    void VisitAnimateBlockNode(AST::AnimateBlockNode& node) override;
    // 错误的访问方法声明已移除 - 这些节点属于JavaScript语法或CJMOD扩展
    // void VisitINeverAwayBlockNode(AST::INeverAwayBlockNode& node) override; - CJMOD扩展
    // void VisitFunctionDefinitionNode(AST::FunctionDefinitionNode& node) override; - JavaScript语法
    // void VisitObjectLiteralNode(AST::ObjectLiteralNode& node) override; - JavaScript语法
    // void VisitArrayLiteralNode(AST::ArrayLiteralNode& node) override; - JavaScript语法
    // CHTL JS核心特征处理（虚对象方法调用）
    void VisitVirtualMethodCallNode(AST::VirtualMethodCallNode& node) override;  // 虚对象方法调用
    // void VisitVariableDeclarationNode(AST::VariableDeclarationNode& node) override; - JavaScript语法
    
    // VisitArrowFunctionNode已移除 - CHTL JS不包含JS语法
    void VisitAnimationKeyframeNode(AST::AnimationKeyframeNode& node) override;
    // void VisitAssignmentExpressionNode(AST::AssignmentExpressionNode& node) override; - JavaScript语法
    // void VisitPropertyAccessNode(AST::PropertyAccessNode& node) override; - JavaScript语法
    void VisitCommentNode(AST::CommentNode& node) override;

private:
    /**
     * @brief 生成增强选择器的DOM查询代码
     */
    std::string GenerateDOMQuery(AST::EnhancedSelectorNode& selector);
    
    /**
     * @brief 生成虚对象的全局函数
     */
    void GenerateVirtualObjectFunctions(AST::VirtualObjectNode& virtualObject);
    
    /**
     * @brief 生成事件监听器代码
     */
    std::string GenerateEventListener(AST::ListenBlockNode& listenBlock);
    
    /**
     * @brief 生成事件委托代码
     */
    std::string GenerateEventDelegate(AST::DelegateBlockNode& delegateBlock);
    
    /**
     * @brief 生成动画代码
     */
    std::string GenerateAnimation(AST::AnimateBlockNode& animateBlock);
    
    // GenerateINeverAwayFunctions已移除 - iNeverAway是CJMOD扩展，不属于CHTL JS核心
    // 语法文档第1485行明确说明iNeverAway属于CJMOD扩展
    
    /**
     * @brief 生成全局函数名
     */
    std::string GenerateGlobalFunctionName(const std::string& baseName);
    
    /**
     * @brief 生成选择器缓存变量名
     */
    std::string GenerateSelectorCacheName(const std::string& selector);
    
    /**
     * @brief 添加缩进
     */
    void AddIndent();
    
    /**
     * @brief 移除缩进
     */
    void RemoveIndent();
    
    /**
     * @brief 获取当前缩进
     */
    std::string GetIndent() const;
    
    /**
     * @brief 收集全局代码
     */
    void CollectGlobalCode(const std::string& code);
    
    /**
     * @brief 收集初始化代码
     */
    void CollectInitCode(const std::string& code);

private:
    GeneratorConfig config_;            // 生成器配置
    GenerationContext context_;         // 生成上下文
    std::ostringstream output_;         // 主输出流
    std::ostringstream globalCode_;     // 全局代码
    std::ostringstream initCode_;       // 初始化代码
    int currentIndent_;                 // 当前缩进级别
    
    // 统计信息
    int virtualObjectCount_;
    int enhancedSelectorCount_;
    int eventListenerCount_;
    int eventDelegateCount_;
    int animationCount_;
    // int iNeverAwayCount_; - 已移除，iNeverAway是CJMOD扩展
};

} // namespace Generator
} // namespace CHTLJS
} // namespace CHTL