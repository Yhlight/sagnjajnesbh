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
    : options_(options), debugMode_(options.debugMode) {
    
    // 初始化统计信息
    stats_.chtlFragments = 0;
    stats_.chtlJSFragments = 0;
    stats_.cssFragments = 0;
    stats_.jsFragments = 0;
    stats_.htmlFragments = 0;
    stats_.compilationTime = 0;
    
    if (debugMode_) {
        std::cout << "[CompilerDispatcher] 初始化完成" << std::endl;
    }
}

CompilerResult CompilerDispatcher::compileFile(const String& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompilerResult result;
    result.success = false;
    
    if (debugMode_) {
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
    
    return compileSource(source, filename);
}

CompilerResult CompilerDispatcher::compileSource(const String& source, const String& filename) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    CompilerResult result;
    result.success = false;
    
    try {
        // 第一步：扫描源代码，分割代码片段
        CHTLUnifiedScanner scanner(options_);
        auto scanResult = scanner.scan(source, filename);
        
        if (!scanResult.success) {
            for (const auto& error : scanResult.errors) {
                result.errors.emplace_back(error.message, filename, error.line, error.column);
            }
            return result;
        }
        
        if (debugMode_) {
            std::cout << "[CompilerDispatcher] 扫描完成，发现 " << scanResult.fragments.size() << " 个代码片段" << std::endl;
        }
        
        // 第二步：解析各个代码片段
        String htmlOutput, cssOutput, jsOutput;
        
        for (const auto& fragment : scanResult.fragments) {
            switch (fragment.type) {
                case FragmentType::CHTL:
                    {
                        auto chtlResult = compileCHTLFragment(fragment, filename);
                        if (!chtlResult.success) {
                            for (const auto& error : chtlResult.errors) {
                                result.errors.push_back(error);
                            }
                        } else {
                            htmlOutput += chtlResult.htmlOutput;
                            cssOutput += chtlResult.cssOutput;
                            jsOutput += chtlResult.jsOutput;
                        }
                        stats_.chtlFragments++;
                    }
                    break;
                    
                case FragmentType::CHTL_JS:
                    {
                        auto jsResult = compileCHTLJSFragment(fragment, filename);
                        if (!jsResult.success) {
                            for (const auto& error : jsResult.errors) {
                                result.errors.emplace_back(error.message, filename, error.line, error.column);
                            }
                        } else {
                            jsOutput += jsResult.jsOutput;
                        }
                        stats_.chtlJSFragments++;
                    }
                    break;
                    
                case FragmentType::CSS:
                    cssOutput += fragment.content;
                    stats_.cssFragments++;
                    break;
                    
                case FragmentType::JAVASCRIPT:
                    jsOutput += fragment.content;
                    stats_.jsFragments++;
                    break;
                    
                case FragmentType::HTML:
                    htmlOutput += fragment.content;
                    stats_.htmlFragments++;
                    break;
                    
                default:
                    break;
            }
        }
        
        // 第三步：合并输出
        result.output = generateFinalOutput(htmlOutput, cssOutput, jsOutput);
        result.success = true;
        
        // 计算编译时间
        auto endTime = std::chrono::high_resolution_clock::now();
        stats_.compilationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        if (debugMode_) {
            std::cout << "[CompilerDispatcher] 编译完成，用时 " << stats_.compilationTime << "ms" << std::endl;
        }
        
    } catch (const std::exception& e) {
        result.errors.emplace_back("编译异常: " + String(e.what()), filename, 0, 0);
    }
    
    return result;
}

GeneratorResult CompilerDispatcher::compileCHTLFragment(const CodeFragment& fragment, const String& filename) {
    // 使用CHTL解析器和生成器
    CHTLParser parser(options_);
    auto parseResult = parser.parse(fragment.content, filename);
    
    GeneratorResult result;
    if (!parseResult.success) {
        for (const auto& error : parseResult.errors) {
            result.addError(GeneratorErrorType::AST_ERROR, error.message, error.context, error.line, error.column);
        }
        return result;
    }
    
    CHTLGenerator generator(options_);
    return generator.generate(parseResult.ast);
}

CHTLJSGeneratorResult CompilerDispatcher::compileCHTLJSFragment(const CodeFragment& fragment, const String& filename) {
    // 使用CHTL JS解析器和生成器
    CHTLJSParser parser(options_);
    auto parseResult = parser.parse(fragment.content, filename);
    
    CHTLJSGeneratorResult result;
    if (!parseResult.success) {
        for (const auto& error : parseResult.errors) {
            result.addError(CHTLJSGeneratorErrorType::AST_ERROR, error.message, error.context, error.line, error.column);
        }
        return result;
    }
    
    CHTLJSGenerator generator(options_);
    return generator.generate(parseResult.ast);
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

const CompilerStats& CompilerDispatcher::getStats() const {
    return stats_;
}

} // namespace chtl