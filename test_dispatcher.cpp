#include <iostream>
#include <string>
#include <vector>
#include "Scanner/CHTLUnifiedScanner.h"

int main() {
    std::cout << "=== 测试编译器调度器架构 ===" << std::endl;
    
    // 测试代码
    std::string testCode = R"(
html {
    head {
        style {
            body {
                background-color: ThemeColor();
                margin: Colors().primary;
            }
        }
        
        script {
            console.log("Hello World");
            
            vir myObject {
                name: "test",
                getValue: function() {
                    return this.name;
                }
            }
        }
    }
    
    body {
        div {
            text { "Hello CHTL!" }
        }
    }
}
)";
    
    try {
        // 1. 测试统一扫描器
        std::cout << "\n1. 测试统一扫描器切割..." << std::endl;
        CHTL::Scanner::CHTLUnifiedScanner scanner;
        auto fragments = scanner.ScanSource(testCode, "test.chtl");
        
        std::cout << "扫描完成，发现 " << fragments.size() << " 个代码片段" << std::endl;
        
        // 2. 显示片段分类
        std::cout << "\n2. 片段分类结果:" << std::endl;
        int chtlCount = 0, chtlJSCount = 0, cssCount = 0, jsCount = 0;
        
        for (size_t i = 0; i < fragments.size(); ++i) {
            const auto& fragment = fragments[i];
            std::string typeName;
            
            switch (fragment.type) {
                case CHTL::Scanner::FragmentType::CHTL:
                    typeName = "CHTL";
                    chtlCount++;
                    break;
                case CHTL::Scanner::FragmentType::CHTL_JS:
                    typeName = "CHTL JS";
                    chtlJSCount++;
                    break;
                case CHTL::Scanner::FragmentType::CSS:
                    typeName = "CSS";
                    cssCount++;
                    break;
                case CHTL::Scanner::FragmentType::JS:
                    typeName = "JavaScript";
                    jsCount++;
                    break;
                default:
                    typeName = "Unknown";
                    break;
            }
            
            std::cout << "片段 " << (i+1) << ": " << typeName 
                      << " (长度: " << fragment.content.length() << ")" << std::endl;
            
            // 显示片段内容的前50个字符
            std::string preview = fragment.content;
            if (preview.length() > 50) {
                preview = preview.substr(0, 47) + "...";
            }
            // 替换换行符为空格以便显示
            for (char& c : preview) {
                if (c == '\n' || c == '\r') c = ' ';
            }
            std::cout << "  内容: \"" << preview << "\"" << std::endl;
        }
        
        // 3. 统计信息
        std::cout << "\n3. 统计信息:" << std::endl;
        std::cout << "CHTL片段: " << chtlCount << std::endl;
        std::cout << "CHTL JS片段: " << chtlJSCount << std::endl;
        std::cout << "CSS片段: " << cssCount << std::endl;
        std::cout << "JavaScript片段: " << jsCount << std::endl;
        std::cout << "总计: " << fragments.size() << std::endl;
        
        // 4. 验证架构正确性
        std::cout << "\n4. 架构验证:" << std::endl;
        bool hasHTMLElements = chtlCount > 0;
        bool hasStyleProcessing = cssCount > 0;
        bool hasScriptProcessing = jsCount > 0 || chtlJSCount > 0;
        
        std::cout << "✓ HTML元素处理: " << (hasHTMLElements ? "是" : "否") << std::endl;
        std::cout << "✓ 样式处理: " << (hasStyleProcessing ? "是" : "否") << std::endl;
        std::cout << "✓ 脚本处理: " << (hasScriptProcessing ? "是" : "否") << std::endl;
        
        if (hasHTMLElements && hasStyleProcessing && hasScriptProcessing) {
            std::cout << "\n🎉 编译器调度器架构验证成功！" << std::endl;
            std::cout << "各类型片段都能被正确识别和分类。" << std::endl;
        } else {
            std::cout << "\n❌ 架构验证失败，某些片段类型未被正确识别。" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "❌ 测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}