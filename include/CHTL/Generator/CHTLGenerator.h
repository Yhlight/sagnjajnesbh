#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <sstream>
#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/AST/CHTLASTVisitor.h"
#include "CHTL/Core/CHTLGlobalMap.h"
#include "CMOD/CMODManager.h"

namespace CHTL {
namespace Generator {

/**
 * @brief 代码生成器配置
 */
struct GeneratorConfig {
    bool prettyPrint;               // 是否格式化输出
    bool minify;                    // 是否压缩输出
    bool includeComments;           // 是否包含注释
    bool includeGeneratorComments;  // 是否包含生成器注释
    bool validateOutput;            // 是否验证输出
    bool enableDebug;               // 是否启用调试模式
    bool autoDoctype;               // 是否自动添加DOCTYPE
    bool autoCharset;               // 是否自动添加字符集
    bool autoViewport;              // 是否自动添加视口
    int indentSize;                 // 缩进大小
    std::string indentChar;         // 缩进字符
    
    GeneratorConfig() : prettyPrint(true), minify(false), includeComments(false),
                       includeGeneratorComments(true), validateOutput(true),
                       enableDebug(false), autoDoctype(true), autoCharset(true), 
                       autoViewport(true), indentSize(2), indentChar(" ") {}
};

/**
 * @brief 生成上下文
 */
struct GenerationContext {
    std::string currentNamespace;           // 当前命名空间
    std::unordered_map<std::string, std::string> variables;  // 变量映射
    std::unordered_set<std::string> generatedClasses;       // 已生成的CSS类
    std::unordered_set<std::string> generatedIds;           // 已生成的CSS ID
    std::vector<std::string> globalStyles;                  // 全局样式
    std::vector<std::string> globalScripts;                 // 全局脚本
    std::string globalHtmlContent;                          // 全局HTML内容
    int autoClassCounter;                                   // 自动类名计数器
    int autoIdCounter;                                      // 自动ID计数器
    
    // 性能优化缓存
    std::unordered_map<std::string, std::string> symbolCache;  // 符号缓存
    std::unordered_map<std::string, std::vector<std::string>> inheritanceCache; // 继承链缓存
    
    GenerationContext() : autoClassCounter(0), autoIdCounter(0) {}
};

/**
 * @brief CHTL代码生成器类
 * 
 * 负责从CHTL AST生成HTML输出，严格按照语法文档规范
 */
class CHTLGenerator : public AST::BaseASTVisitor {
public:
    /**
     * @brief 构造函数
     * @param globalMap 全局映射表
     * @param config 生成器配置
     */
    explicit CHTLGenerator(Core::CHTLGlobalMap& globalMap, 
                          const GeneratorConfig& config = GeneratorConfig());
    
    /**
     * @brief 构造函数（带CMOD管理器）
     * @param globalMap 全局映射表
     * @param cmodManager CMOD管理器
     * @param config 生成器配置
     */
    CHTLGenerator(Core::CHTLGlobalMap& globalMap, CMOD::CMODManager& cmodManager, 
                 const GeneratorConfig& config = GeneratorConfig());
    
    /**
     * @brief 析构函数
     */
    ~CHTLGenerator() = default;
    
    /**
     * @brief 生成HTML代码
     * @param ast AST根节点
     * @return 生成的HTML代码
     */
    std::string Generate(AST::ASTNodePtr ast);
    
    /**
     * @brief 设置配置
     * @param config 新配置
     */
    void SetConfig(const GeneratorConfig& config) { config_ = config; }
    
    /**
     * @brief 获取配置
     * @return 当前配置
     */
    const GeneratorConfig& GetConfig() const { return config_; }
    
    /**
     * @brief 重置生成器状态
     */
    void Reset();
    
    /**
     * @brief 获取生成统计信息
     * @return 统计信息字符串
     */
    std::string GetStatistics() const;
    
    // AST访问者方法重写
    void VisitRootNode(AST::RootNode& node) override;
    void VisitElementNode(AST::ElementNode& node) override;
    void VisitTextNode(AST::TextNode& node) override;
    void VisitAttributeNode(AST::AttributeNode& node) override;
    void VisitStyleBlockNode(AST::StyleBlockNode& node) override;
    void VisitScriptBlockNode(AST::ScriptBlockNode& node) override;
    void VisitCSSSelectorNode(AST::CSSSelectorNode& node) override;
    void VisitCSSPropertyNode(AST::CSSPropertyNode& node) override;
    void VisitTemplateNode(AST::TemplateNode& node) override;
    void VisitCustomNode(AST::CustomNode& node) override;
    void VisitOriginNode(AST::OriginNode& node) override;
    void VisitImportNode(AST::ImportNode& node) override;
    void VisitNamespaceNode(AST::NamespaceNode& node) override;
    void VisitConfigurationNode(AST::ConfigurationNode& node) override;
    void VisitTemplateReferenceNode(AST::TemplateReferenceNode& node) override;
    void VisitCustomReferenceNode(AST::CustomReferenceNode& node) override;
    void VisitVariableReferenceNode(AST::VariableReferenceNode& node) override;
    void VisitCommentNode(AST::CommentNode& node) override;
    void VisitLiteralNode(AST::LiteralNode& node) override;
    void VisitInheritanceNode(AST::InheritanceNode& node) override;
    void VisitDeletionNode(AST::DeletionNode& node) override;
    void VisitInsertionNode(AST::InsertionNode& node) override;
    void VisitIndexAccessNode(AST::IndexAccessNode& node) override;
    void VisitConstraintNode(AST::ConstraintNode& node) override;
    void VisitVariableGroupNode(AST::VariableGroupNode& node) override;
    void VisitSpecializationNode(AST::SpecializationNode& node) override;

private:
    /**
     * @brief 生成HTML开始标签
     * @param element 元素节点
     * @return HTML开始标签
     */
    std::string GenerateStartTag(AST::ElementNode& element);
    
    /**
     * @brief 生成HTML结束标签
     * @param element 元素节点
     * @return HTML结束标签
     */
    std::string GenerateEndTag(AST::ElementNode& element);
    
    /**
     * @brief 生成元素属性
     * @param element 元素节点
     * @return 属性字符串
     */
    std::string GenerateAttributes(AST::ElementNode& element);
    
    /**
     * @brief 处理模板展开
     * @param templateRef 模板引用节点
     * @return 展开后的AST节点列表
     */
    AST::ASTNodeList ExpandTemplate(AST::TemplateReferenceNode& templateRef);
    
    /**
     * @brief 处理自定义展开
     * @param customRef 自定义引用节点
     * @return 展开后的AST节点列表
     */
    AST::ASTNodeList ExpandCustom(AST::CustomReferenceNode& customRef);
    
    /**
     * @brief 处理变量替换
     * @param varRef 变量引用节点
     * @return 替换后的值
     */
    std::string ExpandVariable(AST::VariableReferenceNode& varRef);
    
    /**
     * @brief 生成自动类名
     * @param baseName 基础名称
     * @return 自动生成的类名
     */
    std::string GenerateAutoClassName(const std::string& baseName = "");
    
    /**
     * @brief 生成自动ID
     * @param baseName 基础名称
     * @return 自动生成的ID
     */
    std::string GenerateAutoId(const std::string& baseName = "");
    
    /**
     * @brief 处理CSS选择器的全局样式添加
     * @param selector CSS选择器节点
     */
    void ProcessGlobalStyleSelector(AST::CSSSelectorNode& selector);
    
    /**
     * @brief 执行after插入操作
     * @param node 插入节点
     */
    void ExecuteAfterInsertion(AST::InsertionNode& node);
    
    /**
     * @brief 执行before插入操作
     * @param node 插入节点
     */
    void ExecuteBeforeInsertion(AST::InsertionNode& node);
    
    /**
     * @brief 执行replace插入操作
     * @param node 插入节点
     */
    void ExecuteReplaceInsertion(AST::InsertionNode& node);
    
    /**
     * @brief 执行at top插入操作
     * @param node 插入节点
     */
    void ExecuteAtTopInsertion(AST::InsertionNode& node);
    
    /**
     * @brief 执行at bottom插入操作
     * @param node 插入节点
     */
    void ExecuteAtBottomInsertion(AST::InsertionNode& node);
    
    /**
     * @brief 执行元素删除操作
     * @param node 删除节点
     */
    void ExecuteElementDeletion(AST::DeletionNode& node);
    
    /**
     * @brief 验证约束条件
     * @param node 约束节点
     * @return 是否满足约束
     */
    bool ValidateConstraints(AST::ConstraintNode& node);
    
    /**
     * @brief 应用配置设置
     * @param node 配置节点
     */
    void ApplyConfiguration(AST::ConfigurationNode& node);
    
    /**
     * @brief 处理导入节点
     * @param node 导入节点
     */
    void ProcessImport(AST::ImportNode& node);
    
    /**
     * @brief 加载导入文件
     * @param path 文件路径
     * @param importType 导入类型
     * @return 是否成功加载
     */
    bool LoadImportFile(const std::string& path, AST::ImportNode::ImportType importType);
    
    /**
     * @brief 解析导入的符号
     * @param content 文件内容
     * @param importType 导入类型
     * @param alias 别名
     */
         void ParseImportedSymbols(const std::string& content, 
                              AST::ImportNode::ImportType importType, 
                              const std::string& alias);
    
    /**
     * @brief 处理配置文件
     * @param content 配置文件内容
     */
    void ProcessConfigFile(const std::string& content);
    
    /**
     * @brief 收集全局内容
     * @param content 内容
     * @param type 内容类型
     */
         void CollectGlobalContent(const std::string& content, const std::string& type);
    
    /**
     * @brief 获取缓存的符号
     * @param symbolName 符号名称
     * @return 缓存的符号内容，如果不存在则返回空字符串
     */
    std::string GetCachedSymbol(const std::string& symbolName);
    
    /**
     * @brief 缓存符号
     * @param symbolName 符号名称
     * @param content 符号内容
     */
    void CacheSymbol(const std::string& symbolName, const std::string& content);
    
    /**
     * @brief 获取缓存的继承链
     * @param symbolName 符号名称
     * @return 继承链，如果不存在则返回空向量
     */
    std::vector<std::string> GetCachedInheritanceChain(const std::string& symbolName);
    
    /**
     * @brief 缓存继承链
     * @param symbolName 符号名称
     * @param chain 继承链
     */
    void CacheInheritanceChain(const std::string& symbolName, const std::vector<std::string>& chain);
    
    /**
     * @brief 收集全局样式
     * @param styleContent 样式内容
     */
    void CollectGlobalStyle(const std::string& styleContent);
    
    /**
     * @brief 收集全局脚本
     * @param scriptContent 脚本内容
     */
    void CollectGlobalScript(const std::string& scriptContent);
    
    /**
     * @brief 生成完整的HTML文档
     * @return 完整的HTML文档
     */
    std::string GenerateCompleteHTML();
    
    /**
     * @brief 生成HTML头部
     * @return HTML头部
     */
    std::string GenerateHTMLHead();
    
    /**
     * @brief 生成全局样式块
     * @return 全局样式块
     */
    std::string GenerateGlobalStyles();
    
    /**
     * @brief 生成全局脚本块
     * @return 全局脚本块
     */
    std::string GenerateGlobalScripts();
    
    /**
     * @brief 添加缩进
     */
    void AddIndent();
    
    /**
     * @brief 减少缩进
     */
    void RemoveIndent();
    
    /**
     * @brief 获取当前缩进字符串
     * @return 缩进字符串
     */
    std::string GetIndent() const;
    
    /**
     * @brief 转义HTML特殊字符
     * @param text 原始文本
     * @return 转义后的文本
     */
    std::string EscapeHTML(const std::string& text);
    
    /**
     * @brief 转义CSS特殊字符
     * @param css 原始CSS
     * @return 转义后的CSS
     */
    std::string EscapeCSS(const std::string& css);
    
    /**
     * @brief 转义JavaScript特殊字符
     * @param js 原始JavaScript
     * @return 转义后的JavaScript
     */
    std::string EscapeJavaScript(const std::string& js);
    
    /**
     * @brief 压缩HTML输出
     * @param html 原始HTML
     * @return 压缩后的HTML
     */
    std::string MinifyHTML(const std::string& html);
    
    /**
     * @brief 验证生成的HTML
     * @param html 生成的HTML
     * @return 验证是否通过
     */
    bool ValidateHTML(const std::string& html);
    
    /**
     * @brief 格式化HTML输出
     * @param html 原始HTML
     * @return 格式化后的HTML
     */
    std::string FormatHTML(const std::string& html);

private:
    GeneratorConfig config_;                // 生成器配置
    Core::CHTLGlobalMap& globalMap_;        // 全局映射表引用
    CMOD::CMODManager* cmodManager_;        // CMOD管理器指针（可选）
    GenerationContext context_;             // 生成上下文
    std::ostringstream output_;             // 输出流
    int currentIndent_;                     // 当前缩进级别
    
    // 统计信息
    size_t elementCount_;                   // 元素计数
    size_t templateExpandCount_;            // 模板展开计数
    size_t customExpandCount_;              // 自定义展开计数
    size_t variableSubstitutionCount_;      // 变量替换计数
};

/**
 * @brief 代码生成器工厂类
 */
class GeneratorFactory {
public:
    /**
     * @brief 创建默认代码生成器
     * @param globalMap 全局映射表
     * @return 代码生成器指针
     */
    static std::unique_ptr<CHTLGenerator> CreateDefaultGenerator(Core::CHTLGlobalMap& globalMap);
    
    /**
     * @brief 创建压缩模式代码生成器
     * @param globalMap 全局映射表
     * @return 代码生成器指针
     */
    static std::unique_ptr<CHTLGenerator> CreateMinifyGenerator(Core::CHTLGlobalMap& globalMap);
    
    /**
     * @brief 创建调试模式代码生成器
     * @param globalMap 全局映射表
     * @return 代码生成器指针
     */
    static std::unique_ptr<CHTLGenerator> CreateDebugGenerator(Core::CHTLGlobalMap& globalMap);
};

} // namespace Generator
} // namespace CHTL