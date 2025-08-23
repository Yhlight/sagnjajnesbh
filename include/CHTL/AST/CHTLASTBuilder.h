#pragma once

#include <memory>
#include <stack>
#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/Core/CHTLToken.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CHTL/Core/CHTLState.h"

namespace CHTL {
namespace AST {

/**
 * @brief AST构建器类
 * 
 * 负责从Token流构建CHTL抽象语法树，严格按照语法文档规范
 */
class CHTLASTBuilder {
public:
    /**
     * @brief 构造函数
     * @param globalMap 全局映射表
     * @param stateManager 状态管理器
     */
    CHTLASTBuilder(Core::CHTLGlobalMap& globalMap, Core::CHTLState& stateManager);
    
    /**
     * @brief 析构函数
     */
    ~CHTLASTBuilder() = default;
    
    /**
     * @brief 从Token流构建AST
     * @param tokens Token流
     * @param fileName 文件名
     * @return 根节点指针
     */
    ASTNodePtr BuildAST(Core::TokenStream& tokens, const std::string& fileName = "");
    
    /**
     * @brief 重置构建器状态
     */
    void Reset();
    
    /**
     * @brief 获取构建错误数量
     */
    size_t GetErrorCount() const;
    
    /**
     * @brief 设置是否启用调试模式
     */
    void SetDebugMode(bool debug) { debugMode_ = debug; }
    
    /**
     * @brief 获取构建统计信息
     */
    std::string GetStatistics() const;

private:
    /**
     * @brief 解析顶层声明
     * @param tokens Token流
     * @param root 根节点
     */
    void ParseTopLevelDeclarations(Core::TokenStream& tokens, ASTNodePtr root);
    
    /**
     * @brief 解析模板声明
     * @param tokens Token流
     * @return 模板节点
     */
    ASTNodePtr ParseTemplateDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析自定义声明
     * @param tokens Token流
     * @return 自定义节点
     */
    ASTNodePtr ParseCustomDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析原始嵌入声明
     * @param tokens Token流
     * @return 原始嵌入节点
     */
    ASTNodePtr ParseOriginDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析导入声明
     * @param tokens Token流
     * @return 导入节点
     */
    ASTNodePtr ParseImportDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析命名空间声明
     * @param tokens Token流
     * @return 命名空间节点
     */
    ASTNodePtr ParseNamespaceDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析配置声明
     * @param tokens Token流
     * @return 配置节点
     */
    ASTNodePtr ParseConfigurationDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析HTML元素
     * @param tokens Token流
     * @return 元素节点
     */
    ASTNodePtr ParseHTMLElement(Core::TokenStream& tokens);
    
    /**
     * @brief 解析元素属性
     * @param tokens Token流
     * @param element 元素节点
     */
    void ParseElementAttributes(Core::TokenStream& tokens, std::shared_ptr<ElementNode> element);
    
    /**
     * @brief 解析文本节点
     * @param tokens Token流
     * @return 文本节点
     */
    ASTNodePtr ParseTextNode(Core::TokenStream& tokens);
    
    /**
     * @brief 解析样式块
     * @param tokens Token流
     * @return 样式块节点
     */
    ASTNodePtr ParseStyleBlock(Core::TokenStream& tokens);
    
    /**
     * @brief 解析脚本块
     * @param tokens Token流
     * @return 脚本块节点
     */
    ASTNodePtr ParseScriptBlock(Core::TokenStream& tokens);
    
    /**
     * @brief 解析CSS选择器
     * @param tokens Token流
     * @return CSS选择器节点
     */
    ASTNodePtr ParseCSSSelector(Core::TokenStream& tokens);
    
    /**
     * @brief 解析CSS属性
     * @param tokens Token流
     * @return CSS属性节点
     */
    ASTNodePtr ParseCSSProperty(Core::TokenStream& tokens);
    
    /**
     * @brief 解析继承声明
     * @param tokens Token流
     * @return 继承节点
     */
    ASTNodePtr ParseInheritanceDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析删除声明
     * @param tokens Token流
     * @return 删除节点
     */
    ASTNodePtr ParseDeletionDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析插入声明
     * @param tokens Token流
     * @return 插入节点
     */
    ASTNodePtr ParseInsertionDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析索引访问
     * @param tokens Token流
     * @param elementName 元素名称
     * @return 索引访问节点
     */
    ASTNodePtr ParseIndexAccess(Core::TokenStream& tokens, const std::string& elementName);
    
    /**
     * @brief 解析约束声明
     * @param tokens Token流
     * @return 约束节点
     */
    ASTNodePtr ParseConstraintDeclaration(Core::TokenStream& tokens);
    
    /**
     * @brief 解析变量组
     * @param tokens Token流
     * @return 变量组节点
     */

    
    /**
     * @brief 解析变量引用
     * @param tokens Token流
     * @return 变量引用节点
     */

    
    /**
     * @brief 解析模板引用
     * @param tokens Token流
     * @return 模板引用节点
     */
    ASTNodePtr ParseTemplateReference(Core::TokenStream& tokens);
    
    /**
     * @brief 解析自定义引用
     * @param tokens Token流
     * @return 自定义引用节点
     */
    ASTNodePtr ParseCustomReference(Core::TokenStream& tokens);
    
    /**
     * @brief 解析特例化内容
     * @param tokens Token流
     * @return 特例化节点
     */
    ASTNodePtr ParseSpecialization(Core::TokenStream& tokens);
    
    /**
     * @brief 解析注释
     * @param tokens Token流
     * @return 注释节点
     */
    ASTNodePtr ParseComment(Core::TokenStream& tokens);
    
    /**
     * @brief 解析字面量
     * @param tokens Token流
     * @return 字面量节点
     */
    ASTNodePtr ParseLiteral(Core::TokenStream& tokens);
    
    /**
     * @brief 解析代码块内容
     * @param tokens Token流
     * @param endToken 结束Token类型
     * @return 子节点列表
     */
    ASTNodeList ParseBlockContent(Core::TokenStream& tokens, Core::TokenType endToken);
    
    /**
     * @brief 跳过空白和注释
     * @param tokens Token流
     */
    void SkipWhitespaceAndComments(Core::TokenStream& tokens);
    
    /**
     * @brief 检查并消费期望的Token
     * @param tokens Token流
     * @param expectedType 期望的Token类型
     * @param errorMessage 错误消息
     * @return 是否成功消费Token
     */
    bool ConsumeToken(Core::TokenStream& tokens, Core::TokenType expectedType, 
                      const std::string& errorMessage = "");
    
    /**
     * @brief 检查当前Token是否为指定类型
     * @param tokens Token流
     * @param tokenType Token类型
     * @return 是否匹配
     */
    bool Check(Core::TokenStream& tokens, Core::TokenType tokenType);
    
    /**
     * @brief 检查当前Token是否为指定类型列表中的任意一种
     * @param tokens Token流
     * @param tokenTypes Token类型列表
     * @return 是否匹配任意一种
     */
    bool CheckAny(Core::TokenStream& tokens, const std::vector<Core::TokenType>& tokenTypes);
    
    /**
     * @brief 报告解析错误
     * @param message 错误消息
     * @param token 错误Token
     */
    void ReportError(const std::string& message, const Core::CHTLToken& token);
    
    /**
     * @brief 报告解析警告
     * @param message 警告消息
     * @param token 警告Token
     */
    void ReportWarning(const std::string& message, const Core::CHTLToken& token);
    
    /**
     * @brief 解析字符串值（支持引号和无引号）
     * @param tokens Token流
     * @return 解析出的字符串值
     */
    std::string ParseStringValue(Core::TokenStream& tokens);
    
    /**
     * @brief 解析标识符或关键字
     * @param tokens Token流
     * @return 标识符字符串
     */
    std::string ParseIdentifier(Core::TokenStream& tokens);
    
    /**
     * @brief 解析类型标识符（如@Style, @Element等）
     * @param tokens Token流
     * @return 类型标识符字符串
     */
    std::string ParseTypeIdentifier(Core::TokenStream& tokens);
    
    /**
     * @brief 检查是否为有效的CHTL标识符
     * @param identifier 标识符
     * @return 是否有效
     */
    bool IsValidIdentifier(const std::string& identifier) const;

private:
    Core::CHTLGlobalMap& globalMap_;        // 全局映射表引用
    Core::CHTLState& stateManager_;         // 状态管理器引用
    std::stack<ASTNodePtr> nodeStack_;      // 节点栈
    bool debugMode_;                        // 调试模式
    size_t nodeCount_;                      // 节点计数
    std::string currentFileName_;           // 当前文件名
    
    // 辅助方法
    AST::ImportNode::ImportType ConvertTokenTypeToImportType(Core::TokenType tokenType);
};

/**
 * @brief AST构建器工厂类
 */
class ASTBuilderFactory {
public:
    /**
     * @brief 创建AST构建器
     * @param globalMap 全局映射表
     * @param stateManager 状态管理器
     * @return AST构建器指针
     */
    static std::unique_ptr<CHTLASTBuilder> CreateBuilder(
        Core::CHTLGlobalMap& globalMap, 
        Core::CHTLState& stateManager);
    
    /**
     * @brief 创建默认配置的AST构建器
     * @return AST构建器指针
     */
    static std::unique_ptr<CHTLASTBuilder> CreateDefaultBuilder();
};

} // namespace AST
} // namespace CHTL