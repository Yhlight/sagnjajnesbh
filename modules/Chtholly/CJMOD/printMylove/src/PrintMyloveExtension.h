#pragma once

#include <string>
#include <vector>
#include <any>

namespace Chtholly {
namespace CJMOD {

/**
 * @brief printMylove扩展
 * 严格按照语法文档第1472-1483行实现
 * 通过CJMOD模块分发，不修改CHTL源码
 */
class PrintMyloveExtension {
public:
    struct PrintMyloveParams {
        std::string url;                // 图片URL
        std::string mode;               // 模式：ASCII或Pixel
        std::string width;              // 宽度
        std::string height;             // 高度
        std::string scale;              // 缩放倍数
        
        bool IsValid() const;
    };
    
    // CJMOD扩展接口
    std::string GetExtensionName() const { return "printMylove"; }
    std::string GetExtensionVersion() const { return "1.0.0"; }
    std::vector<std::string> GetSupportedSyntaxPatterns() const;
    bool MatchesSyntax(const std::string& syntaxPattern, const std::any& context) const;
    std::string ParseSyntax(const std::string& input, const std::any& context) const;
    std::string GenerateJavaScript(const PrintMyloveParams& params) const;
    bool Initialize();
    void Cleanup();
    
    static PrintMyloveParams ParseParameters(const std::string& paramString);
    static std::string GeneratePrintMyloveJS(const PrintMyloveParams& params);
};

} // namespace CJMOD
} // namespace Chtholly