// ========================================
// CJMOD创建工具 - 实际创建CJMOD的完整流程
// ========================================

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

class CJMODCreator {
public:
    // 创建新的CJMOD项目
    bool createCJMODProject(const std::string& projectName, const std::string& outputDir) {
        std::cout << "🚀 创建CJMOD项目: " << projectName << std::endl;
        
        // 1. 创建目录结构
        std::string projectPath = outputDir + "/" + projectName;
        
        if (!std::filesystem::create_directories(projectPath + "/src")) {
            std::cerr << "❌ 无法创建src目录" << std::endl;
            return false;
        }
        
        if (!std::filesystem::create_directories(projectPath + "/info")) {
            std::cerr << "❌ 无法创建info目录" << std::endl;
            return false;
        }
        
        // 2. 创建info文件 (二同名规则)
        createInfoFile(projectPath + "/info/" + projectName + ".chtl", projectName);
        
        // 3. 创建主C++源文件
        createMainCppFile(projectPath + "/src/main.cpp", projectName);
        
        // 4. 创建CMakeLists.txt
        createCMakeFile(projectPath + "/CMakeLists.txt", projectName);
        
        // 5. 创建使用示例
        createUsageExample(projectPath + "/example.chtl", projectName);
        
        // 6. 创建编译脚本
        createBuildScript(projectPath + "/build.sh", projectName);
        createBuildScript(projectPath + "/build.bat", projectName);
        
        std::cout << "✅ CJMOD项目创建完成！" << std::endl;
        std::cout << "📁 项目路径: " << projectPath << std::endl;
        
        return true;
    }

private:
    void createInfoFile(const std::string& filePath, const std::string& projectName) {
        std::ofstream file(filePath);
        file << "[Info]\n";
        file << "{\n";
        file << "    name = \"" << projectName << "\";\n";
        file << "    version = \"1.0.0\";\n";
        file << "    description = \"用CJMOD创建的语法扩展\";\n";
        file << "    author = \"CJMOD开发者\";\n";
        file << "    license = \"MIT\";\n";
        file << "    dependencies = \"\";\n";
        file << "    category = \"syntax_extension\";\n";
        file << "    minCHTLVersion = \"1.0.0\";\n";
        file << "    maxCHTLVersion = \"2.0.0\";\n";
        file << "}\n";
        file.close();
    }
    
    void createMainCppFile(const std::string& filePath, const std::string& projectName) {
        std::ofstream file(filePath);
        file << "// ========================================\n";
        file << "// " << projectName << " - CJMOD语法扩展\n";
        file << "// ========================================\n\n";
        file << "#include <string>\n";
        file << "#include <regex>\n";
        file << "#include <sstream>\n\n";
        
        file << "// 示例：创建一个简单的alert扩展\n";
        file << "std::string processAlertExtension(const std::string& chtljs_code) {\n";
        file << "    std::regex alertRegex(R\"(alert\\s*\\(\\s*['\\\"](.*)['\\\"]\s*\\))\");\n";
        file << "    std::smatch matches;\n";
        file << "    \n";
        file << "    if (!std::regex_search(chtljs_code, matches, alertRegex)) {\n";
        file << "        return \"\";\n";
        file << "    }\n";
        file << "    \n";
        file << "    std::string message = matches[1].str();\n";
        file << "    \n";
        file << "    std::stringstream js;\n";
        file << "    js << \"(() => {\\n\";\n";
        file << "    js << \"    console.log('🔔 CJMOD Alert:', '\" << message << \"');\\n\";\n";
        file << "    js << \"    alert('\" << message << \"');\\n\";\n";
        file << "    js << \"})();\";\n";
        file << "    \n";
        file << "    return js.str();\n";
        file << "}\n\n";
        
        file << "// CJMOD主入口函数\n";
        file << "extern \"C\" std::string process" << projectName << "Syntax(const std::string& chtljs_code) {\n";
        file << "    std::string result;\n";
        file << "    \n";
        file << "    // 尝试处理alert扩展\n";
        file << "    result = processAlertExtension(chtljs_code);\n";
        file << "    if (!result.empty()) return result;\n";
        file << "    \n";
        file << "    // 在这里添加更多语法扩展...\n";
        file << "    \n";
        file << "    return chtljs_code; // 如果没有匹配，返回原代码\n";
        file << "}\n";
        file.close();
    }
    
    void createCMakeFile(const std::string& filePath, const std::string& projectName) {
        std::ofstream file(filePath);
        file << "cmake_minimum_required(VERSION 3.16)\n";
        file << "project(" << projectName << " VERSION 1.0.0 LANGUAGES CXX)\n\n";
        file << "set(CMAKE_CXX_STANDARD 17)\n";
        file << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
        file << "# 编译为动态库\n";
        file << "add_library(" << projectName << " SHARED\n";
        file << "    src/main.cpp\n";
        file << ")\n\n";
        file << "# 设置输出名称\n";
        file << "set_target_properties(" << projectName << " PROPERTIES\n";
        file << "    OUTPUT_NAME \"" << projectName << "\"\n";
        file << "    PREFIX \"\"\n";
        file << ")\n\n";
        file << "# Windows平台设置\n";
        file << "if(WIN32)\n";
        file << "    set_target_properties(" << projectName << " PROPERTIES\n";
        file << "        SUFFIX \".cjmod\"\n";
        file << "    )\n";
        file << "else()\n";
        file << "    set_target_properties(" << projectName << " PROPERTIES\n";
        file << "        SUFFIX \".cjmod\"\n";
        file << "    )\n";
        file << "endif()\n";
        file.close();
    }
    
    void createUsageExample(const std::string& filePath, const std::string& projectName) {
        std::ofstream file(filePath);
        file << "// ========================================\n";
        file << "// " << projectName << " 使用示例\n";
        file << "// ========================================\n\n";
        file << "html {\n";
        file << "    head {\n";
        file << "        title { text { " << projectName << " 演示 } }\n";
        file << "    }\n";
        file << "    \n";
        file << "    body {\n";
        file << "        div {\n";
        file << "            h1 { text { " << projectName << " 语法扩展演示 } }\n";
        file << "            \n";
        file << "            button {\n";
        file << "                id: test-btn;\n";
        file << "                text { 测试扩展语法 }\n";
        file << "            }\n";
        file << "            \n";
        file << "            script {\n";
        file << "                // 使用" << projectName << "扩展的语法\n";
        file << "                {{#test-btn}}->listen({\n";
        file << "                    click: function() {\n";
        file << "                        // 🚀 使用CJMOD扩展的alert语法\n";
        file << "                        alert('Hello from " << projectName << "!');\n";
        file << "                    }\n";
        file << "                });\n";
        file << "            }\n";
        file << "        }\n";
        file << "    }\n";
        file << "}\n";
        file.close();
    }
    
    void createBuildScript(const std::string& filePath, const std::string& projectName) {
        std::ofstream file(filePath);
        
        if (filePath.find(".sh") != std::string::npos) {
            // Linux/macOS脚本
            file << "#!/bin/bash\n";
            file << "echo \"🔨 编译CJMOD: " << projectName << "\"\n";
            file << "mkdir -p build\n";
            file << "cd build\n";
            file << "cmake ..\n";
            file << "make\n";
            file << "echo \"✅ 编译完成！\"\n";
            file << "echo \"📦 CJMOD文件: build/" << projectName << ".cjmod\"\n";
        } else {
            // Windows脚本
            file << "@echo off\n";
            file << "echo 🔨 编译CJMOD: " << projectName << "\n";
            file << "if not exist build mkdir build\n";
            file << "cd build\n";
            file << "cmake ..\n";
            file << "cmake --build .\n";
            file << "echo ✅ 编译完成！\n";
            file << "echo 📦 CJMOD文件: build\\" << projectName << ".cjmod\n";
            file << "pause\n";
        }
        
        file.close();
        
        // Linux/macOS设置执行权限
        if (filePath.find(".sh") != std::string::npos) {
            std::filesystem::permissions(filePath, 
                std::filesystem::perms::owner_exec | 
                std::filesystem::perms::group_exec | 
                std::filesystem::perms::others_exec,
                std::filesystem::perm_options::add);
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "🚀 CJMOD创建工具" << std::endl;
    std::cout << "================" << std::endl;
    
    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <项目名称> [输出目录]" << std::endl;
        std::cout << "示例: " << argv[0] << " MyExtension ./projects" << std::endl;
        return 1;
    }
    
    std::string projectName = argv[1];
    std::string outputDir = argc > 2 ? argv[2] : ".";
    
    CJMODCreator creator;
    
    if (creator.createCJMODProject(projectName, outputDir)) {
        std::cout << "\n🎉 CJMOD项目创建成功！" << std::endl;
        std::cout << "\n📋 下一步操作：" << std::endl;
        std::cout << "1. cd " << outputDir << "/" << projectName << std::endl;
        std::cout << "2. 编辑 src/main.cpp 添加你的语法扩展" << std::endl;
        std::cout << "3. 运行 ./build.sh (Linux/macOS) 或 build.bat (Windows)" << std::endl;
        std::cout << "4. 将生成的 .cjmod 文件放到 CHTL 项目的 module 目录" << std::endl;
        std::cout << "5. 在 CHTL 文件中使用 [Import] @CJmod from " << projectName << std::endl;
        
        return 0;
    } else {
        std::cerr << "❌ 项目创建失败" << std::endl;
        return 1;
    }
}