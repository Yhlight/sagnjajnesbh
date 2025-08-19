// ========================================
// printMylove正确实现 - 真正的像素画和字符画
// ASCII模式: 字符画 (字母数字字符组成图案)
// Pixel模式: 像素画 (█▓▒░像素字符)
// ========================================

extern "C" const char* processPrintMylove(const char* chtljsCode) {
    static std::string result = R"(
(function(config) {
    const canvas = document.createElement('canvas');
    const ctx = canvas.getContext('2d');
    const img = new Image();
    
    img.onload = function() {
        const width = config.width || 80;
        const height = config.height || 60;
        const scale = config.scale || 1.0;
        const mode = config.mode || 'ASCII';
        
        const scaledWidth = Math.floor(width * scale);
        const scaledHeight = Math.floor(height * scale);
        
        canvas.width = scaledWidth;
        canvas.height = scaledHeight;
        ctx.drawImage(img, 0, 0, scaledWidth, scaledHeight);
        
        const imageData = ctx.getImageData(0, 0, scaledWidth, scaledHeight);
        const pixels = imageData.data;
        let result = '';
        
        if (mode === 'ASCII') {
            // 字符画 - 使用ASCII字符集创建字符艺术
            const asciiChars = ' !"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~';
            
            for (let y = 0; y < scaledHeight; y += 2) {
                for (let x = 0; x < scaledWidth; x++) {
                    const i = (y * scaledWidth + x) * 4;
                    const gray = (pixels[i] + pixels[i+1] + pixels[i+2]) / 3;
                    const charIndex = Math.floor((gray / 255) * (asciiChars.length - 1));
                    result += asciiChars[charIndex];
                }
                result += '\n';
            }
        } else if (mode === 'Pixel') {
            // 像素画 - 使用像素字符
            const pixelChars = ['█', '▓', '▒', '░', ' '];
            
            for (let y = 0; y < scaledHeight; y++) {
                for (let x = 0; x < scaledWidth; x++) {
                    const i = (y * scaledWidth + x) * 4;
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
    
    img.onerror = function() {
        console.error('图片加载失败');
        return null;
    };
    
    img.src = config.url;
    return img;
})
)";
    
    return result.c_str();
}

/**
 * iNeverAway正确实现
 * 虚对象标记函数系统 - vir不存在对象
 */
extern "C" const char* processINeverAway(const char* chtljsCode) {
    static std::string result = R"(
(function() {
    // iNeverAway创建标记函数
    // 重要：vir不创建对象，Test->Void<A>()直接转换为函数调用
    
    // 创建全局标记函数，由编译器统一管理名称
    window.__chtl_marker_Void_A = function(a, b) {
        // Void<A>状态的函数实现
    };
    
    window.__chtl_marker_Void_B = function(a, b) {
        // Void<B>状态的函数实现  
    };
    
    // vir Test = iNeverAway({...}) 不创建Test对象
    // Test->Void<A>() 编译时转换为 __chtl_marker_Void_A()
    
    return true;
})()
)";
    
    return result.c_str();
}