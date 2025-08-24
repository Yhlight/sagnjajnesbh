// 珂朵莉模块 CJMOD 扩展实现
// 实现 printMylove 和 iNeverAway 等珂朵莉特色 CHTL JS 功能

#include "CJMOD/SimpleCJMODApi.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

using namespace CHTL::CJMOD;

// ========== printMylove 函数实现 ==========
// 按照您的原始API设计：syntaxAnalys -> bind -> scanKeyword -> match -> transform -> result -> generateCode

void implementPrintMylove() {
    // 1. syntaxAnalys - 分析 printMylove 语法结构
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
    
    // 2. bind - 绑定参数处理函数
    // 按照您的设计：支持不同类型的参数处理
    keyword->args.bind<std::string>("url", [](const std::string& str) -> std::string {
        // URL 参数处理：支持相对路径和绝对路径
        if (str.find("http") == 0) {
            return "'" + str + "'";  // 绝对URL
        } else {
            return "'./" + str + "'";  // 相对路径
        }
    });
    
    keyword->args.bind<std::string>("mode", [](const std::string& mode) -> std::string {
        // 模式参数：ASCII 或 Pixel
        if (mode == "ASCII" || mode == "Pixel") {
            return "'" + mode + "'";
        } else {
            return "'ASCII'";  // 默认模式
        }
    });
    
    keyword->args.bind<std::string>("width", [](const std::string& width) -> std::string {
        // 宽度参数：支持CSS单位、百分比、数字
        std::regex cssUnitRegex(R"(\d+(\.\d+)?(px|%|em|rem|vw|vh))");
        if (std::regex_match(width, cssUnitRegex)) {
            return "'" + width + "'";
        } else if (std::all_of(width.begin(), width.end(), ::isdigit)) {
            return width + " + 'px'";  // 纯数字加px
        } else {
            return "'300px'";  // 默认宽度
        }
    });
    
    keyword->args.bind<std::string>("height", [](const std::string& height) -> std::string {
        // 高度参数：同宽度处理
        std::regex cssUnitRegex(R"(\d+(\.\d+)?(px|%|em|rem|vw|vh))");
        if (std::regex_match(height, cssUnitRegex)) {
            return "'" + height + "'";
        } else if (std::all_of(height.begin(), height.end(), ::isdigit)) {
            return height + " + 'px'";
        } else {
            return "'200px'";  // 默认高度
        }
    });
    
    keyword->args.bind<double>("scale", [](double scale) -> std::string {
        // 缩放参数：等比缩放策略
        if (scale > 0 && scale <= 5.0) {
            return std::to_string(scale);
        } else {
            return "1.0";  // 默认缩放
        }
    });
    
    // 3. scanKeyword - 扫描关键字并处理
    auto& scanner = getCJMODScanner();
    scanner.scanKeyword(keyword->args[0], [&]() {
        // 扫描到 printMylove 时的处理
        std::cout << "检测到 printMylove 函数调用" << std::endl;
        
        // 4. match - 匹配参数值
        // 使用 peekKeyword 获取参数值
        keyword->args.match("url", scanner.peekKeyword(2));      // 跳过 { 和 url:
        keyword->args.match("mode", scanner.peekKeyword(4));     // mode 参数位置
        keyword->args.match("width", scanner.peekKeyword(6));    // width 参数位置
        keyword->args.match("height", scanner.peekKeyword(8));   // height 参数位置
        keyword->args.match("scale", scanner.peekKeyword(10));   // scale 参数位置
    });
}

// 处理复杂参数值（函数和对象）
void handleComplexPrintMylove() {
    auto& scanner = getCJMODScanner();
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
    
    // 按照您的设计：使用 policy 处理复杂参数
    scanner.scanKeyword("printMylove", [&]() {
        std::vector<std::string> paramValues;
        
        // 使用策略模式收集参数值
        for (size_t i = 0; i < keyword->args.length(); i++) {
            scanner.policyChangeBegin(":", Policy::COLLECT);
            
            // 检查参数类型并相应处理
            std::string value = scanner.policyChangeEnd(",", Policy::NORMAL);
            
            if (scanner.isFunction(value)) {
                // 函数参数处理
                paramValues.push_back("(" + value + ")()");
            } else if (scanner.isObject(value)) {
                // 对象参数处理
                paramValues.push_back("JSON.stringify(" + value + ")");
            } else {
                // 普通参数处理
                paramValues.push_back(value);
            }
        }
        
        // 匹配处理后的参数值
        if (paramValues.size() >= 5) {
            keyword->args.match("url", paramValues[0]);
            keyword->args.match("mode", paramValues[1]);
            keyword->args.match("width", paramValues[2]);
            keyword->args.match("height", paramValues[3]);
            keyword->args.match("scale", paramValues[4]);
        }
    });
}

// 5. transform - 转换为JS代码
std::string transformPrintMylove(const Syntax& syntax) {
    // 生成最终的JavaScript代码
    std::ostringstream jsCode;
    
    jsCode << "{\n";
    jsCode << "  // 珂朵莉的 printMylove 函数实现\n";
    jsCode << "  const printMyloveConfig = {\n";
    jsCode << "    url: " << syntax.args[1].getValue() << ",\n";
    jsCode << "    mode: " << syntax.args[2].getValue() << ",\n";
    jsCode << "    width: " << syntax.args[3].getValue() << ",\n";
    jsCode << "    height: " << syntax.args[4].getValue() << ",\n";
    jsCode << "    scale: " << syntax.args[5].getValue() << "\n";
    jsCode << "  };\n\n";
    
    jsCode << "  // 创建图像显示容器\n";
    jsCode << "  const container = document.createElement('div');\n";
    jsCode << "  container.className = 'printmylove-container';\n";
    jsCode << "  container.style.cssText = `\n";
    jsCode << "    position: relative;\n";
    jsCode << "    width: ${printMyloveConfig.width};\n";
    jsCode << "    height: ${printMyloveConfig.height};\n";
    jsCode << "    transform: scale(${printMyloveConfig.scale});\n";
    jsCode << "    margin: 20px auto;\n";
    jsCode << "    border: 2px solid var(--chtholly-accent, #ff6b9d);\n";
    jsCode << "    border-radius: 12px;\n";
    jsCode << "    overflow: hidden;\n";
    jsCode << "    box-shadow: 0 8px 32px rgba(255, 107, 157, 0.3);\n";
    jsCode << "  `;\n\n";
    
    jsCode << "  if (printMyloveConfig.mode === 'ASCII') {\n";
    jsCode << "    // ASCII 模式实现\n";
    jsCode << "    fetch(printMyloveConfig.url)\n";
    jsCode << "      .then(response => response.blob())\n";
    jsCode << "      .then(blob => {\n";
    jsCode << "        const img = new Image();\n";
    jsCode << "        img.onload = () => {\n";
    jsCode << "          const canvas = document.createElement('canvas');\n";
    jsCode << "          const ctx = canvas.getContext('2d');\n";
    jsCode << "          canvas.width = 80; canvas.height = 40;\n";
    jsCode << "          ctx.drawImage(img, 0, 0, 80, 40);\n";
    jsCode << "          \n";
    jsCode << "          const ascii = convertToASCII(ctx.getImageData(0, 0, 80, 40));\n";
    jsCode << "          const pre = document.createElement('pre');\n";
    jsCode << "          pre.style.cssText = 'font-family: monospace; font-size: 8px; line-height: 1; color: var(--chtholly-text);';\n";
    jsCode << "          pre.textContent = ascii;\n";
    jsCode << "          container.appendChild(pre);\n";
    jsCode << "        };\n";
    jsCode << "        img.src = URL.createObjectURL(blob);\n";
    jsCode << "      });\n";
    jsCode << "  } else {\n";
    jsCode << "    // Pixel 模式实现\n";
    jsCode << "    const img = document.createElement('img');\n";
    jsCode << "    img.src = printMyloveConfig.url;\n";
    jsCode << "    img.style.cssText = 'width: 100%; height: 100%; object-fit: cover;';\n";
    jsCode << "    \n";
    jsCode << "    // 添加珂朵莉主题的爱心装饰\n";
    jsCode << "    const heartOverlay = document.createElement('div');\n";
    jsCode << "    heartOverlay.innerHTML = '💖';\n";
    jsCode << "    heartOverlay.style.cssText = `\n";
    jsCode << "      position: absolute;\n";
    jsCode << "      top: 10px; right: 10px;\n";
    jsCode << "      font-size: 24px;\n";
    jsCode << "      animation: heartbeat 1.5s ease-in-out infinite;\n";
    jsCode << "    `;\n";
    jsCode << "    \n";
    jsCode << "    container.appendChild(img);\n";
    jsCode << "    container.appendChild(heartOverlay);\n";
    jsCode << "  }\n\n";
    
    jsCode << "  // 添加标题\n";
    jsCode << "  const title = document.createElement('div');\n";
    jsCode << "  title.textContent = '珂朵莉的珍贵回忆 ❀';\n";
    jsCode << "  title.style.cssText = `\n";
    jsCode << "    position: absolute;\n";
    jsCode << "    bottom: 0; left: 0; right: 0;\n";
    jsCode << "    background: linear-gradient(transparent, rgba(0,0,0,0.8));\n";
    jsCode << "    color: white; text-align: center;\n";
    jsCode << "    padding: 20px 10px 10px;\n";
    jsCode << "    font-family: 'Comic Sans MS', cursive;\n";
    jsCode << "  `;\n";
    jsCode << "  container.appendChild(title);\n\n";
    
    jsCode << "  // 添加到页面\n";
    jsCode << "  document.body.appendChild(container);\n\n";
    
    jsCode << "  // ASCII 转换辅助函数\n";
    jsCode << "  function convertToASCII(imageData) {\n";
    jsCode << "    const chars = ' .:-=+*#%@';\n";
    jsCode << "    let ascii = '';\n";
    jsCode << "    for (let y = 0; y < imageData.height; y++) {\n";
    jsCode << "      for (let x = 0; x < imageData.width; x++) {\n";
    jsCode << "        const i = (y * imageData.width + x) * 4;\n";
    jsCode << "        const gray = (imageData.data[i] + imageData.data[i+1] + imageData.data[i+2]) / 3;\n";
    jsCode << "        const charIndex = Math.floor(gray / 255 * (chars.length - 1));\n";
    jsCode << "        ascii += chars[charIndex];\n";
    jsCode << "      }\n";
    jsCode << "      ascii += '\\n';\n";
    jsCode << "    }\n";
    jsCode << "    return ascii;\n";
    jsCode << "  }\n";
    jsCode << "}\n";
    
    return jsCode.str();
}

// ========== iNeverAway 函数实现 ==========
// 珂朵莉的永不离去承诺函数

void implementINeverAway() {
    // 1. syntaxAnalys - 分析 iNeverAway 语法结构
    auto keyword = syntaxAnalys(R"(
        iNeverAway($, $);
    )");
    
    // 2. bind - 绑定参数处理函数
    keyword->args.bind<std::string>("$", [](const std::string& message) -> std::string {
        // 第一个参数：承诺消息
        return "'" + message + "'";
    });
    
    keyword->args.bind<int>("$", [](int duration) -> std::string {
        // 第二个参数：持续时间（秒）
        if (duration > 0 && duration <= 3600) {  // 最长1小时
            return std::to_string(duration * 1000);  // 转换为毫秒
        } else {
            return "10000";  // 默认10秒
        }
    });
    
    // 3. scanKeyword - 扫描关键字
    auto& scanner = getCJMODScanner();
    scanner.scanKeyword(keyword->args[0], [&]() {
        // 匹配参数
        keyword->args.match("$", scanner.peekKeyword(1));  // 消息参数
        keyword->args.match("$", scanner.peekKeyword(2));  // 时间参数
    });
}

// iNeverAway 的 transform 实现
std::string transformINeverAway(const Syntax& syntax) {
    std::ostringstream jsCode;
    
    jsCode << "{\n";
    jsCode << "  // 珂朵莉的永不离去承诺\n";
    jsCode << "  const message = " << syntax.args[1].getValue() << ";\n";
    jsCode << "  const duration = " << syntax.args[2].getValue() << ";\n\n";
    
    jsCode << "  // 创建承诺显示容器\n";
    jsCode << "  const promiseContainer = document.createElement('div');\n";
    jsCode << "  promiseContainer.className = 'ineveraway-promise';\n";
    jsCode << "  promiseContainer.style.cssText = `\n";
    jsCode << "    position: fixed;\n";
    jsCode << "    top: 50%; left: 50%;\n";
    jsCode << "    transform: translate(-50%, -50%);\n";
    jsCode << "    background: linear-gradient(135deg, #ffb3d9, #ffc0cb);\n";
    jsCode << "    color: #654321;\n";
    jsCode << "    padding: 30px;\n";
    jsCode << "    border-radius: 20px;\n";
    jsCode << "    box-shadow: 0 10px 40px rgba(255, 179, 217, 0.6);\n";
    jsCode << "    text-align: center;\n";
    jsCode << "    font-family: 'Comic Sans MS', cursive;\n";
    jsCode << "    z-index: 10000;\n";
    jsCode << "    animation: promiseGlow 2s ease-in-out infinite alternate;\n";
    jsCode << "    max-width: 400px;\n";
    jsCode << "  `;\n\n";
    
    jsCode << "  // 添加承诺内容\n";
    jsCode << "  promiseContainer.innerHTML = `\n";
    jsCode << "    <div style='font-size: 2em; margin-bottom: 15px;'>💖</div>\n";
    jsCode << "    <h3 style='margin: 0 0 15px 0; color: #8b4513;'>珂朵莉的承诺</h3>\n";
    jsCode << "    <p style='margin: 0 0 20px 0; font-size: 1.2em; line-height: 1.6;'>${message}</p>\n";
    jsCode << "    <div class='promise-timer' style='font-size: 0.9em; color: #999;'>承诺将持续 <span class='countdown'>${Math.floor(duration/1000)}</span> 秒</div>\n";
    jsCode << "  `;\n\n";
    
    jsCode << "  // 添加CSS动画\n";
    jsCode << "  if (!document.querySelector('#ineveraway-styles')) {\n";
    jsCode << "    const styles = document.createElement('style');\n";
    jsCode << "    styles.id = 'ineveraway-styles';\n";
    jsCode << "    styles.textContent = `\n";
    jsCode << "      @keyframes promiseGlow {\n";
    jsCode << "        0% { box-shadow: 0 10px 40px rgba(255, 179, 217, 0.6); }\n";
    jsCode << "        100% { box-shadow: 0 15px 50px rgba(255, 179, 217, 0.9); }\n";
    jsCode << "      }\n";
    jsCode << "      @keyframes heartPulse {\n";
    jsCode << "        0%, 100% { transform: scale(1); }\n";
    jsCode << "        50% { transform: scale(1.1); }\n";
    jsCode << "      }\n";
    jsCode << "    `;\n";
    jsCode << "    document.head.appendChild(styles);\n";
    jsCode << "  }\n\n";
    
    jsCode << "  // 添加到页面\n";
    jsCode << "  document.body.appendChild(promiseContainer);\n\n";
    
    jsCode << "  // 倒计时功能\n";
    jsCode << "  let timeLeft = duration;\n";
    jsCode << "  const countdown = promiseContainer.querySelector('.countdown');\n";
    jsCode << "  const timer = setInterval(() => {\n";
    jsCode << "    timeLeft -= 1000;\n";
    jsCode << "    countdown.textContent = Math.floor(timeLeft / 1000);\n";
    jsCode << "    \n";
    jsCode << "    if (timeLeft <= 0) {\n";
    jsCode << "      clearInterval(timer);\n";
    jsCode << "      \n";
    jsCode << "      // 承诺完成效果\n";
    jsCode << "      promiseContainer.style.animation = 'none';\n";
    jsCode << "      promiseContainer.style.background = 'linear-gradient(135deg, #ffd700, #ffed4e)';\n";
    jsCode << "      countdown.textContent = '承诺已完成！';\n";
    jsCode << "      \n";
    jsCode << "      // 3秒后自动消失\n";
    jsCode << "      setTimeout(() => {\n";
    jsCode << "        promiseContainer.style.opacity = '0';\n";
    jsCode << "        promiseContainer.style.transform = 'translate(-50%, -50%) scale(0.8)';\n";
    jsCode << "        promiseContainer.style.transition = 'all 0.5s ease';\n";
    jsCode << "        \n";
    jsCode << "        setTimeout(() => {\n";
    jsCode << "          if (promiseContainer.parentNode) {\n";
    jsCode << "            document.body.removeChild(promiseContainer);\n";
    jsCode << "          }\n";
    jsCode << "        }, 500);\n";
    jsCode << "      }, 3000);\n";
    jsCode << "    }\n";
    jsCode << "  }, 1000);\n\n";
    
    jsCode << "  // 点击关闭功能\n";
    jsCode << "  promiseContainer.addEventListener('click', () => {\n";
    jsCode << "    clearInterval(timer);\n";
    jsCode << "    promiseContainer.style.opacity = '0';\n";
    jsCode << "    promiseContainer.style.transform = 'translate(-50%, -50%) scale(0.8)';\n";
    jsCode << "    promiseContainer.style.transition = 'all 0.3s ease';\n";
    jsCode << "    \n";
    jsCode << "    setTimeout(() => {\n";
    jsCode << "      if (promiseContainer.parentNode) {\n";
    jsCode << "        document.body.removeChild(promiseContainer);\n";
    jsCode << "      }\n";
    jsCode << "    }, 300);\n";
    jsCode << "  });\n\n";
    
    jsCode << "  // 记录承诺到控制台\n";
    jsCode << "  console.log(`💖 珂朵莉的承诺: ${message} (持续 ${duration/1000} 秒)`);\n";
    jsCode << "}\n";
    
    return jsCode.str();
}

// ========== CJMOD 扩展主入口 ==========
// 按照您的原始设计：完整的流程实现

extern "C" {
    // CJMOD 扩展初始化函数
    void initChthollyExtensions() {
        std::cout << "初始化珂朵莉 CJMOD 扩展..." << std::endl;
        
        // 初始化 printMylove 扩展
        implementPrintMylove();
        
        // 初始化 iNeverAway 扩展
        implementINeverAway();
        
        std::cout << "珂朵莉 CJMOD 扩展加载完成 - 世界上最幸福的女孩 ❀" << std::endl;
    }
    
    // CJMOD 代码生成函数
    const char* generateChthollyJS(const char* syntaxPattern, const char* functionName) {
        static std::string result;
        
        try {
            auto syntax = syntaxAnalys(syntaxPattern);
            
            if (std::string(functionName) == "printMylove") {
                result = transformPrintMylove(*syntax);
            } else if (std::string(functionName) == "iNeverAway") {
                result = transformINeverAway(*syntax);
            } else {
                result = generateCode(*syntax);  // 使用默认生成器
            }
            
            return result.c_str();
        } catch (const std::exception& e) {
            static std::string error = "Error: " + std::string(e.what());
            return error.c_str();
        }
    }
}

// ========== 测试和验证功能 ==========
#ifdef CHTHOLLY_CJMOD_TEST
int main() {
    std::cout << "测试珂朵莉 CJMOD 扩展..." << std::endl;
    
    // 测试 printMylove
    std::string printMylovePattern = R"(
        printMylove({
            url: "chtholly.jpg",
            mode: "Pixel",
            width: "400px",
            height: "300px",
            scale: 1.2
        });
    )";
    
    const char* printMyloveJS = generateChthollyJS(printMylovePattern.c_str(), "printMylove");
    std::cout << "生成的 printMylove JS 代码:\n" << printMyloveJS << std::endl;
    
    // 测试 iNeverAway
    std::string iNeverAwayPattern = "iNeverAway(\"我永远不会离开你\", 30);";
    const char* iNeverAwayJS = generateChthollyJS(iNeverAwayPattern.c_str(), "iNeverAway");
    std::cout << "生成的 iNeverAway JS 代码:\n" << iNeverAwayJS << std::endl;
    
    return 0;
}
#endif