// ========================================
// CJMOD项目生成器 - 自动生成标准的CJMOD项目
// ========================================

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

class CJMODGenerator {
public:
    struct ExtensionConfig {
        std::string projectName;
        std::string outputDir;
        std::vector<std::string> syntaxNames;
        std::string description;
        std::string author;
    };
    
    bool generateProject(const ExtensionConfig& config) {
        std::cout << "🚀 生成CJMOD项目: " << config.projectName << std::endl;
        
        std::string projectPath = config.outputDir + "/" + config.projectName;
        
        // 1. 创建目录结构
        if (!createDirectoryStructure(projectPath)) {
            return false;
        }
        
        // 2. 生成info文件
        if (!generateInfoFile(projectPath, config)) {
            return false;
        }
        
        // 3. 生成C++源文件
        if (!generateCppSource(projectPath, config)) {
            return false;
        }
        
        // 4. 生成编译配置
        if (!generateBuildFiles(projectPath, config)) {
            return false;
        }
        
        // 5. 生成使用示例
        if (!generateUsageExample(projectPath, config)) {
            return false;
        }
        
        // 6. 生成文档
        if (!generateDocumentation(projectPath, config)) {
            return false;
        }
        
        std::cout << "✅ CJMOD项目生成完成！" << std::endl;
        printUsageInstructions(config);
        
        return true;
    }

private:
    bool createDirectoryStructure(const std::string& projectPath) {
        try {
            std::filesystem::create_directories(projectPath + "/src");
            std::filesystem::create_directories(projectPath + "/info");
            std::filesystem::create_directories(projectPath + "/examples");
            std::filesystem::create_directories(projectPath + "/docs");
            return true;
        } catch (const std::exception& e) {
            std::cerr << "❌ 创建目录失败: " << e.what() << std::endl;
            return false;
        }
    }
    
    bool generateInfoFile(const std::string& projectPath, const ExtensionConfig& config) {
        std::ofstream file(projectPath + "/info/" + config.projectName + ".chtl");
        
        file << "[Info]\n";
        file << "{\n";
        file << "    name = \"" << config.projectName << "\";\n";
        file << "    version = \"1.0.0\";\n";
        file << "    description = \"" << config.description << "\";\n";
        file << "    author = \"" << config.author << "\";\n";
        file << "    license = \"MIT\";\n";
        file << "    dependencies = \"\";\n";
        file << "    category = \"syntax_extension\";\n";
        file << "    minCHTLVersion = \"1.0.0\";\n";
        file << "    maxCHTLVersion = \"2.0.0\";\n";
        file << "}\n";
        
        file.close();
        return true;
    }
    
    bool generateCppSource(const std::string& projectPath, const ExtensionConfig& config) {
        std::ofstream file(projectPath + "/src/" + config.projectName + ".cpp");
        
        file << "#include \"CJMODInterface.h\"\n";
        file << "#include <regex>\n";
        file << "#include <sstream>\n\n";
        file << "using namespace chtl::cjmod;\n\n";
        
        file << "class " << config.projectName << " : public ICJMODExtension {\n";
        file << "public:\n";
        
        // 基本信息
        file << "    std::string getExtensionName() const override {\n";
        file << "        return \"" << config.projectName << "\";\n";
        file << "    }\n\n";
        
        file << "    std::string getExtensionVersion() const override {\n";
        file << "        return \"1.0.0\";\n";
        file << "    }\n\n";
        
        // 支持的语法
        file << "    std::vector<std::string> getSupportedSyntax() const override {\n";
        file << "        return {";
        for (size_t i = 0; i < config.syntaxNames.size(); ++i) {
            file << "\"" << config.syntaxNames[i] << "\"";
            if (i < config.syntaxNames.size() - 1) file << ", ";
        }
        file << "};\n";
        file << "    }\n\n";
        
        file << "    bool supportsSyntax(const std::string& name) const override {\n";
        file << "        auto supported = getSupportedSyntax();\n";
        file << "        return std::find(supported.begin(), supported.end(), name) != supported.end();\n";
        file << "    }\n\n";
        
        // 语法解析
        file << "    SyntaxMatch parseSyntax(const std::string& chtljs_code) const override {\n";
        file << "        SyntaxMatch match;\n\n";
        
        for (const auto& syntax : config.syntaxNames) {
            file << "        // 解析 " << syntax << " 语法\n";
            file << "        if (parse" << syntax << "Syntax(chtljs_code, match)) {\n";
            file << "            return match;\n";
            file << "        }\n\n";
        }
        
        file << "        match.matched = false;\n";
        file << "        return match;\n";
        file << "    }\n\n";
        
        // JavaScript生成
        file << "    std::string generateJavaScript(const SyntaxMatch& match, const CJMODContext& context) const override {\n";
        
        for (size_t i = 0; i < config.syntaxNames.size(); ++i) {
            const auto& syntax = config.syntaxNames[i];
            file << "        " << (i == 0 ? "if" : "else if") << " (match.syntaxName == \"" << syntax << "\") {\n";
            file << "            return generate" << syntax << "JS(match, context);\n";
            file << "        }";
            if (i < config.syntaxNames.size() - 1) file << " ";
        }
        
        file << "\n        return \"\";\n";
        file << "    }\n\n";
        
        // 其他必需方法
        file << "    bool validateParameters(const SyntaxMatch& match, std::vector<CJMODError>& errors) const override {\n";
        file << "        // TODO: 实现参数验证\n";
        file << "        return true;\n";
        file << "    }\n\n";
        
        file << "    std::string getSyntaxDocumentation(const std::string& syntaxName) const override {\n";
        file << "        // TODO: 返回语法文档\n";
        file << "        return \"暂无文档\";\n";
        file << "    }\n\n";
        
        file << "    bool initialize(const CJMODContext& context) override {\n";
        file << "        std::cout << \"🚀 " << config.projectName << " 初始化完成\" << std::endl;\n";
        file << "        return true;\n";
        file << "    }\n\n";
        
        file << "    void cleanup() override {\n";
        file << "        std::cout << \"🧹 " << config.projectName << " 清理完成\" << std::endl;\n";
        file << "    }\n\n";
        
        file << "private:\n";
        
        // 生成解析函数模板
        for (const auto& syntax : config.syntaxNames) {
            file << "    bool parse" << syntax << "Syntax(const std::string& code, SyntaxMatch& match) const {\n";
            file << "        // TODO: 实现 " << syntax << " 语法解析\n";
            file << "        // 示例：std::regex regex(R\"(" << syntax << "\\s*\\((.*)\\))\");\n";
            file << "        return false;\n";
            file << "    }\n\n";
            
            file << "    std::string generate" << syntax << "JS(const SyntaxMatch& match, const CJMODContext& context) const {\n";
            file << "        // TODO: 实现 " << syntax << " JavaScript生成\n";
            file << "        return \"console.log('🚀 " << syntax << " 执行');\";\n";
            file << "    }\n\n";
        }
        
        file << "};\n\n";
        
        // 注册
        file << "CJMOD_REGISTER_EXTENSION(" << config.projectName << ")\n";
        
        file.close();
        return true;
    }
    
    bool generateBuildFiles(const std::string& projectPath, const ExtensionConfig& config) {
        // 生成CMakeLists.txt
        std::ofstream cmake(projectPath + "/CMakeLists.txt");
        cmake << "cmake_minimum_required(VERSION 3.16)\n";
        cmake << "project(" << config.projectName << " VERSION 1.0.0 LANGUAGES CXX)\n\n";
        cmake << "set(CMAKE_CXX_STANDARD 17)\n";
        cmake << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n\n";
        cmake << "# 包含CJMOD接口\n";
        cmake << "include_directories(${CHTL_INCLUDE_DIR})\n\n";
        cmake << "# 编译为动态库\n";
        cmake << "add_library(" << config.projectName << " SHARED\n";
        cmake << "    src/" << config.projectName << ".cpp\n";
        cmake << ")\n\n";
        cmake << "# 设置输出为.cjmod文件\n";
        cmake << "set_target_properties(" << config.projectName << " PROPERTIES\n";
        cmake << "    OUTPUT_NAME \"" << config.projectName << "\"\n";
        cmake << "    SUFFIX \".cjmod\"\n";
        cmake << "    PREFIX \"\"\n";
        cmake << ")\n";
        cmake.close();
        
        // 生成构建脚本
        std::ofstream buildSh(projectPath + "/build.sh");
        buildSh << "#!/bin/bash\n";
        buildSh << "echo \"🔨 编译CJMOD: " << config.projectName << "\"\n";
        buildSh << "mkdir -p build && cd build\n";
        buildSh << "cmake .. && make\n";
        buildSh << "echo \"✅ 编译完成: " << config.projectName << ".cjmod\"\n";
        buildSh.close();
        
        std::filesystem::permissions(projectPath + "/build.sh", 
            std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec,
            std::filesystem::perm_options::add);
        
        return true;
    }
    
    bool generateUsageExample(const std::string& projectPath, const ExtensionConfig& config) {
        std::ofstream file(projectPath + "/examples/usage.chtl");
        
        file << "// " << config.projectName << " 使用示例\n\n";
        file << "[Import] @CJmod from " << config.projectName << "\n\n";
        file << "html {\n";
        file << "    body {\n";
        file << "        div {\n";
        file << "            h1 { text { " << config.projectName << " 演示 } }\n";
        file << "            \n";
        file << "            script {\n";
        
        for (const auto& syntax : config.syntaxNames) {
            file << "                // 使用 " << syntax << " 语法\n";
            file << "                " << syntax << "('示例参数');\n\n";
        }
        
        file << "            }\n";
        file << "        }\n";
        file << "    }\n";
        file << "}\n";
        
        file.close();
        return true;
    }
    
    bool generateDocumentation(const std::string& projectPath, const ExtensionConfig& config) {
        std::ofstream file(projectPath + "/README.md");
        
        file << "# " << config.projectName << "\n\n";
        file << config.description << "\n\n";
        file << "## 支持的语法\n\n";
        
        for (const auto& syntax : config.syntaxNames) {
            file << "- `" << syntax << "()` - " << syntax << "功能\n";
        }
        
        file << "\n## 使用方法\n\n";
        file << "1. 编译CJMOD：`./build.sh`\n";
        file << "2. 复制到项目：`cp " << config.projectName << ".cjmod /path/to/your/project/module/`\n";
        file << "3. 导入使用：`[Import] @CJmod from " << config.projectName << "`\n\n";
        file << "## 示例\n\n";
        file << "```chtl\n";
        file << "script {\n";
        
        for (const auto& syntax : config.syntaxNames) {
            file << "    " << syntax << "('参数');  // 你的扩展语法\n";
        }
        
        file << "}\n";
        file << "```\n";
        
        file.close();
        return true;
    }
    
    void printUsageInstructions(const ExtensionConfig& config) {
        std::cout << "\n🎉 项目生成完成！" << std::endl;
        std::cout << "==================" << std::endl;
        std::cout << "📁 项目路径: " << config.outputDir << "/" << config.projectName << std::endl;
        std::cout << "\n📋 下一步操作：" << std::endl;
        std::cout << "1. cd " << config.outputDir << "/" << config.projectName << std::endl;
        std::cout << "2. 编辑 src/" << config.projectName << ".cpp 实现你的语法逻辑" << std::endl;
        std::cout << "3. 运行 ./build.sh 编译" << std::endl;
        std::cout << "4. 将 " << config.projectName << ".cjmod 复制到你的CHTL项目" << std::endl;
        std::cout << "5. 在CHTL中使用: [Import] @CJmod from " << config.projectName << std::endl;
        
        std::cout << "\n💡 支持的语法：" << std::endl;
        for (const auto& syntax : config.syntaxNames) {
            std::cout << "   - " << syntax << "()" << std::endl;
        }
    }
};

int main(int argc, char* argv[]) {
    std::cout << "🛠️ CJMOD项目生成器" << std::endl;
    std::cout << "==================" << std::endl;
    
    if (argc < 2) {
        std::cout << "用法: " << argv[0] << " <项目名> [输出目录]" << std::endl;
        std::cout << "\n交互式创建CJMOD项目：" << std::endl;
        
        CJMODGenerator::ExtensionConfig config;
        
        // 交互式输入
        std::cout << "请输入项目名称: ";
        std::getline(std::cin, config.projectName);
        
        std::cout << "请输入输出目录 (默认当前目录): ";
        std::getline(std::cin, config.outputDir);
        if (config.outputDir.empty()) config.outputDir = ".";
        
        std::cout << "请输入项目描述: ";
        std::getline(std::cin, config.description);
        
        std::cout << "请输入作者名称: ";
        std::getline(std::cin, config.author);
        
        std::cout << "请输入要创建的语法名称（用空格分隔）: ";
        std::string syntaxInput;
        std::getline(std::cin, syntaxInput);
        
        // 解析语法名称
        std::stringstream ss(syntaxInput);
        std::string syntax;
        while (ss >> syntax) {
            config.syntaxNames.push_back(syntax);
        }
        
        if (config.syntaxNames.empty()) {
            config.syntaxNames.push_back("myCustomSyntax");
        }
        
        CJMODGenerator generator;
        return generator.generateProject(config) ? 0 : 1;
    }
    
    // 命令行模式
    CJMODGenerator::ExtensionConfig config;
    config.projectName = argv[1];
    config.outputDir = argc > 2 ? argv[2] : ".";
    config.description = "用CJMOD创建的语法扩展";
    config.author = "CJMOD开发者";
    config.syntaxNames = {"customSyntax", "myFunction"};
    
    CJMODGenerator generator;
    return generator.generateProject(config) ? 0 : 1;
}