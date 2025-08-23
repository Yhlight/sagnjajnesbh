#pragma once

#include "CHTLJS/Core/CHTLJSToken.h"
#include "CHTLJS/Core/CHTLJSState.h"
#include "CHTLJS/AST/CHTLJSASTNodes.h"
#include <string>
#include <memory>

namespace CHTL {
namespace CHTLJS {
namespace Parser {

/**
 * @brief CHTL JS解析器配置
 */
struct ParserConfig {
    bool enableDebug;           // 启用调试模式
    bool enableErrorRecovery;   // 启用错误恢复
    bool validateSemantics;     // 启用语义验证
    bool strictMode;            // 严格模式
    
    ParserConfig() : enableDebug(false), enableErrorRecovery(true), 
                    validateSemantics(true), strictMode(true) {}
};

/**
 * @brief CHTL JS解析上下文
 */
struct ParseContext {
    std::string currentVirtualObject;       // 当前虚对象
    std::string currentSelector;            // 当前选择器
    std::vector<std::string> virtualObjects; // 虚对象列表
    std::unordered_map<std::string, std::string> variables; // 变量映射
    int nestingLevel;                       // 嵌套级别
    
    ParseContext() : nestingLevel(0) {}
};

/**
 * @brief CHTL JS语法分析器
 * 
 * 将CHTL JS Token流解析为AST，支持：
 * - 增强选择器解析
 * - 虚对象解析
 * - 监听器块解析
 * - 事件委托解析
 * - 动画块解析
 * - iNeverAway特殊函数解析
 */
class CHTLJSParser {
public:
    /**
     * @brief 构造函数
     * @param stateManager 状态管理器
     * @param config 解析器配置
     */
    explicit CHTLJSParser(Core::CHTLJSState& stateManager, const ParserConfig& config = ParserConfig());
    
    /**
     * @brief 解析Token流为AST
     * @param tokens Token流
     * @param fileName 文件名
     * @return AST根节点
     */
    AST::ASTNodePtr Parse(Core::CHTLJSTokenStream& tokens, const std::string& fileName = "");
    
    /**
     * @brief 设置配置
     */
    void SetConfig(const ParserConfig& config) { config_ = config; }
    
    /**
     * @brief 获取配置
     */
    const ParserConfig& GetConfig() const { return config_; }
    
    /**
     * @brief 获取解析上下文
     */
    const ParseContext& GetContext() const { return context_; }

private:
    // 核心解析方法
    AST::ASTNodePtr ParseStatement();
    AST::ASTNodePtr ParseExpression();
    AST::ASTNodePtr ParsePrimaryExpression();
    
    // CHTL JS特殊语法解析
    AST::ASTNodePtr ParseEnhancedSelector();
    AST::ASTNodePtr ParseVirtualObject();
    AST::ASTNodePtr ParseListenBlock();
    AST::ASTNodePtr ParseDelegateBlock();
    AST::ASTNodePtr ParseAnimateBlock();
    AST::ASTNodePtr ParseINeverAwayBlock();
    
    // 函数解析
    AST::ASTNodePtr ParseFunctionDefinition();
    AST::ASTNodePtr ParseArrowFunction();
    
    // 对象和数组解析
    AST::ASTNodePtr ParseObjectLiteral();
    AST::ASTNodePtr ParseArrayLiteral();
    
    // 方法调用解析
    AST::ASTNodePtr ParseMethodCall(AST::ASTNodePtr object);
    AST::ASTNodePtr ParseVirtualMethodCall();
    
    // 动画相关解析
    AST::ASTNodePtr ParseAnimationKeyframe();
    AST::ASTNodePtr ParseAnimationProperty();
    
    // 变量和赋值解析
    AST::ASTNodePtr ParseVariableDeclaration();
    AST::ASTNodePtr ParseAssignmentExpression();
    AST::ASTNodePtr ParsePropertyAccess(AST::ASTNodePtr object);
    
    // 辅助方法
    bool Check(Core::TokenType type) const;
    bool Match(Core::TokenType type);
    bool Consume(Core::TokenType type, const std::string& message);
    void Advance();
    const Core::CHTLJSToken& Current() const;
    const Core::CHTLJSToken& Previous() const;
    const Core::CHTLJSToken& Peek() const;
    bool IsAtEnd() const;
    
    // 错误处理
    void ReportError(const std::string& message);
    void Synchronize();
    
    // 解析辅助
    std::string ParseIdentifier();
    std::string ParseStringValue();
    std::string ParseNumberValue();
    void SkipWhitespaceAndComments();
    
    // 语法检查辅助
    bool IsArrowFunction() const;

private:
    ParserConfig config_;               // 配置
    Core::CHTLJSState& stateManager_;   // 状态管理器引用
    Core::CHTLJSTokenStream* tokens_;   // Token流指针
    std::string fileName_;              // 文件名
    ParseContext context_;              // 解析上下文
    int nodeCount_;                     // 节点计数
};

} // namespace Parser
} // namespace CHTLJS
} // namespace CHTL