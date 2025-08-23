#include "CHTL/Test/CHTLTestFramework.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace CHTL {
namespace Test {

// ============ UtilTest实现 ============

TestResult UtilTest::RunTest(const std::string& testName, std::function<bool()> testFunc) {
    double startTime = GetCurrentTimeMs();
    
    try {
        bool result = testFunc();
        double endTime = GetCurrentTimeMs();
        double executionTime = endTime - startTime;
        
        if (result) {
            return TestResult(testName, true, "PASSED", executionTime);
        } else {
            return TestResult(testName, false, "FAILED", executionTime);
        }
    } catch (const std::exception& e) {
        double endTime = GetCurrentTimeMs();
        double executionTime = endTime - startTime;
        return TestResult(testName, false, "EXCEPTION: " + std::string(e.what()), executionTime);
    } catch (...) {
        double endTime = GetCurrentTimeMs();
        double executionTime = endTime - startTime;
        return TestResult(testName, false, "UNKNOWN EXCEPTION", executionTime);
    }
}

void UtilTest::RunTestSuite(TestSuite& suite, 
                           const std::unordered_map<std::string, std::function<bool()>>& tests) {
    for (const auto& [testName, testFunc] : tests) {
        TestResult result = RunTest(testName, testFunc);
        suite.AddResult(result);
    }
}

template<typename T>
bool UtilTest::AssertEqual(const T& expected, const T& actual, const std::string& message) {
    if (expected == actual) {
        return true;
    } else {
        std::ostringstream oss;
        oss << "AssertEqual failed: " << message << " (Expected: " << expected 
            << ", Actual: " << actual << ")";
        std::cout << "[ASSERT] " << oss.str() << std::endl;
        return false;
    }
}

// 显式实例化常用类型
template bool UtilTest::AssertEqual<int>(const int&, const int&, const std::string&);
template bool UtilTest::AssertEqual<std::string>(const std::string&, const std::string&, const std::string&);
template bool UtilTest::AssertEqual<bool>(const bool&, const bool&, const std::string&);
template bool UtilTest::AssertEqual<double>(const double&, const double&, const std::string&);

bool UtilTest::AssertNotNull(const void* ptr, const std::string& message) {
    if (ptr != nullptr) {
        return true;
    } else {
        std::cout << "[ASSERT] AssertNotNull failed: " << message << std::endl;
        return false;
    }
}

bool UtilTest::AssertTrue(bool condition, const std::string& message) {
    if (condition) {
        return true;
    } else {
        std::cout << "[ASSERT] AssertTrue failed: " << message << std::endl;
        return false;
    }
}

bool UtilTest::AssertFalse(bool condition, const std::string& message) {
    if (!condition) {
        return true;
    } else {
        std::cout << "[ASSERT] AssertFalse failed: " << message << std::endl;
        return false;
    }
}

bool UtilTest::AssertContains(const std::string& haystack, const std::string& needle, 
                             const std::string& message) {
    if (haystack.find(needle) != std::string::npos) {
        return true;
    } else {
        std::cout << "[ASSERT] AssertContains failed: " << message 
                  << " ('" << needle << "' not found in '" << haystack << "')" << std::endl;
        return false;
    }
}

void UtilTest::PrintTestReport(const TestSuite& suite) {
    std::cout << "\n==================== 测试报告 ====================" << std::endl;
    std::cout << "测试套件: " << suite.GetName() << std::endl;
    std::cout << "总测试数: " << suite.GetTotalCount() << std::endl;
    std::cout << "通过数量: " << suite.GetPassedCount() << std::endl;
    std::cout << "失败数量: " << (suite.GetTotalCount() - suite.GetPassedCount()) << std::endl;
    std::cout << "成功率: " << std::fixed << std::setprecision(2) 
              << (100.0 * suite.GetPassedCount() / suite.GetTotalCount()) << "%" << std::endl;
    std::cout << "=================================================" << std::endl;
    
    for (const auto& result : suite.GetResults()) {
        std::string status = result.passed ? "[PASS]" : "[FAIL]";
        std::cout << status << " " << result.testName 
                  << " (" << std::fixed << std::setprecision(2) << result.executionTime << "ms)";
        
        if (!result.passed) {
            std::cout << " - " << result.message;
        }
        std::cout << std::endl;
    }
    
    std::cout << "=================================================" << std::endl;
}

void UtilTest::PrintSummaryReport(const std::vector<TestSuite>& suites) {
    std::cout << "\n================== 汇总测试报告 ==================" << std::endl;
    
    size_t totalTests = 0;
    size_t totalPassed = 0;
    
    for (const auto& suite : suites) {
        totalTests += suite.GetTotalCount();
        totalPassed += suite.GetPassedCount();
        
        std::string status = suite.AllPassed() ? "[PASS]" : "[FAIL]";
        std::cout << status << " " << suite.GetName() 
                  << " (" << suite.GetPassedCount() << "/" << suite.GetTotalCount() << ")" << std::endl;
    }
    
    std::cout << "=================================================" << std::endl;
    std::cout << "总计 " << suites.size() << " 个测试套件" << std::endl;
    std::cout << "总计 " << totalTests << " 个测试用例" << std::endl;
    std::cout << "通过 " << totalPassed << " 个，失败 " << (totalTests - totalPassed) << " 个" << std::endl;
    std::cout << "整体成功率: " << std::fixed << std::setprecision(2) 
              << (100.0 * totalPassed / totalTests) << "%" << std::endl;
    std::cout << "=================================================" << std::endl;
}

double UtilTest::GetCurrentTimeMs() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count() / 1000.0;
}

// ============ ASTPrint实现 ============

void ASTPrint::PrintNode(const AST::ASTNodePtr& node, int indent, bool showDetails) {
    if (!node) {
        std::cout << GenerateIndent(indent) << "[NULL]" << std::endl;
        return;
    }
    
    std::string nodeDesc = GetNodeDescription(node);
    std::cout << GenerateIndent(indent) << nodeDesc << std::endl;
    
    if (showDetails) {
        // 显示节点的详细信息
        std::cout << GenerateIndent(indent + 1) << "Type: " 
                  << GetNodeTypeName(node->GetNodeType()) << std::endl;
        std::cout << GenerateIndent(indent + 1) << "Address: " 
                  << node.get() << std::endl;
    }
    
    // 递归打印子节点
    const auto& children = node->GetChildren();
    for (const auto& child : children) {
        PrintNode(child, indent + 1, showDetails);
    }
}

void ASTPrint::PrintTree(const AST::ASTNodePtr& root, const std::string& title) {
    std::cout << "\n==================== " << title << " ====================" << std::endl;
    PrintNode(root, 0, true);
    std::cout << "==========================================" << std::endl;
}

std::string ASTPrint::GetNodeDescription(const AST::ASTNodePtr& node) {
    if (!node) return "[NULL]";
    
    std::string typeName = GetNodeTypeName(node->GetNodeType());
    std::string description = "[" + typeName + "]";
    
    // 根据节点类型添加具体信息
    switch (node->GetNodeType()) {
        case AST::NodeType::ELEMENT: {
            auto elementNode = std::dynamic_pointer_cast<AST::ElementNode>(node);
            if (elementNode) {
                description += " <" + elementNode->GetTagName() + ">";
            }
            break;
        }
        case AST::NodeType::TEXT: {
            auto textNode = std::dynamic_pointer_cast<AST::TextNode>(node);
            if (textNode) {
                std::string content = textNode->GetContent();
                if (content.length() > 30) {
                    content = content.substr(0, 27) + "...";
                }
                description += " \"" + content + "\"";
            }
            break;
        }
        case AST::NodeType::TEMPLATE_STYLE:
        case AST::NodeType::TEMPLATE_ELEMENT:
        case AST::NodeType::TEMPLATE_VAR: {
            auto templateNode = std::dynamic_pointer_cast<AST::TemplateNode>(node);
            if (templateNode) {
                description += " @" + templateNode->GetName();
            }
            break;
        }
        case AST::NodeType::CUSTOM_STYLE:
        case AST::NodeType::CUSTOM_ELEMENT:
        case AST::NodeType::CUSTOM_VAR: {
            auto customNode = std::dynamic_pointer_cast<AST::CustomNode>(node);
            if (customNode) {
                description += " @" + customNode->GetName();
            }
            break;
        }
        case AST::NodeType::ORIGIN: {
            auto originNode = std::dynamic_pointer_cast<AST::OriginNode>(node);
            if (originNode) {
                description += " @" + (originNode->IsNamed() ? originNode->GetName() : "Unnamed");
            }
            break;
        }
        default:
            break;
    }
    
    return description;
}

std::string ASTPrint::GetNodeTypeName(AST::NodeType nodeType) {
    switch (nodeType) {
        case AST::NodeType::ROOT: return "ROOT";
        case AST::NodeType::ELEMENT: return "ELEMENT";
        case AST::NodeType::TEXT: return "TEXT";
        case AST::NodeType::ATTRIBUTE: return "ATTRIBUTE";
        case AST::NodeType::COMMENT: return "COMMENT";
        case AST::NodeType::STYLE_BLOCK: return "STYLE_BLOCK";
        case AST::NodeType::SCRIPT_BLOCK: return "SCRIPT_BLOCK";
        case AST::NodeType::CSS_SELECTOR: return "CSS_SELECTOR";
        case AST::NodeType::CSS_PROPERTY: return "CSS_PROPERTY";
        case AST::NodeType::TEMPLATE_STYLE: return "TEMPLATE_STYLE";
        case AST::NodeType::TEMPLATE_ELEMENT: return "TEMPLATE_ELEMENT";
        case AST::NodeType::TEMPLATE_VAR: return "TEMPLATE_VAR";
        case AST::NodeType::CUSTOM_STYLE: return "CUSTOM_STYLE";
        case AST::NodeType::CUSTOM_ELEMENT: return "CUSTOM_ELEMENT";
        case AST::NodeType::CUSTOM_VAR: return "CUSTOM_VAR";
        case AST::NodeType::ORIGIN: return "ORIGIN";
        case AST::NodeType::IMPORT: return "IMPORT";
        case AST::NodeType::NAMESPACE: return "NAMESPACE";
        case AST::NodeType::CONFIGURATION: return "CONFIGURATION";
        default: return "UNKNOWN";
    }
}

std::string ASTPrint::ExportToText(const AST::ASTNodePtr& root) {
    std::ostringstream output;
    PrintNodeRecursive(root, 0, true, output);
    return output.str();
}

std::string ASTPrint::GenerateIndent(int level) {
    return std::string(level * 2, ' ');
}

void ASTPrint::PrintNodeRecursive(const AST::ASTNodePtr& node, int indent, bool showDetails, 
                                  std::ostringstream& output) {
    if (!node) {
        output << GenerateIndent(indent) << "[NULL]\n";
        return;
    }
    
    output << GenerateIndent(indent) << GetNodeDescription(node) << "\n";
    
    const auto& children = node->GetChildren();
    for (const auto& child : children) {
        PrintNodeRecursive(child, indent + 1, showDetails, output);
    }
}

// ============ ASTGraph实现 ============

std::string ASTGraph::GenerateDOT(const AST::ASTNodePtr& root, const std::string& title) {
    std::ostringstream output;
    std::unordered_map<const AST::ASTNode*, std::string> nodeIds;
    
    output << "digraph \"" << title << "\" {\n";
    output << "    rankdir=TB;\n";
    output << "    node [shape=box, style=rounded];\n";
    output << "    edge [color=blue];\n\n";
    
    GenerateDOTRecursive(root, output, nodeIds);
    
    output << "}\n";
    return output.str();
}

std::string ASTGraph::GenerateMermaid(const AST::ASTNodePtr& root, const std::string& title) {
    std::ostringstream output;
    std::unordered_map<const AST::ASTNode*, std::string> nodeIds;
    int idCounter = 0;
    
    output << "graph TD\n";
    GenerateMermaidRecursive(root, output, nodeIds, idCounter);
    
    return output.str();
}

std::string ASTGraph::GenerateASCIIArt(const AST::ASTNodePtr& root) {
    // 简化的ASCII艺术实现
    std::ostringstream output;
    output << "AST ASCII Art:\n";
    output << ASTPrint::ExportToText(root);
    return output.str();
}

bool ASTGraph::ExportToFile(const std::string& content, const std::string& filename, 
                           const std::string& format) {
    std::string fullFilename = filename + "." + format;
    std::ofstream file(fullFilename);
    
    if (!file.is_open()) {
        std::cerr << "无法创建文件: " << fullFilename << std::endl;
        return false;
    }
    
    file << content;
    file.close();
    
    std::cout << "AST图已导出到: " << fullFilename << std::endl;
    return true;
}

std::unordered_map<std::string, int> ASTGraph::AnalyzeStatistics(const AST::ASTNodePtr& root) {
    std::unordered_map<std::string, int> stats;
    CollectStatistics(root, stats);
    return stats;
}

std::string ASTGraph::GenerateNodeId(const AST::ASTNodePtr& node) {
    std::ostringstream oss;
    oss << "node_" << reinterpret_cast<uintptr_t>(node.get());
    return oss.str();
}

std::string ASTGraph::EscapeLabel(const std::string& label) {
    std::string escaped = label;
    std::replace(escaped.begin(), escaped.end(), '"', '\'');
    return escaped;
}

void ASTGraph::GenerateDOTRecursive(const AST::ASTNodePtr& node, std::ostringstream& output, 
                                   std::unordered_map<const AST::ASTNode*, std::string>& nodeIds) {
    if (!node) return;
    
    std::string nodeId = GenerateNodeId(node);
    nodeIds[node.get()] = nodeId;
    
    std::string label = EscapeLabel(ASTPrint::GetNodeDescription(node));
    output << "    " << nodeId << " [label=\"" << label << "\"];\n";
    
    const auto& children = node->GetChildren();
    for (const auto& child : children) {
        if (child) {
            GenerateDOTRecursive(child, output, nodeIds);
            std::string childId = GenerateNodeId(child);
            output << "    " << nodeId << " -> " << childId << ";\n";
        }
    }
}

void ASTGraph::GenerateMermaidRecursive(const AST::ASTNodePtr& node, std::ostringstream& output, 
                                       std::unordered_map<const AST::ASTNode*, std::string>& nodeIds, 
                                       int& idCounter) {
    if (!node) return;
    
    std::string nodeId = "node" + std::to_string(idCounter++);
    nodeIds[node.get()] = nodeId;
    
    std::string label = EscapeLabel(ASTPrint::GetNodeDescription(node));
    output << "    " << nodeId << "[\"" << label << "\"]\n";
    
    const auto& children = node->GetChildren();
    for (const auto& child : children) {
        if (child) {
            GenerateMermaidRecursive(child, output, nodeIds, idCounter);
            std::string childId = nodeIds[child.get()];
            output << "    " << nodeId << " --> " << childId << "\n";
        }
    }
}

void ASTGraph::CollectStatistics(const AST::ASTNodePtr& node, 
                                std::unordered_map<std::string, int>& stats) {
    if (!node) return;
    
    std::string typeName = ASTPrint::GetNodeTypeName(node->GetNodeType());
    stats[typeName]++;
    stats["Total"]++;
    
    const auto& children = node->GetChildren();
    for (const auto& child : children) {
        CollectStatistics(child, stats);
    }
}

// ============ TokenPrint实现 ============

void TokenPrint::PrintToken(const Core::CHTLToken& token, bool showPosition) {
    std::string typeName = GetTokenTypeName(token.GetType());
    std::string value = FormatTokenValue(token);
    
    std::cout << "[" << typeName << "] " << value;
    
    if (showPosition) {
        std::cout << " (Line: " << token.GetLine() 
                  << ", Col: " << token.GetColumn() << ")";
    }
    
    std::cout << std::endl;
}

void TokenPrint::PrintTokenList(const std::vector<Core::CHTLToken>& tokens, 
                               const std::string& title, bool showDetails) {
    std::cout << "\n==================== " << title << " ====================" << std::endl;
    std::cout << "Token数量: " << tokens.size() << std::endl;
    std::cout << "=============================================" << std::endl;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        std::cout << std::setw(4) << i << ": ";
        PrintToken(tokens[i], showDetails);
    }
    
    std::cout << "=============================================" << std::endl;
}

std::string TokenPrint::GenerateTokenTable(const std::vector<Core::CHTLToken>& tokens) {
    std::ostringstream output;
    
    output << "| Index | Type | Value | Line | Column |\n";
    output << "|-------|------|-------|------|--------|\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        output << "| " << std::setw(5) << i << " | "
               << std::setw(12) << GetTokenTypeName(token.GetType()) << " | "
               << std::setw(20) << FormatTokenValue(token) << " | "
               << std::setw(4) << token.GetLine() << " | "
               << std::setw(6) << token.GetColumn() << " |\n";
    }
    
    return output.str();
}

std::string TokenPrint::GetTokenTypeName(Core::TokenType tokenType) {
    // 这里需要根据实际的TokenType枚举来实现
    // 简化实现
    return "TOKEN_" + std::to_string(static_cast<int>(tokenType));
}

std::string TokenPrint::ExportToJSON(const std::vector<Core::CHTLToken>& tokens) {
    std::ostringstream output;
    output << "{\n  \"tokens\": [\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        output << "    {\n";
        output << "      \"index\": " << i << ",\n";
        output << "      \"type\": \"" << GetTokenTypeName(token.GetType()) << "\",\n";
        output << "      \"value\": \"" << EscapeJSON(FormatTokenValue(token)) << "\",\n";
        output << "      \"line\": " << token.GetLine() << ",\n";
        output << "      \"column\": " << token.GetColumn() << "\n";
        output << "    }";
        
        if (i < tokens.size() - 1) {
            output << ",";
        }
        output << "\n";
    }
    
    output << "  ]\n}";
    return output.str();
}

std::string TokenPrint::ExportToCSV(const std::vector<Core::CHTLToken>& tokens) {
    std::ostringstream output;
    output << "Index,Type,Value,Line,Column\n";
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const auto& token = tokens[i];
        output << i << ","
               << GetTokenTypeName(token.GetType()) << ","
               << EscapeCSV(FormatTokenValue(token)) << ","
               << token.GetLine() << ","
               << token.GetColumn() << "\n";
    }
    
    return output.str();
}

std::unordered_map<std::string, int> TokenPrint::AnalyzeTokenStatistics(const std::vector<Core::CHTLToken>& tokens) {
    std::unordered_map<std::string, int> stats;
    
    for (const auto& token : tokens) {
        std::string typeName = GetTokenTypeName(token.GetType());
        stats[typeName]++;
        stats["Total"]++;
    }
    
    return stats;
}

std::string TokenPrint::FormatTokenValue(const Core::CHTLToken& token) {
    std::string value = token.GetValue();
    if (value.length() > 50) {
        return value.substr(0, 47) + "...";
    }
    return value;
}

std::string TokenPrint::EscapeJSON(const std::string& str) {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '"': escaped += "\\\""; break;
            case '\\': escaped += "\\\\"; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c; break;
        }
    }
    return escaped;
}

std::string TokenPrint::EscapeCSV(const std::string& str) {
    if (str.find(',') != std::string::npos || str.find('"') != std::string::npos) {
        std::string escaped = "\"";
        for (char c : str) {
            if (c == '"') {
                escaped += "\"\"";
            } else {
                escaped += c;
            }
        }
        escaped += "\"";
        return escaped;
    }
    return str;
}

} // namespace Test
} // namespace CHTL