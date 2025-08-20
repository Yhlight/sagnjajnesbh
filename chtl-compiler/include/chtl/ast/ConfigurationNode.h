#pragma once

#include "chtl/ast/ASTNode.h"

namespace chtl {
namespace ast {

/**
 * 配置节点
 * 表示 [Configuration] 块
 */
class ConfigurationNode : public ASTNode {
public:
    ConfigurationNode(const std::string& name = "")
        : ASTNode(ASTNodeType::CONFIGURATION, name) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 配置名称（可选）
    const std::string& GetConfigName() const { return m_Value; }
    bool IsNamed() const { return !m_Value.empty(); }
    
    // 配置属性
    void AddProperty(const std::string& key, const std::string& value);
    const std::vector<std::pair<std::string, std::string>>& GetProperties() const {
        return m_Properties;
    }
    std::string GetProperty(const std::string& key) const;
    bool HasProperty(const std::string& key) const;
    
    // [Name] 块
    void SetNameBlock(std::shared_ptr<class ConfigNameBlockNode> nameBlock);
    std::shared_ptr<class ConfigNameBlockNode> GetNameBlock() const { return m_NameBlock; }
    
    // [OriginType] 块
    void SetOriginTypeBlock(std::shared_ptr<class ConfigOriginTypeNode> originTypeBlock);
    std::shared_ptr<class ConfigOriginTypeNode> GetOriginTypeBlock() const { return m_OriginTypeBlock; }
    
private:
    std::vector<std::pair<std::string, std::string>> m_Properties;
    std::shared_ptr<class ConfigNameBlockNode> m_NameBlock;
    std::shared_ptr<class ConfigOriginTypeNode> m_OriginTypeBlock;
};

/**
 * 配置名称块节点
 * 表示 [Name] 块
 */
class ConfigNameBlockNode : public ASTNode {
public:
    ConfigNameBlockNode() : ASTNode(ASTNodeType::CONFIG_NAME_BLOCK) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 名称映射
    void AddNameMapping(const std::string& key, const std::string& value);
    void AddNameMapping(const std::string& key, const std::vector<std::string>& values);
    
    // 获取映射
    const std::unordered_map<std::string, std::vector<std::string>>& GetMappings() const {
        return m_Mappings;
    }
    
    std::vector<std::string> GetMapping(const std::string& key) const;
    bool HasMapping(const std::string& key) const;
    
    // 选项数量限制
    void SetOptionCount(int count) { m_OptionCount = count; }
    int GetOptionCount() const { return m_OptionCount; }
    
private:
    std::unordered_map<std::string, std::vector<std::string>> m_Mappings;
    int m_OptionCount = 3;  // 默认值
};

/**
 * 配置原始类型块节点
 * 表示 [OriginType] 块
 */
class ConfigOriginTypeNode : public ASTNode {
public:
    ConfigOriginTypeNode() : ASTNode(ASTNodeType::CONFIG_ORIGIN_TYPE) {}
    
    void Accept(ASTVisitor* visitor) override;
    
    // 原始类型定义
    void AddOriginType(const std::string& typeName, const std::string& typeValue);
    const std::vector<std::pair<std::string, std::string>>& GetOriginTypes() const {
        return m_OriginTypes;
    }
    
    std::string GetOriginType(const std::string& typeName) const;
    bool HasOriginType(const std::string& typeName) const;
    
private:
    std::vector<std::pair<std::string, std::string>> m_OriginTypes;
};

} // namespace ast
} // namespace chtl