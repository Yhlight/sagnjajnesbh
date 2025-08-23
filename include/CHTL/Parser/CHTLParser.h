#pragma once

#include <memory>
#include <vector>
#include <functional>
#include "CHTL/Core/CHTLToken.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"
#include "CHTL/Core/CHTLStateContext.h"
#include "CHTL/Core/ImportManager.h"
#include "CHTL/Core/NamespaceMerger.h"
#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/Lexer/CHTLLexer.h"
#include "CHTL/Constraints/CHTLConstraintValidator.h"
#include "CHTL/Constraints/ExceptConstraintParser.h"

namespace CHTL {
namespace Parser {

/**
 * @brief 语法分析器配置
 */
struct ParserConfig {
    bool enableErrorRecovery;       // 是否启用错误恢复
    bool enableDebug;               // 是否启用调试模式
    bool strictMode;                // 是否启用严格模式
    size_t maxErrors;               // 最大错误数量
    bool validateSemantics;         // 是否验证语义
    
    ParserConfig() : enableErrorRecovery(true), enableDebug(false), 
                    strictMode(true), maxErrors(100), validateSemantics(true) {}
};

/**
 * @brief 语法分析上下文
 */
struct ParseContext {
    std::string currentNamespace;       // 当前命名空间
    std::string currentTemplate;        // 当前模板
    std::string currentCustom;          // 当前自定义
    bool inLocalStyleBlock;             // 是否在局部样式块中
    bool inLocalScriptBlock;            // 是否在局部脚本块中
    int nestingLevel;                   // 嵌套级别
    std::vector<std::string> constraintStack;  // 约束栈
    
    ParseContext() : inLocalStyleBlock(false), inLocalScriptBlock(false), nestingLevel(0) {}
};

/**
 * @brief CHTL语法分析器类
 * 
 * 实现递归下降语法分析器，严格按照CHTL语法文档规范
 */
class CHTLParser {
public:
    /**
     * @brief 构造函数
     * @param globalMap 全局映射表
     * @param stateManager 状态管理器
     * @param config 语法分析器配置
     */
    CHTLParser(Core::CHTLGlobalMap& globalMap, Core::CHTLState& stateManager,
              const ParserConfig& config = ParserConfig());
    
    /**
     * @brief 析构函数
     */
    ~CHTLParser() = default;
    
    /**
     * @brief 解析Token流生成AST
     * @param tokens Token流
     * @param fileName 文件名
     * @return AST根节点
     */
    AST::ASTNodePtr Parse(Core::TokenStream& tokens, const std::string& fileName = "");
    
    /**
     * @brief 设置配置
     * @param config 新配置
     */
    void SetConfig(const ParserConfig& config) { config_ = config; }
    
    /**
     * @brief 获取配置
     * @return 当前配置
     */
    const ParserConfig& GetConfig() const { return config_; }
    
    /**
     * @brief 重置解析器状态
     */
    void Reset();
    
    /**
     * @brief 获取错误数量
     * @return 错误数量
     */
    size_t GetErrorCount() const;
    
    /**
     * @brief 获取解析统计信息
     * @return 统计信息字符串
     */
    std::string GetStatistics() const;
    
    /**
     * @brief 验证节点是否符合语法约束
     * @param node AST节点
     * @param context 语法上下文
     * @return 是否通过验证
     */
    bool ValidateConstraints(const std::shared_ptr<AST::ASTNode>& node, 
                           Constraints::SyntaxContext context) const;
    
    /**
     * @brief 处理except约束语句
     * @param context 语法上下文
     * @return 是否处理成功
     */
    bool ProcessExceptConstraints(Constraints::SyntaxContext context);

private:
    // 顶层解析方法
    /**
     * @brief 解析程序（顶层）
     * @return AST根节点
     */
    AST::ASTNodePtr ParseProgram();
    
    /**
     * @brief 解析声明
     * @return 声明节点
     */
    AST::ASTNodePtr ParseDeclaration();
    
    // 模板和自定义解析
    /**
     * @brief 解析模板声明
     * @return 模板节点
     */
    AST::ASTNodePtr ParseTemplateDeclaration();
    
    /**
     * @brief 解析自定义声明
     * @return 自定义节点
     */
    AST::ASTNodePtr ParseCustomDeclaration();
    
    /**
     * @brief 解析原始嵌入声明
     * @return 原始嵌入节点
     */
    AST::ASTNodePtr ParseOriginDeclaration();
    
    /**
     * @brief 解析导入声明
     * @return 导入节点
     */
    AST::ASTNodePtr ParseImportDeclaration();
    
    /**
     * @brief 解析命名空间声明（支持省略大括号 - 语法文档第998行）
     * @return 命名空间节点
     */
    AST::ASTNodePtr ParseNamespaceDeclaration();
    
    /**
     * @brief 解析省略大括号的单个命名空间声明 - 语法文档第998行
     * @return 声明节点
     */
    AST::ASTNodePtr ParseSingleNamespaceDeclaration();
    
    /**
     * @brief 解析平级命名空间声明 - 语法文档第998行
     * @param parentNode 父命名空间节点
     */
    void ParseParallelNamespaceDeclarations(std::shared_ptr<AST::NamespaceNode> parentNode);
    
    /**
     * @brief 解析配置声明
     * @return 配置节点
     */
    AST::ASTNodePtr ParseConfigurationDeclaration();
    
    // HTML元素解析
    /**
     * @brief 解析HTML元素
     * @return 元素节点
     */
    AST::ASTNodePtr ParseHTMLElement();
    
    /**
     * @brief 解析元素内容
     * @param element 元素节点
     */
    void ParseElementContent(std::shared_ptr<AST::ElementNode> element);
    
    /**
     * @brief 解析元素属性
     * @param element 元素节点
     */
    void ParseElementAttributes(std::shared_ptr<AST::ElementNode> element);
    
    /**
     * @brief 解析文本节点
     * @return 文本节点
     */
    AST::ASTNodePtr ParseTextNode();
    
    // 样式和脚本解析
    /**
     * @brief 解析样式块
     * @return 样式块节点
     */
    AST::ASTNodePtr ParseStyleBlock();
    
    /**
     * @brief 解析脚本块
     * @return 脚本块节点
     */
    AST::ASTNodePtr ParseScriptBlock();
    
    /**
     * @brief 解析样式内容
     * @param styleBlock 样式块节点
     */
    void ParseStyleContent(std::shared_ptr<AST::StyleBlockNode> styleBlock);
    
    /**
     * @brief 解析CSS选择器
     * @return CSS选择器节点
     */
    AST::ASTNodePtr ParseCSSSelector();
    
    /**
     * @brief 解析CSS属性
     * @return CSS属性节点
     */
    AST::ASTNodePtr ParseCSSProperty();
    
    // 继承和特例化解析
    /**
     * @brief 解析继承声明
     * @return 继承节点
     */
    AST::ASTNodePtr ParseInheritanceDeclaration();
    
    /**
     * @brief 解析删除声明
     * @return 删除节点
     */
    AST::ASTNodePtr ParseDeletionDeclaration();
    
    /**
     * @brief 解析插入声明
     * @return 插入节点
     */
    AST::ASTNodePtr ParseInsertionDeclaration();
    
    /**
     * @brief 解析特例化内容
     * @return 特例化节点
     */
    AST::ASTNodePtr ParseSpecialization();
    
    // 引用和访问解析
    /**
     * @brief 解析模板引用
     * @return 模板引用节点
     */
    AST::ASTNodePtr ParseTemplateReference();
    
    /**
     * @brief 解析自定义引用
     * @return 自定义引用节点
     */
    AST::ASTNodePtr ParseCustomReference();
    
    /**
     * @brief 解析变量引用
     * @return 变量引用节点
     */
    AST::ASTNodePtr ParseVariableReference();
    
    /**
     * @brief 解析索引访问
     * @param elementName 元素名称
     * @return 索引访问节点
     */
    AST::ASTNodePtr ParseIndexAccess(const std::string& elementName);
    
    // 约束解析
    /**
     * @brief 解析约束声明
     * @return 约束节点
     */
    AST::ASTNodePtr ParseConstraintDeclaration();
    
    // 变量组解析
    /**
     * @brief 解析变量组
     * @return 变量组节点
     */
    AST::ASTNodePtr ParseVariableGroup();
    
    // 工具方法
    /**
     * @brief 检查当前Token类型
     * @param type Token类型
     * @return 是否匹配
     */
    bool Check(Core::TokenType type);
    
    /**
     * @brief 检查当前Token是否为指定类型之一
     * @param types Token类型列表
     * @return 是否匹配任意一种
     */
    bool CheckAny(const std::vector<Core::TokenType>& types);
    
    /**
     * @brief 消费指定类型的Token
     * @param type 期望的Token类型
     * @param errorMessage 错误消息
     * @return 是否成功消费
     */
    bool Consume(Core::TokenType type, const std::string& errorMessage = "");
    
    /**
     * @brief 前进到下一个Token
     * @return 前进前的Token
     */
    Core::CHTLToken Advance();
    
    /**
     * @brief 获取当前Token
     * @return 当前Token
     */
    const Core::CHTLToken& Current() const;
    
    /**
     * @brief 查看下一个Token
     * @param offset 偏移量
     * @return 指定位置的Token
     */
    const Core::CHTLToken& Peek(int offset = 1) const;
    
    /**
     * @brief 检查是否到达Token流末尾
     * @return 是否到达末尾
     */
    bool IsAtEnd() const;
    
    /**
     * @brief 跳过空白和注释Token
     */
    void SkipWhitespaceAndComments();
    
    /**
     * @brief 解析标识符
     * @return 标识符字符串
     */
    std::string ParseIdentifier();
    
    /**
     * @brief 解析字符串值
     * @return 字符串值
     */
    std::string ParseStringValue();
    
    /**
     * @brief 解析类型标识符
     * @return 类型标识符字符串
     */
    std::string ParseTypeIdentifier();
    
    /**
     * @brief 解析数字值
     * @return 数字字符串
     */
    std::string ParseNumberValue();
    
    /**
     * @brief 解析代码块
     * @param endToken 结束Token类型
     * @return 子节点列表
     */
    AST::ASTNodeList ParseBlock(Core::TokenType endToken = Core::TokenType::RIGHT_BRACE);
    
    // 错误处理
    /**
     * @brief 报告语法错误
     * @param message 错误消息
     */
    void ReportError(const std::string& message);
    
    /**
     * @brief 报告语法警告
     * @param message 警告消息
     */
    void ReportWarning(const std::string& message);
    
    /**
     * @brief 错误恢复
     * @param synchronizeTokens 同步Token类型列表
     */
    void Synchronize(const std::vector<Core::TokenType>& synchronizeTokens = {});
    
    /**
     * @brief 验证语义
     * @param node AST节点
     * @return 验证是否通过
     */
    bool ValidateSemantics(AST::ASTNodePtr node);
    
    /**
     * @brief 检查约束（完整实现）
     * @param nodeName 节点名称
     * @param nodeType 节点类型
     * @return 是否违反约束
     */
    bool CheckConstraints(const std::string& nodeName, const std::string& nodeType);

private:
    ParserConfig config_;               // 解析器配置
    Core::CHTLGlobalMap& globalMap_;    // 全局映射表引用
    Core::CHTLState& stateManager_;     // 状态管理器引用（兼容性保留）
    Core::TokenStream* tokens_;         // Token流指针
    std::string fileName_;              // 当前文件名
    ParseContext context_;              // 解析上下文
    size_t nodeCount_;                  // 节点计数
    
    // RAII状态机和Context系统 - 增强的状态管理
    std::unique_ptr<Core::CHTLStateMachine> stateMachine_;        // RAII状态机
    std::shared_ptr<Core::CHTLStateContext> stateContext_;        // 状态上下文
    std::unique_ptr<Core::CHTLStateInferenceEngine> stateInferenceEngine_;  // 状态推断引擎
    
    // 增强导入系统 - 严格按照目标规划.ini要求
    std::unique_ptr<Core::ImportManager> importManager_;         // 导入管理器
    
    // 增强命名空间系统 - 严格按照目标规划.ini第107行要求
    std::unique_ptr<Core::NamespaceMerger> namespaceMerger_;     // 命名空间合并管理器
    
    // 错误恢复状态
    bool inErrorRecovery_;              // 是否在错误恢复中
    size_t errorCount_;                 // 错误计数
    
    // 语法约束系统
    std::unique_ptr<Constraints::CHTLConstraintValidator> constraintValidator_;     // 约束验证器
    std::unique_ptr<Constraints::ExceptConstraintIntegrator> constraintIntegrator_; // 约束集成器
};

/**
 * @brief 语法分析器工厂类
 */
class ParserFactory {
public:
    /**
     * @brief 创建默认语法分析器
     * @param globalMap 全局映射表
     * @param stateManager 状态管理器
     * @return 语法分析器指针
     */
    static std::unique_ptr<CHTLParser> CreateDefaultParser(
        Core::CHTLGlobalMap& globalMap, 
        Core::CHTLState& stateManager);
    
    /**
     * @brief 创建严格模式语法分析器
     * @param globalMap 全局映射表
     * @param stateManager 状态管理器
     * @return 语法分析器指针
     */
    static std::unique_ptr<CHTLParser> CreateStrictParser(
        Core::CHTLGlobalMap& globalMap, 
        Core::CHTLState& stateManager);
    
    /**
     * @brief 创建调试模式语法分析器
     * @param globalMap 全局映射表
     * @param stateManager 状态管理器
     * @return 语法分析器指针
     */
    static std::unique_ptr<CHTLParser> CreateDebugParser(
        Core::CHTLGlobalMap& globalMap, 
        Core::CHTLState& stateManager);
};

} // namespace Parser
} // namespace CHTL