#include "src/parser/CHTLBasicParser.cpp"
#include "src/generator/CHTLBasicGenerator.cpp"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace chtl;

int main() {
    std::cout << "=== CHTL基础语法解析与生成测试 ===" << std::endl;
    
    // 读取测试文件
    std::ifstream file("test_basic_syntax.chtl");
    if (!file.good()) {
        std::cerr << "无法打开测试文件 test_basic_syntax.chtl" << std::endl;
        return 1;
    }
    
    std::string source((std::istreambuf_iterator<char>(file)), 
                       std::istreambuf_iterator<char>());
    file.close();
    
    std::cout << "读取源文件完成，大小: " << source.size() << " 字符" << std::endl;
    
    // 配置选项
    CompilerOptions options;
    options.debugMode = true;
    options.generateComments = true;
    
    // 创建基础解析器
    CHTLBasicParser parser(options);
    
    std::cout << "\n=== 开始解析 ===" << std::endl;
    
    // 解析基础语法
    ParseResult parseResult = parser.parseBasicSyntax(source, "test_basic_syntax.chtl");
    
    if (!parseResult.success) {
        std::cout << "解析失败！错误信息:" << std::endl;
        for (const auto& error : parseResult.errors) {
            std::cout << "  行 " << error.line << ", 列 " << error.column 
                     << ": " << error.message << std::endl;
        }
        return 1;
    }
    
    std::cout << "解析成功！" << std::endl;
    
    // 创建基础生成器
    CHTLBasicGenerator generator(options);
    
    std::cout << "\n=== 开始生成HTML ===" << std::endl;
    
    // 生成HTML
    GeneratorResult generateResult = generator.generateBasicHTML(parseResult.ast);
    
    if (!generateResult.success) {
        std::cout << "生成失败！错误信息:" << std::endl;
        for (const auto& error : generateResult.errors) {
            std::cout << "  " << error.message << std::endl;
        }
        return 1;
    }
    
    std::cout << "生成成功！" << std::endl;
    
    // 输出生成的HTML
    std::cout << "\n=== 生成的HTML ===" << std::endl;
    std::cout << generateResult.htmlOutput << std::endl;
    
    // 保存到文件
    std::ofstream outputFile("test_basic_syntax_output.html");
    if (outputFile.good()) {
        // 创建完整的HTML文档
        outputFile << "<!DOCTYPE html>\n";
        outputFile << "<html lang=\"zh-CN\">\n";
        outputFile << "<head>\n";
        outputFile << "    <meta charset=\"UTF-8\">\n";
        outputFile << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        outputFile << "    <title>CHTL基础语法测试</title>\n";
        outputFile << "    <style>\n";
        outputFile << "        body { font-family: Arial, sans-serif; margin: 20px; }\n";
        outputFile << "        .container { padding: 20px; border: 1px solid #ddd; margin: 10px 0; }\n";
        outputFile << "        .content-section { margin: 20px 0; }\n";
        outputFile << "        .nested-div { background-color: #f5f5f5; padding: 10px; }\n";
        outputFile << "        .item-list { list-style-type: disc; margin-left: 20px; }\n";
        outputFile << "    </style>\n";
        outputFile << "</head>\n";
        outputFile << "<body>\n";
        outputFile << generateResult.htmlOutput;
        outputFile << "\n</body>\n";
        outputFile << "</html>\n";
        outputFile.close();
        
        std::cout << "\n输出已保存到 test_basic_syntax_output.html" << std::endl;
    }
    
    // 统计信息
    std::cout << "\n=== 统计信息 ===" << std::endl;
    std::cout << "解析警告: " << parseResult.warnings.size() << " 个" << std::endl;
    std::cout << "生成警告: " << generateResult.warnings.size() << " 个" << std::endl;
    std::cout << "生成的HTML大小: " << generateResult.htmlOutput.size() << " 字符" << std::endl;
    
    std::cout << "\n=== 测试完成 ===" << std::endl;
    
    return 0;
}