#pragma once
#include "../../Common/SimpleZip/SimpleZip.h"
#include "../../Common/Core/ErrorHandler.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace chtl {
namespace cjmod {

/**
 * CJMOD子模块信息
 * 每个子模块都有自己的src和info目录
 */
struct CJMODSubmodule {
    std::string name;                           // 子模块名称
    std::string srcPath;                        // src目录路径
    std::string infoPath;                       // info目录路径
    std::vector<std::string> cppFiles;          // C++源文件列表
    std::vector<std::string> headerFiles;       // 头文件列表
    std::string infoFile;                       // info/SubmoduleName.chtl
    
    // 解析后的信息
    std::unordered_map<std::string, std::string> metadata;  // 从info文件解析的元数据
    
    CJMODSubmodule() = default;
    CJMODSubmodule(const std::string& n) : name(n) {}
};

/**
 * CJMOD主模块信息
 * 严格按照您提供的结构格式
 */
struct CJMODModule {
    std::string name;                           // CJMOD文件夹名称
    std::string rootPath;                       // 根目录路径
    std::string srcPath;                        // 主src目录
    std::string infoPath;                       // 主info目录
    
    // 主模块文件
    std::vector<std::string> mainCppFiles;      // 主src目录的cpp文件
    std::vector<std::string> mainHeaderFiles;   // 主src目录的h文件
    std::string mainInfoFile;                   // info/CJmod文件夹名称.chtl
    
    // 子模块列表
    std::vector<CJMODSubmodule> submodules;
    
    // 模块信息
    std::unordered_map<std::string, std::string> info;
    
    // 验证二同名规则：CJMOD文件夹名和info文件名相同
    bool isValidNaming() const {
        std::string folderName = std::filesystem::path(rootPath).filename().string();
        std::string infoFileName = std::filesystem::path(mainInfoFile).stem().string();
        return folderName == name && folderName == infoFileName;
    }
    
    CJMODModule() = default;
};

/**
 * CJMOD语法扩展接口
 * 更高级的语法解析方法，不仅仅依赖正则表达式
 */
class ICJMODSyntaxExtension {
public:
    virtual ~ICJMODSyntaxExtension() = default;
    
    /**
     * 获取扩展名称
     */
    virtual std::string getName() const = 0;
    
    /**
     * 获取支持的语法关键字
     */
    virtual std::vector<std::string> getSyntaxKeywords() const = 0;
    
    /**
     * 检查代码片段是否包含此扩展的语法
     * 这里可以使用更复杂的解析方法，不仅仅是正则
     */
    virtual bool containsSyntax(const std::string& codeFragment) const = 0;
    
    /**
     * 解析语法结构
     * 使用词法分析、语法树等更高级的方法
     */
    virtual bool parseSyntaxStructure(const std::string& codeFragment, 
                                     std::unordered_map<std::string, std::string>& parsedData) const = 0;
    
    /**
     * 生成JavaScript代码
     * 基于解析的结构数据生成JS代码
     */
    virtual std::string generateJavaScriptCode(const std::unordered_map<std::string, std::string>& parsedData) const = 0;
    
    /**
     * 获取依赖的头文件
     * 返回这个扩展需要的C++头文件
     */
    virtual std::vector<std::string> getRequiredHeaders() const = 0;
    
    /**
     * 获取全局变量定义
     * 返回需要的全局变量代码
     */
    virtual std::string getGlobalVariables() const = 0;
    
    /**
     * 获取初始化代码
     * 返回模块加载时需要执行的C++代码
     */
    virtual std::string getInitializationCode() const = 0;
    
    /**
     * 获取清理代码
     * 返回模块卸载时需要执行的C++代码
     */
    virtual std::string getCleanupCode() const = 0;
};

/**
 * CJMOD高级语法解析器
 * 提供比正则表达式更强大的解析能力
 */
class CJMODAdvancedParser {
public:
    /**
     * 词法分析
     * 将代码分解为token流
     */
    struct Token {
        enum Type {
            IDENTIFIER,     // 标识符
            KEYWORD,        // 关键字
            OPERATOR,       // 操作符
            LITERAL,        // 字面量
            PUNCTUATION,    // 标点符号
            WHITESPACE,     // 空白字符
            COMMENT,        // 注释
            UNKNOWN         // 未知
        };
        
        Type type;
        std::string value;
        int position;
        int line;
        int column;
        
        Token() : type(UNKNOWN), position(0), line(0), column(0) {}
        Token(Type t, const std::string& v, int pos, int l, int c) 
            : type(t), value(v), position(pos), line(l), column(c) {}
    };
    
    /**
     * 对代码进行词法分析
     */
    std::vector<Token> tokenize(const std::string& code) const;
    
    /**
     * 语法树节点
     */
    struct SyntaxNode {
        std::string type;                               // 节点类型
        std::string value;                              // 节点值
        std::vector<std::unique_ptr<SyntaxNode>> children; // 子节点
        std::unordered_map<std::string, std::string> attributes; // 属性
        
        SyntaxNode() = default;
        SyntaxNode(const std::string& t, const std::string& v) : type(t), value(v) {}
    };
    
    /**
     * 构建语法树
     */
    std::unique_ptr<SyntaxNode> buildSyntaxTree(const std::vector<Token>& tokens) const;
    
    /**
     * 查找特定模式的语法节点
     */
    std::vector<SyntaxNode*> findSyntaxPattern(SyntaxNode* root, const std::string& pattern) const;
    
    /**
     * 提取函数调用信息
     */
    struct FunctionCall {
        std::string functionName;
        std::vector<std::string> parameters;
        std::unordered_map<std::string, std::string> namedParameters;
        int startPosition;
        int endPosition;
    };
    
    std::vector<FunctionCall> extractFunctionCalls(const std::string& code) const;
    
    /**
     * 提取对象字面量
     */
    struct ObjectLiteral {
        std::unordered_map<std::string, std::string> properties;
        int startPosition;
        int endPosition;
    };
    
    std::vector<ObjectLiteral> extractObjectLiterals(const std::string& code) const;
    
    /**
     * 上下文感知解析
     * 根据代码上下文决定如何解析
     */
    bool isInFunctionContext(const std::string& code, int position) const;
    bool isInObjectContext(const std::string& code, int position) const;
    bool isInStringContext(const std::string& code, int position) const;
};

/**
 * CJMOD编译器集成接口
 * CHTL JS编译器通过这个接口调用CJMOD
 */
class CJMODCompilerInterface {
public:
    /**
     * 加载CJMOD模块
     */
    bool loadCJMODModule(const std::string& modulePath);
    
    /**
     * 卸载CJMOD模块
     */
    bool unloadCJMODModule(const std::string& moduleName);
    
    /**
     * 处理CHTL JS代码片段
     * 这是CHTL JS编译器调用的主要方法
     */
    std::string processCHTLJSFragment(const std::string& codeFragment, 
                                     const std::string& currentFile,
                                     int lineNumber,
                                     int columnNumber);
    
    /**
     * 获取所有已加载的语法扩展
     */
    std::vector<std::string> getLoadedSyntaxExtensions() const;
    
    /**
     * 获取语法提示信息
     * 用于IDE自动完成
     */
    std::vector<std::string> getSyntaxSuggestions(const std::string& partialCode) const;
    
    /**
     * 验证语法
     * 检查语法是否正确
     */
    bool validateSyntax(const std::string& codeFragment, std::vector<std::string>& errors) const;

private:
    std::vector<std::unique_ptr<ICJMODSyntaxExtension>> loadedExtensions_;
    CJMODAdvancedParser parser_;
};

/**
 * CJMOD结构管理器
 * 管理CJMOD的目录结构和文件组织
 */
class CJMODStructureManager {
public:
    /**
     * 解析CJMOD目录结构
     */
    bool parseCJMODStructure(const std::string& cjmodPath, CJMODModule& module);
    
    /**
     * 验证CJMOD结构
     * 检查是否符合标准结构
     */
    bool validateCJMODStructure(const std::string& cjmodPath);
    
    /**
     * 扫描主src目录
     */
    bool scanMainSrcDirectory(const std::string& srcPath, 
                             std::vector<std::string>& cppFiles,
                             std::vector<std::string>& headerFiles);
    
    /**
     * 扫描子模块
     */
    bool scanSubmodules(const std::string& srcPath, std::vector<CJMODSubmodule>& submodules);
    
    /**
     * 解析info文件
     */
    bool parseInfoFile(const std::string& infoFilePath, std::unordered_map<std::string, std::string>& info);
    
    /**
     * 生成CJMOD包
     */
    bool packCJMOD(const CJMODModule& module, const std::string& outputPath);
    
    /**
     * 解包CJMOD
     */
    bool unpackCJMOD(const std::string& cjmodFilePath, const std::string& outputDir);
    
    /**
     * 编译C++代码为动态库
     */
    bool compileToDynamicLibrary(const CJMODModule& module, const std::string& outputPath);

private:
    void reportError(const std::string& message);
    bool fileExists(const std::string& path);
    bool directoryExists(const std::string& path);
    std::string getFileBaseName(const std::string& path);
};

} // namespace cjmod
} // namespace chtl