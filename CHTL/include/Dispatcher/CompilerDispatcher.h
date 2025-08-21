#ifndef CHTL_DISPATCHER_COMPILERDISPATCHER_H
#define CHTL_DISPATCHER_COMPILERDISPATCHER_H

#include <string>
#include <vector>
#include <memory>

namespace Dispatcher {

class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    // 添加模块搜索路径
    void AddModulePath(const std::string& path);
    
    // 编译文件
    bool CompileFile(const std::string& inputFile, const std::string& outputFile);
    
    // 编译字符串
    bool CompileString(const std::string& content, const std::string& outputFile,
                      const std::string& sourceName = "<string>");
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace Dispatcher

#endif // CHTL_DISPATCHER_COMPILERDISPATCHER_H