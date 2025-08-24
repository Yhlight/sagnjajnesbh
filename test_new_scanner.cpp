#include <iostream>
#include <string>
#include "Scanner/CHTLUnifiedScanner_v2.h"

int main() {
    Scanner::CHTLUnifiedScanner scanner;
    scanner.EnableDebugMode(true);
    
    // 测试CHTL JS代码：正确的虚对象语法
    std::string testCode = "vir testObj = listen({click: \"handleClick\"});";
    
    std::cout << "测试代码: " << testCode << std::endl;
    std::cout << "=========================" << std::endl;
    
    auto fragments = scanner.ScanSource(testCode, "test.chtl");
    
    std::cout << "\n扫描结果:" << std::endl;
    std::cout << "片段数量: " << fragments.size() << std::endl;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        std::string typeName;
        
        switch (fragment.type) {
            case Scanner::FragmentType::CHTL: typeName = "CHTL"; break;
            case Scanner::FragmentType::CHTL_JS: typeName = "CHTL_JS"; break;
            case Scanner::FragmentType::CSS: typeName = "CSS"; break;
            case Scanner::FragmentType::JS: typeName = "JS"; break;
            case Scanner::FragmentType::Unknown: typeName = "Unknown"; break;
        }
        
        std::cout << "片段 " << i + 1 << ":" << std::endl;
        std::cout << "  类型: " << typeName << std::endl;
        std::cout << "  内容: '" << fragment.content << "'" << std::endl;
        std::cout << "  位置: " << fragment.startPosition << "-" << fragment.endPosition << std::endl;
        std::cout << "  行列: " << fragment.lineNumber << ":" << fragment.columnNumber << std::endl;
        std::cout << std::endl;
    }
    
    return 0;
}