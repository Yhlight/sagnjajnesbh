#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <unordered_map>
#include "CHTL/AST/CHTLASTNodes.h"
#include "CHTL/Core/CHTLToken.h"

namespace CHTL {
namespace Test {

/**
 * @brief 测试结果结构
 */
struct TestResult {
    std::string testName;
    bool passed;
    std::string message;
    double executionTime; // 毫秒
    
    TestResult(const std::string& name, bool pass, const std::string& msg = "", double time = 0.0)
        : testName(name), passed(pass), message(msg), executionTime(time) {}
};

/**
 * @brief 测试套件
 */
class TestSuite {
private:
    std::string suiteName_;
    std::vector<TestResult> results_;
    
public:
    TestSuite(const std::string& name) : suiteName_(name) {}
    
    void AddResult(const TestResult& result) { results_.push_back(result); }
    const std::vector<TestResult>& GetResults() const { return results_; }
    const std::string& GetName() const { return suiteName_; }
    
    size_t GetPassedCount() const {
        return std::count_if(results_.begin(), results_.end(), 
                            [](const TestResult& r) { return r.passed; });
    }
    
    size_t GetTotalCount() const { return results_.size(); }
    bool AllPassed() const { return GetPassedCount() == GetTotalCount(); }
};

/**
 * @brief 实用测试工具类
 */
class UtilTest {
public:
    /**
     * @brief 运行单个测试
     * @param testName 测试名称
     * @param testFunc 测试函数
     * @return 测试结果
     */
    static TestResult RunTest(const std::string& testName, std::function<bool()> testFunc);
    
    /**
     * @brief 运行测试套件
     * @param suite 测试套件
     * @param tests 测试函数映射
     */
    static void RunTestSuite(TestSuite& suite, 
                            const std::unordered_map<std::string, std::function<bool()>>& tests);
    
    /**
     * @brief 断言相等
     */
    template<typename T>
    static bool AssertEqual(const T& expected, const T& actual, const std::string& message = "");
    
    /**
     * @brief 断言不为空
     */
    static bool AssertNotNull(const void* ptr, const std::string& message = "");
    
    /**
     * @brief 断言为真
     */
    static bool AssertTrue(bool condition, const std::string& message = "");
    
    /**
     * @brief 断言为假
     */
    static bool AssertFalse(bool condition, const std::string& message = "");
    
    /**
     * @brief 断言字符串包含
     */
    static bool AssertContains(const std::string& haystack, const std::string& needle, 
                              const std::string& message = "");
    
    /**
     * @brief 输出测试报告
     */
    static void PrintTestReport(const TestSuite& suite);
    
    /**
     * @brief 输出所有测试套件的汇总报告
     */
    static void PrintSummaryReport(const std::vector<TestSuite>& suites);
    
private:
    static double GetCurrentTimeMs();
};

/**
 * @brief AST打印工具
 */
class ASTPrint {
public:
    /**
     * @brief 打印AST节点
     * @param node AST节点
     * @param indent 缩进级别
     * @param showDetails 是否显示详细信息
     */
    static void PrintNode(const AST::ASTNodePtr& node, int indent = 0, bool showDetails = true);
    
    /**
     * @brief 打印AST树
     * @param root 根节点
     * @param title 标题
     */
    static void PrintTree(const AST::ASTNodePtr& root, const std::string& title = "AST Tree");
    
    /**
     * @brief 生成节点的简短描述
     * @param node AST节点
     * @return 节点描述
     */
    static std::string GetNodeDescription(const AST::ASTNodePtr& node);
    
    /**
     * @brief 获取节点类型名称
     * @param nodeType 节点类型
     * @return 类型名称
     */
    static std::string GetNodeTypeName(AST::NodeType nodeType);
    
    /**
     * @brief 导出AST为文本格式
     * @param root 根节点
     * @return 文本表示
     */
    static std::string ExportToText(const AST::ASTNodePtr& root);

private:
    static std::string GenerateIndent(int level);
    static void PrintNodeRecursive(const AST::ASTNodePtr& node, int indent, bool showDetails, 
                                   std::ostringstream& output);
};

/**
 * @brief AST图形化工具
 */
class ASTGraph {
public:
    /**
     * @brief 生成Graphviz DOT格式的AST图
     * @param root 根节点
     * @param title 图标题
     * @return DOT格式字符串
     */
    static std::string GenerateDOT(const AST::ASTNodePtr& root, const std::string& title = "AST");
    
    /**
     * @brief 生成Mermaid格式的AST图
     * @param root 根节点
     * @param title 图标题
     * @return Mermaid格式字符串
     */
    static std::string GenerateMermaid(const AST::ASTNodePtr& root, const std::string& title = "AST");
    
    /**
     * @brief 生成ASCII艺术格式的AST图
     * @param root 根节点
     * @return ASCII艺术字符串
     */
    static std::string GenerateASCIIArt(const AST::ASTNodePtr& root);
    
    /**
     * @brief 导出图到文件
     * @param content 图内容
     * @param filename 文件名
     * @param format 格式（dot、mermaid、ascii）
     */
    static bool ExportToFile(const std::string& content, const std::string& filename, 
                            const std::string& format);
    
    /**
     * @brief 分析AST统计信息
     * @param root 根节点
     * @return 统计信息映射
     */
    static std::unordered_map<std::string, int> AnalyzeStatistics(const AST::ASTNodePtr& root);

private:
    static std::string GenerateNodeId(const AST::ASTNodePtr& node);
    static std::string EscapeLabel(const std::string& label);
    static void GenerateDOTRecursive(const AST::ASTNodePtr& node, std::ostringstream& output, 
                                    std::unordered_map<const AST::ASTNode*, std::string>& nodeIds);
    static void GenerateMermaidRecursive(const AST::ASTNodePtr& node, std::ostringstream& output, 
                                        std::unordered_map<const AST::ASTNode*, std::string>& nodeIds, 
                                        int& idCounter);
    static void CollectStatistics(const AST::ASTNodePtr& node, 
                                 std::unordered_map<std::string, int>& stats);
};

/**
 * @brief Token打印工具
 */
class TokenPrint {
public:
    /**
     * @brief 打印单个Token
     * @param token Token对象
     * @param showPosition 是否显示位置信息
     */
    static void PrintToken(const Core::CHTLToken& token, bool showPosition = true);
    
    /**
     * @brief 打印Token列表
     * @param tokens Token列表
     * @param title 标题
     * @param showDetails 是否显示详细信息
     */
    static void PrintTokenList(const std::vector<Core::CHTLToken>& tokens, 
                              const std::string& title = "Tokens", bool showDetails = true);
    
    /**
     * @brief 生成Token的可视化表格
     * @param tokens Token列表
     * @return 表格字符串
     */
    static std::string GenerateTokenTable(const std::vector<Core::CHTLToken>& tokens);
    
    /**
     * @brief 获取Token类型名称
     * @param tokenType Token类型
     * @return 类型名称
     */
    static std::string GetTokenTypeName(Core::TokenType tokenType);
    
    /**
     * @brief 导出Token为JSON格式
     * @param tokens Token列表
     * @return JSON字符串
     */
    static std::string ExportToJSON(const std::vector<Core::CHTLToken>& tokens);
    
    /**
     * @brief 导出Token为CSV格式
     * @param tokens Token列表
     * @return CSV字符串
     */
    static std::string ExportToCSV(const std::vector<Core::CHTLToken>& tokens);
    
    /**
     * @brief 分析Token统计信息
     * @param tokens Token列表
     * @return 统计信息
     */
    static std::unordered_map<std::string, int> AnalyzeTokenStatistics(const std::vector<Core::CHTLToken>& tokens);

private:
    static std::string FormatTokenValue(const Core::CHTLToken& token);
    static std::string EscapeJSON(const std::string& str);
    static std::string EscapeCSV(const std::string& str);
};

// ============ 便捷宏定义 ============

#define CHTL_TEST(name) \
    bool test_##name()

#define CHTL_ASSERT_EQ(expected, actual) \
    if (!UtilTest::AssertEqual(expected, actual, #expected " == " #actual)) return false

#define CHTL_ASSERT_TRUE(condition) \
    if (!UtilTest::AssertTrue(condition, #condition)) return false

#define CHTL_ASSERT_FALSE(condition) \
    if (!UtilTest::AssertFalse(condition, "!(" #condition ")")) return false

#define CHTL_ASSERT_NOT_NULL(ptr) \
    if (!UtilTest::AssertNotNull(ptr, #ptr " != nullptr")) return false

#define CHTL_ASSERT_CONTAINS(haystack, needle) \
    if (!UtilTest::AssertContains(haystack, needle, #haystack " contains " #needle)) return false

// 运行测试套件的便捷宏
#define CHTL_RUN_TEST_SUITE(suite_name, ...) \
    do { \
        TestSuite suite(#suite_name); \
        std::unordered_map<std::string, std::function<bool()>> tests = __VA_ARGS__; \
        UtilTest::RunTestSuite(suite, tests); \
        UtilTest::PrintTestReport(suite); \
    } while(0)

} // namespace Test
} // namespace CHTL