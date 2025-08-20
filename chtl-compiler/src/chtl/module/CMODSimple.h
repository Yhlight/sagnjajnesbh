#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>

namespace chtl {
namespace module {

// 简化的CMOD模块格式
// 使用简单的文本格式而非二进制格式
class CMODSimple {
public:
    struct Module {
        // 元数据
        std::string name;
        std::string version;
        std::string author;
        std::string description;
        
        // 内容
        std::map<std::string, std::string> files;  // 路径 -> 内容
        std::vector<std::string> exports;          // 导出的模板/元素
        std::vector<std::string> dependencies;     // 依赖的模块
    };
    
    // 打包
    static bool Pack(const Module& module, const std::string& outputFile) {
        std::ofstream out(outputFile);
        if (!out.is_open()) return false;
        
        // 写入头部
        out << "CMOD:1.0\n";
        out << "NAME:" << module.name << "\n";
        out << "VERSION:" << module.version << "\n";
        out << "AUTHOR:" << module.author << "\n";
        out << "DESCRIPTION:" << module.description << "\n";
        
        // 写入依赖
        out << "DEPENDENCIES:" << module.dependencies.size() << "\n";
        for (const auto& dep : module.dependencies) {
            out << dep << "\n";
        }
        
        // 写入导出
        out << "EXPORTS:" << module.exports.size() << "\n";
        for (const auto& exp : module.exports) {
            out << exp << "\n";
        }
        
        // 写入文件
        out << "FILES:" << module.files.size() << "\n";
        for (const auto& [path, content] : module.files) {
            out << "FILE:" << path << "\n";
            out << "SIZE:" << content.size() << "\n";
            out << content << "\n";
            out << "ENDFILE\n";
        }
        
        out << "ENDCMOD\n";
        return true;
    }
    
    // 解包
    static bool Unpack(const std::string& inputFile, Module& module) {
        std::ifstream in(inputFile);
        if (!in.is_open()) return false;
        
        std::string line;
        
        // 读取头部
        std::getline(in, line);
        if (line != "CMOD:1.0") return false;
        
        // 读取元数据
        while (std::getline(in, line)) {
            if (line.find("NAME:") == 0) {
                module.name = line.substr(5);
            } else if (line.find("VERSION:") == 0) {
                module.version = line.substr(8);
            } else if (line.find("AUTHOR:") == 0) {
                module.author = line.substr(7);
            } else if (line.find("DESCRIPTION:") == 0) {
                module.description = line.substr(12);
            } else if (line.find("DEPENDENCIES:") == 0) {
                int count = std::stoi(line.substr(13));
                for (int i = 0; i < count; i++) {
                    std::getline(in, line);
                    module.dependencies.push_back(line);
                }
            } else if (line.find("EXPORTS:") == 0) {
                int count = std::stoi(line.substr(8));
                for (int i = 0; i < count; i++) {
                    std::getline(in, line);
                    module.exports.push_back(line);
                }
            } else if (line.find("FILES:") == 0) {
                int count = std::stoi(line.substr(6));
                for (int i = 0; i < count; i++) {
                    std::getline(in, line); // FILE:path
                    std::string path = line.substr(5);
                    
                    std::getline(in, line); // SIZE:size
                    int size = std::stoi(line.substr(5));
                    
                    std::string content;
                    content.resize(size);
                    in.read(&content[0], size);
                    in.ignore(); // 跳过换行
                    
                    module.files[path] = content;
                    
                    std::getline(in, line); // ENDFILE
                }
                break;
            }
        }
        
        return true;
    }
    
    // 从目录创建模块
    static Module CreateFromDirectory(const std::string& dir, const std::string& name, const std::string& version) {
        Module module;
        module.name = name;
        module.version = version;
        
        // TODO: 扫描目录并添加文件
        
        return module;
    }
    
    // 提取到目录
    static bool ExtractToDirectory(const Module& module, const std::string& dir) {
        // TODO: 创建目录并写入文件
        return true;
    }
};

} // namespace module
} // namespace chtl