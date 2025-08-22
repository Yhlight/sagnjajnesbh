#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <any>

namespace Chtholly {
namespace CJMOD {

/**
 * @brief iNeverAway扩展
 * 严格按照语法文档第1485-1520行实现
 * 通过CJMOD模块分发，不修改CHTL源码
 */
class INeverAwayExtension {
public:
    enum class VoidStateType {
        SIMPLE_VOID,        // Void
        TYPED_VOID          // Void<A>, Void<B>
    };
    
    struct INeverAwayMethod {
        std::string name;               // 方法名称
        VoidStateType stateType;        // 状态类型
        std::string stateIdentifier;    // 状态标识符
        std::string generatedFunctionName;  // 生成的全局函数名称
    };
    
    struct INeverAwayVirtualObject {
        std::string objectName;         // 虚对象名称
        std::vector<INeverAwayMethod> methods;  // 方法列表
        std::unordered_map<std::string, std::string> methodNameMap;  // 方法名映射
    };
    
    // CJMOD扩展接口
    std::string GetExtensionName() const { return "iNeverAway"; }
    std::string GetExtensionVersion() const { return "1.0.0"; }
    std::vector<std::string> GetSupportedSyntaxPatterns() const;
    bool MatchesSyntax(const std::string& syntaxPattern, const std::any& context) const;
    std::string ParseSyntax(const std::string& input, const std::any& context) const;
    std::string GenerateJavaScript(const INeverAwayVirtualObject& virtualObject) const;
    bool Initialize();
    void Cleanup();
    
    static INeverAwayVirtualObject ParseINeverAwaySyntax(const std::string& input);
    static std::string GenerateGlobalFunctionName(const std::string& objectName, const std::string& methodName);
    static std::string GenerateINeverAwayJS(const INeverAwayVirtualObject& virtualObject);
    static std::string HandleVirtualObjectCall(const std::string& input);

private:
    // 全局状态管理
    static std::unordered_map<std::string, INeverAwayVirtualObject> globalVirtualObjects_;
    static int globalFunctionCounter_;
    
    static VoidStateType DetermineVoidStateType(const std::string& methodName);
    static std::string ExtractStateIdentifier(const std::string& methodName);
};

} // namespace CJMOD
} // namespace Chtholly