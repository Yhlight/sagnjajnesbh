// ========================================
// 珂朵莉CJMOD扩展 - 简洁版本
// printMylove: 图片转控制台字符
// iNeverAway: 虚对象标记函数(无对象)
// ========================================

#include <string>
#include <iostream>

/**
 * printMylove - 简洁实现
 * 图片转字符输出到控制台
 */
extern "C" const char* processPrintMylove(const char* chtljsCode) {
    static std::string result = R"(
(function() {
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    const img = new Image();
    
    img.onload = function() {
        canvas.width = 80;
        canvas.height = 60;
        ctx.drawImage(img, 0, 0, 80, 60);
        
        const imageData = ctx.getImageData(0, 0, 80, 60);
        const pixels = imageData.data;
        let result = '';
        const chars = ' .:-=+*#%@';
        
        for (let y = 0; y < 60; y += 2) {
            for (let x = 0; x < 80; x++) {
                const i = (y * 80 + x) * 4;
                const gray = (pixels[i] + pixels[i+1] + pixels[i+2]) / 3;
                const charIndex = Math.floor((gray / 255) * 9);
                result += chars[charIndex];
            }
            result += '\n';
        }
        
        console.log(result);
        return result;
    };
    
    img.src = arguments[0].url;
})()
)";
    
    return result.c_str();
}

/**
 * iNeverAway - 简洁实现
 * 虚对象标记函数系统(vir不创建对象)
 */
extern "C" const char* processINeverAway(const char* chtljsCode) {
    static std::string result = R"(
(function() {
    // iNeverAway创建全局标记函数
    // vir对象不存在，只是编译期语法糖
    
    window.__chtl_markers = window.__chtl_markers || {};
    
    // 创建标记函数
    window.__chtl_marker_Void_A = function(a, b) {
        console.log('Void<A>:', a, b);
    };
    
    window.__chtl_marker_Void_B = function(a, b) {
        console.log('Void<B>:', a, b);
    };
    
    console.log('iNeverAway标记系统初始化');
    return true;
})()
)";
    
    return result.c_str();
}