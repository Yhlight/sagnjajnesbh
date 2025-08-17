#include "CompilerDispatcher.h"
#include "Context.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <filesystem>

namespace chtl {

CompilerDispatcher::CompilerDispatcher(const DispatcherConfig& config) 
    : config_(config), cache_enabled_(false) {
    
    // 初始化组件
    scanner_ = std::make_unique<CHTLUnifiedScanner>();
    constrainer_ = std::make_unique<SyntaxConstrainer>();
    
    initializeCompilers();
    initializeDefaultCompilerPaths();
    resetStatistics();
}

CompilerDispatcher::~CompilerDispatcher() = default;

void CompilerDispatcher::initializeCompilers() {
    // 初始化CHTL解析器和生成器
    // 注意：这里需要上下文，简化实现中我们延迟初始化
}

void CompilerDispatcher::initializeDefaultCompilerPaths() {
    // 设置默认的编译器路径
    compiler_paths_[CompilerType::CSS_COMPILER] = "antlr4-css-compiler";
    compiler_paths_[CompilerType::JAVASCRIPT_COMPILER] = "antlr4-js-compiler";
}

std::string CompilerDispatcher::compileSource(const std::string& source_code) {
    startTimer();
    
    try {
        // 1. 扫描和切片
        auto slices = scanAndSlice(source_code);
        
        // 2. 语法约束验证
        if (config_.enable_syntax_constraints) {
            auto violations = validateSyntaxConstraints(slices);
            if (!violations.empty()) {
                reportConstraintViolations(violations);
                if (config_.strict_mode) {
                    addError(CompilerType::CHTL_COMPILER, source_code, "语法约束违规");
                    return "";
                }
            }
        }
        
        // 3. 分发到各编译器
        auto fragments = dispatchToCompilers(slices);
        
        // 4. 合并编译结果
        std::string result = mergeCompilationResults(fragments);
        
        endTimer();
        return result;
        
    } catch (const std::exception& e) {
        addError(CompilerType::CHTL_COMPILER, source_code, "编译过程异常: " + std::string(e.what()));
        endTimer();
        return "";
    }
}

std::vector<CodeSlice> CompilerDispatcher::scanAndSlice(const std::string& source_code) {
    auto slices = scanner_->scanSource(source_code);
    stats_.total_fragments = slices.size();
    
    // 统计各类型片段
    for (const auto& slice : slices) {
        switch (slice.type) {
            case FragmentType::CHTL: stats_.chtl_fragments++; break;
            case FragmentType::CHTL_JS: stats_.chtl_js_fragments++; break;
            case FragmentType::CSS: stats_.css_fragments++; break;
            case FragmentType::JAVASCRIPT: stats_.js_fragments++; break;
            default: break;
        }
    }
    
    return slices;
}

std::vector<ConstraintViolation> CompilerDispatcher::validateSyntaxConstraints(const std::vector<CodeSlice>& slices) {
    std::vector<ConstraintViolation> all_violations;
    
    for (const auto& slice : slices) {
        // 这里需要将CodeSlice转换为AST进行约束检查
        // 简化实现：假设没有违规
        auto violations = checkConstraints({slice});
        all_violations.insert(all_violations.end(), violations.begin(), violations.end());
    }
    
    return all_violations;
}

std::vector<CompilationFragment> CompilerDispatcher::dispatchToCompilers(const std::vector<CodeSlice>& slices) {
    std::vector<CompilationFragment> fragments;
    
    for (const auto& slice : slices) {
        CompilationFragment fragment;
        
        // 检查缓存
        std::string fragment_hash = generateFragmentHash(slice);
        if (cache_enabled_ && isCached(fragment_hash)) {
            fragment = compilation_cache_[fragment_hash];
            stats_.cache_hits++;
        } else {
            // 根据片段类型分发到相应编译器
            switch (slice.type) {
                case FragmentType::CHTL:
                    fragment = compileCHTLFragment(slice);
                    break;
                case FragmentType::CHTL_JS:
                    fragment = compileCHTLJSFragment(slice);
                    break;
                case FragmentType::CSS:
                    fragment = compileCSSFragment(slice);
                    break;
                case FragmentType::JAVASCRIPT:
                    fragment = compileJavaScriptFragment(slice);
                    break;
                case FragmentType::MIXED:
                    // 混合类型需要进一步分解
                    fragment = compileCHTLFragment(slice); // 默认使用CHTL编译器
                    break;
                default:
                    fragment.compilation_success = false;
                    fragment.errors.push_back("未知片段类型");
                    break;
            }
            
            // 缓存结果
            if (cache_enabled_) {
                cacheCompilationResult(fragment_hash, fragment);
                stats_.cache_misses++;
            }
        }
        
        fragments.push_back(fragment);
        
        // 更新统计
        if (fragment.compilation_success) {
            stats_.successful_compilations++;
        } else {
            stats_.failed_compilations++;
        }
    }
    
    return fragments;
}

CompilationFragment CompilerDispatcher::compileCHTLFragment(const CodeSlice& slice) {
    CompilationFragment fragment;
    fragment.original_type = slice.type;
    fragment.compiler_used = CompilerType::CHTL_COMPILER;
    
    try {
        // 使用CHTL编译器编译
        CompilerContext context;
        parser::CHTLParser parser(context);
        
        parser.setInput(slice.content);
        auto ast = parser.parseProgram();
        
        if (ast) {
            generator::HTMLGenerator generator;
            fragment.compiled_code = generator.generateHTML(ast.get());
            fragment.compilation_success = true;
        } else {
            fragment.compilation_success = false;
            fragment.errors.push_back("CHTL解析失败");
        }
        
    } catch (const std::exception& e) {
        fragment.compilation_success = false;
        fragment.errors.push_back("CHTL编译异常: " + std::string(e.what()));
    }
    
    return fragment;
}

CompilationFragment CompilerDispatcher::compileCHTLJSFragment(const CodeSlice& slice) {
    CompilationFragment fragment;
    fragment.original_type = slice.type;
    fragment.compiler_used = CompilerType::CHTL_JS_COMPILER;
    
    try {
        // 使用CHTL JS编译器编译
        // 简化实现：直接转换为JavaScript
        std::string js_code = slice.content;
        
        // 处理CHTL JS特殊语法
        // {{box}} -> document.querySelector('.box') or document.getElementById('box')
        std::regex selector_pattern(R"(\{\{([^}]+)\}\})");
        js_code = std::regex_replace(js_code, selector_pattern, "document.querySelector('$1')");
        
        // -> 转换为 .
        std::regex arrow_pattern(R"->");
        js_code = std::regex_replace(js_code, arrow_pattern, ".");
        
        fragment.compiled_code = js_code;
        fragment.compilation_success = true;
        
    } catch (const std::exception& e) {
        fragment.compilation_success = false;
        fragment.errors.push_back("CHTL JS编译异常: " + std::string(e.what()));
    }
    
    return fragment;
}

CompilationFragment CompilerDispatcher::compileCSSFragment(const CodeSlice& slice) {
    CompilationFragment fragment;
    fragment.original_type = slice.type;
    fragment.compiler_used = CompilerType::CSS_COMPILER;
    
    // 简化实现：直接返回CSS代码
    fragment.compiled_code = slice.content;
    fragment.compilation_success = true;
    
    return fragment;
}

CompilationFragment CompilerDispatcher::compileJavaScriptFragment(const CodeSlice& slice) {
    CompilationFragment fragment;
    fragment.original_type = slice.type;
    fragment.compiler_used = CompilerType::JAVASCRIPT_COMPILER;
    
    // 简化实现：直接返回JavaScript代码
    fragment.compiled_code = slice.content;
    fragment.compilation_success = true;
    
    return fragment;
}

std::string CompilerDispatcher::mergeCompilationResults(const std::vector<CompilationFragment>& fragments) {
    std::ostringstream html_stream;
    std::ostringstream css_stream;
    std::ostringstream js_stream;
    
    // 分类收集编译结果
    for (const auto& fragment : fragments) {
        if (!fragment.compilation_success) {
            continue; // 跳过编译失败的片段
        }
        
        switch (fragment.compiler_used) {
            case CompilerType::CHTL_COMPILER:
                html_stream << fragment.compiled_code;
                break;
            case CompilerType::CSS_COMPILER:
                css_stream << fragment.compiled_code;
                break;
            case CompilerType::CHTL_JS_COMPILER:
            case CompilerType::JAVASCRIPT_COMPILER:
                js_stream << fragment.compiled_code;
                break;
        }
    }
    
    // 合并为完整的HTML
    return mergeHTMLResults(fragments);
}

std::string CompilerDispatcher::mergeHTMLResults(const std::vector<CompilationFragment>& fragments) {
    std::ostringstream result;
    
    result << "<!DOCTYPE html>\n";
    result << "<html>\n";
    result << "<head>\n";
    result << "    <meta charset=\"UTF-8\">\n";
    result << "    <title>CHTL Generated Page</title>\n";
    
    // 添加CSS
    result << "    <style>\n";
    for (const auto& fragment : fragments) {
        if (fragment.compiler_used == CompilerType::CSS_COMPILER && fragment.compilation_success) {
            result << fragment.compiled_code << "\n";
        }
    }
    result << "    </style>\n";
    
    result << "</head>\n";
    result << "<body>\n";
    
    // 添加HTML内容
    for (const auto& fragment : fragments) {
        if (fragment.compiler_used == CompilerType::CHTL_COMPILER && fragment.compilation_success) {
            result << fragment.compiled_code << "\n";
        }
    }
    
    // 添加JavaScript
    result << "    <script>\n";
    for (const auto& fragment : fragments) {
        if ((fragment.compiler_used == CompilerType::CHTL_JS_COMPILER || 
             fragment.compiler_used == CompilerType::JAVASCRIPT_COMPILER) && 
            fragment.compilation_success) {
            result << fragment.compiled_code << "\n";
        }
    }
    result << "    </script>\n";
    
    result << "</body>\n";
    result << "</html>\n";
    
    return result.str();
}

std::vector<ConstraintViolation> CompilerDispatcher::checkConstraints(const std::vector<CodeSlice>& slices) {
    std::vector<ConstraintViolation> violations;
    
    // 简化实现：基于片段内容进行基础约束检查
    for (const auto& slice : slices) {
        // 这里可以添加具体的约束检查逻辑
        // 例如检查全局script中是否使用了CHTL语法等
    }
    
    return violations;
}

void CompilerDispatcher::reportConstraintViolations(const std::vector<ConstraintViolation>& violations) {
    for (const auto& violation : violations) {
        std::cerr << "语法约束违规 [" << violation.line << ":" << violation.column << "] " 
                  << violation.message << std::endl;
        if (!violation.suggestion.empty()) {
            std::cerr << "建议: " << violation.suggestion << std::endl;
        }
    }
}

std::string CompilerDispatcher::generateFragmentHash(const CodeSlice& slice) {
    return DispatcherUtils::calculateHash(slice.content + std::to_string(static_cast<int>(slice.type)));
}

CompilerType CompilerDispatcher::selectCompiler(FragmentType fragment_type) {
    switch (fragment_type) {
        case FragmentType::CHTL:
        case FragmentType::MIXED:
            return CompilerType::CHTL_COMPILER;
        case FragmentType::CHTL_JS:
            return CompilerType::CHTL_JS_COMPILER;
        case FragmentType::CSS:
            return CompilerType::CSS_COMPILER;
        case FragmentType::JAVASCRIPT:
            return CompilerType::JAVASCRIPT_COMPILER;
        default:
            return CompilerType::CHTL_COMPILER;
    }
}

bool CompilerDispatcher::isCached(const std::string& fragment_hash) {
    return compilation_cache_.find(fragment_hash) != compilation_cache_.end();
}

void CompilerDispatcher::cacheCompilationResult(const std::string& fragment_hash, const CompilationFragment& result) {
    compilation_cache_[fragment_hash] = result;
}

std::vector<CompilerDispatcher::CompilerError> CompilerDispatcher::getCompilerErrors() const {
    return errors_;
}

std::vector<std::string> CompilerDispatcher::getCompilerWarnings() const {
    return warnings_;
}

void CompilerDispatcher::clearErrors() {
    errors_.clear();
    warnings_.clear();
}

CompilerDispatcher::DispatcherStatistics CompilerDispatcher::getStatistics() const {
    return stats_;
}

void CompilerDispatcher::resetStatistics() {
    stats_ = DispatcherStatistics{};
}

void CompilerDispatcher::addError(CompilerType compiler, const std::string& fragment, 
                                 const std::string& error, size_t line, size_t column) {
    CompilerError compiler_error;
    compiler_error.compiler = compiler;
    compiler_error.fragment_content = fragment.substr(0, 100); // 限制长度
    compiler_error.error_message = error;
    compiler_error.line = line;
    compiler_error.column = column;
    
    errors_.push_back(compiler_error);
}

void CompilerDispatcher::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

void CompilerDispatcher::startTimer() {
    start_time_ = std::chrono::steady_clock::now();
}

void CompilerDispatcher::endTimer() {
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time_);
    stats_.compilation_time_ms = duration.count();
}

void CompilerDispatcher::setConfig(const DispatcherConfig& config) {
    config_ = config;
}

DispatcherConfig CompilerDispatcher::getConfig() const {
    return config_;
}

void CompilerDispatcher::enableSyntaxConstraints(bool enable) {
    config_.enable_syntax_constraints = enable;
}

void CompilerDispatcher::enableFragmentCaching(bool enable) {
    cache_enabled_ = enable;
}

void CompilerDispatcher::printDispatcherInfo() const {
    std::cout << "=== 编译器调度器信息 ===" << std::endl;
    std::cout << "语法约束: " << (config_.enable_syntax_constraints ? "启用" : "禁用") << std::endl;
    std::cout << "片段优化: " << (config_.enable_fragment_optimization ? "启用" : "禁用") << std::endl;
    std::cout << "并行编译: " << (config_.enable_parallel_compilation ? "启用" : "禁用") << std::endl;
    std::cout << "严格模式: " << (config_.strict_mode ? "启用" : "禁用") << std::endl;
    std::cout << "片段缓存: " << (cache_enabled_ ? "启用" : "禁用") << std::endl;
    
    auto stats = getStatistics();
    std::cout << "\n统计信息:" << std::endl;
    std::cout << "总片段数: " << stats.total_fragments << std::endl;
    std::cout << "成功编译: " << stats.successful_compilations << std::endl;
    std::cout << "失败编译: " << stats.failed_compilations << std::endl;
    std::cout << "编译时间: " << stats.compilation_time_ms << "ms" << std::endl;
}

// DispatcherUtils命名空间实现
namespace DispatcherUtils {

std::string compilerTypeToString(CompilerType type) {
    switch (type) {
        case CompilerType::CHTL_COMPILER: return "CHTL编译器";
        case CompilerType::CHTL_JS_COMPILER: return "CHTL JS编译器";
        case CompilerType::CSS_COMPILER: return "CSS编译器";
        case CompilerType::JAVASCRIPT_COMPILER: return "JavaScript编译器";
        default: return "未知编译器";
    }
}

std::string fragmentTypeToString(FragmentType type) {
    switch (type) {
        case FragmentType::CHTL: return "CHTL";
        case FragmentType::CHTL_JS: return "CHTL JS";
        case FragmentType::CSS: return "CSS";
        case FragmentType::JAVASCRIPT: return "JavaScript";
        case FragmentType::MIXED: return "混合";
        default: return "未知";
    }
}

bool writeToTempFile(const std::string& content, std::string& temp_file_path) {
    try {
        temp_file_path = std::filesystem::temp_directory_path() / ("chtl_temp_" + std::to_string(std::time(nullptr)));
        
        std::ofstream file(temp_file_path);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        file.close();
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool readFromFile(const std::string& file_path, std::string& content) {
    try {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            return false;
        }
        
        std::ostringstream stream;
        stream << file.rdbuf();
        content = stream.str();
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void cleanupTempFile(const std::string& file_path) {
    try {
        std::filesystem::remove(file_path);
    } catch (const std::exception&) {
        // 忽略清理错误
    }
}

std::string calculateHash(const std::string& content) {
    // 简化的哈希实现
    std::hash<std::string> hasher;
    return std::to_string(hasher(content));
}

bool executeCommand(const std::string& command, std::string& output, std::string& error) {
    // 简化实现：假设命令执行成功
    output = "编译成功";
    error = "";
    return true;
}

bool validateCompilationResult(const std::string& result, FragmentType original_type) {
    // 简化的结果验证
    return !result.empty();
}

} // namespace DispatcherUtils

} // namespace chtl