#include "CJMOD/ProperCJMODApi.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "module/demo/src/demo.h"
#include <iostream>
#include <memory>
#include <fstream>

using namespace CHTL;
using namespace CHTL::CJMOD;

/**
 * @brief 完整的CHTL与CJMOD集成演示
 * 
 * 展示：
 * 1. 统一扫描器如何扫描CHTL源码
 * 2. CJMOD API如何处理CodeFragment
 * 3. 完整的架构协作流程
 */

void demonstrateCompleteIntegration() {
    std::cout << "🏗️ CHTL与CJMOD完整集成演示" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // ============ 第1步：创建统一扫描器 ============
    std::cout << "\n🔍 第1步：创建统一扫描器" << std::endl;
    Scanner::CHTLUnifiedScanner scanner;
    
    // ============ 第2步：创建并注册CJMOD扩展 ============
    std::cout << "\n📝 第2步：创建并注册CJMOD扩展" << std::endl;
    auto demoProcessor = Demo::createDemoProcessor();
    demoProcessor->registerAllExtensions(scanner);
    
    // ============ 第3步：读取CHTL源文件 ============
    std::cout << "\n📄 第3步：读取CHTL源文件" << std::endl;
    
    std::string chtlSource;
    std::ifstream file("/workspace/demo_integration.chtl");
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            chtlSource += line + "\n";
        }
        file.close();
        std::cout << "   ✅ 成功读取CHTL文件，大小: " << chtlSource.length() << " 字节" << std::endl;
    } else {
        // 如果文件不存在，使用内嵌的演示代码
        std::cout << "   📝 使用内嵌演示代码" << std::endl;
        chtlSource = R"(
<!DOCTYPE html>
<html>
<head>
    <title>CHTL与CJMOD集成演示</title>
</head>
<body>
    <h1>CHTL架构演示</h1>
    
    [Import] @CJmod from "demo"
    
    <script>
    {
        // printMylove演示 - 会被统一扫描器识别
        const loveArt = printMylove({
            url: "https://example.com/chtholly.jpg",
            mode: "ASCII",
            width: "300px",
            height: "200px",
            scale: 0.8
        });
        
        // 虚对象演示
        vir eventHandler = listen({
            click: function(element) {
                console.log("点击:", element);
            },
            hover: function(element) {
                console.log("悬停:", element);
            }
        });
        
        eventHandler->click(document.body);
        
        // iNeverAway状态管理演示
        vir stateManager = iNeverAway({
            Void<Loading>: function(message) {
                console.log("加载中:", message);
            },
            Void<Success>: function(data) {
                console.log("成功:", data);
            }
        });
        
        stateManager->Loading("处理CJMOD扩展...");
        
        // 自定义动画演示
        const animation = createAnimation({
            target: ".demo-element",
            type: "fadeInUp",
            duration: 1000,
            easing: "ease-out"
        });
        
        animation.play();
        
        // 普通JavaScript - 不会被CJMOD处理
        console.log("这是普通的JavaScript代码");
    }
    </script>
</body>
</html>
        )";
    }
    
    // ============ 第4步：统一扫描器扫描源码 ============
    std::cout << "\n🔍 第4步：统一扫描器扫描源码" << std::endl;
    auto fragments = scanner.ScanSource(chtlSource, "demo_integration.chtl");
    
    std::cout << "   🎯 扫描完成，发现 " << fragments.size() << " 个代码片段:" << std::endl;
    
    // 分析扫描结果
    int chtlFragments = 0, chtlJSFragments = 0, cssFragments = 0, jsFragments = 0;
    
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        
        std::cout << "   片段 " << (i+1) << ": ";
        
        switch (fragment.type) {
            case Scanner::FragmentType::CHTL:
                std::cout << "CHTL片段";
                chtlFragments++;
                break;
            case Scanner::FragmentType::CHTL_JS:
                std::cout << "CHTL JS片段 ⭐";
                chtlJSFragments++;
                break;
            case Scanner::FragmentType::CSS:
                std::cout << "CSS片段";
                cssFragments++;
                break;
            case Scanner::FragmentType::JS:
                std::cout << "JS片段";
                jsFragments++;
                break;
            default:
                std::cout << "未知片段";
                break;
        }
        
        std::cout << " (长度: " << fragment.content.length() << ")" << std::endl;
        
        // 如果是CHTL JS片段，显示内容预览
        if (fragment.type == Scanner::FragmentType::CHTL_JS) {
            std::string preview = fragment.content;
            if (preview.length() > 100) {
                preview = preview.substr(0, 100) + "...";
            }
            std::cout << "      内容预览: " << preview << std::endl;
        }
    }
    
    std::cout << "\n   📊 片段统计:" << std::endl;
    std::cout << "      CHTL片段: " << chtlFragments << std::endl;
    std::cout << "      CHTL JS片段: " << chtlJSFragments << " ⭐" << std::endl;
    std::cout << "      CSS片段: " << cssFragments << std::endl;
    std::cout << "      JS片段: " << jsFragments << std::endl;
    
    // ============ 第5步：CJMOD API处理CHTL JS片段 ============
    std::cout << "\n⚙️ 第5步：CJMOD API处理CHTL JS片段" << std::endl;
    
    std::vector<std::string> generatedJS;
    
    for (const auto& fragment : fragments) {
        if (fragment.type == Scanner::FragmentType::CHTL_JS) {
            std::cout << "\n   🎯 处理CHTL JS片段:" << std::endl;
            
            // 检测片段中的CJMOD扩展
            std::vector<std::string> detectedExtensions;
            
            if (fragment.content.find("printMylove") != std::string::npos) {
                detectedExtensions.push_back("printMylove");
            }
            if (fragment.content.find("iNeverAway") != std::string::npos) {
                detectedExtensions.push_back("iNeverAway");
            }
            if (fragment.content.find("createAnimation") != std::string::npos) {
                detectedExtensions.push_back("createAnimation");
            }
            if (fragment.content.find("vir ") != std::string::npos && 
                fragment.content.find("listen") != std::string::npos) {
                detectedExtensions.push_back("virtualObject");
            }
            
            std::cout << "      检测到的扩展: ";
            for (const auto& ext : detectedExtensions) {
                std::cout << ext << " ";
            }
            std::cout << std::endl;
            
            // 使用CJMOD API处理每个检测到的扩展
            for (const auto& extension : detectedExtensions) {
                if (extension == "virtualObject") {
                    // 虚对象由CHTL JS处理，CJMOD只需委托
                    std::cout << "      🔮 虚对象: 委托给CHTL JS处理" << std::endl;
                    VirtualObjectDelegate::registerVirtualObject("eventHandler", "listen_function");
                } else {
                    std::cout << "      ⚙️ 处理扩展: " << extension << std::endl;
                    std::string jsCode = demoProcessor->processCodeFragment(fragment, extension);
                    
                    if (!jsCode.empty()) {
                        generatedJS.push_back(jsCode);
                        std::cout << "      ✅ 生成JS代码 (" << jsCode.length() << " 字节)" << std::endl;
                    }
                }
            }
        }
    }
    
    // ============ 第6步：展示生成的JavaScript代码 ============
    std::cout << "\n📜 第6步：展示生成的JavaScript代码" << std::endl;
    
    if (!generatedJS.empty()) {
        std::cout << "   🎉 成功生成 " << generatedJS.size() << " 个JavaScript函数:" << std::endl;
        
        for (size_t i = 0; i < generatedJS.size(); ++i) {
            std::cout << "\n   === 生成的JS函数 " << (i+1) << " ===" << std::endl;
            std::cout << generatedJS[i] << std::endl;
        }
    } else {
        std::cout << "   ℹ️ 没有检测到需要处理的CJMOD扩展" << std::endl;
    }
    
    // ============ 第7步：架构优势总结 ============
    std::cout << "\n🏆 第7步：CHTL架构优势总结" << std::endl;
    Demo::demonstrateArchitectureAdvantages();
}

void demonstrateArchitectureFlow() {
    std::cout << "\n🚀 CHTL架构流程详细演示" << std::endl;
    std::cout << "================================" << std::endl;
    
    std::cout << "\n📋 完整的架构协作流程:" << std::endl;
    std::cout << "   1️⃣ CHTL源代码 → 包含CHTL JS扩展语法" << std::endl;
    std::cout << "   2️⃣ 统一扫描器 → 识别并切割代码片段（不做词法分析）" << std::endl;
    std::cout << "   3️⃣ CodeFragment → 分类为CHTL_JS类型" << std::endl;
    std::cout << "   4️⃣ CJMOD API → 使用peekKeyword提取参数" << std::endl;
    std::cout << "   5️⃣ 参数处理 → 应用处理器和转换规则" << std::endl;
    std::cout << "   6️⃣ JS代码生成 → 输出标准JavaScript" << std::endl;
    std::cout << "   7️⃣ 编译器集成 → 与其他片段合并输出" << std::endl;
    
    std::cout << "\n💡 关键理解点:" << std::endl;
    std::cout << "   🎯 统一扫描器：只做切割，不做词法分析" << std::endl;
    std::cout << "   ⚙️ CJMOD API：专注参数处理和代码生成" << std::endl;
    std::cout << "   🔄 完美协作：各组件职责分离，避免重复实现" << std::endl;
    std::cout << "   🏗️ 可扩展性：新扩展可轻松集成到现有架构" << std::endl;
    
    std::cout << "\n🎉 这就是CHTL优秀架构设计的威力！" << std::endl;
}

void demonstrateRealWorldUsage() {
    std::cout << "\n🌍 真实世界使用场景演示" << std::endl;
    std::cout << "=============================" << std::endl;
    
    // 模拟真实的CHTL开发场景
    std::cout << "\n📝 场景：开发一个交互式图片处理页面" << std::endl;
    
    std::string realWorldCHTL = R"(
<!DOCTYPE html>
<html>
<head>
    <title>图片处理工具</title>
    [Import] @CJmod from "demo"
</head>
<body>
    <div class="image-processor">
        <input type="file" id="imageInput" accept="image/*">
        <div id="output"></div>
        
        <script>
        {
            // 使用printMylove处理上传的图片
            document.getElementById('imageInput').addEventListener('change', function(e) {
                const file = e.target.files[0];
                if (file) {
                    const reader = new FileReader();
                    reader.onload = function(event) {
                        // CJMOD扩展：将图片转换为ASCII艺术
                        const asciiArt = printMylove({
                            url: event.target.result,
                            mode: "ASCII",
                            width: "400px",
                            height: "300px",
                            scale: 0.6
                        });
                        
                        // 状态管理
                        vir processor = iNeverAway({
                            Void<Processing>: function(message) {
                                document.getElementById('output').innerHTML = 
                                    '<div class="loading">' + message + '</div>';
                            },
                            Void<Complete>: function(result) {
                                document.getElementById('output').innerHTML = 
                                    '<pre class="ascii-art">' + result + '</pre>';
                                
                                // 添加完成动画
                                const animation = createAnimation({
                                    target: ".ascii-art",
                                    type: "fadeInUp",
                                    duration: 800
                                });
                                animation.play();
                            }
                        });
                        
                        processor->Processing("正在转换图片...");
                        
                        // 模拟处理完成
                        setTimeout(() => {
                            processor->Complete(asciiArt);
                        }, 2000);
                    };
                    reader.readAsDataURL(file);
                }
            });
        }
        </script>
    </div>
</body>
</html>
    )";
    
    std::cout << "\n🔍 统一扫描器处理真实场景:" << std::endl;
    
    Scanner::CHTLUnifiedScanner scanner;
    auto demoProcessor = Demo::createDemoProcessor();
    demoProcessor->registerAllExtensions(scanner);
    
    auto fragments = scanner.ScanSource(realWorldCHTL, "image_processor.chtl");
    
    std::cout << "   📊 扫描结果: " << fragments.size() << " 个片段" << std::endl;
    
    // 统计CJMOD扩展使用情况
    int printMyloveCount = 0, iNeverAwayCount = 0, createAnimationCount = 0;
    
    for (const auto& fragment : fragments) {
        if (fragment.type == Scanner::FragmentType::CHTL_JS) {
            if (fragment.content.find("printMylove") != std::string::npos) printMyloveCount++;
            if (fragment.content.find("iNeverAway") != std::string::npos) iNeverAwayCount++;
            if (fragment.content.find("createAnimation") != std::string::npos) createAnimationCount++;
        }
    }
    
    std::cout << "\n   🎯 CJMOD扩展使用统计:" << std::endl;
    std::cout << "      printMylove: " << printMyloveCount << " 次" << std::endl;
    std::cout << "      iNeverAway: " << iNeverAwayCount << " 次" << std::endl;
    std::cout << "      createAnimation: " << createAnimationCount << " 次" << std::endl;
    
    std::cout << "\n   ✅ 真实场景验证了CHTL架构的实用性和强大功能！" << std::endl;
}

int main() {
    std::cout << "🏗️ CHTL与CJMOD完整集成演示程序" << std::endl;
    std::cout << "================================================" << std::endl;
    std::cout << "展示统一扫描器与CJMOD API的完美协作" << std::endl;
    
    try {
        // 主要集成演示
        demonstrateCompleteIntegration();
        
        // 架构流程演示
        demonstrateArchitectureFlow();
        
        // 真实世界使用场景
        demonstrateRealWorldUsage();
        
        std::cout << "\n🎉 演示完成！" << std::endl;
        std::cout << "CHTL的优秀架构设计让CJMOD扩展变得如此简单和强大！" << std::endl;
        std::cout << "这就是为什么要使用现有的优秀架构，而不是重新发明轮子！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 演示过程中发生错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}