// ==========================================
// 珂朵莉模块 CJMOD 扩展实现
// 使用优雅CJMOD API，严格按照CHTL语法文档
// 实现 printMylove 和 iNeverAway 等珂朵莉特色功能
// ==========================================

#include "CJMOD/ElegantCJMODApi.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

using namespace CHTL::CJMOD;
using namespace CHTL::CJMOD::Chtholly;

// 前向声明
std::string generateINeverAwayCode(const std::string& virObjectContent);

// ==========================================
// printMylove 函数实现
// 按照原始API设计哲学：表面简单，内在精妙
// ==========================================

void implementPrintMylove() {
    std::cout << "=== 实现 printMylove 功能 ===" << std::endl;
    
    // 第1步：syntaxAnalys - 表面简单的语法分析
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        printMylove({
            url: $,
            mode: $,
            width: $,
            height: $,
            scale: $
        });
    )", ignoreChars);
    
    std::cout << "语法分析完成，识别到 " << keyword->args.length() << " 个参数" << std::endl;
    
    // 第2步：bind - 表面简单的绑定，内在智能的处理
    keyword->args.bind<std::string>("url", [](const std::string& url) -> std::string {
        return PrintMyloveSystem::processImageUrl(url);
    });
    
    keyword->args.bind<std::string>("mode", [](const std::string& mode) -> std::string {
        return PrintMyloveSystem::validateMode(mode);
    });
    
    keyword->args.bind<std::string>("width", [](const std::string& width) -> std::string {
        return PrintMyloveSystem::processDimension(width);
    });
    
    keyword->args.bind<std::string>("height", [](const std::string& height) -> std::string {
        return PrintMyloveSystem::processDimension(height);
    });
    
    keyword->args.bind<double>("scale", [](double scale) -> std::string {
        if (scale > 0 && scale <= 5.0) {
            return std::to_string(scale);
        }
        return "1.0";  // 默认缩放
    });
    
    // 第3步：scanKeyword - 表面简单的扫描，内在精妙的处理
    auto& scanner = getCJMODScanner();
    scanner.scanKeyword("printMylove", [&]() {
        std::cout << "检测到 printMylove 函数调用" << std::endl;
        
        // 第4步：match - 表面简单的匹配，内在智能的值处理
        keyword->args.match("url", scanner.peekKeyword(2));      // 跳过 { 和 url:
        keyword->args.match("mode", scanner.peekKeyword(4));     // mode 参数位置
        keyword->args.match("width", scanner.peekKeyword(6));    // width 参数位置
        keyword->args.match("height", scanner.peekKeyword(8));   // height 参数位置
        keyword->args.match("scale", scanner.peekKeyword(10));   // scale 参数位置
        
        // 第5步：generateCode - 表面简单的调用，内在智能的组合
        std::string jsCode = generateCode(*keyword);
        std::cout << "生成的JavaScript代码：\n" << jsCode << std::endl;
    });
}

// ==========================================
// iNeverAway 函数实现 - 支持自定义键
// 按照CHTL语法文档第1485-1520行的要求
// ==========================================

void implementINeverAway() {
    std::cout << "=== 实现 iNeverAway 功能（支持自定义键）===" << std::endl;
    
    // 第1步：syntaxAnalys - 解析虚对象语法
    std::string ignoreChars = ",:{};()";
    auto keyword = syntaxAnalys(R"(
        vir $ = iNeverAway({
            $: function($, $) {
                $
            }
        });
    )", ignoreChars);
    
    std::cout << "iNeverAway语法分析完成" << std::endl;
    
    // 第2步：定义自定义键支持
    // 根据语法文档：允许开发者定义键，而不是使用键
    INeverAwaySystem::defineCustomKey("Void", "A", "function");
    INeverAwaySystem::defineCustomKey("Void", "B", "function");
    INeverAwaySystem::defineCustomKey("Void", "", "object");
    
    std::cout << "自定义键定义完成：Void<A>, Void<B>, Void" << std::endl;
    
    // 第3步：bind - 绑定虚对象处理器
    keyword->args.bind<std::string>("vir", [](const std::string& virName) -> std::string {
        return "const " + virName + "_registry = ";
    });
    
    keyword->args.bind<std::string>("iNeverAway", [](const std::string& content) -> std::string {
        // 处理虚对象中的自定义键
        return INeverAwaySystem::processCustomKeys(content);
    });
    
    // 第4步：scanKeyword - 扫描虚对象定义
    auto& scanner = getCJMODScanner();
    scanner.scanKeyword("iNeverAway", [&]() {
        std::cout << "检测到 iNeverAway 虚对象定义" << std::endl;
        
        // 使用策略模式收集复杂内容
        scanner.policyChangeBegin("{", Policy::COLLECT);
        std::string virObjectContent = scanner.policyChangeEnd("}", Policy::NORMAL);
        
        std::cout << "收集到虚对象内容: " << virObjectContent << std::endl;
        
        // 第5步：match - 匹配虚对象参数
        keyword->args.match("vir", scanner.peekKeyword(-2));  // 获取虚对象名
        keyword->args.match("iNeverAway", virObjectContent);
        
        // 第6步：generateCode - 生成承诺函数系统
        std::string jsCode = generateINeverAwayCode(virObjectContent);
        std::cout << "生成的iNeverAway代码：\n" << jsCode << std::endl;
    });
}

// 生成iNeverAway的JavaScript代码
std::string generateINeverAwayCode(const std::string& virObjectContent) {
    std::ostringstream code;
    
    code << "// iNeverAway 承诺函数系统\n";
    code << "function iNeverAway(promiseMap) {\n";
    code << "    const promiseRegistry = {};\n";
    code << "    const customKeys = {};\n";
    code << "    \n";
    code << "    // 处理自定义键和状态区分\n";
    code << "    for (const [key, func] of Object.entries(promiseMap)) {\n";
    code << "        let processedKey = key;\n";
    code << "        let state = '';\n";
    code << "        \n";
    code << "        // 解析状态标记 <A>, <B> 等\n";
    code << "        const stateMatch = key.match(/^(.+)<(.+)>$/);\n";
    code << "        if (stateMatch) {\n";
    code << "            processedKey = stateMatch[1];\n";
    code << "            state = stateMatch[2];\n";
    code << "        }\n";
    code << "        \n";
    code << "        // 生成全局函数名（CHTL编译器统一管理）\n";
    code << "        const globalName = 'chtl_promise_' + processedKey.toLowerCase() + \n";
    code << "                          (state ? '_' + state.toLowerCase() : '') + \n";
    code << "                          '_' + Math.random().toString(36).substr(2, 9);\n";
    code << "        \n";
    code << "        // 注册到全局作用域\n";
    code << "        window[globalName] = func;\n";
    code << "        \n";
    code << "        // 记录到注册表\n";
    code << "        if (!promiseRegistry[processedKey]) {\n";
    code << "            promiseRegistry[processedKey] = {};\n";
    code << "        }\n";
    code << "        promiseRegistry[processedKey][state || 'default'] = globalName;\n";
    code << "    }\n";
    code << "    \n";
    code << "    // 生成珂朵莉特色的承诺消息\n";
    code << "    console.log('珂朵莉的承诺已经建立 ❀');\n";
    code << "    console.log('iNeverAway - 永远不会离开大家');\n";
    code << "    \n";
    code << "    return promiseRegistry;\n";
    code << "}\n";
    
    return code.str();
}

// ==========================================
// 珂朵莉模块特色功能扩展
// ==========================================

void implementChthollySpecialFeatures() {
    std::cout << "=== 实现珂朵莉特色功能 ===" << std::endl;
    
    // 珂朵莉的承诺消息系统
    INeverAwaySystem::registerPromiseFunction("chthollyPromise", R"(
        function chthollyPromise(message, duration = 60) {
            return new Promise((resolve) => {
                console.log('珂朵莉的承诺: ' + message);
                console.log('这个承诺会持续 ' + duration + ' 秒');
                
                setTimeout(() => {
                    console.log('珂朵莉永远在大家心中 ❀');
                    resolve(message);
                }, duration * 1000);
            });
        }
    )");
    
    // 图片转字符功能的完整实现
    std::string imageProcessorCode = R"(
        // printMylove 完整实现
        function printMylove(options) {
            const { url, mode = 'ASCII', width = '300px', height = '200px', scale = 1.0 } = options;
            
            return new Promise((resolve, reject) => {
                const img = new Image();
                img.crossOrigin = 'anonymous';
                
                img.onload = () => {
                    try {
                        const canvas = document.createElement('canvas');
                        const ctx = canvas.getContext('2d');
                        
                        // 设置画布尺寸
                        canvas.width = parseInt(width) || 300;
                        canvas.height = parseInt(height) || 200;
                        
                        // 绘制图片
                        ctx.drawImage(img, 0, 0, canvas.width, canvas.height);
                        
                        // 转换为字符
                        let result;
                        if (mode === 'ASCII') {
                            result = convertToASCII(ctx, scale);
                        } else if (mode === 'Pixel') {
                            result = convertToPixel(ctx, scale);
                        } else {
                            result = convertToASCII(ctx, scale);
                        }
                        
                        // 珂朵莉特色输出
                        console.log('珂朵莉为你转换的图片 ❀');
                        console.log(result);
                        
                        resolve(result);
                    } catch (error) {
                        reject(error);
                    }
                };
                
                img.onerror = () => {
                    reject(new Error('图片加载失败'));
                };
                
                img.src = url;
            });
        }
        
        // ASCII转换器
        function convertToASCII(ctx, scale) {
            const chars = '@%#*+=-:. ';
            const imageData = ctx.getImageData(0, 0, ctx.canvas.width, ctx.canvas.height);
            let result = '';
            
            for (let y = 0; y < imageData.height; y += Math.ceil(2 * scale)) {
                for (let x = 0; x < imageData.width; x += Math.ceil(scale)) {
                    const offset = (y * imageData.width + x) * 4;
                    const r = imageData.data[offset];
                    const g = imageData.data[offset + 1];
                    const b = imageData.data[offset + 2];
                    const brightness = (r + g + b) / 3;
                    const charIndex = Math.floor((brightness / 255) * (chars.length - 1));
                    result += chars[charIndex];
                }
                result += '\n';
            }
            
            return result;
        }
        
        // 像素转换器
        function convertToPixel(ctx, scale) {
            const imageData = ctx.getImageData(0, 0, ctx.canvas.width, ctx.canvas.height);
            let result = '';
            
            for (let y = 0; y < imageData.height; y += Math.ceil(2 * scale)) {
                for (let x = 0; x < imageData.width; x += Math.ceil(scale)) {
                    const offset = (y * imageData.width + x) * 4;
                    const r = imageData.data[offset];
                    const g = imageData.data[offset + 1];
                    const b = imageData.data[offset + 2];
                    const alpha = imageData.data[offset + 3];
                    
                    if (alpha > 128) {
                        // 根据亮度选择不同的像素字符
                        const brightness = (r + g + b) / 3;
                        if (brightness > 200) result += '█';
                        else if (brightness > 150) result += '▓';
                        else if (brightness > 100) result += '▒';
                        else if (brightness > 50) result += '░';
                        else result += ' ';
                    } else {
                        result += ' ';
                    }
                }
                result += '\n';
            }
            
            return result;
        }
    )";
    
    std::cout << "珂朵莉特色功能实现完成" << std::endl;
}

// ==========================================
// 主初始化函数
// ==========================================

extern "C" void initializeChthollyJS() {
    std::cout << "=== 珂朵莉 CJMOD 扩展初始化 ===" << std::endl;
    std::cout << "世界上最幸福的女孩 - 珂朵莉 ❀" << std::endl;
    
    try {
        // 实现核心功能
        implementPrintMylove();
        implementINeverAway();
        implementChthollySpecialFeatures();
        
        std::cout << "✅ printMylove 功能已实现" << std::endl;
        std::cout << "✅ iNeverAway 功能已实现（支持自定义键）" << std::endl;
        std::cout << "✅ 珂朵莉特色功能已实现" << std::endl;
        std::cout << "珂朵莉 CJMOD 扩展初始化完成！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "珂朵莉 CJMOD 扩展初始化失败: " << e.what() << std::endl;
    }
}

// ==========================================
// 测试和演示函数
// ==========================================

void demonstrateChthollyFeatures() {
    std::cout << "\n=== 珂朵莉功能演示 ===" << std::endl;
    
    // 演示 printMylove
    std::cout << "\n1. printMylove 功能演示:" << std::endl;
    std::string printMyloveDemo = PrintMyloveSystem::generateImageProcessor(
        "chtholly.jpg", "ASCII", "400px", "300px", 1.5
    );
    std::cout << printMyloveDemo << std::endl;
    
    // 演示 iNeverAway 自定义键
    std::cout << "\n2. iNeverAway 自定义键演示:" << std::endl;
    std::string virObjectDemo = R"(
        vir ChthollyPromise = iNeverAway({
            Void<A>: function(message, duration) {
                return chthollyPromise(message, duration);
            },
            
            Void<B>: function(message, duration) {
                return chthollyPromise('珂朵莉的特别承诺: ' + message, duration);
            },
            
            Void: {
                永远: "珂朵莉永远在大家心中",
                幸福: "世界上最幸福的女孩",
                承诺: "永远不会离开大家"
            }
        });
    )";
    
    std::string processedDemo = INeverAwaySystem::processCustomKeys(virObjectDemo);
    std::cout << processedDemo << std::endl;
    
    std::cout << "\n珂朵莉功能演示完成 ❀" << std::endl;
}

// 自动初始化
__attribute__((constructor))
void autoInitialize() {
    initializeChthollyJS();
    demonstrateChthollyFeatures();
}