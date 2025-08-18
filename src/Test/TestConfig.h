#pragma once
#include <string>
#include <map>
#include <vector>

namespace chtl {
namespace test {

/**
 * 测试配置管理
 */
class TestConfig {
public:
    static TestConfig& getInstance();
    
    // 配置加载
    bool loadFromFile(const std::string& configFile);
    bool loadFromEnvironment();
    void setDefaults();
    
    // 配置访问
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::vector<std::string> getStringList(const std::string& key) const;
    
    // 配置设置
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int value);
    void setBool(const std::string& key, bool value);
    void setStringList(const std::string& key, const std::vector<std::string>& values);
    
    // 测试环境配置
    std::string getTestDataPath() const;
    std::string getTempPath() const;
    std::string getReportPath() const;
    std::string getCompilerPath() const;
    std::string getModulePath() const;
    
    // 性能配置
    int getPerformanceIterations() const;
    int getMaxParallelTests() const;
    bool isPerformanceTestingEnabled() const;
    
    // 调试配置
    bool isDebugMode() const;
    bool isVerboseMode() const;
    std::string getLogLevel() const;

private:
    std::map<std::string, std::string> config_;
    
    TestConfig() = default;
    void parseConfigLine(const std::string& line);
    std::string expandPath(const std::string& path) const;
};

/**
 * 测试环境管理
 */
class TestEnvironment {
public:
    static TestEnvironment& getInstance();
    
    // 环境初始化
    bool initialize();
    void cleanup();
    
    // 路径管理
    std::string getWorkspacePath() const;
    std::string getTestDataPath() const;
    std::string getTempPath() const;
    
    // 资源管理
    bool setupTestResources();
    bool cleanupTestResources();
    
    // 环境验证
    bool validateEnvironment();
    std::vector<std::string> checkDependencies();
    
private:
    bool initialized_;
    std::string workspacePath_;
    std::string testDataPath_;
    std::string tempPath_;
    std::vector<std::string> createdPaths_;
    
    TestEnvironment() : initialized_(false) {}
};

} // namespace test
} // namespace chtl