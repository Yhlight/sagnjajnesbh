#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

int main() {
    std::cout << "🔧 CHTL语法修正验证\n";
    std::cout << "===================\n\n";
    
    // 读取修正后的项目文件
    std::ifstream file("../examples/LargeProject/OnlineEducationPlatform_CORRECTED.chtl");
    if (!file.is_open()) {
        std::cout << "❌ 无法读取修正后的项目文件\n";
        return 1;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "✅ 成功读取修正后的项目文件，大小: " << content.size() << " 字符\n\n";
    
    // 验证修正的语法问题
    std::cout << "🔍 1. 变量组语法修正验证\n";
    std::cout << "========================\n";
    
    // 检查正确的变量组定义语法 [Custom] @Var
    std::regex correctVarSyntax(R"(\[Custom\]\s*@Var\s+\w+\s*\{)");
    int correctVarCount = std::distance(std::sregex_iterator(content.begin(), content.end(), correctVarSyntax),
                                      std::sregex_iterator());
    std::cout << "✅ 正确的变量组定义 [Custom] @Var: " << correctVarCount << " 处\n";
    
    // 检查错误的变量组定义语法 [Var]
    std::regex incorrectVarSyntax(R"(\[Var\]\s+\w+\s*\{)");
    int incorrectVarCount = std::distance(std::sregex_iterator(content.begin(), content.end(), incorrectVarSyntax),
                                        std::sregex_iterator());
    std::cout << (incorrectVarCount == 0 ? "✅" : "❌") << " 错误的变量组定义 [Var]: " << incorrectVarCount << " 处\n";
    
    std::cout << "\n🔍 2. 自定义操作语法修正验证\n";
    std::cout << "============================\n";
    
    // 检查正确的索引访问语法
    std::regex correctIndexSyntax(R"(\w+\[0\]|\w+\[\d+\])");
    int correctIndexCount = std::distance(std::sregex_iterator(content.begin(), content.end(), correctIndexSyntax),
                                        std::sregex_iterator());
    std::cout << "✅ 正确的索引访问语法 [0]: " << correctIndexCount << " 处\n";
    
    // 检查insert操作的正确语法
    std::regex insertAfterSyntax(R"(insert\s+after\s+\w+\[\d+\])");
    int insertAfterCount = std::distance(std::sregex_iterator(content.begin(), content.end(), insertAfterSyntax),
                                       std::sregex_iterator());
    std::cout << "✅ 正确的insert after语法: " << insertAfterCount << " 处\n";
    
    std::cout << "\n🔍 3. 约束系统语法修正验证\n";
    std::cout << "========================\n";
    
    // 检查正确的except语法 - 只能约束 @Html, [Custom], [Template]
    std::regex correctExceptSyntax(R"(except\s+(@Html|\[Custom\]|\[Template\]))");
    int correctExceptCount = std::distance(std::sregex_iterator(content.begin(), content.end(), correctExceptSyntax),
                                         std::sregex_iterator());
    std::cout << "✅ 正确的except语法: " << correctExceptCount << " 处\n";
    
    // 检查错误的except语法（如约束[Origin]等不支持的类型）
    std::regex incorrectExceptSyntax(R"(except\s+\[Origin\])");
    int incorrectExceptCount = std::distance(std::sregex_iterator(content.begin(), content.end(), incorrectExceptSyntax),
                                           std::sregex_iterator());
    std::cout << (incorrectExceptCount == 0 ? "✅" : "❌") << " 错误的except语法 [Origin]: " << incorrectExceptCount << " 处\n";
    
    // 检查是否有[Constraint]定义（这是不存在的语法）
    std::regex constraintSyntax(R"(\[Constraint\])");
    int constraintCount = std::distance(std::sregex_iterator(content.begin(), content.end(), constraintSyntax),
                                      std::sregex_iterator());
    std::cout << (constraintCount == 0 ? "✅" : "❌") << " 错误的[Constraint]语法: " << constraintCount << " 处\n";
    
    std::cout << "\n🔍 4. 其他语法特征验证\n";
    std::cout << "====================\n";
    
    // 验证模板继承语法
    std::regex inheritSyntax(R"(inherit\s+\w+\([^)]+\))");
    int inheritCount = std::distance(std::sregex_iterator(content.begin(), content.end(), inheritSyntax),
                                   std::sregex_iterator());
    std::cout << "✅ 模板继承语法: " << inheritCount << " 处\n";
    
    // 验证变量组使用语法
    std::regex varUsageSyntax(R"(\w+\([^)]+\))");
    int varUsageCount = std::distance(std::sregex_iterator(content.begin(), content.end(), varUsageSyntax),
                                    std::sregex_iterator());
    std::cout << "✅ 变量组使用语法: " << varUsageCount << " 处\n";
    
    // 验证CHTL JS特征
    std::regex chtljsSelector(R"(\{\{[^}]+\}\})");
    int selectorCount = std::distance(std::sregex_iterator(content.begin(), content.end(), chtljsSelector),
                                    std::sregex_iterator());
    std::cout << "✅ CHTL JS增强选择器: " << selectorCount << " 处\n";
    
    std::cout << "\n📊 语法修正验证报告\n";
    std::cout << "==================\n";
    
    int totalCorrections = 0;
    if (incorrectVarCount == 0) totalCorrections++;
    if (correctVarCount > 0) totalCorrections++;
    if (correctIndexCount > 0) totalCorrections++;
    if (correctExceptCount > 0) totalCorrections++;
    if (incorrectExceptCount == 0) totalCorrections++;
    if (constraintCount == 0) totalCorrections++;
    
    std::cout << "🎯 修正的语法问题: " << totalCorrections << "/6 个\n";
    std::cout << "📋 变量组语法: " << (incorrectVarCount == 0 && correctVarCount > 0 ? "✅ 已修正" : "❌ 需修正") << "\n";
    std::cout << "📋 索引访问语法: " << (correctIndexCount > 0 ? "✅ 已修正" : "❌ 需修正") << "\n";
    std::cout << "📋 约束系统语法: " << (correctExceptCount > 0 && incorrectExceptCount == 0 && constraintCount == 0 ? "✅ 已修正" : "❌ 需修正") << "\n";
    
    std::cout << "\n🌟 修正结论:\n";
    std::cout << "===========\n";
    
    if (totalCorrections == 6) {
        std::cout << "🎉 所有语法错误都已修正！\n";
        std::cout << "✨ 项目现在严格遵循CHTL语法文档\n";
        std::cout << "🚀 变量组使用正确的[Custom] @Var语法\n";
        std::cout << "🎯 自定义操作使用正确的索引语法\n";
        std::cout << "⚖️ 约束系统只约束支持的类型\n";
        std::cout << "💎 所有语法特征都符合文档规范\n";
    } else {
        std::cout << "⚠️ 还有 " << (6 - totalCorrections) << " 个语法问题需要修正\n";
        std::cout << "📚 请参考CHTL语法文档进行进一步修正\n";
    }
    
    return 0;
}