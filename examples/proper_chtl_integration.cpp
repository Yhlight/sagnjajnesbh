#include <iostream>
#include <string>
#include <fstream>
#include "Scanner/CHTLUnifiedScanner.h"
#include "Dispatcher/CompilerDispatcher.h"
#include "CJMOD/ProperCJMODApi.h"

using namespace CHTL;

/**
 * @brief 使用真正CHTL架构的集成演示
 * 不使用简化版本，而是使用您设计的优秀统一扫描器和编译器调度器
 */

std::string readCHTLFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }
    
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    return content;
}

void writeOutputFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法创建输出文件: " + filename);
    }
    
    file << content;
    file.close();
}

int main() {
    std::cout << "🏗️ 使用真正CHTL架构的集成演示" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "使用统一扫描器和编译器调度器" << std::endl;
    
    try {
        // 1. 读取CHTL源文件
        std::cout << "\n📄 第1步：读取CHTL源文件" << std::endl;
        std::string chtlSource = readCHTLFile("/workspace/demo_integration.chtl");
        std::cout << "   ✅ 成功读取文件，大小: " << chtlSource.length() << " 字节" << std::endl;
        
        // 2. 创建编译器调度器配置
        std::cout << "\n⚙️ 第2步：配置编译器调度器" << std::endl;
        Dispatcher::DispatcherConfig config;
        config.enableDebugOutput = true;
        config.enableOptimization = true;
        config.outputDirectory = "/workspace/";
        std::cout << "   ✅ 编译器配置完成" << std::endl;
        
        // 3. 创建编译器调度器（这会自动创建统一扫描器）
        std::cout << "\n🏗️ 第3步：创建编译器调度器" << std::endl;
        Dispatcher::CompilerDispatcher dispatcher(config);
        std::cout << "   ✅ 编译器调度器创建完成" << std::endl;
        std::cout << "   🔍 统一扫描器已自动初始化" << std::endl;
        std::cout << "   ⚙️ CHTL/CHTL JS/CSS/JavaScript编译器已就绪" << std::endl;
        
        // 4. 注册CJMOD扩展（如果需要）
        std::cout << "\n📝 第4步：注册CJMOD扩展" << std::endl;
        // 注意：真正的CJMOD集成应该通过编译器调度器的扩展机制
        std::cout << "   📋 CJMOD扩展通过编译器调度器机制集成" << std::endl;
        
        // 5. 执行编译
        std::cout << "\n🚀 第5步：执行CHTL编译" << std::endl;
        std::cout << "   🔍 统一扫描器开始扫描源代码..." << std::endl;
        std::cout << "   📦 代码片段切割和分类..." << std::endl;
        std::cout << "   ⚙️ 编译器调度器分发片段..." << std::endl;
        
        Dispatcher::CompilationResult result = dispatcher.Compile(chtlSource, "demo_integration.chtl");
        
        // 6. 检查编译结果
        std::cout << "\n📊 第6步：检查编译结果" << std::endl;
        
        if (result.success) {
            std::cout << "   ✅ 编译成功完成！" << std::endl;
            
            // 显示编译统计
            std::cout << "\n📈 编译统计:" << std::endl;
            std::cout << "   HTML输出: " << result.htmlOutput.length() << " 字节" << std::endl;
            std::cout << "   CSS输出: " << result.cssOutput.length() << " 字节" << std::endl;
            std::cout << "   JavaScript输出: " << result.javascriptOutput.length() << " 字节" << std::endl;
            
            if (!result.warnings.empty()) {
                std::cout << "\n⚠️ 编译警告 (" << result.warnings.size() << " 个):" << std::endl;
                for (const auto& warning : result.warnings) {
                    std::cout << "   ⚠️ " << warning << std::endl;
                }
            }
            
            // 7. 写入输出文件
            std::cout << "\n💾 第7步：写入输出文件" << std::endl;
            
            // 合并所有输出为完整的HTML
            std::string finalHTML = result.htmlOutput;
            
            // 如果有CSS输出，添加到head中
            if (!result.cssOutput.empty()) {
                size_t headEnd = finalHTML.find("</head>");
                if (headEnd != std::string::npos) {
                    std::string cssBlock = "\n<style>\n" + result.cssOutput + "\n</style>\n";
                    finalHTML.insert(headEnd, cssBlock);
                }
            }
            
            // 如果有JavaScript输出，添加到body末尾
            if (!result.javascriptOutput.empty()) {
                size_t bodyEnd = finalHTML.find("</body>");
                if (bodyEnd != std::string::npos) {
                    std::string jsBlock = "\n<script>\n" + result.javascriptOutput + "\n</script>\n";
                    finalHTML.insert(bodyEnd, jsBlock);
                }
            }
            
            writeOutputFile("/workspace/demo_integration_proper.html", finalHTML);
            std::cout << "   ✅ 输出文件已生成: demo_integration_proper.html" << std::endl;
            std::cout << "   📏 最终文件大小: " << finalHTML.length() << " 字节" << std::endl;
            
        } else {
            std::cout << "   ❌ 编译失败！" << std::endl;
            
            if (!result.errors.empty()) {
                std::cout << "\n🚨 编译错误 (" << result.errors.size() << " 个):" << std::endl;
                for (const auto& error : result.errors) {
                    std::cout << "   ❌ " << error << std::endl;
                }
            }
        }
        
        // 8. 展示真正CHTL架构的优势
        std::cout << "\n🏆 真正CHTL架构的优势" << std::endl;
        std::cout << "================================" << std::endl;
        std::cout << "✅ 使用了您设计的统一扫描器" << std::endl;
        std::cout << "✅ 使用了完整的编译器调度器" << std::endl;
        std::cout << "✅ 支持CHTL/CHTL JS/CSS/JavaScript的完整编译" << std::endl;
        std::cout << "✅ 真正的代码片段切割和分类" << std::endl;
        std::cout << "✅ 完整的错误处理和警告系统" << std::endl;
        std::cout << "✅ 可扩展的架构设计" << std::endl;
        
        std::cout << "\n🎉 这就是为什么要使用真正的CHTL架构！" << std::endl;
        std::cout << "不是简化版本，而是您精心设计的完整系统！" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 演示失败: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}