#pragma once
#include "printMylove.h"
#include "iNeverAway.h"
#include <string>
#include <memory>

namespace chtl {
namespace chtholly {

/**
 * Chtholly模块的主接口
 * 统一管理printMylove和iNeverAway功能
 */
class ChthollyModuleInterface {
public:
    static ChthollyModuleInterface& getInstance();
    
    // printMylove接口
    std::string printMylove(const std::string& config_json);
    bool hasPrintMyloveError() const;
    std::string getPrintMyloveError() const;
    
    // iNeverAway接口  
    std::string processVirDeclaration(const std::string& vir_code);
    std::string processVirCall(const std::string& call_code);
    std::string generateINeverAwayCode();
    bool hasINeverAwayError() const;
    std::string getINeverAwayError() const;
    
    // 模块管理
    bool initialize();
    void cleanup();
    bool isInitialized() const { return initialized_; }
    
    // 配置管理
    void setDebugMode(bool enable);
    bool isDebugMode() const { return debug_mode_; }
    
    // 统计和信息
    struct ModuleStatistics {
        // printMylove统计
        size_t images_processed;
        size_t ascii_conversions;
        size_t pixel_conversions;
        
        // iNeverAway统计
        size_t virtual_objects_created;
        size_t function_calls_processed;
        size_t js_functions_generated;
        
        ModuleStatistics() : images_processed(0), ascii_conversions(0), pixel_conversions(0),
                           virtual_objects_created(0), function_calls_processed(0), 
                           js_functions_generated(0) {}
    };
    
    ModuleStatistics getStatistics() const;
    void printModuleInfo() const;
    
    // 错误处理
    bool hasAnyError() const;
    std::vector<std::string> getAllErrors() const;
    void clearAllErrors();
    
private:
    ChthollyModuleInterface() : printmylove_interface_(nullptr), ineveraway_interface_(nullptr), 
                               initialized_(false), debug_mode_(false) {}
    
    PrintMyloveInterface* printmylove_interface_;
    INeverAwayInterface* ineveraway_interface_;
    bool initialized_;
    bool debug_mode_;
    mutable ModuleStatistics stats_;
    
    static std::unique_ptr<ChthollyModuleInterface> instance_;
    
    // 内部管理
    void updateStatistics();
    void logDebugMessage(const std::string& message) const;
};

} // namespace chtholly
} // namespace chtl

// C接口，供CHTL JS编译器调用
extern "C" {
    // printMylove C接口
    const char* chtholly_printMylove(const char* config_json);
    
    // iNeverAway C接口
    const char* chtholly_processVirDeclaration(const char* vir_code);
    const char* chtholly_processVirCall(const char* call_code);
    const char* chtholly_generateINeverAwayCode();
    
    // 模块管理C接口
    int chtholly_initialize();
    void chtholly_cleanup();
    int chtholly_hasError();
    const char* chtholly_getLastError();
    void chtholly_clearErrors();
    
    // 调试接口
    void chtholly_setDebugMode(int enable);
    void chtholly_printModuleInfo();
}