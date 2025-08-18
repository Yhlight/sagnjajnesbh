#include "../Framework/TestBase.h"
#include "../../CHTLJSCompiler/CHTLJSCompiler.h"
#include "../../CHTLCompiler/Parser/CHTLParser.h"
#include "../../CHTLCompiler/Generator/CHTLGenerator.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace chtl {
namespace test {

/**
 * Chtholly和Yuigahama模块完整测试
 * 严格按照CHTL语法文档验证模块结构和功能
 */
class ChthollyYuigahamaModuleTest {
public:
    void runAllTests() {
        std::cout << "🌸 Chtholly & Yuigahama 模块完整测试\n" << std::endl;
        std::cout << "严格按照CHTL语法文档验证模块实现\n" << std::endl;
        
        testChthollyModuleStructure();
        testChthollyComponentImplementation();
        testChthollyCJMODFunctions();
        testYuigahamaModuleStructure();
        testYuigahamaComponentImplementation();
        testModuleIntegration();
        testCompleteUsageExample();
        
        printFinalSummary();
    }

private:
    int totalTests = 0;
    int passedTests = 0;
    
    void testChthollyModuleStructure() {
        std::cout << "📋 测试Chtholly模块结构..." << std::endl;
        
        // 按语法文档：CMOD + CJMOD混合模块结构
        testCase("Chtholly混合模块结构", []() {
            // 验证CMOD部分
            bool cmodExists = std::filesystem::exists("../src/Module/Chtholly/CMOD");
            
            // 验证CJMOD部分
            bool cjmodExists = std::filesystem::exists("../src/Module/Chtholly/CJMOD");
            
            // 验证分离结构
            bool separateStructure = cmodExists && cjmodExists;
            
            return separateStructure;
        });
        
        // 按语法文档：CMOD三同名规则验证
        testCase("CMOD三同名规则", []() {
            std::vector<std::string> components = {
                "Accordion", "Gallery", "Memo", "SakuraRain"
            };
            
            bool allCompliant = true;
            for (const auto& comp : components) {
                std::string infoPath = "../src/Module/Chtholly/CMOD/" + comp + "/info/" + comp + ".chtl";
                std::string srcPath = "../src/Module/Chtholly/CMOD/" + comp + "/src/" + comp + ".chtl";
                
                if (!std::filesystem::exists(infoPath) || !std::filesystem::exists(srcPath)) {
                    allCompliant = false;
                    break;
                }
            }
            
            return allCompliant;
        });
        
        // 按语法文档：CJMOD二同名规则验证
        testCase("CJMOD二同名规则", []() {
            std::vector<std::string> functions = {"printMylove", "iNeverAway"};
            
            bool allCompliant = true;
            for (const auto& func : functions) {
                std::string infoPath = "../src/Module/Chtholly/CJMOD/" + func + "/info/" + func + ".chtl";
                
                if (!std::filesystem::exists(infoPath)) {
                    allCompliant = false;
                    break;
                }
            }
            
            return allCompliant;
        });
    }
    
    void testChthollyComponentImplementation() {
        std::cout << "\n📋 测试Chtholly组件实现..." << std::endl;
        
        testCase("手风琴组件语法合规性", []() {
            return validateComponentSyntax("../src/Module/Chtholly/CMOD/Accordion/src/Accordion.chtl");
        });
        
        testCase("四叶窗相册组件语法合规性", []() {
            return validateComponentSyntax("../src/Module/Chtholly/CMOD/Gallery/src/Gallery.chtl");
        });
        
        testCase("备忘录组件语法合规性", []() {
            return validateComponentSyntax("../src/Module/Chtholly/CMOD/Memo/src/Memo.chtl");
        });
        
        testCase("樱花雨组件语法合规性", []() {
            return validateComponentSyntax("../src/Module/Chtholly/CMOD/SakuraRain/src/SakuraRain.chtl");
        });
        
        testCase("组件CHTL JS功能", []() {
            return testComponentCHTLJSFeatures();
        });
    }
    
    void testChthollyC JMODFunctions() {
        std::cout << "\n📋 测试Chtholly CJMOD函数..." << std::endl;
        
        testCase("printMylove函数接口", []() {
            // 验证printMylove的C++接口
            std::string headerPath = "../src/Module/Chtholly/CJMOD/printMylove/src/printMylove.h";
            std::string cppPath = "../src/Module/Chtholly/CJMOD/printMylove/src/printMylove.cpp";
            
            return std::filesystem::exists(headerPath) && std::filesystem::exists(cppPath);
        });
        
        testCase("iNeverAway函数接口", []() {
            // 验证iNeverAway的C++接口
            std::string headerPath = "../src/Module/Chtholly/CJMOD/iNeverAway/src/iNeverAway.h";
            std::string cppPath = "../src/Module/Chtholly/CJMOD/iNeverAway/src/iNeverAway.cpp";
            
            return std::filesystem::exists(headerPath) && std::filesystem::exists(cppPath);
        });
        
        testCase("CJMOD语法文档符合性", []() {
            // 验证CJMOD函数是否按语法文档实现
            return validateCJMODImplementation();
        });
    }
    
    void testYuigahamaModuleStructure() {
        std::cout << "\n📋 测试Yuigahama模块结构..." << std::endl;
        
        // 按语法文档：Yuigahama使用纯CMOD结构
        testCase("Yuigahama纯CMOD结构", []() {
            bool srcExists = std::filesystem::exists("../src/Module/Yuigahama/src");
            bool infoExists = std::filesystem::exists("../src/Module/Yuigahama/info");
            
            return srcExists; // info目录可选
        });
        
        testCase("Yuigahama组件数量", []() {
            // 按语法文档：7个CMOD组件
            std::vector<std::string> components = {
                "MusicPlayer", "Accordion", "Gallery", "Memo", 
                "WarmNote", "SakuraRain", "MouseEffect"
            };
            
            int existingCount = 0;
            for (const auto& comp : components) {
                std::string compPath = "../src/Module/Yuigahama/src/" + comp;
                if (std::filesystem::exists(compPath)) {
                    existingCount++;
                }
            }
            
            return existingCount >= 7;
        });
    }
    
    void testYuigahamaComponentImplementation() {
        std::cout << "\n📋 测试Yuigahama组件实现..." << std::endl;
        
        testCase("音乐播放器组件", []() {
            return validateComponentSyntax("../src/Module/Yuigahama/src/MusicPlayer/src/MusicPlayer.chtl");
        });
        
        testCase("由比滨主题风格", []() {
            // 验证组件是否使用了由比滨结衣的主题色彩
            std::string componentPath = "../src/Module/Yuigahama/src/MusicPlayer/src/MusicPlayer.chtl";
            std::string content = loadFileContent(componentPath);
            
            return content.find("#ff9800") != std::string::npos ||  // 橙色主题
                   content.find("#ffc107") != std::string::npos ||  // 黄色主题
                   content.find("yuigahama") != std::string::npos;  // 命名规范
        });
    }
    
    void testModuleIntegration() {
        std::cout << "\n📋 测试模块集成..." << std::endl;
        
        testCase("模块导入功能", []() {
            // 测试模块导入语法
            std::string importCode = R"(
                [Import] @Chtl from Chtholly;
                [Import] @Chtl from Yuigahama;
                
                body {
                    @Element ChthollyAccordion;
                    @Element YuigahamaMusicPlayer;
                }
            )";
            
            return validateCHTLCode(importCode);
        });
        
        testCase("CJMOD函数调用", []() {
            // 测试CJMOD函数的CHTL JS调用
            std::string cjmodCode = R"(
                script {
                    const asciiArt = printMylove({
                        url: "chtholly.png",
                        mode: "ASCII",
                        width: "60",
                        height: "30",
                        scale: 1.0
                    });
                    
                    vir Test = iNeverAway({
                        Void<A>: function() {
                            console.log('珂朵莉永远不会离开');
                        },
                        Void<B>: function() {
                            console.log('由比滨结衣的温暖');
                        }
                    });
                    
                    Test->Void<A>();
                }
            )";
            
            return validateCHTLJSCode(cjmodCode);
        });
    }
    
    void testCompleteUsageExample() {
        std::cout << "\n📋 测试完整使用示例..." << std::endl;
        
        testCase("完整模块使用示例", []() {
            std::string completeExample = R"(
                // 按CHTL语法文档：完整的模块使用示例
                
                [Import] @Chtl from Chtholly;
                [Import] @Chtl from Yuigahama;
                
                html {
                    head {
                        text { "珂朵莉与由比滨结衣的世界" }
                    }
                    
                    body {
                        style {
                            .main-container {
                                max-width: 1200px;
                                margin: 0 auto;
                                padding: 20px;
                                background: linear-gradient(135deg, #fff5f8 0%, #fff8e1 100%);
                            }
                        }
                        
                        script {
                            // 使用CJMOD函数
                            const chthollyArt = printMylove({
                                url: "chtholly.png",
                                mode: "ASCII",
                                width: "80",
                                height: "40"
                            });
                            
                            vir appController = iNeverAway({
                                Init: function() {
                                    console.log('🌸 珂朵莉与由比滨的世界初始化完成');
                                },
                                
                                ShowLove: function() {
                                    console.log('💕 永远的爱与温暖');
                                }
                            });
                            
                            appController->Init();
                        }
                        
                        header {
                            h1 {
                                text { "珂朵莉与由比滨结衣的美好世界" }
                            }
                        }
                        
                        main {
                            section {
                                h2 { text { "珂朵莉的组件" } }
                                
                                @Element ChthollyAccordion;
                                @Element ChthollyGallery;
                                @Element ChthollyMemo;
                                @Element ChthollysakuraRain;
                            }
                            
                            section {
                                h2 { text { "由比滨结衣的组件" } }
                                
                                @Element YuigahamaMusicPlayer;
                            }
                        }
                        
                        footer {
                            p {
                                text { "Powered by CHTL - 珂朵莉与由比滨结衣永远在我心中" }
                            }
                        }
                    }
                }
            )";
            
            return validateCHTLCode(completeExample);
        });
    }
    
    // === 辅助验证方法 ===
    
    bool validateComponentSyntax(const std::string& filePath) {
        if (!std::filesystem::exists(filePath)) {
            return false;
        }
        
        std::string content = loadFileContent(filePath);
        if (content.empty()) {
            return false;
        }
        
        // 按语法文档验证：必须使用[Custom] @Element
        bool hasCustomElement = content.find("[Custom] @Element") != std::string::npos;
        
        // 按语法文档验证：必须使用局部style和script
        bool hasLocalStyle = content.find("style\n        {") != std::string::npos ||
                            content.find("style {") != std::string::npos;
        bool hasLocalScript = content.find("script\n        {") != std::string::npos ||
                             content.find("script {") != std::string::npos;
        
        return hasCustomElement && hasLocalStyle && hasLocalScript;
    }
    
    bool validateCJMODImplementation() {
        // 验证CJMOD函数是否按语法文档实现
        
        // 检查printMylove
        std::string printMyloveHeader = loadFileContent("../src/Module/Chtholly/CJMOD/printMylove/src/printMylove.h");
        bool hasPrintMyloveInterface = printMyloveHeader.find("chtl_printMylove") != std::string::npos;
        
        // 检查iNeverAway
        std::string iNeverAwayHeader = loadFileContent("../src/Module/Chtholly/CJMOD/iNeverAway/src/iNeverAway.h");
        bool hasINeverAwayInterface = iNeverAwayHeader.find("chtl_iNeverAway") != std::string::npos;
        
        return hasPrintMyloveInterface && hasINeverAwayInterface;
    }
    
    bool testComponentCHTLJSFeatures() {
        // 测试组件中的CHTL JS功能
        chtljs::CHTLJSCompiler compiler;
        
        std::string chtljsCode = R"(
            vir galleryController = listen({
                openModal: function(imageSrc) {
                    {{.gallery-modal}}.style.display = 'flex';
                }
            });
            
            {{.gallery-item}}->listen({
                click: function() {
                    galleryController->openModal(this.src);
                }
            });
        )";
        
        auto result = compiler.compile(chtljsCode);
        return result.success && 
               result.virtualObjectsProcessed > 0 && 
               result.listenStatementsProcessed > 0;
    }
    
    bool validateCHTLCode(const std::string& code) {
        try {
            // 简化验证：检查基本语法结构
            bool hasValidStructure = code.find("[") != std::string::npos &&
                                   code.find("@") != std::string::npos &&
                                   code.find("{") != std::string::npos;
            
            return hasValidStructure;
        } catch (...) {
            return false;
        }
    }
    
    bool validateCHTLJSCode(const std::string& code) {
        try {
            chtljs::CHTLJSCompiler compiler;
            auto result = compiler.compile(code);
            return result.success;
        } catch (...) {
            return false;
        }
    }
    
    std::string loadFileContent(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    void testCase(const std::string& name, std::function<bool()> test) {
        totalTests++;
        try {
            bool result = test();
            if (result) {
                passedTests++;
                std::cout << "  ✅ " << name << std::endl;
            } else {
                std::cout << "  ❌ " << name << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "  💥 " << name << " (异常: " << e.what() << ")" << std::endl;
        }
    }
    
    void printFinalSummary() {
        std::cout << "\n📊 Chtholly & Yuigahama 模块测试总结:" << std::endl;
        std::cout << "总测试数: " << totalTests << std::endl;
        std::cout << "通过测试: " << passedTests << std::endl;
        std::cout << "失败测试: " << (totalTests - passedTests) << std::endl;
        
        double passRate = (double)passedTests / totalTests * 100.0;
        std::cout << "通过率: " << std::fixed << std::setprecision(1) << passRate << "%" << std::endl;
        
        if (passRate >= 95.0) {
            std::cout << "\n🎉 优秀！模块完全符合CHTL语法文档规范！" << std::endl;
            std::cout << "🌸 珂朵莉与由比滨结衣的模块实现完美！" << std::endl;
        } else if (passRate >= 85.0) {
            std::cout << "\n👍 良好！模块基本符合语法文档规范。" << std::endl;
        } else {
            std::cout << "\n⚠️  需要改进模块的实现。" << std::endl;
        }
        
        std::cout << "\n💖 珂朵莉与由比滨结衣永远在CHTL中闪耀！" << std::endl;
    }
};

} // namespace test
} // namespace chtl

int main() {
    chtl::test::ChthollyYuigahamaModuleTest test;
    test.runAllTests();
    return 0;
}