#include "Dispatcher/CompilerDispatcher.h"
#include "Scanner/UnifiedScanner.h"
#include "Utils/FileUtils.h"
#include "Utils/Logger.h"
#include <vector>

namespace Dispatcher {

class CompilerDispatcher::Impl {
public:
    std::vector<std::string> modulePaths_;
    
    bool CompileFile(const std::string& inputFile, const std::string& outputFile) {
        try {
            // 读取文件内容
            std::string content = Utils::FileUtils::ReadFile(inputFile);
            
            // 创建统一扫描器
            Scanner::UnifiedScanner scanner;
            Scanner::ScanConfig config;
            config.debugMode = false;
            scanner.SetConfig(config);
            
            // 扫描文件
            auto slices = scanner.ScanFile(inputFile);
            
            LOG_INFO("扫描完成，共 " + std::to_string(slices.size()) + " 个代码片段");
            
            // TODO: 调度各个编译器处理不同类型的切片
            
            // 临时：直接输出扫描结果
            std::string output = "<!-- CHTL编译结果 -->\n";
            output += "<html>\n<body>\n";
            output += "<h1>编译成功</h1>\n";
            output += "<p>共扫描到 " + std::to_string(slices.size()) + " 个代码片段</p>\n";
            output += "</body>\n</html>";
            
            return Utils::FileUtils::WriteFile(outputFile, output);
            
        } catch (const std::exception& e) {
            LOG_ERROR("编译失败: " + std::string(e.what()));
            return false;
        }
    }
};

CompilerDispatcher::CompilerDispatcher() 
    : impl_(std::make_unique<Impl>()) {}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::AddModulePath(const std::string& path) {
    impl_->modulePaths_.push_back(path);
}

bool CompilerDispatcher::CompileFile(const std::string& inputFile, 
                                    const std::string& outputFile) {
    return impl_->CompileFile(inputFile, outputFile);
}

bool CompilerDispatcher::CompileString(const std::string& content, 
                                      const std::string& outputFile,
                                      const std::string& sourceName) {
    // TODO: 实现字符串编译
    return false;
}

} // namespace Dispatcher