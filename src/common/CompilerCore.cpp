#include "CompilerCore.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>

namespace chtl {

// CompilerCore 实现
CompilerCore::CompilerCore() 
    : is_compiling_(false), current_progress_(0.0) {
    initializeComponents();
}

CompilationResult CompilerCore::compileFromString(const std::string& source_code, 
                                                const CompilationOptions& options) {
    if (!validateInput(source_code) || !validateOptions(options)) {
        CompilationResult result;
        result.success = false;
        result.errors.push_back("Invalid input or options");
        return result;
    }
    
    compilation_start_time_ = std::chrono::steady_clock::now();
    
    // 执行完整的编译流程
    bool success = true;
    success &= initializeCompilation(options);
    success &= lexicalAnalysis(source_code);
    success &= syntaxAnalysis();
    success &= semanticAnalysis();
    
    if (options.enable_optimization) {
        success &= optimization();
    }
    
    success &= codeGeneration();
    success &= finalize();
    
    return getCompilationResult();
}

CompilationResult CompilerCore::compileFromFile(const std::string& file_path,
                                              const CompilationOptions& options) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        CompilationResult result;
        result.success = false;
        result.errors.push_back("Cannot open file: " + file_path);
        return result;
    }
    
    std::string source_code((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
    file.close();
    
    // 设置文件路径到上下文管理器
    context_manager_.updatePosition(file_path, 1, 1);
    
    return compileFromString(source_code, options);
}

CompilationResult CompilerCore::compileFromFiles(const std::vector<std::string>& file_paths,
                                               const CompilationOptions& options) {
    CompilationResult combined_result;
    combined_result.success = true;
    
    // 初始化编译
    if (!initializeCompilation(options)) {
        combined_result.success = false;
        combined_result.errors = getErrors();
        return combined_result;
    }
    
    // 创建一个合并的AST
    auto merged_program = std::make_unique<ast::ProgramNode>();
    
    // 逐个编译文件并合并结果
    for (const std::string& file_path : file_paths) {
        CompilationResult file_result = compileFromFile(file_path, options);
        
        if (!file_result.success) {
            combined_result.success = false;
        }
        
        // 合并错误和警告
        combined_result.errors.insert(combined_result.errors.end(),
                                    file_result.errors.begin(), file_result.errors.end());
        combined_result.warnings.insert(combined_result.warnings.end(),
                                       file_result.warnings.begin(), file_result.warnings.end());
        
        // 合并AST（如果文件编译成功）
        if (file_result.success && file_result.ast) {
            // 将文件的AST节点添加到合并的程序中
            for (size_t i = 0; i < file_result.ast->getChildCount(); ++i) {
                // 注意：这里需要克隆节点，因为unique_ptr不能共享所有权
                if (auto child = file_result.ast->getChild(i)) {
                    merged_program->addChild(child->clone());
                }
            }
        }
    }
    
    combined_result.ast = std::move(merged_program);
    return combined_result;
}

bool CompilerCore::initializeCompilation(const CompilationOptions& options) {
    CHTL_PHASE_GUARD(context_manager_, CompilationPhase::INITIALIZATION);
    
    if (is_compiling_) {
        handleCompilationError("Compilation already in progress", CompilationPhase::INITIALIZATION);
        return false;
    }
    
    is_compiling_ = true;
    current_options_ = options;
    current_progress_ = 0.0;
    
    // 清理之前的状态
    compilation_errors_.clear();
    compilation_warnings_.clear();
    generated_code_.clear();
    current_ast_.reset();
    
    // 重置组件
    context_manager_.reset();
    
    updateProgress(CompilationPhase::INITIALIZATION);
    return true;
}

bool CompilerCore::lexicalAnalysis(const std::string& input, const std::string& file_path) {
    CHTL_PHASE_GUARD(context_manager_, CompilationPhase::LEXICAL_SCANNING);
    
    if (!file_path.empty()) {
        context_manager_.updatePosition(file_path, 1, 1);
    }
    
    try {
        // 创建词法分析器
        lexer_ = std::make_unique<Lexer>(context_manager_.getCompilerContext());
        
        if (!file_path.empty()) {
            lexer_->setInputFile(file_path);
        } else {
            lexer_->setInput(input);
        }
        
        // 执行词法分析（通过Token生成验证）
        Token token;
        do {
            token = lexer_->nextToken();
            
            // 检查词法错误
            if (token.type == TokenType::INVALID) {
                handleCompilationError("Lexical error: " + token.value, 
                                     CompilationPhase::LEXICAL_SCANNING);
                return false;
            }
        } while (token.type != TokenType::EOF_TOKEN);
        
        // 重置词法分析器以供解析器使用
        if (!file_path.empty()) {
            lexer_->setInputFile(file_path);
        } else {
            lexer_->setInput(input);
        }
        
        updateProgress(CompilationPhase::LEXICAL_SCANNING);
        return true;
        
    } catch (const std::exception& e) {
        handleCompilationError("Lexical analysis failed: " + std::string(e.what()),
                             CompilationPhase::LEXICAL_SCANNING);
        return false;
    }
}

bool CompilerCore::syntaxAnalysis() {
    CHTL_PHASE_GUARD(context_manager_, CompilationPhase::SYNTAX_PARSING);
    
    if (!lexer_) {
        handleCompilationError("Lexer not initialized", CompilationPhase::SYNTAX_PARSING);
        return false;
    }
    
    try {
        // 创建解析器
        parser_ = std::make_unique<parser::CHTLParser>(context_manager_.getCompilerContext());
        parser_->setLexer(std::move(lexer_));
        
        // 执行语法分析
        current_ast_ = parser_->parseProgram();
        
        if (!current_ast_) {
            handleCompilationError("Failed to parse program", CompilationPhase::SYNTAX_PARSING);
            return false;
        }
        
        // 检查解析错误
        if (parser_->hasError()) {
            auto parser_errors = parser_->getErrors();
            for (const auto& error : parser_errors) {
                handleCompilationError("Parse error: " + error.message, 
                                     CompilationPhase::SYNTAX_PARSING);
            }
            return false;
        }
        
        // 同步AST节点状态
        context_manager_.getASTStateManager().setSubtreeState(
            current_ast_.get(), 
            ast::NodeState::PARSED, 
            ast::ProcessingPhase::SYNTAX_ANALYSIS
        );
        
        updateProgress(CompilationPhase::SYNTAX_PARSING);
        return true;
        
    } catch (const std::exception& e) {
        handleCompilationError("Syntax analysis failed: " + std::string(e.what()),
                             CompilationPhase::SYNTAX_PARSING);
        return false;
    }
}

bool CompilerCore::semanticAnalysis() {
    CHTL_PHASE_GUARD(context_manager_, CompilationPhase::SEMANTIC_ANALYSIS);
    
    if (!current_ast_) {
        handleCompilationError("AST not available", CompilationPhase::SEMANTIC_ANALYSIS);
        return false;
    }
    
    try {
        // 执行语义分析
        // 1. 验证AST节点状态一致性
        if (!context_manager_.getASTStateManager().validateSubtreeStates(current_ast_.get())) {
            handleCompilationError("AST state validation failed", CompilationPhase::SEMANTIC_ANALYSIS);
            return false;
        }
        
        // 2. 检查CHTL语法约束
        if (!validateCHTLSemantics(current_ast_.get())) {
            return false;
        }
        
        // 3. 构建符号表和依赖关系
        if (!buildSymbolTable(current_ast_.get())) {
            return false;
        }
        
        // 4. 类型检查和语义验证
        if (!performSemanticValidation(current_ast_.get())) {
            return false;
        }
        
        // 更新AST节点状态
        context_manager_.getASTStateManager().setSubtreeState(
            current_ast_.get(), 
            ast::NodeState::VALIDATED, 
            ast::ProcessingPhase::SEMANTIC_ANALYSIS
        );
        
        updateProgress(CompilationPhase::SEMANTIC_ANALYSIS);
        return true;
        
    } catch (const std::exception& e) {
        handleCompilationError("Semantic analysis failed: " + std::string(e.what()),
                             CompilationPhase::SEMANTIC_ANALYSIS);
        return false;
    }
}

bool CompilerCore::optimization() {
    CHTL_PHASE_GUARD(context_manager_, CompilationPhase::OPTIMIZATION);
    
    if (!current_ast_ || !current_options_.enable_optimization) {
        updateProgress(CompilationPhase::OPTIMIZATION);
        return true; // 跳过优化
    }
    
    try {
        // 执行AST优化
        if (!optimizeAST(current_ast_.get())) {
            return false;
        }
        
        // 更新AST节点状态
        context_manager_.getASTStateManager().setSubtreeState(
            current_ast_.get(), 
            ast::NodeState::TRANSFORMED, 
            ast::ProcessingPhase::OPTIMIZATION
        );
        
        updateProgress(CompilationPhase::OPTIMIZATION);
        return true;
        
    } catch (const std::exception& e) {
        handleCompilationError("Optimization failed: " + std::string(e.what()),
                             CompilationPhase::OPTIMIZATION);
        return false;
    }
}

bool CompilerCore::codeGeneration() {
    CHTL_PHASE_GUARD(context_manager_, CompilationPhase::CODE_GENERATION);
    
    if (!current_ast_) {
        handleCompilationError("AST not available", CompilationPhase::CODE_GENERATION);
        return false;
    }
    
    try {
        // TODO: 创建代码生成器 - 待实现
        // generator_ = std::make_unique<generator::Generator>(context_manager_.getCompilerContext());
        
        // 执行代码生成 - 临时实现
        generated_code_ = "<!-- Generated HTML from CHTL -->\n";
        generated_code_ += "<!-- TODO: Implement actual code generation -->\n";
        
        // 更新AST节点状态
        context_manager_.getASTStateManager().setSubtreeState(
            current_ast_.get(), 
            ast::NodeState::GENERATED, 
            ast::ProcessingPhase::CODE_GENERATION
        );
        
        updateProgress(CompilationPhase::CODE_GENERATION);
        return true;
        
    } catch (const std::exception& e) {
        handleCompilationError("Code generation failed: " + std::string(e.what()),
                             CompilationPhase::CODE_GENERATION);
        return false;
    }
}

bool CompilerCore::finalize() {
    CHTL_PHASE_GUARD(context_manager_, CompilationPhase::FINALIZATION);
    
    try {
        // 收集编译统计信息
        collectCompilationStatistics();
        
        // 验证最终状态一致性
        if (!validateStateConsistency()) {
            handleCompilationWarning("State consistency validation failed", 
                                   CompilationPhase::FINALIZATION);
        }
        
        // 清理临时状态
        cleanupComponents();
        
        is_compiling_ = false;
        updateProgress(CompilationPhase::FINALIZATION);
        
        return true;
        
    } catch (const std::exception& e) {
        handleCompilationError("Finalization failed: " + std::string(e.what()),
                             CompilationPhase::FINALIZATION);
        is_compiling_ = false;
        return false;
    }
}

CompilationResult CompilerCore::getCompilationResult() {
    CompilationResult result;
    
    result.success = compilation_errors_.empty();
    result.errors = compilation_errors_;
    result.warnings = compilation_warnings_;
    result.generated_code = generated_code_;
    
    // 计算编译时间
    if (compilation_start_time_ != std::chrono::steady_clock::time_point{}) {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
            end_time - compilation_start_time_);
        result.compilation_time_ms = duration.count() / 1000.0;
    }
    
    // 克隆AST（如果存在）
    if (current_ast_) {
        result.ast = std::unique_ptr<ast::ProgramNode>(
            static_cast<ast::ProgramNode*>(current_ast_->clone().release()));
    }
    
    return result;
}

CompilationPhase CompilerCore::getCurrentPhase() const {
    return context_manager_.getCurrentPhase();
}

bool CompilerCore::isCompiling() const {
    return is_compiling_;
}

double CompilerCore::getProgress() const {
    return current_progress_;
}

std::vector<std::string> CompilerCore::getErrors() const {
    return compilation_errors_;
}

std::vector<std::string> CompilerCore::getWarnings() const {
    return compilation_warnings_;
}

bool CompilerCore::hasErrors() const {
    return !compilation_errors_.empty();
}

bool CompilerCore::hasWarnings() const {
    return !compilation_warnings_.empty();
}

void CompilerCore::clearErrors() {
    compilation_errors_.clear();
}

void CompilerCore::clearWarnings() {
    compilation_warnings_.clear();
}

void CompilerCore::setProgressCallback(CompilationProgressCallback callback) {
    progress_callback_ = callback;
}

void CompilerCore::removeProgressCallback() {
    progress_callback_ = nullptr;
}

void CompilerCore::setAST(std::unique_ptr<ast::ProgramNode> ast) {
    current_ast_ = std::move(ast);
}

std::string CompilerCore::getCompilationStatistics() const {
    std::ostringstream oss;
    oss << "Compilation Statistics:\n";
    oss << "Phase: " << context_manager_.compilationPhaseToString(getCurrentPhase()) << "\n";
    oss << "Progress: " << (current_progress_ * 100.0) << "%\n";
    oss << "Errors: " << compilation_errors_.size() << "\n";
    oss << "Warnings: " << compilation_warnings_.size() << "\n";
    
    if (current_ast_) {
        oss << "AST Nodes: " << context_manager_.getASTStateManager().getNodeCount() << "\n";
    }
    
    oss << context_manager_.getStatistics();
    
    return oss.str();
}

void CompilerCore::printCompilationTrace() const {
    std::cout << getCompilationStatistics() << std::endl;
}

void CompilerCore::dumpAST() const {
    if (current_ast_) {
        std::cout << "AST Dump:\n";
        std::cout << current_ast_->toString() << std::endl;
    } else {
        std::cout << "No AST available\n";
    }
}

void CompilerCore::dumpStates() const {
    std::cout << "State Dump:\n";
    context_manager_.printCurrentState();
    context_manager_.getASTStateManager().printNodeStates();
}

void CompilerCore::reset() {
    is_compiling_ = false;
    current_progress_ = 0.0;
    compilation_errors_.clear();
    compilation_warnings_.clear();
    generated_code_.clear();
    current_ast_.reset();
    
    context_manager_.reset();
    cleanupComponents();
}

void CompilerCore::clear() {
    reset();
    context_manager_.clear();
    progress_callback_ = nullptr;
}

// 私有方法实现
bool CompilerCore::validateInput(const std::string& input) const {
    if (input.empty()) {
        return false;
    }
    
    // 基本的输入验证
    // 可以添加更多的验证逻辑
    return true;
}

bool CompilerCore::validateOptions(const CompilationOptions& options) const {
    // 验证输出格式
    if (options.output_format != "html" && 
        options.output_format != "css" && 
        options.output_format != "js") {
        return false;
    }
    
    return true;
}

void CompilerCore::updateProgress(CompilationPhase phase) {
    // 根据编译阶段计算进度
    double progress = 0.0;
    switch (phase) {
        case CompilationPhase::INITIALIZATION:
            progress = 0.1;
            break;
        case CompilationPhase::LEXICAL_SCANNING:
            progress = 0.3;
            break;
        case CompilationPhase::SYNTAX_PARSING:
            progress = 0.5;
            break;
        case CompilationPhase::SEMANTIC_ANALYSIS:
            progress = 0.7;
            break;
        case CompilationPhase::OPTIMIZATION:
            progress = 0.8;
            break;
        case CompilationPhase::CODE_GENERATION:
            progress = 0.9;
            break;
        case CompilationPhase::FINALIZATION:
            progress = 1.0;
            break;
    }
    
    current_progress_ = progress;
    reportProgress(phase, progress);
}

void CompilerCore::reportProgress(CompilationPhase phase, double progress) {
    if (progress_callback_) {
        progress_callback_(phase, progress);
    }
}

void CompilerCore::handleCompilationError(const std::string& error_message, CompilationPhase phase) {
    std::string full_message = "[" + context_manager_.compilationPhaseToString(phase) + "] " + error_message;
    compilation_errors_.push_back(full_message);
    context_manager_.reportContextError(full_message);
}

void CompilerCore::handleCompilationWarning(const std::string& warning_message, CompilationPhase phase) {
    std::string full_message = "[" + context_manager_.compilationPhaseToString(phase) + "] " + warning_message;
    compilation_warnings_.push_back(full_message);
}

void CompilerCore::syncStatesBetweenPhases() {
    context_manager_.syncWithCompilerContext();
    context_manager_.syncWithASTStateManager();
}

bool CompilerCore::validatePhaseTransition(CompilationPhase from, CompilationPhase to) const {
    return context_manager_.canTransitionToPhase(to);
}

void CompilerCore::initializeComponents() {
    // 组件在需要时动态创建
}

void CompilerCore::cleanupComponents() {
    lexer_.reset();
    parser_.reset();
    // generator_.reset();  // 待实现
}

void CompilerCore::collectCompilationStatistics() {
    // 收集各种统计信息
    // 这里可以添加更详细的统计逻辑
}

bool CompilerCore::validateStateConsistency() const {
    return context_manager_.validateContextConsistency();
}

// 语义分析辅助方法
bool CompilerCore::validateCHTLSemantics(ast::ASTNode* node) {
    if (!node) return false;
    
    // 根据CHTL语法规范验证语义
    switch (node->type) {
        case ast::NodeType::TEMPLATE_DEFINITION:
            return validateTemplateSemantics(node);
        case ast::NodeType::CUSTOM_DEFINITION:
            return validateCustomSemantics(node);
        case ast::NodeType::STYLE_BLOCK:
            return validateStyleBlockSemantics(node);
        case ast::NodeType::SCRIPT_BLOCK:
            return validateScriptBlockSemantics(node);
        case ast::NodeType::IMPORT_STATEMENT:
            return validateImportSemantics(node);
        case ast::NodeType::NAMESPACE_DEFINITION:
            return validateNamespaceSemantics(node);
        default:
            break;
    }
    
    // 递归验证子节点
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        if (!validateCHTLSemantics(node->getChild(i))) {
            return false;
        }
    }
    
    return true;
}

bool CompilerCore::buildSymbolTable(ast::ASTNode* node) {
    // 构建符号表的实现
    // 这里需要根据CHTL语法规范实现
    return true;
}

bool CompilerCore::performSemanticValidation(ast::ASTNode* node) {
    // 执行语义验证的实现
    // 这里需要根据CHTL语法规范实现
    return true;
}

bool CompilerCore::optimizeAST(ast::ASTNode* node) {
    // AST优化的实现
    return true;
}

// 语义验证辅助方法
bool CompilerCore::validateTemplateSemantics(ast::ASTNode* node) {
    // 验证Template语义
    return true;
}

bool CompilerCore::validateCustomSemantics(ast::ASTNode* node) {
    // 验证Custom语义
    return true;
}

bool CompilerCore::validateStyleBlockSemantics(ast::ASTNode* node) {
    // 验证样式块语义
    return true;
}

bool CompilerCore::validateScriptBlockSemantics(ast::ASTNode* node) {
    // 验证脚本块语义
    return true;
}

bool CompilerCore::validateImportSemantics(ast::ASTNode* node) {
    // 验证Import语义
    return true;
}

bool CompilerCore::validateNamespaceSemantics(ast::ASTNode* node) {
    // 验证Namespace语义
    return true;
}

// CompilerFactory 实现
std::unique_ptr<CompilerCore> CompilerFactory::createStandardCompiler() {
    return std::make_unique<CompilerCore>();
}

std::unique_ptr<CompilerCore> CompilerFactory::createDebugCompiler() {
    auto compiler = std::make_unique<CompilerCore>();
    // 配置调试选项
    return compiler;
}

std::unique_ptr<CompilerCore> CompilerFactory::createOptimizedCompiler() {
    auto compiler = std::make_unique<CompilerCore>();
    // 配置优化选项
    return compiler;
}

std::unique_ptr<CompilerCore> CompilerFactory::createCustomCompiler(const CompilationOptions& options) {
    auto compiler = std::make_unique<CompilerCore>();
    // 使用自定义选项配置
    return compiler;
}

// 便利函数实现
namespace compiler_utils {

CompilationResult quickCompile(const std::string& source_code) {
    auto compiler = CompilerFactory::createStandardCompiler();
    return compiler->compileFromString(source_code);
}

CompilationResult quickCompileFile(const std::string& file_path) {
    auto compiler = CompilerFactory::createStandardCompiler();
    return compiler->compileFromFile(file_path);
}

bool validateSyntax(const std::string& source_code) {
    auto result = quickCompile(source_code);
    return result.success;
}

bool validateFile(const std::string& file_path) {
    auto result = quickCompileFile(file_path);
    return result.success;
}

std::string formatCode(const std::string& source_code) {
    // 代码格式化实现
    return source_code; // 简单实现，返回原代码
}

std::string astToString(const ast::ASTNode* node) {
    if (!node) return "";
    return node->toString();
}

std::string astToJson(const ast::ASTNode* node) {
    // AST转JSON的实现
    if (!node) return "{}";
    
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"type\": \"" << node->getNodeTypeName() << "\",\n";
    oss << "  \"children\": [\n";
    
    for (size_t i = 0; i < node->getChildCount(); ++i) {
        if (i > 0) oss << ",\n";
        oss << "    " << astToJson(node->getChild(i));
    }
    
    oss << "\n  ]\n";
    oss << "}";
    
    return oss.str();
}

} // namespace compiler_utils

} // namespace chtl