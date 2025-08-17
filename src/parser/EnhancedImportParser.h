#pragma once
#include "Parser.h"
#include "../common/ImportManager.h"
#include "../common/ContextManager.h"
#include <memory>
#include <unordered_set>

namespace chtl {
namespace parser {

// 增强的导入解析器
class EnhancedImportParser : public CHTLParser {
public:
    explicit EnhancedImportParser(ContextManager& context_manager);
    ~EnhancedImportParser() = default;
    
    // 重写导入解析方法
    std::unique_ptr<ast::ImportStatementNode> parseImportStatement() override;
    
    // 解析各种导入类型
    std::unique_ptr<ast::ImportStatementNode> parseHtmlImport(const std::string& path, const std::string& alias);
    std::unique_ptr<ast::ImportStatementNode> parseStyleImport(const std::string& path, const std::string& alias);
    std::unique_ptr<ast::ImportStatementNode> parseJavaScriptImport(const std::string& path, const std::string& alias);
    std::unique_ptr<ast::ImportStatementNode> parseChtlImport(const std::string& path, const std::string& alias);
    std::unique_ptr<ast::ImportStatementNode> parseCJmodImport(const std::string& path, const std::string& alias);
    
    // 解析模板和自定义导入
    std::unique_ptr<ast::ImportStatementNode> parseTemplateImport(const std::string& type, 
                                                                 const std::string& name,
                                                                 const std::string& path, 
                                                                 const std::string& alias);
    std::unique_ptr<ast::ImportStatementNode> parseCustomImport(const std::string& type,
                                                               const std::string& name,
                                                               const std::string& path,
                                                               const std::string& alias);
    
    // 处理通配符导入
    std::vector<std::unique_ptr<ast::ImportStatementNode>> parseWildcardImport(const std::string& path, ImportType type);
    
    // 处理导入的内容
    std::unique_ptr<ast::ASTNode> processImportedContent(const ImportInfo& import_info);
    std::unique_ptr<ast::OriginBlockNode> createOriginBlockFromImport(const ImportInfo& import_info);
    
    // 验证导入语法
    bool validateImportSyntax(const std::string& import_statement) const;
    bool validateImportPath(const std::string& path, ImportType type) const;
    bool validateImportAlias(const std::string& alias) const;
    
    // 导入依赖处理
    void processImportDependencies(const std::vector<ImportInfo>& imports);
    void resolveImportOrder(std::vector<ImportInfo>& imports);
    
    // 错误处理
    void handleImportError(const std::string& error_message, const TokenPosition& position);
    void handleImportWarning(const std::string& warning_message, const TokenPosition& position);
    
    // 统计和诊断
    std::string getImportStatistics() const;
    void printImportSummary() const;
    
private:
    std::unique_ptr<ImportManager> import_manager_;
    ContextManager& context_manager_;
    
    // 导入处理状态
    std::unordered_set<std::string> processed_imports_;
    std::vector<ImportInfo> pending_imports_;
    
    // 内部辅助方法
    ImportType parseImportType(const std::string& type_token) const;
    std::string parseImportPath() const;
    std::string parseImportAlias() const;
    
    // 处理特殊导入语法
    bool isWildcardImportPath(const std::string& path) const;
    std::string normalizeImportPath(const std::string& path) const;
    
    // 导入内容处理
    std::string readImportedFileContent(const std::string& file_path) const;
    std::unique_ptr<ast::ASTNode> parseImportedChtlContent(const std::string& content, const std::string& file_path);
    
    // 循环依赖处理
    void checkCircularDependencies(const std::vector<ImportInfo>& imports);
    void reportCircularDependency(const std::vector<std::string>& dependency_chain);
    
    // 重复导入处理
    void checkDuplicateImports(const std::vector<ImportInfo>& imports);
    void reportDuplicateImport(const ImportInfo& import_info);
};

// 导入解析工厂
class ImportParserFactory {
public:
    static std::unique_ptr<EnhancedImportParser> createEnhancedParser(ContextManager& context_manager);
    static std::unique_ptr<EnhancedImportParser> createStrictParser(ContextManager& context_manager);
    static std::unique_ptr<EnhancedImportParser> createTolerantParser(ContextManager& context_manager);
};

// 导入处理助手类
class ImportProcessingHelper {
public:
    // 路径处理
    static std::string resolvePath(const std::string& base_path, const std::string& relative_path);
    static std::string canonicalizePath(const std::string& path);
    static bool isValidPath(const std::string& path);
    
    // 文件类型检测
    static ImportType detectFileType(const std::string& file_path);
    static bool isHtmlFile(const std::string& file_path);
    static bool isStyleFile(const std::string& file_path);
    static bool isJavaScriptFile(const std::string& file_path);
    static bool isChtlFile(const std::string& file_path);
    static bool isCJmodFile(const std::string& file_path);
    
    // 内容验证
    static bool validateFileContent(const std::string& file_path, ImportType expected_type);
    static std::string extractFileContent(const std::string& file_path);
    
    // 别名处理
    static bool isValidAlias(const std::string& alias);
    static std::string generateDefaultAlias(const std::string& file_path);
    static bool isReservedAlias(const std::string& alias);
};

// 便利宏定义
#define CHTL_PARSE_IMPORT(parser, type, path, alias) \
    do { \
        auto import_node = (parser).parseImportStatement(); \
        if (import_node) { \
            /* 处理导入节点 */ \
        } \
    } while(0)

#define CHTL_VALIDATE_IMPORT_SYNTAX(parser, statement) \
    ((parser).validateImportSyntax(statement))

#define CHTL_CHECK_IMPORT_DEPENDENCIES(parser, imports) \
    (parser).processImportDependencies(imports)

} // namespace parser
} // namespace chtl