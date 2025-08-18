#include "../Framework/TestBase.h"
#include "../../CHTLCompiler/Parser/CHTLParser.h"
#include "../../CHTLCompiler/Generator/CHTLGenerator.h"
#include "../../CHTLCompiler/Lexer/CHTLLexer.h"
#include "../../UnifiedCore/Import/ImportManager.h"
#include <iostream>
#include <fstream>

namespace chtl {
namespace test {

/**
 * 完整的CHTL编译器集成测试
 * 测试从词法分析到代码生成的完整流程
 */
class CompleteCompilerTest {
public:
    void runAllTests() {
        std::cout << "🔬 完整CHTL编译器集成测试\n" << std::endl;
        
        testBasicCompilationFlow();
        testTemplateSystemIntegration();
        testCustomSystemIntegration();
        testOriginEmbeddingIntegration();
        testImportSystemIntegration();
        testConstraintSystemIntegration();
        testCompleteExample();
        
        printFinalSummary();
    }

private:
    int totalTests = 0;
    int passedTests = 0;
    
    void testBasicCompilationFlow() {
        std::cout << "📋 测试基础编译流程..." << std::endl;
        
        testCase("词法分析 -> 语法分析 -> 代码生成", []() {
            // 创建简单的CHTL代码
            std::string chtlCode = R"(
                html {
                    head {
                        text { "CHTL Test Page" }
                    }
                    body {
                        div {
                            id: "main";
                            class: "container";
                            
                            style {
                                width: 100%;
                                height: 100vh;
                            }
                            
                            text { "Hello CHTL!" }
                        }
                    }
                }
            )";
            
            try {
                // 词法分析
                lexer::CHTLLexer lexer;
                lexer.setInput(chtlCode);
                auto tokens = lexer.tokenize();
                
                if (tokens.empty()) {
                    std::cout << "  ⚠️  词法分析未产生token" << std::endl;
                    return false;
                }
                
                // 语法分析
                parser::CHTLParser parser;
                parser.setTokens(tokens);
                auto ast = parser.parseProgram();
                
                if (!ast) {
                    std::cout << "  ⚠️  语法分析失败" << std::endl;
                    return false;
                }
                
                // 代码生成
                generator::CHTLGenerator generator;
                std::string htmlOutput = generator.generate(*ast);
                
                if (htmlOutput.empty()) {
                    std::cout << "  ⚠️  代码生成失败" << std::endl;
                    return false;
                }
                
                // 验证输出包含预期内容
                return htmlOutput.find("<!DOCTYPE html>") != std::string::npos &&
                       htmlOutput.find("<div") != std::string::npos &&
                       htmlOutput.find("Hello CHTL!") != std::string::npos;
                       
            } catch (const std::exception& e) {
                std::cout << "  💥 编译流程异常: " << e.what() << std::endl;
                return false;
            }
        });
    }
    
    void testTemplateSystemIntegration() {
        std::cout << "📋 测试模板系统集成..." << std::endl;
        
        testCase("模板定义和使用", []() {
            std::string chtlCode = R"(
                [Template] @Style DefaultText {
                    color: "black";
                    line-height: 1.6;
                }
                
                [Template] @Element Box {
                    div {
                        style {
                            width: 200px;
                            height: 200px;
                        }
                    }
                }
                
                body {
                    div {
                        style {
                            @Style DefaultText;
                        }
                    }
                    
                    @Element Box;
                }
            )";
            
            return compileAndValidate(chtlCode, {
                "Template",
                "DefaultText",
                "Box",
                "color: black",
                "line-height: 1.6"
            });
        });
        
        testCase("模板继承", []() {
            std::string chtlCode = R"(
                [Template] @Style BaseStyle {
                    color: "black";
                    font-size: 16px;
                }
                
                [Template] @Style ThemeStyle {
                    @Style BaseStyle;
                    background-color: "white";
                }
                
                div {
                    style {
                        @Style ThemeStyle;
                    }
                }
            )";
            
            return compileAndValidate(chtlCode, {
                "BaseStyle",
                "ThemeStyle",
                "color: black",
                "background-color: white"
            });
        });
    }
    
    void testCustomSystemIntegration() {
        std::cout << "📋 测试自定义系统集成..." << std::endl;
        
        testCase("无值样式组", []() {
            std::string chtlCode = R"(
                [Custom] @Style FlexibleText {
                    color,
                    font-size;
                }
                
                div {
                    style {
                        @Style FlexibleText {
                            color: red;
                            font-size: 18px;
                        }
                    }
                }
            )";
            
            return compileAndValidate(chtlCode, {
                "FlexibleText",
                "color: red",
                "font-size: 18px"
            });
        });
        
        testCase("自定义元素特例化", []() {
            std::string chtlCode = R"(
                [Custom] @Element Card {
                    div {
                        span { text { "Title" } }
                        div { text { "Content" } }
                    }
                }
                
                body {
                    @Element Card {
                        span {
                            style {
                                font-weight: bold;
                            }
                        }
                        
                        insert after span {
                            div {
                                text { "Subtitle" }
                            }
                        }
                        
                        delete div[1];
                    }
                }
            )";
            
            return compileAndValidate(chtlCode, {
                "Card",
                "Title",
                "Subtitle",
                "font-weight: bold"
            });
        });
    }
    
    void testOriginEmbeddingIntegration() {
        std::cout << "📋 测试原始嵌入集成..." << std::endl;
        
        testCase("HTML原始嵌入", []() {
            std::string chtlCode = R"(
                [Origin] @Html customWidget {
                    <div class="widget">
                        <h3>Custom Widget</h3>
                        <p>This is raw HTML content</p>
                    </div>
                }
                
                body {
                    [Origin] @Html customWidget;
                }
            )";
            
            return compileAndValidate(chtlCode, {
                "customWidget",
                "<div class=\"widget\">",
                "<h3>Custom Widget</h3>",
                "raw HTML content"
            });
        });
        
        testCase("CSS和JavaScript原始嵌入", []() {
            std::string chtlCode = R"(
                [Origin] @Style customStyles {
                    .custom-button {
                        background: linear-gradient(45deg, #ff6b6b, #4ecdc4);
                        border: none;
                        border-radius: 25px;
                    }
                }
                
                [Origin] @JavaScript customScript {
                    function initCustomWidget() {
                        console.log('Custom widget initialized');
                    }
                    
                    document.addEventListener('DOMContentLoaded', initCustomWidget);
                }
                
                head {
                    [Origin] @Style customStyles;
                    [Origin] @JavaScript customScript;
                }
            )";
            
            return compileAndValidate(chtlCode, {
                "customStyles",
                "customScript",
                "linear-gradient",
                "initCustomWidget",
                "DOMContentLoaded"
            });
        });
        
        testCase("自定义原始类型", []() {
            std::string chtlCode = R"(
                [Origin] @Vue vueComponent {
                    <template>
                        <div class="vue-component">
                            <h2>{{ title }}</h2>
                            <button @click="handleClick">Click me</button>
                        </div>
                    </template>
                    
                    <script>
                    export default {
                        data() {
                            return { title: 'Vue Component' };
                        },
                        methods: {
                            handleClick() {
                                alert('Vue button clicked!');
                            }
                        }
                    };
                    </script>
                }
                
                body {
                    [Origin] @Vue vueComponent;
                }
            )";
            
            return compileAndValidate(chtlCode, {
                "vueComponent",
                "@Vue",
                "<template>",
                "Vue Component",
                "@click=\"handleClick\""
            });
        });
    }
    
    void testImportSystemIntegration() {
        std::cout << "📋 测试导入系统集成..." << std::endl;
        
        testCase("基本导入功能", []() {
            // 创建临时文件用于测试
            createTestFile("test_styles.css", R"(
                .imported-style {
                    color: blue;
                    margin: 10px;
                }
            )");
            
            createTestFile("test_script.js", R"(
                function importedFunction() {
                    console.log('This is an imported function');
                }
            )");
            
            std::string chtlCode = R"(
                [Import] @Style from test_styles.css as myStyles;
                [Import] @JavaScript from test_script.js as myScript;
                
                head {
                    [Origin] @Style myStyles;
                    [Origin] @JavaScript myScript;
                }
                
                body {
                    div {
                        class: "imported-style";
                        text { "Styled with imported CSS" }
                    }
                }
            )";
            
            bool result = compileAndValidate(chtlCode, {
                "myStyles",
                "myScript",
                "imported-style",
                "importedFunction"
            });
            
            // 清理测试文件
            cleanupTestFile("test_styles.css");
            cleanupTestFile("test_script.js");
            
            return result;
        });
    }
    
    void testConstraintSystemIntegration() {
        std::cout << "📋 测试约束系统集成..." << std::endl;
        
        testCase("约束验证", []() {
            std::string chtlCode = R"(
                div {
                    except span, [Custom] @Element Box;
                    
                    p {
                        text { "This is allowed" }
                    }
                    
                    // span { text { "This should be constrained" } }
                    // [Custom] @Element Box; // This should be constrained
                }
            )";
            
            return compileAndValidate(chtlCode, {
                "except",
                "span",
                "This is allowed"
            });
        });
    }
    
    void testCompleteExample() {
        std::cout << "📋 测试完整示例..." << std::endl;
        
        testCase("综合CHTL功能示例", []() {
            std::string chtlCode = R"(
                [Configuration] @Config WebApp {
                    DEBUG_MODE = true;
                    INDEX_INITIAL_COUNT = 0;
                }
                
                [Template] @Style AppTheme {
                    background-color: "#f5f5f5";
                    color: "#333";
                    font-family: "Arial, sans-serif";
                }
                
                [Template] @Element Header {
                    header {
                        style {
                            .app-header {
                                padding: 20px;
                                border-bottom: 1px solid #ddd;
                            }
                        }
                        
                        h1 {
                            text { "CHTL Web Application" }
                        }
                    }
                }
                
                [Custom] @Style ButtonSet {
                    background-color,
                    border-color;
                    border-radius: 5px;
                    padding: 10px 20px;
                    cursor: pointer;
                }
                
                [Origin] @JavaScript appLogic {
                    function initApp() {
                        console.log('CHTL App initialized');
                        
                        document.querySelectorAll('.interactive-button').forEach(btn => {
                            btn.addEventListener('click', function() {
                                alert('Button clicked: ' + this.textContent);
                            });
                        });
                    }
                    
                    document.addEventListener('DOMContentLoaded', initApp);
                }
                
                html {
                    head {
                        text { "CHTL Complete Example" }
                        [Origin] @JavaScript appLogic;
                    }
                    
                    body {
                        style {
                            @Style AppTheme;
                        }
                        
                        @Element Header;
                        
                        main {
                            style {
                                .main-content {
                                    max-width: 800px;
                                    margin: 0 auto;
                                    padding: 20px;
                                }
                            }
                            
                            div {
                                style {
                                    .welcome-section {
                                        text-align: center;
                                        margin-bottom: 30px;
                                    }
                                }
                                
                                h2 {
                                    text { "Welcome to CHTL" }
                                }
                                
                                p {
                                    text { "This is a complete example showcasing CHTL features." }
                                }
                                
                                button {
                                    class: "interactive-button";
                                    
                                    style {
                                        @Style ButtonSet {
                                            background-color: "#007acc";
                                            border-color: "#0056b3";
                                        }
                                        
                                        &:hover {
                                            background-color: "#0056b3";
                                        }
                                    }
                                    
                                    text { "Click Me!" }
                                }
                            }
                        }
                        
                        footer {
                            style {
                                .app-footer {
                                    margin-top: 50px;
                                    padding: 20px;
                                    text-align: center;
                                    border-top: 1px solid #ddd;
                                }
                            }
                            
                            p {
                                text { "Powered by CHTL Compiler" }
                            }
                        }
                    }
                }
            )";
            
            return compileAndValidate(chtlCode, {
                "<!DOCTYPE html>",
                "CHTL Web Application",
                "Welcome to CHTL",
                "app-header",
                "interactive-button",
                "background-color: #007acc",
                "initApp",
                "DOMContentLoaded",
                "Powered by CHTL Compiler"
            });
        });
    }
    
    bool compileAndValidate(const std::string& chtlCode, const std::vector<std::string>& expectedContent) {
        try {
            // 词法分析
            lexer::CHTLLexer lexer;
            lexer.setInput(chtlCode);
            auto tokens = lexer.tokenize();
            
            if (tokens.empty()) {
                std::cout << "    ⚠️  词法分析失败" << std::endl;
                return false;
            }
            
            // 语法分析
            parser::CHTLParser parser;
            parser.setTokens(tokens);
            auto ast = parser.parseProgram();
            
            if (!ast) {
                std::cout << "    ⚠️  语法分析失败" << std::endl;
                if (parser.hasErrors()) {
                    for (const auto& error : parser.getErrors()) {
                        std::cout << "      错误: " << error << std::endl;
                    }
                }
                return false;
            }
            
            // 代码生成
            generator::CHTLGenerator generator;
            std::string htmlOutput = generator.generate(*ast);
            
            if (htmlOutput.empty()) {
                std::cout << "    ⚠️  代码生成失败" << std::endl;
                return false;
            }
            
            // 验证输出内容
            bool allFound = true;
            for (const auto& expected : expectedContent) {
                if (htmlOutput.find(expected) == std::string::npos) {
                    std::cout << "    ⚠️  输出中缺少预期内容: " << expected << std::endl;
                    allFound = false;
                }
            }
            
            if (!allFound) {
                std::cout << "    生成的HTML:" << std::endl;
                std::cout << htmlOutput << std::endl;
            }
            
            return allFound;
            
        } catch (const std::exception& e) {
            std::cout << "    💥 编译异常: " << e.what() << std::endl;
            return false;
        }
    }
    
    void createTestFile(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        file << content;
        file.close();
    }
    
    void cleanupTestFile(const std::string& filename) {
        std::remove(filename.c_str());
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
        std::cout << "\n📊 完整编译器集成测试总结:" << std::endl;
        std::cout << "总测试数: " << totalTests << std::endl;
        std::cout << "通过测试: " << passedTests << std::endl;
        std::cout << "失败测试: " << (totalTests - passedTests) << std::endl;
        
        double passRate = (double)passedTests / totalTests * 100.0;
        std::cout << "通过率: " << std::fixed << std::setprecision(1) << passRate << "%" << std::endl;
        
        if (passRate >= 90.0) {
            std::cout << "\n🎉 优秀！CHTL编译器完整流程运行良好！" << std::endl;
        } else if (passRate >= 75.0) {
            std::cout << "\n👍 良好！大部分编译器功能正常工作。" << std::endl;
        } else if (passRate >= 60.0) {
            std::cout << "\n⚠️  及格！但编译器需要进一步改进。" << std::endl;
        } else {
            std::cout << "\n❌ 不合格！编译器需要大幅修复。" << std::endl;
        }
        
        std::cout << "\n🎯 完整CHTL编译器集成测试完成！" << std::endl;
    }
};

} // namespace test
} // namespace chtl

int main() {
    chtl::test::CompleteCompilerTest test;
    test.runAllTests();
    return 0;
}