#include "merger/result_merger.h"
#include <sstream>
#include <algorithm>

namespace chtl {

ResultMerger::ResultMerger() 
    : charset_("UTF-8"), 
      lang_("en") {
}

ResultMerger::~ResultMerger() = default;

void ResultMerger::addHTMLFragment(const std::string& fragment) {
    if (!fragment.empty()) {
        htmlFragments_.push_back(fragment);
    }
}

void ResultMerger::addGlobalStyle(const std::string& style) {
    if (!style.empty()) {
        document_.globalStyles.push_back(style);
    }
}

void ResultMerger::addLocalStyle(const std::string& style) {
    if (!style.empty()) {
        document_.localStyles.push_back(style);
    }
}

void ResultMerger::addGlobalScript(const std::string& script) {
    if (!script.empty()) {
        document_.globalScripts.push_back(script);
    }
}

void ResultMerger::addLocalScript(const std::string& script) {
    if (!script.empty()) {
        document_.localScripts.push_back(script);
    }
}

std::string ResultMerger::merge() {
    std::stringstream output;
    
    // DOCTYPE
    output << "<!DOCTYPE html>\n";
    
    // HTML开始标签
    output << "<html lang=\"" << lang_ << "\">\n";
    
    // HEAD部分
    output << buildHead();
    
    // BODY部分
    output << buildBody();
    
    // HTML结束标签
    output << "</html>\n";
    
    std::string result = output.str();
    optimizeOutput(result);
    
    return result;
}

void ResultMerger::reset() {
    document_ = HTMLDocument();
    htmlFragments_.clear();
    charset_ = "UTF-8";
    title_.clear();
    lang_ = "en";
    metaTags_.clear();
}

void ResultMerger::setDocumentOption(const std::string& key, const std::string& value) {
    if (key == "charset") {
        charset_ = value;
    } else if (key == "title") {
        title_ = value;
    } else if (key == "lang") {
        lang_ = value;
    } else if (key == "meta") {
        metaTags_.push_back(value);
    }
}

std::string ResultMerger::buildHead() {
    std::stringstream head;
    
    head << "<head>\n";
    
    // 字符集
    head << "  <meta charset=\"" << charset_ << "\">\n";
    
    // 视口
    head << "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    
    // 标题
    if (!title_.empty()) {
        head << "  <title>" << title_ << "</title>\n";
    } else {
        head << "  <title>CHTL Generated Page</title>\n";
    }
    
    // 其他meta标签
    for (const auto& meta : metaTags_) {
        head << "  " << meta << "\n";
    }
    
    // 合并的样式
    std::string mergedStyles = mergeStyles();
    if (!mergedStyles.empty()) {
        head << "  <style>\n";
        head << mergedStyles;
        head << "  </style>\n";
    }
    
    head << "</head>\n";
    
    return head.str();
}

std::string ResultMerger::buildBody() {
    std::stringstream body;
    
    body << "<body>\n";
    
    // 添加所有HTML片段
    for (const auto& fragment : htmlFragments_) {
        body << fragment;
        if (fragment.back() != '\n') {
            body << "\n";
        }
    }
    
    // 合并的脚本
    std::string mergedScripts = mergeScripts();
    if (!mergedScripts.empty()) {
        body << "  <script>\n";
        body << mergedScripts;
        body << "  </script>\n";
    }
    
    body << "</body>\n";
    
    return body.str();
}

std::string ResultMerger::mergeStyles() {
    std::stringstream styles;
    
    // 添加注释标记
    if (!document_.globalStyles.empty() || !document_.localStyles.empty()) {
        styles << "    /* CHTL Generated Styles */\n";
    }
    
    // 全局样式
    if (!document_.globalStyles.empty()) {
        styles << "    /* Global Styles */\n";
        for (const auto& style : document_.globalStyles) {
            styles << "    " << style;
            if (style.back() != '\n') {
                styles << "\n";
            }
        }
        styles << "\n";
    }
    
    // 局部样式（已被提升为全局）
    if (!document_.localStyles.empty()) {
        styles << "    /* Local Styles (Promoted) */\n";
        for (const auto& style : document_.localStyles) {
            styles << "    " << style;
            if (style.back() != '\n') {
                styles << "\n";
            }
        }
    }
    
    return styles.str();
}

std::string ResultMerger::mergeScripts() {
    std::stringstream scripts;
    
    // 添加注释标记
    if (!document_.globalScripts.empty() || !document_.localScripts.empty()) {
        scripts << "    /* CHTL Generated Scripts */\n";
    }
    
    // 全局脚本
    if (!document_.globalScripts.empty()) {
        scripts << "    /* Global Scripts */\n";
        for (const auto& script : document_.globalScripts) {
            scripts << "    " << script;
            if (script.back() != '\n') {
                scripts << "\n";
            }
        }
        scripts << "\n";
    }
    
    // 局部脚本（如果有）
    if (!document_.localScripts.empty()) {
        scripts << "    /* Local Scripts */\n";
        scripts << "    (function() {\n";
        for (const auto& script : document_.localScripts) {
            scripts << "      " << script;
            if (script.back() != '\n') {
                scripts << "\n";
            }
        }
        scripts << "    })();\n";
    }
    
    return scripts.str();
}

void ResultMerger::optimizeOutput(std::string& html) {
    // 移除多余的空行
    std::string::size_type pos = 0;
    while ((pos = html.find("\n\n\n", pos)) != std::string::npos) {
        html.replace(pos, 3, "\n\n");
        pos += 2;
    }
    
    // 确保正确的缩进
    std::istringstream stream(html);
    std::string line;
    std::stringstream optimized;
    
    while (std::getline(stream, line)) {
        // 移除行尾空白
        size_t end = line.find_last_not_of(" \t\r");
        if (end != std::string::npos) {
            line = line.substr(0, end + 1);
        }
        
        // 保留非空行
        if (!line.empty() || optimized.str().empty() || optimized.str().back() != '\n') {
            optimized << line << "\n";
        }
    }
    
    html = optimized.str();
}

} // namespace chtl