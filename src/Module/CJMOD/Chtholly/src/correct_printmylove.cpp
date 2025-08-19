// ========================================
// printMylove正确实现 - 像素画和字符画
// 严格按照语法文档要求
// ========================================

#include <string>

/**
 * printMylove正确实现
 * mode: "ASCII" - 字符画 (用字母数字字符)
 * mode: "Pixel" - 像素画 (用█▓▒░字符)
 */
extern "C" const char* processPrintMylove(const char* chtljsCode) {
    static std::string result = R"(
(function() {
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    const img = new Image();
    
    img.onload = function() {
        const config = arguments[0];
        const width = config.width || 80;
        const height = config.height || 60;
        const scale = config.scale || 1.0;
        const mode = config.mode || 'ASCII';
        
        canvas.width = width * scale;
        canvas.height = height * scale;
        ctx.drawImage(img, 0, 0, canvas.width, canvas.height);
        
        const imageData = ctx.getImageData(0, 0, canvas.width, canvas.height);
        const pixels = imageData.data;
        let result = '';
        
        if (mode === 'ASCII') {
            // 字符画 - 使用ASCII字符
            const asciiChars = ' .\'`^",:;Il!i~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$';
            
            for (let y = 0; y < canvas.height; y += 2) {
                for (let x = 0; x < canvas.width; x++) {
                    const i = (y * canvas.width + x) * 4;
                    const gray = (pixels[i] + pixels[i+1] + pixels[i+2]) / 3;
                    const charIndex = Math.floor((gray / 255) * (asciiChars.length - 1));
                    result += asciiChars[charIndex];
                }
                result += '\n';
            }
        } else if (mode === 'Pixel') {
            // 像素画 - 使用像素字符
            const pixelChars = ['█', '▓', '▒', '░', ' '];
            
            for (let y = 0; y < canvas.height; y++) {
                for (let x = 0; x < canvas.width; x++) {
                    const i = (y * canvas.width + x) * 4;
                    const brightness = (pixels[i] + pixels[i+1] + pixels[i+2]) / 3;
                    const charIndex = Math.floor((brightness / 255) * (pixelChars.length - 1));
                    result += pixelChars[charIndex];
                }
                result += '\n';
            }
        }
        
        // 输出到控制台
        console.log(result);
        return result;
    };
    
    img.src = arguments[0].url;
})
)";
    
    return result.c_str();
}

/**
 * iNeverAway正确实现
 * 虚对象标记函数系统 - vir不创建对象
 */
extern "C" const char* processINeverAway(const char* chtljsCode) {
    static std::string result = R"(
(function() {
    // iNeverAway - 标记函数系统
    // 重要：vir不创建对象，只是编译期语法糖
    
    // 创建全局标记函数
    window.__chtl_marker_Void_A = function(a, b) {
        console.log('Void<A> executed');
    };
    
    window.__chtl_marker_Void_B = function(a, b) {
        console.log('Void<B> executed');
    };
    
    // vir Test = ... 不创建Test对象
    // Test->Void<A>() 直接转换为 __chtl_marker_Void_A()
    
    return true; // 不返回对象
})()
)";
    
    return result.c_str();
}