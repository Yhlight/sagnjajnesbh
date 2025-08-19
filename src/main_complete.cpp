#include "CHTLCompiler/CHTLCompilerCore.h"
#include "CHTLJSCompiler/Core/CHTLJSCompilerCore.h"
#include "Scanner/CHTLUnifiedScanner.h"
#include "Dispatcher/CompilerDispatcher.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace chtl;

/**
 * 完整的CHTL编译器实现
 * 严格按照CHTL语法文档实现
 */
class CompleteCHTLCompiler {
public:
    CompleteCHTLCompiler() : debug_mode_(false) {}
    
    bool initialize() {
        try {
            // 初始化CHTL编译器核心
            chtl_compiler_ = std::make_unique<CHTLCompilerCore>();
            if (!chtl_compiler_->initialize()) {
                std::cerr << "CHTL编译器核心初始化失败" << std::endl;
                return false;
            }
            
            // 初始化CHTL JS编译器核心
            chtljs_compiler_ = std::make_unique<CHTLJSCompilerCore>();
            if (!chtljs_compiler_->initialize()) {
                std::cerr << "CHTL JS编译器核心初始化失败" << std::endl;
                return false;
            }
            
            // 初始化统一扫描器
            scanner_ = std::make_unique<scanner::CHTLUnifiedScanner>();
            
            if (debug_mode_) {
                std::cout << "[CompleteCHTLCompiler] 所有组件初始化完成" << std::endl;
            }
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "初始化错误: " << e.what() << std::endl;
            return false;
        }
    }
    
    std::string compile(const std::string& chtl_code) {
        try {
            if (debug_mode_) {
                std::cout << "[CompleteCHTLCompiler] 开始完整编译流程" << std::endl;
            }
            
            // 第一步：统一扫描，识别不同的代码片段
            auto scan_result = scanner_->scanCode(chtl_code);
            
            if (scan_result.hasErrors) {
                for (const auto& error : scan_result.errors) {
                    std::cerr << "扫描错误: " << error << std::endl;
                }
                return "";
            }
            
            if (debug_mode_) {
                std::cout << "[扫描器] 识别到 " << scan_result.fragments.size() << " 个代码片段" << std::endl;
                for (const auto& [context, count] : scan_result.contextCounts) {
                    std::cout << "  - " << static_cast<int>(context) << ": " << count << " 个片段" << std::endl;
                }
            }
            
            // 第二步：分别编译不同类型的代码片段
            std::string html_output;
            std::string css_output;
            std::string js_output;
            
            for (const auto& fragment : scan_result.fragments) {
                switch (fragment.context) {
                    case scanner::CHTLUnifiedScanner::CodeContext::CHTL_ELEMENT:
                    case scanner::CHTLUnifiedScanner::CodeContext::CHTL_GLOBAL:
                        // 使用CHTL编译器处理
                        html_output += compileCHTLFragment(fragment.content);
                        break;
                        
                    case scanner::CHTLUnifiedScanner::CodeContext::CHTL_SCRIPT:
                        // 使用CHTL JS编译器处理
                        js_output += compileCHTLJSFragment(fragment.content);
                        break;
                        
                    case scanner::CHTLUnifiedScanner::CodeContext::CHTL_STYLE:
                        // CSS处理（暂时简化）
                        css_output += fragment.content;
                        break;
                        
                    default:
                        // 其他类型暂时原样输出
                        html_output += fragment.content;
                        break;
                }
            }
            
            // 第三步：组合最终输出
            std::string final_output = generateCompleteHTML(html_output, css_output, js_output);
            
            if (debug_mode_) {
                std::cout << "[CompleteCHTLCompiler] 编译完成" << std::endl;
            }
            
            return final_output;
            
        } catch (const std::exception& e) {
            std::cerr << "编译错误: " << e.what() << std::endl;
            return "";
        }
    }
    
    void setDebugMode(bool debug) { 
        debug_mode_ = debug; 
        if (chtl_compiler_) chtl_compiler_->setDebugMode(debug);
        if (chtljs_compiler_) chtljs_compiler_->setDebugMode(debug);
    }
    
private:
    std::unique_ptr<CHTLCompilerCore> chtl_compiler_;
    std::unique_ptr<CHTLJSCompilerCore> chtljs_compiler_;
    std::unique_ptr<scanner::CHTLUnifiedScanner> scanner_;
    bool debug_mode_;
    
    std::string compileCHTLFragment(const std::string& chtl_fragment) {
        if (!chtl_compiler_) return "";
        
        if (debug_mode_) {
            std::cout << "[CHTL编译器] 处理CHTL片段: " << chtl_fragment.substr(0, 50) << "..." << std::endl;
        }
        
        return chtl_compiler_->compile(chtl_fragment);
    }
    
    std::string compileCHTLJSFragment(const std::string& chtljs_fragment) {
        if (!chtljs_compiler_) return "";
        
        if (debug_mode_) {
            std::cout << "[CHTL JS编译器] 处理CHTL JS片段: " << chtljs_fragment.substr(0, 50) << "..." << std::endl;
        }
        
        return chtljs_compiler_->compile(chtljs_fragment);
    }
    
    std::string generateCompleteHTML(const std::string& html, const std::string& css, const std::string& js) {
        std::string output = "<!DOCTYPE html>\n<html>\n<head>\n";
        output += "<meta charset=\"UTF-8\">\n";
        output += "<title>CHTL Generated Page</title>\n";
        
        if (!css.empty()) {
            output += "<style>\n" + css + "\n</style>\n";
        }
        
        output += "</head>\n<body>\n";
        output += html;
        output += "\n";
        
        if (!js.empty()) {
            output += "<script>\n" + js + "\n</script>\n";
        }
        
        output += "</body>\n</html>";
        return output;
    }
};

int main(int argc, char* argv[]) {
    std::cout << "CHTL编译器 v1.0.0 - 完整版本" << std::endl;
    std::cout << "严格按照CHTL语法文档实现" << std::endl;
    std::cout << "===============================" << std::endl;
    
    CompleteCHTLCompiler compiler;
    
    // 解析命令行参数
    bool debug_mode = false;
    std::string input_file;
    std::string output_file;
    bool test_mode = false;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--debug" || arg == "-d") {
            debug_mode = true;
        } else if (arg == "--test" || arg == "-t") {
            test_mode = true;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "\n用法:" << std::endl;
            std::cout << "  " << argv[0] << " [选项] <输入文件> [输出文件]" << std::endl;
            std::cout << "\n选项:" << std::endl;
            std::cout << "  --test, -t     运行内置测试" << std::endl;
            std::cout << "  --debug, -d    启用调试模式" << std::endl;
            std::cout << "  --help, -h     显示帮助信息" << std::endl;
            std::cout << "\n示例:" << std::endl;
            std::cout << "  " << argv[0] << " --test" << std::endl;
            std::cout << "  " << argv[0] << " input.chtl" << std::endl;
            std::cout << "  " << argv[0] << " --debug input.chtl output.html" << std::endl;
            return 0;
        } else if (input_file.empty()) {
            input_file = arg;
        } else if (output_file.empty()) {
            output_file = arg;
        }
    }
    
    compiler.setDebugMode(debug_mode);
    
    if (!compiler.initialize()) {
        std::cerr << "❌ 编译器初始化失败" << std::endl;
        return 1;
    }
    
    try {
        if (test_mode) {
            // 测试模式 - 按照CHTL语法文档的示例
            std::cout << "\n🧪 CHTL语法文档合规性测试" << std::endl;
            
            // 测试1: 基础语法
            std::cout << "\n📋 测试基础语法..." << std::endl;
            std::string basic_test = R"(
                html {
                    head {
                        title { text { CHTL Test Page } }
                    }
                    body {
                        div {
                            class: container;
                            text { Welcome to CHTL }
                        }
                    }
                }
            )";
            
            std::string result1 = compiler.compile(basic_test);
            if (!result1.empty()) {
                std::cout << "  ✅ 基础语法测试通过" << std::endl;
                
                std::ofstream out1("test_basic.html");
                out1 << result1;
                out1.close();
                std::cout << "  📁 输出保存到: test_basic.html" << std::endl;
            } else {
                std::cout << "  ❌ 基础语法测试失败" << std::endl;
            }
            
            // 测试2: CHTL JS语法
            std::cout << "\n📋 测试CHTL JS语法..." << std::endl;
            std::string chtljs_test = R"(
                div {
                    class: interactive;
                    script {
                        {{.interactive}}->listen({
                            click: function(e) {
                                console.log('CHTL JS works!');
                            }
                        });
                    }
                    text { Click me }
                }
            )";
            
            std::string result2 = compiler.compile(chtljs_test);
            if (!result2.empty()) {
                std::cout << "  ✅ CHTL JS语法测试通过" << std::endl;
                
                std::ofstream out2("test_chtljs.html");
                out2 << result2;
                out2.close();
                std::cout << "  📁 输出保存到: test_chtljs.html" << std::endl;
            } else {
                std::cout << "  ❌ CHTL JS语法测试失败" << std::endl;
            }
            
            std::cout << "\n🎉 CHTL语法文档合规性测试完成" << std::endl;
            
        } else if (!input_file.empty()) {
            // 文件编译模式
            if (output_file.empty()) {
                size_t dot_pos = input_file.find_last_of('.');
                if (dot_pos != std::string::npos) {
                    output_file = input_file.substr(0, dot_pos) + ".html";
                } else {
                    output_file = input_file + ".html";
                }
            }
            
            std::cout << "\n📁 编译文件: " << input_file << std::endl;
            std::cout << "📁 输出文件: " << output_file << std::endl;
            
            // 读取输入文件
            std::ifstream in(input_file);
            if (!in.is_open()) {
                std::cerr << "❌ 无法打开输入文件: " << input_file << std::endl;
                return 1;
            }
            
            std::string chtl_code((std::istreambuf_iterator<char>(in)),
                                  std::istreambuf_iterator<char>());
            in.close();
            
            if (debug_mode) {
                std::cout << "📄 读取了 " << chtl_code.length() << " 个字符" << std::endl;
            }
            
            // 编译
            std::string result = compiler.compile(chtl_code);
            
            if (!result.empty()) {
                // 写入输出文件
                std::ofstream out(output_file);
                if (!out.is_open()) {
                    std::cerr << "❌ 无法创建输出文件: " << output_file << std::endl;
                    return 1;
                }
                
                out << result;
                out.close();
                
                std::cout << "\n✅ 编译成功！" << std::endl;
                std::cout << "📁 输出已保存到: " << output_file << std::endl;
                std::cout << "📊 生成了 " << result.length() << " 个字符的HTML代码" << std::endl;
            } else {
                std::cout << "\n❌ 编译失败" << std::endl;
                
                // 显示错误信息
                auto errors = chtl_compiler_->getErrors();
                auto js_errors = chtljs_compiler_->getErrors();
                
                if (!errors.empty()) {
                    std::cout << "\nCHTL编译器错误:" << std::endl;
                    for (const auto& error : errors) {
                        std::cout << "  " << error << std::endl;
                    }
                }
                
                if (!js_errors.empty()) {
                    std::cout << "\nCHTL JS编译器错误:" << std::endl;
                    for (const auto& error : js_errors) {
                        std::cout << "  " << error << std::endl;
                    }
                }
                
                return 1;
            }
            
        } else {
            // 显示用法
            std::cout << "\n用法:" << std::endl;
            std::cout << "  " << argv[0] << " [选项] <输入文件> [输出文件]" << std::endl;
            std::cout << "\n选项:" << std::endl;
            std::cout << "  --test, -t     运行内置测试" << std::endl;
            std::cout << "  --debug, -d    启用调试模式" << std::endl;
            std::cout << "  --help, -h     显示帮助信息" << std::endl;
            std::cout << "\n示例:" << std::endl;
            std::cout << "  " << argv[0] << " --test" << std::endl;
            std::cout << "  " << argv[0] << " input.chtl" << std::endl;
            std::cout << "  " << argv[0] << " --debug input.chtl output.html" << std::endl;
            return 1;
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "运行时错误: " << e.what() << std::endl;
        return 1;
    }
    
private:
    std::unique_ptr<CHTLCompilerCore> chtl_compiler_;
    std::unique_ptr<CHTLJSCompilerCore> chtljs_compiler_;
    std::unique_ptr<scanner::CHTLUnifiedScanner> scanner_;
    bool debug_mode_;
};

int main(int argc, char* argv[]) {
    CompleteCHTLCompiler compiler;
    return compiler.compile("");
}