#include <iostream>
#include <iomanip>
#include "scanner/chtl_unified_scanner_v2.h"

void printFragment(const chtl::CodeFragment* fragment, size_t index) {
    std::cout << "\n[Fragment " << index << "] "
              << "Type: " << chtl::CodeFragment::fragmentTypeToString(fragment->getType())
              << " (Line " << fragment->getStartLine() 
              << ", Col " << fragment->getStartColumn() << ")\n";
    std::cout << "----------------------------------------\n";
    std::cout << fragment->getContent() << "\n";
    std::cout << "----------------------------------------\n";
}

int main() {
    chtl::CHTLUnifiedScannerV2 scanner;
    
    // 设置较小的初始切片以演示可变长度机制
    scanner.setInitialSliceSize(50);
    scanner.setExpandStep(25);
    
    std::string source = R"(
// CHTL示例代码，展示精准切割

[Template] @Style ThemeColors {
    primary: #007bff;
    secondary: #6c757d;
}

div {
    id: container;
    class: main;
    
    style {
        .main {
            max-width: 800px;
            margin: 0 auto;
        }
        
        &:hover {
            background-color: #f0f0f0;
        }
    }
    
    h1 {
        text {
            "Welcome to CHTL"
        }
    }
    
    script {
        // CHTL JS代码，展示语法单元切割
        vir ButtonManager = {
            click: function() {
                console.log('Button clicked');
            }
        };
        
        {{#container}}->addEventListener('click', function(e) {
            if (e.target.matches('.button')) {
                ButtonManager->click();
            }
        });
        
        {{.button}}->listen({
            mouseenter: function() {
                this.style.opacity = '0.8';
            },
            mouseleave: function() {
                this.style.opacity = '1';
            }
        });
    }
}

// 全局样式
style {
    body {
        font-family: sans-serif;
        line-height: 1.6;
    }
}
)";
    
    std::cout << "CHTL Unified Scanner V2 Demo\n";
    std::cout << "============================\n";
    std::cout << "\nSource code length: " << source.length() << " bytes\n";
    std::cout << "Initial slice size: 50 bytes\n";
    std::cout << "Expand step: 25 bytes\n";
    
    // 扫描源代码
    auto fragments = scanner.scan(source);
    
    // 检查错误
    if (!scanner.getErrors().empty()) {
        std::cout << "\nScanning errors:\n";
        for (const auto& error : scanner.getErrors()) {
            std::cout << "  - " << error << "\n";
        }
    }
    
    // 打印结果
    std::cout << "\nScanning completed. Found " << fragments.size() << " fragments:\n";
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        printFragment(fragments[i].get(), i + 1);
    }
    
    // 统计信息
    std::cout << "\nFragment Type Statistics:\n";
    std::cout << "------------------------\n";
    
    int chtlCount = 0, chtlJsCount = 0, localStyleCount = 0, cssCount = 0, jsCount = 0;
    
    for (const auto& fragment : fragments) {
        switch (fragment->getType()) {
            case chtl::FragmentType::CHTL:
                chtlCount++;
                break;
            case chtl::FragmentType::CHTL_JS:
                chtlJsCount++;
                break;
            case chtl::FragmentType::LOCAL_STYLE:
                localStyleCount++;
                break;
            case chtl::FragmentType::CSS:
                cssCount++;
                break;
            case chtl::FragmentType::JAVASCRIPT:
                jsCount++;
                break;
            default:
                break;
        }
    }
    
    std::cout << "CHTL fragments:        " << chtlCount << "\n";
    std::cout << "CHTL JS fragments:     " << chtlJsCount << "\n";
    std::cout << "Local Style fragments: " << localStyleCount << "\n";
    std::cout << "CSS fragments:         " << cssCount << "\n";
    std::cout << "JavaScript fragments:  " << jsCount << "\n";
    
    return 0;
}