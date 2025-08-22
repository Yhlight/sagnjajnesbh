#pragma once

#include "CJMOD/CJMODApi.h"
#include <memory>

namespace CHTL {
namespace CJMOD {
namespace Extensions {

/**
 * @brief printMylove CJMOD扩展
 * 
 * 实现printMylove功能，将图片转换为字符像素块或ASCII形式
 * 语法文档第1472行：珂朵莉模块的printMylove功能
 */
class PrintMyloveExtension : public CJMODExtension {
public:
    /**
     * @brief 构造函数
     */
    PrintMyloveExtension();
    
    /**
     * @brief 初始化扩展
     */
    bool Initialize(CJMODScanner& scanner) override;
    
    /**
     * @brief 获取扩展名称
     */
    std::string GetName() const override { return "PrintMylove"; }
    
    /**
     * @brief 获取扩展版本
     */
    std::string GetVersion() const override { return "1.0.0"; }
    
    /**
     * @brief 获取支持的语法列表
     */
    std::vector<std::string> GetSupportedSyntax() const override;

private:
    /**
     * @brief 处理printMylove语法
     */
    void HandlePrintMylove();
    
    /**
     * @brief 生成printMylove的JavaScript实现
     * @param url 图片URL
     * @param mode 模式（ASCII或Pixel）
     * @param width 宽度
     * @param height 高度
     * @param scale 缩放倍数
     * @return 生成的JavaScript代码
     */
    std::string GeneratePrintMyloveCode(const std::string& url, 
                                       const std::string& mode,
                                       const std::string& width,
                                       const std::string& height,
                                       const std::string& scale);

private:
    std::unique_ptr<Syntax> printMyloveSyntax_;     // printMylove语法
    CJMODScanner* scanner_;                         // 扫描器指针
};

} // namespace Extensions
} // namespace CJMOD
} // namespace CHTL