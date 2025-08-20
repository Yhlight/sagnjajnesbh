#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include "../../../../include/chtl_js/module/CJMODSystem.h"

using namespace chtl::chtljs::module;

/**
 * 珂朵莉CJMOD扩展
 * 实现iNeverAway和printMylove功能
 */
class ChthollyExtension : public CJMODExtensionBase {
public:
    ChthollyExtension() 
        : CJMODExtensionBase("Chtholly", "1.0.0", 
                           "珂朵莉扩展 - 世界上最幸福的女孩") {
        m_Keywords = {"iNeverAway", "printMylove"};
    }
    
    std::vector<std::string> GetKeywords() const override {
        return m_Keywords;
    }
    
    std::vector<std::string> SyntaxAnalysis(const std::string& code, 
                                          const std::string& ignoreChars) override {
        std::vector<std::string> args;
        
        // 移除忽略字符
        std::string cleanCode = code;
        for (char c : ignoreChars) {
            cleanCode.erase(std::remove(cleanCode.begin(), cleanCode.end(), c), 
                          cleanCode.end());
        }
        
        // 分割参数
        std::istringstream stream(cleanCode);
        std::string token;
        
        while (stream >> token) {
            if (token == "$") {
                args.push_back("$"); // 占位符
            } else {
                args.push_back(token);
            }
        }
        
        return args;
    }
    
    void ScanKeyword(IScannerInterface* scanner,
                    const std::string& keyword,
                    std::function<void()> callback) override {
        if (keyword == "iNeverAway" || keyword == "printMylove") {
            callback();
        }
    }
    
    std::string GenerateCode(ast::CHTLJSASTNode* node) override {
        // 这里应该根据AST节点生成代码
        // 为了示例，返回简单的JavaScript代码
        return GenerateINeverAwayCode();
    }
    
private:
    std::string GenerateINeverAwayCode() {
        return R"js(
// iNeverAway implementation
window.__chtholly_iNeverAway = function(config) {
    const defaults = {
        loop: 10,
        max: 5,
        min: 1,
        delay: 1000
    };
    
    const settings = Object.assign({}, defaults, config);
    const stateGroups = {};
    
    // 解析状态组
    Object.keys(config).forEach(key => {
        if (key.includes('<') && key.includes('>')) {
            const [funcName, state] = key.split('<');
            const stateName = state.replace('>', '');
            
            if (!stateGroups[stateName]) {
                stateGroups[stateName] = {};
            }
            
            stateGroups[stateName][funcName] = config[key];
        }
    });
    
    // 执行循环
    let count = 0;
    const interval = setInterval(() => {
        // 为每个状态组选择随机函数
        Object.keys(stateGroups).forEach(stateName => {
            const funcs = Object.keys(stateGroups[stateName]);
            const selected = Math.floor(Math.random() * 
                           Math.min(funcs.length, settings.max - settings.min + 1)) + 
                           settings.min;
            
            // 执行选中的函数
            for (let i = 0; i < selected && i < funcs.length; i++) {
                const funcName = funcs[i];
                const func = stateGroups[stateName][funcName];
                if (typeof func === 'function') {
                    func();
                }
            }
        });
        
        count++;
        if (count >= settings.loop) {
            clearInterval(interval);
        }
    }, settings.delay);
};
)js";
    }
    
    std::string GeneratePrintMyloveCode() {
        return R"js(
// printMylove implementation  
window.__chtholly_printMylove = function(config) {
    const { url, mode, width, height, scale } = config;
    
    if (!url) {
        console.error('printMylove: URL is required');
        return;
    }
    
    const img = new Image();
    img.onload = function() {
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d');
        
        // 计算尺寸
        let targetWidth = width || img.width;
        let targetHeight = height || img.height;
        
        if (scale) {
            targetWidth *= scale;
            targetHeight *= scale;
        }
        
        canvas.width = targetWidth;
        canvas.height = targetHeight;
        
        if (mode === 'ASCII') {
            // ASCII艺术模式
            ctx.drawImage(img, 0, 0, targetWidth, targetHeight);
            const imageData = ctx.getImageData(0, 0, targetWidth, targetHeight);
            const ascii = convertToASCII(imageData);
            console.log(ascii);
        } else if (mode === 'Pixel') {
            // 像素艺术模式
            ctx.imageSmoothingEnabled = false;
            ctx.drawImage(img, 0, 0, targetWidth, targetHeight);
            document.body.appendChild(canvas);
        } else {
            // 普通模式
            ctx.drawImage(img, 0, 0, targetWidth, targetHeight);
            document.body.appendChild(canvas);
        }
    };
    
    img.onerror = function() {
        console.error('printMylove: Failed to load image from', url);
    };
    
    img.src = url;
};

function convertToASCII(imageData) {
    const chars = ' .:-=+*#%@';
    const { data, width, height } = imageData;
    let ascii = '';
    
    for (let y = 0; y < height; y += 2) {
        for (let x = 0; x < width; x++) {
            const i = (y * width + x) * 4;
            const brightness = (data[i] + data[i + 1] + data[i + 2]) / 3;
            const charIndex = Math.floor(brightness / 255 * (chars.length - 1));
            ascii += chars[charIndex];
        }
        ascii += '\n';
    }
    
    return ascii;
}
)js";
    }
};

// 导出函数
extern "C" {
    ICJMODExtension* CreateCJMODExtension() {
        return new ChthollyExtension();
    }
    
    void DestroyCJMODExtension(ICJMODExtension* extension) {
        delete extension;
    }
    
    const char* GetCJMODVersion() {
        return "1.0.0";
    }
}