#pragma once

#include "CJMOD/CJMODApi.h"

namespace Chtholly {
namespace CJMOD {

/**
 * @brief iNeverAway扩展
 * 严格按照语法文档第1485-1520行实现
 * 使用您设计的原始CJMOD API
 */
class INeverAwayExtension : public CHTL::CJMOD::CJMODExtension {
public:
    // 使用您的CJMOD API接口
    bool Initialize(CHTL::CJMOD::CJMODScanner& scanner) override;
    std::string GetName() const override;
    std::string GetVersion() const override;
    std::vector<std::string> GetSupportedSyntax() const override;

private:
    // 全局状态管理 - 语法文档第1425行：可以编写全局变量管理状态
    static std::unordered_map<std::string, std::string> globalVirtualObjects_;
    static int globalFunctionCounter_;
    
    // 使用您的API处理iNeverAway
    void ProcessINeverAwayDefinition(const std::string& objectName, const std::string& methodBlock);
    void ProcessVirtualObjectCall(const std::string& objectName, const std::string& methodName);
    
    // 生成相关方法
    std::string GenerateGlobalFunctionName(const std::string& objectName, const std::string& methodName);
    std::string GenerateINeverAwayJS(const std::string& objectName, const CHTL::CJMOD::Syntax& syntax);
};

} // namespace CJMOD
} // namespace Chtholly