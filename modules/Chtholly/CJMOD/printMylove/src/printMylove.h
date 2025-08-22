#pragma once

#include "CJMOD/CompleteCJMODSystem.h"

namespace CHTL {
namespace CJMOD {

/**
 * @brief printMylove扩展头文件
 * 严格按照语法文档第1472-1483行实现
 */
class PrintMyloveExtension : public CJMODExtensionInterface {
public:
    struct PrintMyloveParams {
        std::string url;                // 图片URL
        std::string mode;               // 模式：ASCII或Pixel
        std::string width;              // 宽度
        std::string height;             // 高度
        std::string scale;              // 缩放倍数
        
        bool IsValid() const;
    };
    
    // CJMODExtensionInterface实现
    std::string GetExtensionName() const override;
    std::string GetExtensionVersion() const override;
    std::vector<std::string> GetSupportedSyntaxPatterns() const override;
    bool MatchesSyntax(const std::string& syntaxPattern, const std::any& context) const override;
    CHTLJS::AST::ASTNodePtr ParseSyntax(const std::string& input, const std::any& context) const override;
    std::string GenerateJavaScript(CHTLJS::AST::ASTNodePtr ast, const std::any& context) const override;
    bool Initialize() override;
    void Cleanup() override;
    
    static PrintMyloveParams ParseParameters(const std::string& paramString);
    static std::string GeneratePrintMyloveJS(const PrintMyloveParams& params);

private:
    static std::pair<int, int> ParseDimensions(const std::string& width, const std::string& height);
    static double ParseScale(const std::string& scale);
};

} // namespace CJMOD
} // namespace CHTL