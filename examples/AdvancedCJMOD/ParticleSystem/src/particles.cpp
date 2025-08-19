// ========================================
// 粒子系统子模块 - 展示CJMOD子模块结构
// ========================================

#include <string>
#include <vector>
#include <cmath>
#include <random>
#include <sstream>

// 粒子系统的全局状态
namespace ParticleSystemGlobals {
    std::random_device rd;
    std::mt19937 gen(rd());
    int activeParticleSystems = 0;
    
    struct ParticleSystem {
        std::string id;
        int particleCount;
        bool isActive;
        double startTime;
    };
    
    std::vector<ParticleSystem> systems;
}

/**
 * 高级粒子效果处理
 * 使用复杂的物理计算和数学算法
 */
std::string generateAdvancedParticleEffect(const std::string& selector, 
                                          const std::string& config) {
    std::stringstream js;
    
    js << "(() => {\n";
    js << "    const container = " << selector << ";\n";
    js << "    if (!container) return;\n";
    js << "    \n";
    js << "    const config = " << config << ";\n";
    js << "    const particleCount = config.count || 100;\n";
    js << "    const duration = config.duration || 5000;\n";
    js << "    const gravity = config.gravity || 0.5;\n";
    js << "    const wind = config.wind || 0.1;\n";
    js << "    const colors = config.colors || ['#ff6b6b', '#4ecdc4', '#45b7d1', '#f9ca24', '#a55eea'];\n";
    js << "    \n";
    js << "    // 高级粒子物理系统\n";
    js << "    class Particle {\n";
    js << "        constructor(x, y) {\n";
    js << "            this.x = x;\n";
    js << "            this.y = y;\n";
    js << "            this.vx = (Math.random() - 0.5) * 10;\n";
    js << "            this.vy = (Math.random() - 0.5) * 10 - 5;\n";
    js << "            this.life = 1.0;\n";
    js << "            this.decay = Math.random() * 0.02 + 0.01;\n";
    js << "            this.size = Math.random() * 6 + 2;\n";
    js << "            this.color = colors[Math.floor(Math.random() * colors.length)];\n";
    js << "            this.rotation = 0;\n";
    js << "            this.rotationSpeed = (Math.random() - 0.5) * 0.2;\n";
    js << "            \n";
    js << "            // 创建DOM元素\n";
    js << "            this.element = document.createElement('div');\n";
    js << "            this.element.style.cssText = `\n";
    js << "                position: absolute;\n";
    js << "                width: ${this.size}px;\n";
    js << "                height: ${this.size}px;\n";
    js << "                background: ${this.color};\n";
    js << "                border-radius: 50%;\n";
    js << "                pointer-events: none;\n";
    js << "                z-index: 9999;\n";
    js << "                box-shadow: 0 0 ${this.size}px ${this.color};\n";
    js << "            `;\n";
    js << "            \n";
    js << "            document.body.appendChild(this.element);\n";
    js << "        }\n";
    js << "        \n";
    js << "        update() {\n";
    js << "            // 物理更新\n";
    js << "            this.vy += gravity;\n";
    js << "            this.vx += wind;\n";
    js << "            \n";
    js << "            this.x += this.vx;\n";
    js << "            this.y += this.vy;\n";
    js << "            \n";
    js << "            // 旋转\n";
    js << "            this.rotation += this.rotationSpeed;\n";
    js << "            \n";
    js << "            // 生命值衰减\n";
    js << "            this.life -= this.decay;\n";
    js << "            \n";
    js << "            // 更新DOM\n";
    js << "            this.element.style.left = this.x + 'px';\n";
    js << "            this.element.style.top = this.y + 'px';\n";
    js << "            this.element.style.opacity = Math.max(0, this.life);\n";
    js << "            this.element.style.transform = `rotate(${this.rotation}rad) scale(${this.life})`;\n";
    js << "            \n";
    js << "            return this.life > 0;\n";
    js << "        }\n";
    js << "        \n";
    js << "        destroy() {\n";
    js << "            if (this.element.parentNode) {\n";
    js << "                this.element.parentNode.removeChild(this.element);\n";
    js << "            }\n";
    js << "        }\n";
    js << "    }\n";
    js << "    \n";
    js << "    // 创建粒子\n";
    js << "    const particles = [];\n";
    js << "    const containerRect = container.getBoundingClientRect();\n";
    js << "    const centerX = containerRect.left + containerRect.width / 2;\n";
    js << "    const centerY = containerRect.top + containerRect.height / 2;\n";
    js << "    \n";
    js << "    for (let i = 0; i < particleCount; i++) {\n";
    js << "        // 从中心点发射粒子\n";
    js << "        const angle = (Math.PI * 2 * i) / particleCount;\n";
    js << "        const distance = Math.random() * 50;\n";
    js << "        const x = centerX + Math.cos(angle) * distance;\n";
    js << "        const y = centerY + Math.sin(angle) * distance;\n";
    js << "        \n";
    js << "        particles.push(new Particle(x, y));\n";
    js << "    }\n";
    js << "    \n";
    js << "    // 粒子系统主循环\n";
    js << "    function particleLoop() {\n";
    js << "        const aliveParticles = [];\n";
    js << "        \n";
    js << "        particles.forEach(particle => {\n";
    js << "            if (particle.update()) {\n";
    js << "                aliveParticles.push(particle);\n";
    js << "            } else {\n";
    js << "                particle.destroy();\n";
    js << "            }\n";
    js << "        });\n";
    js << "        \n";
    js << "        particles.length = 0;\n";
    js << "        particles.push(...aliveParticles);\n";
    js << "        \n";
    js << "        if (particles.length > 0) {\n";
    js << "            requestAnimationFrame(particleLoop);\n";
    js << "        } else {\n";
    js << "            console.log('✨ 所有粒子已消失，效果结束');\n";
    js << "        }\n";
    js << "    }\n";
    js << "    \n";
    js << "    requestAnimationFrame(particleLoop);\n";
    js << "    console.log('🎆 高级粒子系统启动，粒子数量:', particleCount);\n";
    js << "})()";
    
    return js.str();
}

// 子模块导出函数
extern "C" const char* processParticleSystemSyntax(const char* chtljs_code) {
    std::string code(chtljs_code);
    
    // 检查是否包含粒子系统相关语法
    if (code.find("particleEffect") != std::string::npos ||
        code.find("fireworks") != std::string::npos ||
        code.find("explosion") != std::string::npos) {
        
        // 这里可以使用更复杂的解析逻辑
        // 不仅仅是字符串查找，可以使用AST、语法分析等
        
        static std::string result = generateAdvancedParticleEffect("{{target}}", "{}");
        return result.c_str();
    }
    
    return chtljs_code;
}