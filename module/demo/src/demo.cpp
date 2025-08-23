#include "demo.h"
#include <iostream>
#include <sstream>
#include <regex>

namespace CHTL {
namespace CJMOD {
namespace Demo {

// ============ 全局注册表实现 ============
std::unordered_map<std::string, std::string> g_demoFunctionRegistry;
std::unordered_map<std::string, std::string> g_animationRegistry;

// ============ PrintMyloveProcessor实现 ============

PrintMyloveProcessor::PrintMyloveProcessor() : CHTLJSFunction("printMylove") {
    setupParameterProcessors();
}

void PrintMyloveProcessor::setupParameterProcessors() {
    // 设置参数处理器
    bind("url", [](const std::string& value) {
        // 确保URL被正确引用
        if (value.front() != '"' && value.front() != '\'') {
            return "\"" + value + "\"";
        }
        return value;
    });
    
    bind("mode", [](const std::string& value) {
        // 验证模式参数
        std::string cleanValue = value;
        if (cleanValue.front() == '"' || cleanValue.front() == '\'') {
            cleanValue = cleanValue.substr(1, cleanValue.length() - 2);
        }
        return (cleanValue == "ASCII" || cleanValue == "Pixel") ? "\"" + cleanValue + "\"" : "\"ASCII\"";
    });
    
    bind("width", [](const std::string& value) {
        return value;
    });
    
    bind("height", [](const std::string& value) {
        return value;
    });
    
    bind("scale", [](const std::string& value) {
        return value;
    });
}

std::string PrintMyloveProcessor::processFragment(const Scanner::CodeFragment& fragment) {
    std::cout << "🎨 PrintMyloveProcessor: 处理来自统一扫描器的CodeFragment" << std::endl;
    std::cout << "   片段类型: CHTL_JS" << std::endl;
    std::cout << "   片段内容: " << fragment.content.substr(0, 50) << "..." << std::endl;
    
    // 使用父类的processFragment方法，它会调用正确的CJMOD API流程
    return CHTLJSFunction::processFragment(fragment);
}

void PrintMyloveProcessor::registerToScanner(Scanner::CHTLUnifiedScanner& scanner) {
    // 注册关键字到统一扫描器
    registerKeyword("printMylove", scanner);
    std::cout << "✅ printMylove已注册到统一扫描器" << std::endl;
}

std::string PrintMyloveProcessor::generateImageProcessingJS(const std::unordered_map<std::string, std::string>& params) {
    std::ostringstream js;
    
    js << "function printMylove(config) {\n";
    js << "    console.log('🎨 printMylove: 开始处理图像转字符');\n";
    js << "    \n";
    js << "    const canvas = document.createElement('canvas');\n";
    js << "    const ctx = canvas.getContext('2d');\n";
    js << "    const img = new Image();\n";
    js << "    \n";
    js << "    img.crossOrigin = 'anonymous';\n";
    js << "    \n";
    js << "    img.onload = function() {\n";
    js << "        try {\n";
    js << "            // 设置画布尺寸\n";
    js << "            const targetWidth = parseInt(config.width) || 100;\n";
    js << "            const targetHeight = parseInt(config.height) || 100;\n";
    js << "            \n";
    js << "            canvas.width = targetWidth;\n";
    js << "            canvas.height = targetHeight;\n";
    js << "            \n";
    js << "            // 绘制图像\n";
    js << "            ctx.drawImage(img, 0, 0, targetWidth, targetHeight);\n";
    js << "            \n";
    js << "            // 获取图像数据\n";
    js << "            const imageData = ctx.getImageData(0, 0, targetWidth, targetHeight);\n";
    js << "            const pixels = imageData.data;\n";
    js << "            \n";
    js << "            let result = '';\n";
    js << "            const step = Math.max(1, Math.floor(config.scale || 1));\n";
    js << "            \n";
    js << "            for (let y = 0; y < targetHeight; y += step) {\n";
    js << "                for (let x = 0; x < targetWidth; x += step) {\n";
    js << "                    const i = (y * targetWidth + x) * 4;\n";
    js << "                    const r = pixels[i];\n";
    js << "                    const g = pixels[i + 1];\n";
    js << "                    const b = pixels[i + 2];\n";
    js << "                    const brightness = (r + g + b) / 3;\n";
    js << "                    \n";
    js << "                    if (config.mode === 'ASCII') {\n";
    js << "                        const chars = '@%#*+=-:. ';\n";
    js << "                        const charIndex = Math.floor(brightness / 255 * (chars.length - 1));\n";
    js << "                        result += chars[charIndex];\n";
    js << "                    } else {\n";
    js << "                        result += brightness > 128 ? '█' : ' ';\n";
    js << "                    }\n";
    js << "                }\n";
    js << "                result += '\\n';\n";
    js << "            }\n";
    js << "            \n";
    js << "            console.log('✅ printMylove: 图像转换完成');\n";
    js << "            console.log(result);\n";
    js << "            return result;\n";
    js << "            \n";
    js << "        } catch (error) {\n";
    js << "            console.error('❌ printMylove: 处理失败', error);\n";
    js << "            return 'Error: ' + error.message;\n";
    js << "        }\n";
    js << "    };\n";
    js << "    \n";
    js << "    img.onerror = function() {\n";
    js << "        console.error('❌ printMylove: 图像加载失败');\n";
    js << "        return 'Error: Failed to load image';\n";
    js << "    };\n";
    js << "    \n";
    js << "    img.src = config.url;\n";
    js << "    return 'printMylove processing started...';\n";
    js << "}\n";
    
    return js.str();
}

// ============ INeverAwayProcessor实现 ============

INeverAwayProcessor::INeverAwayProcessor() : CHTLJSFunction("iNeverAway") {
    setupStateProcessors();
}

void INeverAwayProcessor::setupStateProcessors() {
    // 设置状态处理器
    bind("state", [](const std::string& value) {
        return value;
    });
    
    bind("function", [](const std::string& value) {
        return value;
    });
}

std::string INeverAwayProcessor::processFragment(const Scanner::CodeFragment& fragment) {
    std::cout << "💫 INeverAwayProcessor: 处理来自统一扫描器的CodeFragment" << std::endl;
    std::cout << "   片段类型: CHTL_JS" << std::endl;
    std::cout << "   片段内容: " << fragment.content.substr(0, 50) << "..." << std::endl;
    
    return CHTLJSFunction::processFragment(fragment);
}

void INeverAwayProcessor::registerToScanner(Scanner::CHTLUnifiedScanner& scanner) {
    registerKeyword("iNeverAway", scanner);
    std::cout << "✅ iNeverAway已注册到统一扫描器" << std::endl;
}

std::string INeverAwayProcessor::generateStateManagementJS(const std::unordered_map<std::string, std::string>& stateMap) {
    std::ostringstream js;
    
    js << "function iNeverAway(stateConfig) {\n";
    js << "    console.log('💫 iNeverAway: 创建状态管理器');\n";
    js << "    \n";
    js << "    const stateManager = {\n";
    js << "        states: {},\n";
    js << "        currentState: null,\n";
    js << "        \n";
    js << "        // 注册状态处理函数\n";
    js << "        register: function(stateName, handler) {\n";
    js << "            this.states[stateName] = handler;\n";
    js << "            console.log('📝 注册状态:', stateName);\n";
    js << "        },\n";
    js << "        \n";
    js << "        // 切换到指定状态\n";
    js << "        switchTo: function(stateName, ...args) {\n";
    js << "            if (this.states[stateName]) {\n";
    js << "                this.currentState = stateName;\n";
    js << "                console.log('🔄 切换状态:', stateName);\n";
    js << "                return this.states[stateName].apply(this, args);\n";
    js << "            } else {\n";
    js << "                console.warn('⚠️ 未知状态:', stateName);\n";
    js << "            }\n";
    js << "        },\n";
    js << "        \n";
    js << "        // 获取当前状态\n";
    js << "        getCurrentState: function() {\n";
    js << "            return this.currentState;\n";
    js << "        }\n";
    js << "    };\n";
    js << "    \n";
    js << "    // 注册配置中的状态\n";
    js << "    for (const [stateName, handler] of Object.entries(stateConfig)) {\n";
    js << "        stateManager.register(stateName, handler);\n";
    js << "    }\n";
    js << "    \n";
    js << "    console.log('✅ iNeverAway: 状态管理器创建完成');\n";
    js << "    return stateManager;\n";
    js << "}\n";
    
    return js.str();
}

// ============ CreateAnimationProcessor实现 ============

CreateAnimationProcessor::CreateAnimationProcessor() : CHTLJSFunction("createAnimation") {
    setupAnimationProcessors();
}

void CreateAnimationProcessor::setupAnimationProcessors() {
    bind("target", [](const std::string& value) {
        return value;
    });
    
    bind("type", [](const std::string& value) {
        return value;
    });
    
    bind("duration", [](const std::string& value) {
        return value;
    });
    
    bind("easing", [](const std::string& value) {
        return value;
    });
}

std::string CreateAnimationProcessor::processFragment(const Scanner::CodeFragment& fragment) {
    std::cout << "🎬 CreateAnimationProcessor: 处理来自统一扫描器的CodeFragment" << std::endl;
    std::cout << "   片段类型: CHTL_JS" << std::endl;
    std::cout << "   片段内容: " << fragment.content.substr(0, 50) << "..." << std::endl;
    
    return CHTLJSFunction::processFragment(fragment);
}

void CreateAnimationProcessor::registerToScanner(Scanner::CHTLUnifiedScanner& scanner) {
    registerKeyword("createAnimation", scanner);
    std::cout << "✅ createAnimation已注册到统一扫描器" << std::endl;
}

std::string CreateAnimationProcessor::generateAnimationJS(const std::unordered_map<std::string, std::string>& animConfig) {
    std::ostringstream js;
    
    js << "function createAnimation(config) {\n";
    js << "    console.log('🎬 createAnimation: 创建动画系统');\n";
    js << "    \n";
    js << "    const animation = {\n";
    js << "        config: config,\n";
    js << "        isPlaying: false,\n";
    js << "        \n";
    js << "        play: function() {\n";
    js << "            return new Promise((resolve) => {\n";
    js << "                console.log('▶️ 播放动画:', this.config.type);\n";
    js << "                this.isPlaying = true;\n";
    js << "                \n";
    js << "                const elements = document.querySelectorAll(this.config.target);\n";
    js << "                const duration = this.config.duration || 1000;\n";
    js << "                const easing = this.config.easing || 'ease';\n";
    js << "                \n";
    js << "                elements.forEach(element => {\n";
    js << "                    element.style.transition = `all ${duration}ms ${easing}`;\n";
    js << "                    \n";
    js << "                    switch (this.config.type) {\n";
    js << "                        case 'fadeInUp':\n";
    js << "                            element.style.opacity = '0';\n";
    js << "                            element.style.transform = 'translateY(30px)';\n";
    js << "                            setTimeout(() => {\n";
    js << "                                element.style.opacity = '1';\n";
    js << "                                element.style.transform = 'translateY(0)';\n";
    js << "                            }, 50);\n";
    js << "                            break;\n";
    js << "                            \n";
    js << "                        case 'pulse':\n";
    js << "                            let pulseCount = 0;\n";
    js << "                            const pulseInterval = setInterval(() => {\n";
    js << "                                element.style.transform = pulseCount % 2 === 0 ? 'scale(1.05)' : 'scale(1)';\n";
    js << "                                pulseCount++;\n";
    js << "                                if (pulseCount >= (this.config.iterations || 2) * 2) {\n";
    js << "                                    clearInterval(pulseInterval);\n";
    js << "                                    element.style.transform = 'scale(1)';\n";
    js << "                                }\n";
    js << "                            }, duration / 4);\n";
    js << "                            break;\n";
    js << "                            \n";
    js << "                        default:\n";
    js << "                            console.warn('⚠️ 未知动画类型:', this.config.type);\n";
    js << "                    }\n";
    js << "                });\n";
    js << "                \n";
    js << "                setTimeout(() => {\n";
    js << "                    this.isPlaying = false;\n";
    js << "                    console.log('✅ 动画完成:', this.config.type);\n";
    js << "                    resolve();\n";
    js << "                }, duration);\n";
    js << "            });\n";
    js << "        },\n";
    js << "        \n";
    js << "        stop: function() {\n";
    js << "            this.isPlaying = false;\n";
    js << "            console.log('⏹️ 停止动画');\n";
    js << "        }\n";
    js << "    };\n";
    js << "    \n";
    js << "    console.log('✅ createAnimation: 动画对象创建完成');\n";
    js << "    return animation;\n";
    js << "}\n";
    
    return js.str();
}

// ============ DemoModuleProcessor实现 ============

DemoModuleProcessor::DemoModuleProcessor() {
    initialize();
}

void DemoModuleProcessor::initialize() {
    std::cout << "🏗️ DemoModuleProcessor: 初始化Demo CJMOD模块" << std::endl;
    
    // 创建各个扩展处理器
    printMyloveProcessor_ = std::make_unique<PrintMyloveProcessor>();
    iNeverAwayProcessor_ = std::make_unique<INeverAwayProcessor>();
    createAnimationProcessor_ = std::make_unique<CreateAnimationProcessor>();
    
    // 设置扩展处理器映射
    setupExtensionHandlers();
    
    std::cout << "✅ Demo CJMOD模块初始化完成" << std::endl;
}

void DemoModuleProcessor::setupExtensionHandlers() {
    extensionHandlers_["printMylove"] = [this](const Scanner::CodeFragment& fragment) {
        return printMyloveProcessor_->processFragment(fragment);
    };
    
    extensionHandlers_["iNeverAway"] = [this](const Scanner::CodeFragment& fragment) {
        return iNeverAwayProcessor_->processFragment(fragment);
    };
    
    extensionHandlers_["createAnimation"] = [this](const Scanner::CodeFragment& fragment) {
        return createAnimationProcessor_->processFragment(fragment);
    };
}

void DemoModuleProcessor::registerAllExtensions(Scanner::CHTLUnifiedScanner& scanner) {
    std::cout << "📝 注册所有Demo扩展到统一扫描器" << std::endl;
    
    printMyloveProcessor_->registerToScanner(scanner);
    iNeverAwayProcessor_->registerToScanner(scanner);
    createAnimationProcessor_->registerToScanner(scanner);
    
    std::cout << "✅ 所有Demo扩展已注册到统一扫描器" << std::endl;
}

std::string DemoModuleProcessor::processCodeFragment(const Scanner::CodeFragment& fragment, const std::string& functionName) {
    std::cout << "⚙️ DemoModuleProcessor: 处理CodeFragment" << std::endl;
    std::cout << "   函数名: " << functionName << std::endl;
    std::cout << "   片段类型: " << static_cast<int>(fragment.type) << std::endl;
    
    auto it = extensionHandlers_.find(functionName);
    if (it != extensionHandlers_.end()) {
        return it->second(fragment);
    } else {
        std::cout << "⚠️ 未知的扩展函数: " << functionName << std::endl;
        return "// Unknown CJMOD extension: " + functionName;
    }
}

std::vector<std::string> DemoModuleProcessor::getSupportedExtensions() const {
    return {"printMylove", "iNeverAway", "createAnimation"};
}

// ============ 便利函数实现 ============

std::unique_ptr<DemoModuleProcessor> createDemoProcessor() {
    return std::make_unique<DemoModuleProcessor>();
}

std::string processPrintMylove(const Scanner::CodeFragment& fragment) {
    PrintMyloveProcessor processor;
    return processor.processFragment(fragment);
}

std::string processINeverAway(const Scanner::CodeFragment& fragment) {
    INeverAwayProcessor processor;
    return processor.processFragment(fragment);
}

std::string processCreateAnimation(const Scanner::CodeFragment& fragment) {
    CreateAnimationProcessor processor;
    return processor.processFragment(fragment);
}

void registerDemoExtensions(Scanner::CHTLUnifiedScanner& scanner) {
    auto processor = createDemoProcessor();
    processor->registerAllExtensions(scanner);
}

void demonstrateArchitectureAdvantages() {
    std::cout << "\n🏆 CHTL架构优势演示" << std::endl;
    std::cout << "================================" << std::endl;
    
    std::cout << "\n🎯 统一扫描器职责:" << std::endl;
    std::cout << "   ✅ 代码片段切割和分类" << std::endl;
    std::cout << "   ✅ 状态机驱动的扫描" << std::endl;
    std::cout << "   ✅ 双指针滑动窗口机制" << std::endl;
    std::cout << "   ❌ 不进行词法分析（这是关键！）" << std::endl;
    
    std::cout << "\n⚙️ CJMOD API职责:" << std::endl;
    std::cout << "   ✅ 接收统一扫描器的CodeFragment" << std::endl;
    std::cout << "   ✅ 使用peekKeyword技术提取参数" << std::endl;
    std::cout << "   ✅ 专注于参数处理和JS代码生成" << std::endl;
    std::cout << "   ❌ 不重新实现扫描和词法分析" << std::endl;
    
    std::cout << "\n🚀 架构协作流程:" << std::endl;
    std::cout << "   源代码 → 统一扫描器 → CodeFragment → CJMOD API → 标准JS" << std::endl;
    
    std::cout << "\n💡 设计哲学:" << std::endl;
    std::cout << "   🎯 职责分离：各组件专注核心功能" << std::endl;
    std::cout << "   🔄 协作优先：组件间完美配合，避免重复实现" << std::endl;
    std::cout << "   ⚡ 效率至上：利用现有优秀架构，不重新发明轮子" << std::endl;
    std::cout << "   🏗️ 可扩展性：新的CJMOD扩展可以轻松集成" << std::endl;
}

} // namespace Demo
} // namespace CJMOD
} // namespace CHTL

// ============ C接口实现 ============

static std::unique_ptr<CHTL::CJMOD::Demo::DemoModuleProcessor> g_demoProcessor;
static std::string g_lastResult;

extern "C" {
    void InitializeDemoCJMOD() {
        std::cout << "🚀 初始化Demo CJMOD模块（C接口）" << std::endl;
        g_demoProcessor = CHTL::CJMOD::Demo::createDemoProcessor();
        std::cout << "✅ Demo CJMOD模块初始化完成" << std::endl;
    }
    
    const char* ProcessDemoCJMODFunction(const char* functionName, const char* fragmentData, size_t fragmentSize) {
        if (!g_demoProcessor) {
            InitializeDemoCJMOD();
        }
        
        std::cout << "⚙️ 处理CJMOD函数调用（C接口）" << std::endl;
        std::cout << "   函数名: " << functionName << std::endl;
        std::cout << "   片段大小: " << fragmentSize << " bytes" << std::endl;
        
        // 构造CodeFragment
        CHTL::Scanner::CodeFragment fragment;
        fragment.type = CHTL::Scanner::FragmentType::CHTL_JS;
        fragment.content = std::string(fragmentData, fragmentSize);
        fragment.startPos = 0;
        fragment.endPos = fragmentSize;
        
        // 处理片段
        g_lastResult = g_demoProcessor->processCodeFragment(fragment, functionName);
        
        std::cout << "✅ CJMOD函数处理完成" << std::endl;
        return g_lastResult.c_str();
    }
    
    void CleanupDemoCJMOD() {
        std::cout << "🧹 清理Demo CJMOD模块" << std::endl;
        g_demoProcessor.reset();
        g_lastResult.clear();
        std::cout << "✅ Demo CJMOD模块清理完成" << std::endl;
    }
    
    const char* GetDemoSupportedExtensions() {
        static std::string extensions = "printMylove,iNeverAway,createAnimation";
        return extensions.c_str();
    }
}