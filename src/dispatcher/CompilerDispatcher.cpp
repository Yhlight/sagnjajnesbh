#include "../../include/CompilerDispatcher.h"
#include "../../include/CHTLUnifiedScanner.h"
#include "../../include/CHTLParser.h"
#include "../../include/CHTLJSParser.h"
#include "../../include/CHTLGenerator.h"
#include "../../include/CHTLJSGenerator.h"
#include <iostream>
#include <fstream>
#include <chrono>

namespace chtl {

CompilerDispatcher::CompilerDispatcher(const CompilerOptions& options)
    : options_(options) {
    
    // 初始化统计信息
    stats_.chtlFragments = 0;
    stats_.chtlJSFragments = 0;
    stats_.cssFragments = 0;
    stats_.jsFragments = 0;
    stats_.htmlFragments = 0;
    stats_.compilationTime = 0;
    
    if (options_.debugMode) {
        std::cout << "[CompilerDispatcher] 初始化完成" << std::endl;
    }
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::setOptions(const CompilerOptions& options) {
    options_ = options;
}

CompilerResult CompilerDispatcher::compileFile(const String& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompilerResult result;
    result.success = false;
    
    if (options_.debugMode) {
        std::cout << "[CompilerDispatcher] 开始编译文件: " << filename << std::endl;
    }
    
    // 读取文件
    std::ifstream file(filename);
    if (!file.good()) {
        result.errors.emplace_back("无法打开文件: " + filename, filename, 0, 0);
        return result;
    }
    
    String source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    return compile(source, filename);
}

CompilerResult CompilerDispatcher::compile(const String& source, const String& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompilerResult result;
    result.success = false;
    
    try {
        // 直接解析CHTL代码
        CHTLParser parser(options_);
        auto parseResult = parser.parse(source, filename);
        
        if (!parseResult.success) {
            for (const auto& error : parseResult.errors) {
                result.errors.emplace_back(error.message, filename, error.line, error.column);
            }
            return result;
        }
        
        if (options_.debugMode) {
            std::cout << "[CompilerDispatcher] 解析完成" << std::endl;
        }
        
        // 生成HTML/CSS/JS
        CHTLGenerator generator(options_);
        auto generateResult = generator.generate(parseResult.ast);
        
        if (!generateResult.success) {
            for (const auto& error : generateResult.errors) {
                result.errors.emplace_back(error.message, filename, 0, 0);
            }
            return result;
        }
        
        result.output = generateFinalOutput(generateResult.htmlOutput, generateResult.cssOutput, generateResult.jsOutput);
        result.success = true;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        stats_.compilationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        if (options_.debugMode) {
            std::cout << "[CompilerDispatcher] 编译完成，用时 " << stats_.compilationTime << "ms" << std::endl;
        }
        
    } catch (const std::exception& e) {
        result.errors.emplace_back("编译异常: " + String(e.what()), filename, 0, 0);
    }
    
    return result;
}

String CompilerDispatcher::generateFinalOutput(const String& html, const String& css, const String& js) {
    std::ostringstream output;
    
    output << "<!DOCTYPE html>\n";
    output << "<html lang=\"zh-CN\">\n";
    output << "<head>\n";
    output << "    <meta charset=\"UTF-8\">\n";
    output << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    output << "    <title>CHTL Generated Page</title>\n";
    
    if (!css.empty()) {
        output << "    <style>\n";
        output << css;
        output << "    </style>\n";
    }
    
    output << "</head>\n";
    output << "<body>\n";
    output << html;
    output << "\n";
    
    if (!js.empty()) {
        output << "    <script>\n";
        output << js;
        output << "    </script>\n";
    }
    
    output << "</body>\n";
    output << "</html>\n";
    
    return output.str();
}

const CompileStats& CompilerDispatcher::getStats() const {
    return stats_;
}

} // namespace chtl