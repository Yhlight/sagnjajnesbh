#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 导入节点
 * 表示 [Import] 语句
 */
class ImportNode : public ASTNode {
public:
    ImportNode() : ASTNode(ASTNodeType::IMPORT) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    enum ImportType {
        HTML,           // @Html
        STYLE,          // @Style
        JAVASCRIPT,     // @JavaScript
        CHTL,           // @Chtl
        CJMOD,          // @CJmod
        CONFIG,         // @Config
        TEMPLATE_ALL,   // [Template]
        CUSTOM_ALL,     // [Custom]
        ORIGIN_ALL,     // [Origin]
        SPECIFIC        // 特定项目
    };
    
    void SetImportType(ImportType type) { m_ImportType = type; }
    ImportType GetImportType() const { return m_ImportType; }
    
    // 导入源文件路径
    void SetFromPath(const std::string& path) { m_FromPath = path; }
    const std::string& GetFromPath() const { return m_FromPath; }
    
    // as 别名
    void SetAlias(const std::string& alias) { m_Alias = alias; }
    const std::string& GetAlias() const { return m_Alias; }
    bool HasAlias() const { return !m_Alias.empty(); }
    
    // 特定导入项
    void SetSpecificType(const std::string& type) { m_SpecificType = type; }
    const std::string& GetSpecificType() const { return m_SpecificType; }
    
    void SetSpecificName(const std::string& name) { m_SpecificName = name; }
    const std::string& GetSpecificName() const { return m_SpecificName; }
    
    // 是否是模板或自定义
    void SetIsTemplate(bool isTemplate) { m_IsTemplate = isTemplate; }
    bool IsTemplate() const { return m_IsTemplate; }
    
    void SetIsCustom(bool isCustom) { m_IsCustom = isCustom; }
    bool IsCustom() const { return m_IsCustom; }
    
    void SetIsOrigin(bool isOrigin) { m_IsOrigin = isOrigin; }
    bool IsOrigin() const { return m_IsOrigin; }
    
private:
    ImportType m_ImportType = CHTL;
    std::string m_FromPath;
    std::string m_Alias;
    std::string m_SpecificType;  // @Style, @Element, @Var等
    std::string m_SpecificName;   // 具体的名称
    bool m_IsTemplate = false;
    bool m_IsCustom = false;
    bool m_IsOrigin = false;
};

} // namespace ast
} // namespace chtl