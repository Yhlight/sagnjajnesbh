#include "CHTLCompilerV3.h"
#include <iostream>
#include <string>
#include <cstring>

void printUsage(const char* programName) {
    std::cout << "CHTL Compiler V3 - Complete implementation of all CHTL features\n";
    std::cout << "Version: " << chtl::CHTLCompilerV3::GetVersion() << "\n";
    std::cout << "Language: " << chtl::CHTLCompilerV3::GetLanguageVersion() << "\n\n";
    
    std::cout << "Usage: " << programName << " [options] input.chtl [output.html]\n\n";
    
    std::cout << "Options:\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "  -v, --version       Show version information\n";
    std::cout << "  -o, --output FILE   Specify output file\n";
    std::cout << "  -d, --debug         Enable debug mode\n";
    std::cout << "  -m, --minify        Minify output\n";
    std::cout << "  -f, --fragment      Generate fragment only (no HTML wrapper)\n";
    std::cout << "  --no-pretty         Disable pretty printing\n";
    std::cout << "  --strict            Enable strict mode\n";
    std::cout << "  --ast               Print AST instead of generating HTML\n";
    std::cout << "\n";
    
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " input.chtl                    # Generate input.html\n";
    std::cout << "  " << programName << " input.chtl -o output.html    # Specify output file\n";
    std::cout << "  " << programName << " -d input.chtl                # Debug mode\n";
    std::cout << "  " << programName << " -m input.chtl                # Minified output\n";
    std::cout << "  " << programName << " --ast input.chtl             # Print AST\n";
}

// AST打印器（用于调试）
class ASTPrinter : public chtl::ast::v3::ASTVisitor {
public:
    ASTPrinter(std::ostream& out) : m_Out(out), m_Indent(0) {}
    
    void VisitDocument(chtl::ast::v3::DocumentNode* node) override {
        PrintIndent() << "Document\n";
        IncreaseIndent();
        VisitChildren(node);
        DecreaseIndent();
    }
    
    void VisitElement(chtl::ast::v3::ElementNode* node) override {
        PrintIndent() << "Element: " << node->GetTagName();
        if (node->HasIndex()) {
            m_Out << "[" << node->GetIndex() << "]";
        }
        m_Out << "\n";
        
        // 打印属性
        if (!node->GetAttributes().empty()) {
            IncreaseIndent();
            for (const auto& attr : node->GetAttributes()) {
                PrintIndent() << "Attribute: " << attr.first << " = " << attr.second << "\n";
            }
            DecreaseIndent();
        }
        
        IncreaseIndent();
        VisitChildren(node);
        DecreaseIndent();
    }
    
    void VisitText(chtl::ast::v3::TextNode* node) override {
        PrintIndent() << "Text: \"" << node->GetContent() << "\"\n";
    }
    
    void VisitStyle(chtl::ast::v3::StyleNode* node) override {
        PrintIndent() << "Style: " << 
            (node->GetType() == chtl::ast::v3::StyleNode::LOCAL ? "LOCAL" : 
             node->GetType() == chtl::ast::v3::StyleNode::GLOBAL ? "GLOBAL" : "INLINE") << "\n";
        
        IncreaseIndent();
        
        // 打印内联属性
        for (const auto& prop : node->GetInlineProperties()) {
            PrintIndent() << "Property: " << prop.first << " = " << prop.second << "\n";
        }
        
        // 打印无值属性
        for (const auto& prop : node->GetNoValueProperties()) {
            PrintIndent() << "NoValueProperty: " << prop << "\n";
        }
        
        // 打印规则
        for (const auto& rule : node->GetRules()) {
            PrintIndent() << "Rule: " << rule.first << " { " << rule.second << " }\n";
        }
        
        VisitChildren(node);
        DecreaseIndent();
    }
    
    void VisitScript(chtl::ast::v3::ScriptNode* node) override {
        PrintIndent() << "Script: " << 
            (node->GetType() == chtl::ast::v3::ScriptNode::LOCAL ? "LOCAL" : "GLOBAL") <<
            (node->HasCHTLJS() ? " [CHTL JS]" : "") << "\n";
        IncreaseIndent();
        PrintIndent() << "Content: " << node->GetContent() << "\n";
        DecreaseIndent();
    }
    
    void VisitComment(chtl::ast::v3::CommentNode* node) override {
        PrintIndent() << "Comment: " << node->GetContent() << "\n";
    }
    
    void VisitTemplate(chtl::ast::v3::TemplateNode* node) override {
        PrintIndent() << "Template: " << 
            (node->GetType() == chtl::ast::v3::TemplateNode::STYLE ? "@Style" :
             node->GetType() == chtl::ast::v3::TemplateNode::ELEMENT ? "@Element" : "@Var") <<
            " " << node->GetName() << "\n";
        
        IncreaseIndent();
        VisitChildren(node);
        DecreaseIndent();
    }
    
    void VisitCustom(chtl::ast::v3::CustomNode* node) override {
        PrintIndent() << "Custom: " << 
            (node->GetType() == chtl::ast::v3::CustomNode::STYLE ? "@Style" :
             node->GetType() == chtl::ast::v3::CustomNode::ELEMENT ? "@Element" : "@Var") <<
            " " << node->GetName();
        
        if (node->IsSpecialization()) {
            m_Out << " [Specialization]";
        }
        if (node->HasNoValueProperties()) {
            m_Out << " [NoValue]";
        }
        m_Out << "\n";
        
        IncreaseIndent();
        VisitChildren(node);
        DecreaseIndent();
    }
    
    void VisitVar(chtl::ast::v3::VarNode* node) override {
        PrintIndent() << "Var: " << node->GetName() << " = " << node->GetValue() << "\n";
    }
    
    void VisitVarCall(chtl::ast::v3::VarCallNode* node) override {
        PrintIndent() << "VarCall: " << node->GetVarGroup() << "(" << node->GetVarName();
        if (node->HasOverride()) {
            m_Out << " = " << node->GetOverrideValue();
        }
        m_Out << ")\n";
    }
    
    void VisitImport(chtl::ast::v3::ImportNode* node) override {
        PrintIndent() << "Import: " << node->GetPath();
        if (!node->GetAlias().empty()) {
            m_Out << " as " << node->GetAlias();
        }
        m_Out << "\n";
    }
    
    void VisitNamespace(chtl::ast::v3::NamespaceNode* node) override {
        PrintIndent() << "Namespace: " << node->GetName();
        if (!node->GetFromClause().empty()) {
            m_Out << " from " << node->GetFromClause();
        }
        m_Out << "\n";
        
        IncreaseIndent();
        VisitChildren(node);
        DecreaseIndent();
    }
    
    void VisitConfiguration(chtl::ast::v3::ConfigurationNode* node) override {
        PrintIndent() << "Configuration";
        if (!node->GetName().empty()) {
            m_Out << " @Config " << node->GetName();
        }
        m_Out << "\n";
        
        IncreaseIndent();
        for (const auto& cfg : node->GetConfigs()) {
            PrintIndent() << cfg.first << " = " << cfg.second << "\n";
        }
        DecreaseIndent();
    }
    
    void VisitOrigin(chtl::ast::v3::OriginNode* node) override {
        PrintIndent() << "Origin: ";
        switch (node->GetType()) {
            case chtl::ast::v3::OriginNode::HTML: m_Out << "@Html"; break;
            case chtl::ast::v3::OriginNode::STYLE: m_Out << "@Style"; break;
            case chtl::ast::v3::OriginNode::JAVASCRIPT: m_Out << "@JavaScript"; break;
            case chtl::ast::v3::OriginNode::CUSTOM: m_Out << node->GetCustomTypeName(); break;
        }
        if (!node->GetName().empty()) {
            m_Out << " " << node->GetName();
        }
        m_Out << "\n";
        
        if (!node->GetContent().empty()) {
            IncreaseIndent();
            PrintIndent() << "Content: " << node->GetContent() << "\n";
            DecreaseIndent();
        }
    }
    
    void VisitInherit(chtl::ast::v3::InheritNode* node) override {
        PrintIndent() << "Inherit: ";
        if (node->IsExplicit()) {
            m_Out << "inherit ";
        }
        m_Out << node->GetBase();
        if (!node->GetQualifier().empty()) {
            m_Out << "." << node->GetQualifier();
        }
        m_Out << "\n";
    }
    
    void VisitDelete(chtl::ast::v3::DeleteNode* node) override {
        PrintIndent() << "Delete: ";
        switch (node->GetType()) {
            case chtl::ast::v3::DeleteNode::PROPERTY: m_Out << "property"; break;
            case chtl::ast::v3::DeleteNode::INHERITANCE: m_Out << "inheritance"; break;
            case chtl::ast::v3::DeleteNode::ELEMENT: m_Out << "element"; break;
        }
        m_Out << " " << node->GetTarget();
        for (const auto& target : node->GetTargets()) {
            m_Out << ", " << target;
        }
        m_Out << "\n";
    }
    
    void VisitInsert(chtl::ast::v3::InsertNode* node) override {
        PrintIndent() << "Insert: ";
        switch (node->GetPosition()) {
            case chtl::ast::v3::InsertNode::BEFORE: m_Out << "before " << node->GetTarget(); break;
            case chtl::ast::v3::InsertNode::AFTER: m_Out << "after " << node->GetTarget(); break;
            case chtl::ast::v3::InsertNode::REPLACE: m_Out << "replace " << node->GetTarget(); break;
            case chtl::ast::v3::InsertNode::AT_TOP: m_Out << "at top"; break;
            case chtl::ast::v3::InsertNode::AT_BOTTOM: m_Out << "at bottom"; break;
            case chtl::ast::v3::InsertNode::AT_INDEX: m_Out << "at index " << node->GetIndex(); break;
        }
        m_Out << "\n";
        
        IncreaseIndent();
        VisitChildren(node);
        DecreaseIndent();
    }
    
    void VisitExcept(chtl::ast::v3::ExceptNode* node) override {
        PrintIndent() << "Except: ";
        bool first = true;
        for (const auto& target : node->GetTargets()) {
            if (!first) m_Out << ", ";
            m_Out << target;
            first = false;
        }
        m_Out << "\n";
    }
    
    void VisitAttribute(chtl::ast::v3::AttributeNode* node) override {
        PrintIndent() << "Attribute: " << node->GetName() << " = " << node->GetValue() << "\n";
    }

private:
    std::ostream& m_Out;
    int m_Indent;
    
    std::ostream& PrintIndent() {
        for (int i = 0; i < m_Indent; ++i) {
            m_Out << "  ";
        }
        return m_Out;
    }
    
    void IncreaseIndent() { m_Indent++; }
    void DecreaseIndent() { if (m_Indent > 0) m_Indent--; }
    
    void VisitChildren(chtl::ast::v3::ASTNode* node) {
        for (const auto& child : node->GetChildren()) {
            child->Accept(this);
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // 解析命令行参数
    chtl::CompilerOptions options;
    std::string inputFile;
    std::string outputFile;
    bool printAst = false;
    bool showVersion = false;
    bool showHelp = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            showHelp = true;
        } else if (arg == "-v" || arg == "--version") {
            showVersion = true;
        } else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o requires an argument\n";
                return 1;
            }
        } else if (arg == "-d" || arg == "--debug") {
            options.debugMode = true;
        } else if (arg == "-m" || arg == "--minify") {
            options.minify = true;
            options.prettyPrint = false;
        } else if (arg == "-f" || arg == "--fragment") {
            options.fragmentOnly = true;
        } else if (arg == "--no-pretty") {
            options.prettyPrint = false;
        } else if (arg == "--strict") {
            options.strictMode = true;
        } else if (arg == "--ast") {
            printAst = true;
        } else if (arg[0] != '-') {
            if (inputFile.empty()) {
                inputFile = arg;
            } else if (outputFile.empty()) {
                outputFile = arg;
            }
        } else {
            std::cerr << "Error: Unknown option " << arg << "\n";
            return 1;
        }
    }
    
    if (showHelp) {
        printUsage(argv[0]);
        return 0;
    }
    
    if (showVersion) {
        std::cout << "CHTL Compiler V3\n";
        std::cout << "Version: " << chtl::CHTLCompilerV3::GetVersion() << "\n";
        std::cout << "Language: " << chtl::CHTLCompilerV3::GetLanguageVersion() << "\n";
        return 0;
    }
    
    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // 默认输出文件名
    if (outputFile.empty()) {
        outputFile = inputFile;
        size_t dotPos = outputFile.rfind('.');
        if (dotPos != std::string::npos) {
            outputFile = outputFile.substr(0, dotPos);
        }
        outputFile += ".html";
    }
    
    // 创建编译器
    chtl::CHTLCompilerV3 compiler;
    
    if (printAst) {
        // AST模式
        std::string source;
        std::ifstream file(inputFile);
        if (!file.is_open()) {
            std::cerr << "Error: Failed to open file: " << inputFile << "\n";
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        source = buffer.str();
        
        // 编译以获取AST
        compiler.CompileString(source, inputFile, options);
        
        if (compiler.HasErrors()) {
            std::cerr << "Compilation failed with errors:\n";
            for (const auto& error : compiler.GetErrors()) {
                std::cerr << "  " << error << "\n";
            }
            return 1;
        }
        
        // 打印AST
        auto ast = compiler.GetAST();
        if (ast) {
            ASTPrinter printer(std::cout);
            ast->Accept(&printer);
        }
        
        return 0;
    }
    
    // 正常编译模式
    bool success = compiler.CompileFile(inputFile, outputFile, options);
    
    if (!success) {
        std::cerr << "Compilation failed with errors:\n";
        for (const auto& error : compiler.GetErrors()) {
            std::cerr << "  " << error << "\n";
        }
        return 1;
    }
    
    if (compiler.HasWarnings()) {
        std::cout << "Warnings:\n";
        for (const auto& warning : compiler.GetWarnings()) {
            std::cout << "  " << warning << "\n";
        }
    }
    
    if (options.debugMode) {
        std::cout << "Successfully compiled " << inputFile << " to " << outputFile << "\n";
    }
    
    return 0;
}