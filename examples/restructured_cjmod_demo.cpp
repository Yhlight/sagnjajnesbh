#include "CJMOD/RestructuredCJMODApi.h"
#include <iostream>
#include <string>

using namespace CHTL::CJMOD;

void demonstrateOriginalApiCompatibility() {
    std::cout << "=== 重构后CJMOD API - 兼容原始API设计 ===" << std::endl;
    
    // 1. 语法分析 - 完全兼容原始API
    auto syntax = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )", ",:{};()");
    
    std::cout << "语法分析完成，识别到 " << syntax->length() << " 个参数" << std::endl;
    
    // 2. 参数绑定 - 完全兼容原始API
    syntax->bind("url", [](const std::string& url) -> std::string {
        std::cout << "  处理URL参数: " << url << std::endl;
        return "\"" + url + "\"";
    });
    
    syntax->bind("mode", [](const std::string& mode) -> std::string {
        std::cout << "  处理模式参数: " << mode << std::endl;
        return "\"" + mode + "\"";
    });
    
    // 占位符绑定 - 智能重复绑定
    syntax->bind("$", [](const std::string& value) -> std::string {
        std::cout << "  处理数值参数: " << value << std::endl;
        return value;
    });
    
    // 3. 扫描关键字 - 完全兼容原始API
    syntax->scanKeyword("printMylove", [&]() {
        std::cout << "  扫描到printMylove关键字" << std::endl;
        
        // peekKeyword功能 - 完全兼容原始API
        std::string prevToken = syntax->peekKeyword(-1);
        std::string nextToken = syntax->peekKeyword(1);
        
        std::cout << "    前一个Token: " << prevToken << std::endl;
        std::cout << "    后一个Token: " << nextToken << std::endl;
        
        // 4. 参数匹配 - 完全兼容原始API
        syntax->match("url", "avatar.jpg");
        syntax->match("mode", "ASCII");
        syntax->match("$", "80");    // 自动匹配第一个未匹配的$
        syntax->match("$", "40");    // 自动匹配第二个未匹配的$
        syntax->match("$", "0.5");   // 自动匹配第三个未匹配的$
    });
    
    // 5. 代码生成 - 完全兼容原始API
    std::string result = generateCode(*syntax);
    std::cout << "生成结果: " << result << std::endl;
}

void demonstrateSlidingWindowScanning() {
    std::cout << "\n=== 正确的滑动窗口双指针扫描机制 ===" << std::endl;
    
    // 设置滑动窗口扫描策略
    setGlobalScanStrategy(ScanStrategy::SLIDING_WINDOW);
    
    auto syntax = std::make_unique<Syntax>();
    syntax->setScanStrategy(ScanStrategy::SLIDING_WINDOW);
    
    // 模拟带有CHTL JS语法的源代码
    std::string sourceCode = R"(
        // 普通JavaScript代码
        const userName = "Alice";
        let score = 100;
        
        // CHTL JS语法开始
        printMylove({
            url: "user-avatar.jpg",
            mode: "ASCII",
            width: 80,
            height: 40
        });
        
        // 更多普通代码
        console.log("用户信息:", userName, score);
        
        // 另一个CHTL JS语法
        vir myTest = iNeverAway({
            TestMethod: function(a, b) {
                return a + b;
            }
        });
        
        // 结束的普通代码
        document.getElementById("result").textContent = "完成";
    )";
    
    std::cout << "原始源代码长度: " << sourceCode.length() << " 字符" << std::endl;
    
    // 执行滑动窗口扫描
    std::string processedCode = syntax->scanAndProcess(sourceCode);
    
    std::cout << "处理后代码:" << std::endl;
    std::cout << processedCode << std::endl;
    std::cout << "处理后代码长度: " << processedCode.length() << " 字符" << std::endl;
}

void demonstrateFrontExtractScanning() {
    std::cout << "\n=== 前置代码截取机制（不发送给编译器）===" << std::endl;
    
    // 设置前置截取扫描策略
    setGlobalScanStrategy(ScanStrategy::FRONT_EXTRACT);
    
    auto syntax = std::make_unique<Syntax>();
    syntax->setScanStrategy(ScanStrategy::FRONT_EXTRACT);
    
    // 模拟带有大量前置代码的源码
    std::string sourceCodeWithFrontPart = R"(
        // === 这些是前置代码，不应该发送给编译器 ===
        import React from 'react';
        import { useState, useEffect } from 'react';
        import axios from 'axios';
        
        const MyComponent = () => {
            const [data, setData] = useState(null);
            const [loading, setLoading] = useState(true);
            
            useEffect(() => {
                fetchData();
            }, []);
            
            const fetchData = async () => {
                try {
                    const response = await axios.get('/api/data');
                    setData(response.data);
                } catch (error) {
                    console.error('Error:', error);
                } finally {
                    setLoading(false);
                }
            };
            
            // === CHTL JS语法开始（这部分需要处理）===
            printMylove({
                url: data?.avatar || "default.jpg",
                mode: "Pixel",
                width: 100,
                height: 100,
                scale: 1.2
            });
            
            // === 更多前置代码（不发送给编译器）===
            return (
                <div className="my-component">
                    {loading ? (
                        <div>Loading...</div>
                    ) : (
                        <div>
                            <h1>欢迎 {data?.name}</h1>
                            <p>数据加载完成</p>
                        </div>
                    )}
                </div>
            );
        };
        
        export default MyComponent;
        
        // === 另一个CHTL JS语法 ===
        vir imageProcessor = iNeverAway({
            ProcessImage: function(imageData) {
                return processImageToASCII(imageData);
            },
            SaveResult: function(result) {
                localStorage.setItem('processedImage', result);
            }
        });
        
        // === 更多后置代码（不发送给编译器）===
        const utils = {
            formatDate: (date) => date.toLocaleDateString(),
            calculateAge: (birthDate) => new Date().getFullYear() - birthDate.getFullYear()
        };
    )";
    
    std::cout << "原始源代码（包含大量前置代码）长度: " << sourceCodeWithFrontPart.length() << " 字符" << std::endl;
    
    // 执行前置截取扫描
    std::string extractedCode = syntax->scanAndProcess(sourceCodeWithFrontPart);
    
    std::cout << "\n*** 重要：注意前置代码截取的效果 ***" << std::endl;
    std::cout << "最终输出（只包含处理后的CHTL JS代码）:" << std::endl;
    std::cout << extractedCode << std::endl;
    std::cout << "最终输出代码长度: " << extractedCode.length() << " 字符" << std::endl;
    std::cout << "\n前置代码已被截取，不会发送给编译器！" << std::endl;
}

void demonstratePolicyModes() {
    std::cout << "\n=== 策略模式演示 ===" << std::endl;
    
    auto syntax = std::make_unique<Syntax>();
    
    // 模拟扫描器使用策略模式
    syntax->scanKeyword("complexFunction", [&]() {
        std::cout << "  处理复杂函数" << std::endl;
        
        // 策略模式开始：收集模式
        syntax->policyChangeBegin("{", Policy::COLLECT);
        std::cout << "    策略切换到COLLECT模式" << std::endl;
        
        // 模拟收集复杂内容
        std::string collectedContent = syntax->policyChangeEnd("}", Policy::NORMAL);
        std::cout << "    收集完成，恢复NORMAL模式" << std::endl;
        std::cout << "    收集到的内容: " << collectedContent << std::endl;
    });
}

void demonstrateCHTLJSFunctionIntegration() {
    std::cout << "\n=== CHTLJSFunction与Syntax集成 ===" << std::endl;
    
    // 创建CHTL JS函数
    auto printMyloveFunc = createCHTLJSFunction("printMylove", 
                                              {"url", "mode", "width", "height", "scale"});
    
    // 设置函数体模板
    printMyloveFunc->setBodyTemplate(R"(
        const image = loadImage(config.url || '');
        const mode = config.mode || 'ASCII';
        const width = config.width || 80;
        const height = config.height || 40;
        const scale = config.scale || 1.0;
        
        return convertImageToText(image, mode, width, height, scale);
    )");
    
    // 启用虚对象支持
    printMyloveFunc->enableVirSupport(true);
    
    std::cout << "函数名: " << printMyloveFunc->getName() << std::endl;
    std::cout << "虚对象支持: " << (printMyloveFunc->isVirSupported() ? "是" : "否") << std::endl;
    
    // 生成JavaScript代码
    std::string jsCode = printMyloveFunc->generateJS();
    std::cout << "生成的JavaScript代码:" << std::endl;
    std::cout << jsCode << std::endl;
    
    // 生成虚对象访问代码
    std::string virAccess = printMyloveFunc->generateVirAccess("myImage", "display");
    std::cout << "虚对象访问代码:" << std::endl;
    std::cout << virAccess << std::endl;
    
    // 与Syntax集成
    auto syntax = std::make_unique<Syntax>();
    if (printMyloveFunc->bindToSyntax(*syntax)) {
        std::cout << "成功绑定到Syntax对象" << std::endl;
        std::cout << "Syntax对象现在有 " << syntax->length() << " 个参数" << std::endl;
    }
}

int main() {
    std::cout << "重构后CJMOD API演示程序" << std::endl;
    std::cout << "基于Syntax、Arg、CHTLJSFunction三个核心类" << std::endl;
    std::cout << "实现正确的滑动窗口双指针扫描和前置代码截取机制" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    try {
        // 演示原始API兼容性
        demonstrateOriginalApiCompatibility();
        
        // 演示正确的滑动窗口双指针扫描
        demonstrateSlidingWindowScanning();
        
        // 演示前置代码截取机制
        demonstrateFrontExtractScanning();
        
        // 演示策略模式
        demonstratePolicyModes();
        
        // 演示CHTLJSFunction集成
        demonstrateCHTLJSFunctionIntegration();
        
        std::cout << "\n=== 总结 ===" << std::endl;
        std::cout << "✅ 重构后的CJMOD API特点:" << std::endl;
        std::cout << "1. 完全兼容原始API设计思路" << std::endl;
        std::cout << "2. 实现正确的滑动窗口双指针扫描（前后指针协同移动）" << std::endl;
        std::cout << "3. 实现前置代码截取（不发送无关代码给编译器）" << std::endl;
        std::cout << "4. 基于Syntax、Arg、CHTLJSFunction三个核心类" << std::endl;
        std::cout << "5. 保持表面简单，内在精妙的设计哲学" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}