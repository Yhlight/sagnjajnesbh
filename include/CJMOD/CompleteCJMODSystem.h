#pragma once

#include "CHTLJS/Core/CHTLJSToken.h"
#include "CHTLJS/AST/CHTLJSASTNodes.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <any>

// 前向声明
namespace CHTL {
namespace CMOD {
    class CompleteCMODManager;
}
}

namespace CHTL {
namespace CJMOD {

/**
 * @brief CJMOD模块信息
 * 与CMOD类似的信息格式，但专门用于CJMOD
 */
struct CJMODModuleInfo {
    std::string name;               // 模块名称
    std::string version;            // 版本号
    std::string description;        // 描述
    std::string author;             // 作者
    std::string license;            // 许可证
    std::string dependencies;       // 依赖
    std::string category;           // 分类
    std::string minCHTLVersion;     // 最小CHTL版本
    std::string maxCHTLVersion;     // 最大CHTL版本
    std::string targetCHTLJS;       // 目标CHTL JS版本
    
    bool IsValid() const;
    bool IsVersionCompatible(const std::string& currentCHTLVersion) const;
    std::vector<std::string> GetDependencies() const;
};

/**
 * @brief CJMOD扩展接口
 * 语法文档第1409行：允许通过CHTL JS编译器的一些接口使用C++自由实现CHTL JS语法
 */
class CJMODExtensionInterface {
public:
    virtual ~CJMODExtensionInterface() = default;
    
    /**
     * @brief 获取扩展名称
     */
    virtual std::string GetExtensionName() const = 0;
    
    /**
     * @brief 获取扩展版本
     */
    virtual std::string GetExtensionVersion() const = 0;
    
    /**
     * @brief 获取支持的语法模式
     * @return 语法模式列表
     */
    virtual std::vector<std::string> GetSupportedSyntaxPatterns() const = 0;
    
    /**
     * @brief 检查语法匹配
     * @param syntaxPattern 语法模式
     * @param context 上下文
     * @return 是否匹配
     */
    virtual bool MatchesSyntax(const std::string& syntaxPattern, const std::any& context) const = 0;
    
    /**
     * @brief 解析扩展语法
     * @param input 输入内容
     * @param context 解析上下文
     * @return 解析结果AST
     */
    virtual CHTLJS::AST::ASTNodePtr ParseSyntax(const std::string& input, const std::any& context) const = 0;
    
    /**
     * @brief 生成JavaScript代码
     * @param ast 扩展语法AST
     * @param context 生成上下文
     * @return 生成的JavaScript代码
     */
    virtual std::string GenerateJavaScript(CHTLJS::AST::ASTNodePtr ast, const std::any& context) const = 0;
    
    /**
     * @brief 初始化扩展
     * @return 是否初始化成功
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief 清理扩展
     */
    virtual void Cleanup() = 0;
};

/**
 * @brief CJMOD扩展注册信息
 */
struct CJMODExtensionRegistration {
    std::string extensionName;          // 扩展名称
    std::string syntaxPattern;          // 语法模式
    std::shared_ptr<CJMODExtensionInterface> extension;  // 扩展实现
    int priority;                       // 优先级
    bool isActive;                      // 是否激活
};

/**
 * @brief CJMOD语法扩展管理器
 * 语法文档第1410行：让创建无限的JS语法成为一种可能
 */
class CJMODSyntaxExtensionManager {
public:
    /**
     * @brief 注册语法扩展
     * @param registration 扩展注册信息
     * @return 是否注册成功
     */
    bool RegisterExtension(const CJMODExtensionRegistration& registration);
    
    /**
     * @brief 注销语法扩展
     * @param extensionName 扩展名称
     * @return 是否注销成功
     */
    bool UnregisterExtension(const std::string& extensionName);
    
    /**
     * @brief 查找匹配的语法扩展
     * @param syntaxPattern 语法模式
     * @param context 上下文
     * @return 匹配的扩展列表
     */
    std::vector<std::shared_ptr<CJMODExtensionInterface>> FindMatchingExtensions(
        const std::string& syntaxPattern, const std::any& context) const;
    
    /**
     * @brief 解析扩展语法
     * @param input 输入内容
     * @param context 解析上下文
     * @return 解析结果AST
     */
    CHTLJS::AST::ASTNodePtr ParseExtensionSyntax(const std::string& input, const std::any& context) const;
    
    /**
     * @brief 生成扩展JavaScript代码
     * @param ast 扩展语法AST
     * @param context 生成上下文
     * @return 生成的JavaScript代码
     */
    std::string GenerateExtensionJavaScript(CHTLJS::AST::ASTNodePtr ast, const std::any& context) const;
    
    /**
     * @brief 获取所有注册的扩展
     */
    std::vector<CJMODExtensionRegistration> GetAllExtensions() const;
    
    /**
     * @brief 清理所有扩展
     */
    void ClearAllExtensions();

private:
    std::vector<CJMODExtensionRegistration> registeredExtensions_;
    
    // 排序扩展（按优先级）
    void SortExtensionsByPriority();
};

/**
 * @brief printMylove扩展实现
 * 严格按照语法文档第1472-1483行的功能要求
 */
class PrintMyloveExtension : public CJMODExtensionInterface {
public:
    /**
     * @brief printMylove参数
     */
    struct PrintMyloveParams {
        std::string url;                // 图片URL
        std::string mode;               // 模式：ASCII或Pixel
        std::string width;              // 宽度（CSS单位、百分比、小数、纯数字像素）
        std::string height;             // 高度
        std::string scale;              // 缩放倍数（等比缩放）
        
        bool IsValid() const;
    };
    
    // CJMODExtensionInterface实现
    std::string GetExtensionName() const override { return "printMylove"; }
    std::string GetExtensionVersion() const override { return "1.0.0"; }
    std::vector<std::string> GetSupportedSyntaxPatterns() const override;
    bool MatchesSyntax(const std::string& syntaxPattern, const std::any& context) const override;
    CHTLJS::AST::ASTNodePtr ParseSyntax(const std::string& input, const std::any& context) const override;
    std::string GenerateJavaScript(CHTLJS::AST::ASTNodePtr ast, const std::any& context) const override;
    bool Initialize() override;
    void Cleanup() override;
    
    /**
     * @brief 解析printMylove参数
     * @param paramString 参数字符串
     * @return 解析的参数
     */
    static PrintMyloveParams ParseParameters(const std::string& paramString);
    
    /**
     * @brief 生成printMylove JavaScript实现
     * @param params printMylove参数
     * @return JavaScript代码
     */
    static std::string GeneratePrintMyloveJS(const PrintMyloveParams& params);

private:
    // 图片处理辅助方法
    static std::string ConvertImageToASCII(const std::string& imageUrl, int width, int height);
    static std::string ConvertImageToPixel(const std::string& imageUrl, int width, int height);
    static std::pair<int, int> ParseDimensions(const std::string& width, const std::string& height);
    static double ParseScale(const std::string& scale);
};

/**
 * @brief iNeverAway扩展实现
 * 严格按照语法文档第1485-1520行的功能要求
 */
class INeverAwayExtension : public CJMODExtensionInterface {
public:
    /**
     * @brief Void状态类型
     */
    enum class VoidStateType {
        SIMPLE_VOID,        // Void
        TYPED_VOID          // Void<A>, Void<B>
    };
    
    /**
     * @brief iNeverAway方法定义
     */
    struct INeverAwayMethod {
        std::string name;               // 方法名称（如"Void<A>"、"Ax"）
        VoidStateType stateType;        // 状态类型
        std::string stateIdentifier;    // 状态标识符（如"A"、"B"）
        std::vector<std::string> parameters;  // 参数类型（如"int, int"）
        std::string functionBody;       // 函数体
        std::string generatedFunctionName;  // 生成的全局函数名称
    };
    
    /**
     * @brief iNeverAway虚对象
     */
    struct INeverAwayVirtualObject {
        std::string objectName;         // 虚对象名称
        std::vector<INeverAwayMethod> methods;  // 方法列表
        std::unordered_map<std::string, std::string> methodNameMap;  // 方法名映射到全局函数名
    };
    
    // CJMODExtensionInterface实现
    std::string GetExtensionName() const override { return "iNeverAway"; }
    std::string GetExtensionVersion() const override { return "1.0.0"; }
    std::vector<std::string> GetSupportedSyntaxPatterns() const override;
    bool MatchesSyntax(const std::string& syntaxPattern, const std::any& context) const override;
    CHTLJS::AST::ASTNodePtr ParseSyntax(const std::string& input, const std::any& context) const override;
    std::string GenerateJavaScript(CHTLJS::AST::ASTNodePtr ast, const std::any& context) const override;
    bool Initialize() override;
    void Cleanup() override;
    
    /**
     * @brief 解析iNeverAway语法
     * @param input iNeverAway语法字符串
     * @return 解析的虚对象
     */
    static INeverAwayVirtualObject ParseINeverAwaySyntax(const std::string& input);
    
    /**
     * @brief 生成全局函数名称
     * @param objectName 虚对象名称
     * @param methodName 方法名称
     * @return 全局函数名称
     */
    static std::string GenerateGlobalFunctionName(const std::string& objectName, const std::string& methodName);
    
    /**
     * @brief 生成iNeverAway JavaScript实现
     * @param virtualObject 虚对象定义
     * @return JavaScript代码
     */
    static std::string GenerateINeverAwayJS(const INeverAwayVirtualObject& virtualObject);
    
    /**
     * @brief 处理虚对象调用
     * @param objectName 虚对象名称
     * @param methodCall 方法调用（如"Void<A>()"）
     * @return 转换后的函数调用
     */
    static std::string HandleVirtualObjectCall(const std::string& objectName, const std::string& methodCall);

private:
    // iNeverAway全局状态管理 - 语法文档第1425行：可以编写全局变量管理状态
    static std::unordered_map<std::string, INeverAwayVirtualObject> globalVirtualObjects_;
    static int globalFunctionCounter_;
    
    // 解析辅助方法
    static INeverAwayMethod ParseMethod(const std::string& methodDef);
    static VoidStateType DetermineVoidStateType(const std::string& methodName);
    static std::string ExtractStateIdentifier(const std::string& methodName);
    static std::vector<std::string> ParseParameterTypes(const std::string& paramStr);
};

/**
 * @brief CJMOD文件结构
 * 严格按照语法文档第1413-1424行的CJMOD结构
 */
struct CJMODFileStructure {
    std::string moduleName;         // 模块名称
    std::string modulePath;         // 模块路径
    
    // 主要文件路径
    std::string srcPath;            // src文件夹路径
    std::string infoPath;           // info文件夹路径
    std::string infoFile;           // 信息.chtl文件路径
    
    // C++源文件
    std::vector<std::string> cppFiles;      // .cpp文件列表
    std::vector<std::string> headerFiles;   // .h文件列表
    std::vector<std::string> subModules;    // 子模块列表
    
    // 验证结构完整性
    bool ValidateStructure() const;
    
    // 检查命名规则
    bool ValidateNamingRules() const;
    
    // 扫描CJMOD结构
    static CJMODFileStructure ScanCJMODStructure(const std::string& modulePath);
};

/**
 * @brief CJMOD模块
 * 完整的CJMOD模块表示，支持C++扩展
 */
class CJMODModule {
public:
    /**
     * @brief 构造函数
     * @param moduleStructure 模块结构
     */
    explicit CJMODModule(const CJMODFileStructure& moduleStructure);
    
    /**
     * @brief 加载模块
     * @return 是否加载成功
     */
    bool Load();
    
    /**
     * @brief 编译C++扩展
     * @return 是否编译成功
     */
    bool CompileCppExtensions();
    
    /**
     * @brief 注册扩展到CHTL JS编译器
     * @return 是否注册成功
     */
    bool RegisterExtensions();
    
    /**
     * @brief 验证模块完整性
     * @return 是否验证通过
     */
    bool Validate();
    
    /**
     * @brief 获取模块信息
     */
    const CJMODModuleInfo& GetModuleInfo() const { return moduleInfo_; }
    
    /**
     * @brief 获取模块结构
     */
    const CJMODFileStructure& GetModuleStructure() const { return moduleStructure_; }
    
    /**
     * @brief 获取注册的扩展
     */
    const std::vector<std::shared_ptr<CJMODExtensionInterface>>& GetExtensions() const { return extensions_; }
    
    /**
     * @brief 查找扩展
     * @param extensionName 扩展名称
     * @return 扩展指针，未找到返回nullptr
     */
    std::shared_ptr<CJMODExtensionInterface> FindExtension(const std::string& extensionName) const;

private:
    CJMODFileStructure moduleStructure_;    // 模块结构
    CJMODModuleInfo moduleInfo_;            // 模块信息
    std::vector<std::shared_ptr<CJMODExtensionInterface>> extensions_;  // 扩展列表
    
    // 加载辅助方法
    bool LoadModuleInfo();
    bool LoadCppSources();
    bool LoadSubModules();
    
    // 编译辅助方法
    bool CompileSingleCppFile(const std::string& cppFile);
    bool LinkExtensions();
    
    // 扩展创建
    bool CreateExtensionInstances();
    std::shared_ptr<CJMODExtensionInterface> CreateExtensionFromName(const std::string& extensionName);
};

/**
 * @brief CJMOD编译器接口
 * 语法文档第1409行：通过CHTL JS编译器的一些接口使用C++自由实现CHTL JS语法
 */
class CJMODCompilerInterface {
public:
    /**
     * @brief 注册到CHTL JS编译器
     * @param extension 扩展接口
     * @return 是否注册成功
     */
    static bool RegisterToCHTLJSCompiler(std::shared_ptr<CJMODExtensionInterface> extension);
    
    /**
     * @brief 从CHTL JS编译器注销
     * @param extensionName 扩展名称
     * @return 是否注销成功
     */
    static bool UnregisterFromCHTLJSCompiler(const std::string& extensionName);
    
    /**
     * @brief 检查扩展语法
     * @param syntaxPattern 语法模式
     * @param tokens Token流
     * @return 是否匹配扩展语法
     */
    static bool CheckExtensionSyntax(const std::string& syntaxPattern, 
                                    const std::vector<CHTLJS::Core::CHTLJSToken>& tokens);
    
    /**
     * @brief 调用扩展解析
     * @param extensionName 扩展名称
     * @param input 输入内容
     * @param context 上下文
     * @return 解析结果
     */
    static CHTLJS::AST::ASTNodePtr InvokeExtensionParsing(const std::string& extensionName,
                                                          const std::string& input,
                                                          const std::any& context);
    
    /**
     * @brief 调用扩展代码生成
     * @param extensionName 扩展名称
     * @param ast AST节点
     * @param context 上下文
     * @return 生成的代码
     */
    static std::string InvokeExtensionGeneration(const std::string& extensionName,
                                                CHTLJS::AST::ASTNodePtr ast,
                                                const std::any& context);

private:
    static std::unordered_map<std::string, std::shared_ptr<CJMODExtensionInterface>> registeredExtensions_;
};

/**
 * @brief 完整CJMOD管理器
 * 管理所有CJMOD模块和扩展
 */
class CompleteCJMODManager {
public:
    /**
     * @brief 构造函数
     * @param compilerPath 编译器路径
     * @param currentFilePath 当前文件路径
     */
    CompleteCJMODManager(const std::string& compilerPath, const std::string& currentFilePath);
    
    /**
     * @brief 析构函数
     */
    ~CompleteCJMODManager();
    
    /**
     * @brief 加载CJMOD模块
     * @param moduleName 模块名称
     * @return 是否加载成功
     */
    bool LoadCJMODModule(const std::string& moduleName);
    
    /**
     * @brief 获取CJMOD模块
     * @param moduleName 模块名称
     * @return 模块指针，未找到返回nullptr
     */
    std::shared_ptr<CJMODModule> GetCJMODModule(const std::string& moduleName) const;
    
    /**
     * @brief 搜索CJMOD模块
     * @param moduleName 模块名称
     * @return 模块路径，未找到返回空字符串
     */
    std::string SearchCJMODModule(const std::string& moduleName) const;
    
    /**
     * @brief 获取搜索路径
     */
    std::vector<std::string> GetSearchPaths() const;
    
    /**
     * @brief 编译所有C++扩展
     * @return 是否编译成功
     */
    bool CompileAllExtensions();
    
    /**
     * @brief 注册所有扩展到CHTL JS编译器
     * @return 是否注册成功
     */
    bool RegisterAllExtensions();
    
    /**
     * @brief 获取语法扩展管理器
     */
    CJMODSyntaxExtensionManager& GetSyntaxExtensionManager() { return syntaxExtensionManager_; }
    
    /**
     * @brief 清理所有CJMOD模块
     */
    void ClearAllModules();
    
    /**
     * @brief 获取统计信息
     */
    std::string GetStatistics() const;

private:
    std::string compilerPath_;          // 编译器路径
    std::string currentFilePath_;       // 当前文件路径
    std::string officialModulePath_;    // 官方模块目录
    std::string currentModulePath_;     // 当前模块目录
    std::string currentDirPath_;        // 当前目录
    
    // 已加载的CJMOD模块
    std::unordered_map<std::string, std::shared_ptr<CJMODModule>> loadedModules_;
    
    // 语法扩展管理器
    CJMODSyntaxExtensionManager syntaxExtensionManager_;
    
    // 初始化方法
    void InitializeSearchPaths();
    void InitializePredefinedExtensions();
    
    // 搜索辅助方法
    std::string SearchInOfficialModules(const std::string& moduleName) const;
    std::string SearchInCurrentModuleFolder(const std::string& moduleName) const;
    std::string SearchInCurrentDirectory(const std::string& moduleName) const;
    
    // 编译辅助方法
    bool SetupCompilerEnvironment();
    bool CompileModuleExtensions(std::shared_ptr<CJMODModule> module);
    
    // 文件系统辅助
    bool DirectoryExists(const std::string& path) const;
    bool FileExists(const std::string& path) const;
    std::vector<std::string> ListDirectory(const std::string& path) const;
    bool IsCJMODDirectory(const std::string& path) const;
};

/**
 * @brief CJMOD混合模块管理器
 * 处理语法文档第1427-1453行的CMOD + CJMOD混合模块
 */
class CJMODMixedModuleManager {
public:
    /**
     * @brief 混合模块信息
     */
    struct MixedModuleInfo {
        std::string moduleName;         // 模块名称
        std::string modulePath;         // 模块路径
        bool hasCMOD;                   // 是否有CMOD部分
        bool hasCJMOD;                  // 是否有CJMOD部分
        std::string cmodPath;           // CMOD路径
        std::string cjmodPath;          // CJMOD路径
    };
    
    /**
     * @brief 扫描混合模块
     * @param modulePath 模块路径
     * @return 混合模块信息
     */
    static MixedModuleInfo ScanMixedModule(const std::string& modulePath);
    
    /**
     * @brief 加载混合模块
     * @param moduleInfo 混合模块信息
     * @param cmodManager CMOD管理器
     * @param cjmodManager CJMOD管理器
     * @return 是否加载成功
     */
    static bool LoadMixedModule(const MixedModuleInfo& moduleInfo,
                               CHTL::CMOD::CompleteCMODManager& cmodManager,
                               CompleteCJMODManager& cjmodManager);
    
    /**
     * @brief 验证混合模块结构
     * @param moduleInfo 混合模块信息
     * @return 是否验证通过
     */
    static bool ValidateMixedModuleStructure(const MixedModuleInfo& moduleInfo);

private:
    static std::string FindCMODFolder(const std::string& modulePath);
    static std::string FindCJMODFolder(const std::string& modulePath);
};

} // namespace CJMOD
} // namespace CHTL