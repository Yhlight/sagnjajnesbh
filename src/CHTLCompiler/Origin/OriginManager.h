#pragma once
#include "../AST/CHTLNodes.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace chtl {
namespace origin_system {

// 原始嵌入类型
enum class OriginType {
    HTML,
    STYLE,
    JAVASCRIPT,
    CUSTOM // 自定义类型如@Vue, @React等
};

// 原始嵌入项
struct OriginItem {
    OriginType type;
    std::string typeName; // @Html, @Style, @JavaScript, @Vue等
    std::string name; // 带名原始嵌入的名称
    std::string content; // 原始内容
    bool isNamed; // 是否为带名原始嵌入
};

// 自定义原始类型
struct CustomOriginType {
    std::string typeName; // @Vue, @React等
    std::string description;
    bool isEnabled;
};

// 原始嵌入管理器
class OriginManager {
public:
    OriginManager();
    ~OriginManager();
    
    // 原始嵌入管理
    void addOriginItem(const OriginItem& item);
    std::shared_ptr<OriginItem> getOriginItem(const std::string& typeName, const std::string& name = "");
    std::vector<std::shared_ptr<OriginItem>> getOriginItemsByType(OriginType type);
    std::vector<std::shared_ptr<OriginItem>> getOriginItemsByTypeName(const std::string& typeName);
    
    // 带名原始嵌入
    void addNamedOrigin(const std::string& typeName, const std::string& name, const std::string& content);
    std::string getNamedOriginContent(const std::string& typeName, const std::string& name);
    bool hasNamedOrigin(const std::string& typeName, const std::string& name) const;
    
    // 自定义原始类型管理
    void registerCustomOriginType(const std::string& typeName, const std::string& description = "");
    void unregisterCustomOriginType(const std::string& typeName);
    bool isCustomOriginTypeRegistered(const std::string& typeName) const;
    std::vector<std::string> getAllCustomOriginTypes() const;
    
    // 原始嵌入解析
    bool parseOriginBlock(const std::string& content, const std::string& typeName, const std::string& name = "");
    
    // 原始嵌入生成
    std::string generateHTMLOrigin(const std::string& name = "");
    std::string generateStyleOrigin(const std::string& name = "");
    std::string generateJavaScriptOrigin(const std::string& name = "");
    std::string generateCustomOrigin(const std::string& typeName, const std::string& name = "");
    
    // 原始嵌入使用
    bool useOriginItem(const std::string& typeName, const std::string& name = "");
    std::string getOriginContent(const std::string& typeName, const std::string& name = "");
    
    // 验证
    bool validateOriginType(const std::string& typeName);
    bool validateOriginContent(const std::string& content, const std::string& typeName);
    std::vector<std::string> getOriginErrors() const { return errors_; }
    
    // 配置集成
    void updateFromConfiguration(const std::unordered_map<std::string, std::string>& originTypeConfig);
    
    // 清理
    void clear();
    void clearErrors() { errors_.clear(); }
    
    // 统计
    size_t getOriginItemCount() const;
    size_t getCustomOriginTypeCount() const { return customOriginTypes_.size(); }

private:
    std::unordered_map<std::string, std::vector<std::shared_ptr<OriginItem>>> originItems_; // 按类型名分组
    std::unordered_map<std::string, std::shared_ptr<CustomOriginType>> customOriginTypes_;
    
    std::vector<std::string> errors_;
    
    // 内部方法
    OriginType parseOriginType(const std::string& typeName);
    std::string formatOriginTypeName(const std::string& typeName);
    bool isValidOriginTypeName(const std::string& typeName);
    bool isStandardOriginType(const std::string& typeName);
    bool isValidIdentifier(const std::string& identifier);
    
    // 内容验证
    bool validateHTMLContent(const std::string& content);
    bool validateStyleContent(const std::string& content);
    bool validateJavaScriptContent(const std::string& content);
    bool validateCustomContent(const std::string& content, const std::string& typeName);
    
    // 默认原始类型初始化
    void initializeDefaultOriginTypes();
};

// 全局原始嵌入管理器实例
extern std::shared_ptr<OriginManager> g_originManager;

} // namespace origin_system
} // namespace chtl