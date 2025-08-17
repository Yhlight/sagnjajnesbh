#pragma once
#include <string>

namespace chtl {

// 版本信息
constexpr const char* CHTL_VERSION = "1.0.0";
constexpr const char* CHTL_BUILD_DATE = __DATE__;

// 文件扩展名
constexpr const char* CHTL_FILE_EXT = ".chtl";
constexpr const char* CMOD_FILE_EXT = ".cmod";
constexpr const char* CJMOD_FILE_EXT = ".cjmod";
constexpr const char* HTML_FILE_EXT = ".html";
constexpr const char* CSS_FILE_EXT = ".css";
constexpr const char* JS_FILE_EXT = ".js";

// 目录名称
constexpr const char* MODULE_DIR = "module";
constexpr const char* SRC_DIR = "src";
constexpr const char* INFO_DIR = "info";

// 关键字
constexpr const char* TEMPLATE_KEYWORD = "Template";
constexpr const char* CUSTOM_KEYWORD = "Custom";
constexpr const char* ORIGIN_KEYWORD = "Origin";
constexpr const char* IMPORT_KEYWORD = "Import";
constexpr const char* NAMESPACE_KEYWORD = "Namespace";
constexpr const char* CONFIGURATION_KEYWORD = "Configuration";

// CHTL JS关键字
constexpr const char* LISTEN_KEYWORD = "listen";
constexpr const char* DELEGATE_KEYWORD = "delegate";
constexpr const char* ANIMATE_KEYWORD = "animate";
constexpr const char* VIR_KEYWORD = "vir";

// 特殊符号
constexpr const char* ARROW_OPERATOR = "->";
constexpr const char* SELECTOR_START = "{{";
constexpr const char* SELECTOR_END = "}}";
constexpr const char* AT_SYMBOL = "@";

// 官方模块前缀
constexpr const char* OFFICIAL_MODULE_PREFIX = "chtl::";

// 默认配置
constexpr size_t DEFAULT_BUFFER_SIZE = 8192;
constexpr size_t MAX_RECURSION_DEPTH = 1000;
constexpr size_t MAX_ERROR_COUNT = 100;

} // namespace chtl