#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include "CHTL/AST/CHTLASTNodes.h"

namespace CHTL {
namespace Core {

/**
 * @brief 导入搜索结果
 */
struct ImportSearchResult {
    std::vector<std::string> foundPaths;    // 找到的文件路径
    bool success;                           // 搜索是否成功
    std::string errorMessage;               // 错误信息
    
    ImportSearchResult() : success(false) {}
};

/**
 * @brief 导入路径解析结果
 */
struct ImportPathInfo {
    std::string originalPath;               // 原始路径
    std::string resolvedPath;               // 解析后的完整路径
    std::string fileName;                   // 文件名
    std::string extension;                  // 扩展名
    bool isWildcard;                        // 是否为通配符
    bool isDirectory;                       // 是否为目录
    
    ImportPathInfo() : isWildcard(false), isDirectory(false) {}
};

/**
 * @brief 导入管理器
 * 严格按照目标规划.ini第68-105行实现增强导入功能
 */
class ImportManager {
public:
    ImportManager();
    
    /**
     * @brief 设置当前编译文件的目录
     * @param currentDir 当前目录
     */
    void SetCurrentDirectory(const std::string& currentDir);
    
    /**
     * @brief 设置官方模块目录
     * @param moduleDir 官方模块目录
     */
    void SetOfficialModuleDirectory(const std::string& moduleDir);
    
    /**
     * @brief 解析导入路径
     * @param importPath 导入路径
     * @param importType 导入类型
     * @return 搜索结果
     */
    ImportSearchResult ResolveImportPath(const std::string& importPath, AST::ImportNode::ImportType importType);
    
    /**
     * @brief 检查循环依赖
     * @param fromFile 源文件
     * @param toFile 目标文件
     * @return 是否存在循环依赖
     */
    bool CheckCircularDependency(const std::string& fromFile, const std::string& toFile);
    
    /**
     * @brief 检查重复导入
     * @param fromFile 源文件
     * @param importPath 导入路径
     * @param importType 导入类型
     * @return 是否为重复导入
     */
    bool CheckDuplicateImport(const std::string& fromFile, const std::string& importPath, AST::ImportNode::ImportType importType);
    
    /**
     * @brief 注册导入关系
     * @param fromFile 源文件
     * @param toFile 目标文件
     * @param importType 导入类型
     */
    void RegisterImport(const std::string& fromFile, const std::string& toFile, AST::ImportNode::ImportType importType);
    
    /**
     * @brief 清理导入状态
     */
    void Clear();

private:
    std::string currentDir_;                // 当前编译文件目录
    std::string officialModuleDir_;         // 官方模块目录
    
    // 导入依赖图
    std::unordered_map<std::string, std::unordered_set<std::string>> importGraph_;
    
    // 重复导入检查
    std::unordered_map<std::string, std::unordered_set<std::string>> importHistory_;
    
    /**
     * @brief 解析路径信息
     * @param path 原始路径
     * @return 路径信息
     */
    ImportPathInfo ParsePathInfo(const std::string& path);
    
    /**
     * @brief 搜索HTML/CSS/JS文件
     * @param pathInfo 路径信息
     * @param importType 导入类型
     * @return 搜索结果
     */
    ImportSearchResult SearchMediaFiles(const ImportPathInfo& pathInfo, AST::ImportNode::ImportType importType);
    
    /**
     * @brief 搜索CHTL文件
     * @param pathInfo 路径信息
     * @return 搜索结果
     */
    ImportSearchResult SearchCHTLFiles(const ImportPathInfo& pathInfo);
    
    /**
     * @brief 搜索CJMOD文件
     * @param pathInfo 路径信息
     * @return 搜索结果
     */
    ImportSearchResult SearchCJMODFiles(const ImportPathInfo& pathInfo);
    
    /**
     * @brief 搜索原始嵌入文件
     * @param pathInfo 路径信息
     * @param importType 导入类型
     * @return 搜索结果
     */
    ImportSearchResult SearchOriginFiles(const ImportPathInfo& pathInfo, AST::ImportNode::ImportType importType);
    
    /**
     * @brief 在指定目录搜索文件
     * @param directory 目录
     * @param pattern 文件模式
     * @return 找到的文件列表
     */
    std::vector<std::string> SearchFilesInDirectory(const std::string& directory, const std::string& pattern);
    
    /**
     * @brief 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    bool FileExists(const std::string& filePath);
    
    /**
     * @brief 检查是否为目录
     * @param path 路径
     * @return 是否为目录
     */
    bool IsDirectory(const std::string& path);
    
    /**
     * @brief 获取文件扩展名
     * @param fileName 文件名
     * @return 扩展名
     */
    std::string GetFileExtension(const std::string& fileName);
    
    /**
     * @brief 标准化路径
     * @param path 原始路径
     * @return 标准化后的路径
     */
    std::string NormalizePath(const std::string& path);
    
    /**
     * @brief DFS检查循环依赖
     * @param current 当前文件
     * @param target 目标文件
     * @param visited 已访问文件
     * @param path 当前路径
     * @return 是否存在循环
     */
    bool DFSCheckCircular(const std::string& current, const std::string& target, 
                         std::unordered_set<std::string>& visited, 
                         std::unordered_set<std::string>& path);
};

} // namespace Core
} // namespace CHTL