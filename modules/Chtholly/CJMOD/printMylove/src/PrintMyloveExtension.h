#pragma once

#include "CJMOD/CJMODApi.h"

namespace Chtholly {
namespace CJMOD {

/**
 * @brief printMylove扩展
 * 严格按照语法文档第1472-1483行实现
 * 使用您设计的原始CJMOD API
 */
class PrintMyloveExtension : public CHTL::CJMOD::CJMODExtension {
public:
    // 使用您的CJMOD API接口
    bool Initialize(CHTL::CJMOD::CJMODScanner& scanner) override;
    std::string GetName() const override;
    std::string GetVersion() const override;
    std::vector<std::string> GetSupportedSyntax() const override;

private:
    // 使用您的API处理printMylove调用
    void ProcessPrintMyloveCall(const std::string& paramBlock);
    
    // 提取并匹配参数
    void ExtractAndMatchParams(const std::string& paramBlock, CHTL::CJMOD::Syntax& syntax);
    
    // 生成JavaScript代码
    std::string GeneratePrintMyloveJS(const CHTL::CJMOD::Syntax& syntax);
};

} // namespace CJMOD
} // namespace Chtholly