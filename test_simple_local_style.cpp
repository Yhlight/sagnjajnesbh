#include "include/CHTLParser.h"
#include "include/CHTLGenerator.h"
#include <iostream>
#include <fstream>

using namespace chtl;

int main() {
    std::cout << "=== CHTL局部样式块简化测试 ===" << std::endl;
    
    // 简单的测试代码
    std::string source = R"(
div
{
    style
    {
        width: 200px;
        height: 100px;
        
        .test-box
        {
            background-color: blue;
            color: white;
        }
        
        &:hover
        {
            background-color: darkblue;
        }
    }
    
    text
    {
        "测试文本"
    }
}
)";
    
    std::cout << "测试代码准备完成" << std::endl;
    
    // 配置选项
    CompilerOptions options;
    options.debugMode = true;
    
    try {
        // 创建解析器
        CHTLParser parser(options);
        
        std::cout << "\n=== 开始解析 ===" << std::endl;
        
        // 解析CHTL代码
        ParseResult parseResult = parser.parse(source, "test");
        
        if (!parseResult.success) {
            std::cout << "解析失败！" << std::endl;
            for (const auto& error : parseResult.errors) {
                std::cout << "  错误: " << error.message << std::endl;
            }
            return 1;
        }
        
        std::cout << "解析成功！" << std::endl;
        
        // 创建生成器
        CHTLGenerator generator(options);
        
        std::cout << "\n=== 开始生成 ===" << std::endl;
        
        // 生成HTML和CSS
        GeneratorResult generateResult = generator.generate(parseResult.ast);
        
        if (!generateResult.success) {
            std::cout << "生成失败！" << std::endl;
            for (const auto& error : generateResult.errors) {
                std::cout << "  错误: " << error.message << std::endl;
            }
            return 1;
        }
        
        std::cout << "生成成功！" << std::endl;
        
        // 输出结果
        std::cout << "\n=== HTML输出 ===" << std::endl;
        std::cout << generateResult.htmlOutput << std::endl;
        
        std::cout << "\n=== CSS输出 ===" << std::endl;
        std::cout << generateResult.cssOutput << std::endl;
        
        std::cout << "\n=== 测试完成 ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "异常: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}