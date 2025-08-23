#include <iostream>
#include <cassert>
#include <string>
#include "Scanner/CHTLUnifiedScanner.h"

using namespace CHTL;

void test_scanner_basic() {
    Scanner::CHTLUnifiedScanner scanner;
    std::string testCode = "html { body { text { Hello World } } }";
    
    auto fragments = scanner.ScanSource(testCode, "test.chtl");
    
    // 应该至少有一个片段
    assert(!fragments.empty());
    std::cout << "扫描器基础测试通过 - 发现 " << fragments.size() << " 个片段\n";
}

void test_scanner_chtl_js_detection() {
    Scanner::CHTLUnifiedScanner scanner;
    std::string testCode = "script { {{box}}->click(); }";
    
    auto fragments = scanner.ScanSource(testCode, "test.chtl");
    
    // 应该检测到CHTL JS特征
    bool foundCHTLJS = false;
    for (const auto& fragment : fragments) {
        if (fragment.type == Scanner::FragmentType::CHTL_JS) {
            foundCHTLJS = true;
            break;
        }
    }
    
    assert(foundCHTLJS);
    std::cout << "CHTL JS检测测试通过\n";
}

void test_scanner_css_detection() {
    Scanner::CHTLUnifiedScanner scanner;
    std::string testCode = "style { color: red; background-color: blue; }";
    
    auto fragments = scanner.ScanSource(testCode, "test.chtl");
    
    // 应该检测到CSS特征
    bool foundCSS = false;
    for (const auto& fragment : fragments) {
        if (fragment.type == Scanner::FragmentType::CSS) {
            foundCSS = true;
            break;
        }
    }
    
    assert(foundCSS);
    std::cout << "CSS检测测试通过\n";
}

int main() {
    std::cout << "运行扫描器测试...\n";
    
    test_scanner_basic();
    test_scanner_chtl_js_detection();
    test_scanner_css_detection();
    
    std::cout << "所有扫描器测试通过!\n";
    return 0;
}