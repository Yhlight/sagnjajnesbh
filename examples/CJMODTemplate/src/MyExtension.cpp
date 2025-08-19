// ========================================
// CJMOD扩展模板 - 标准的CJMOD开发模板
// 按照这个模板开发，保证与CHTL JS编译器兼容
// ========================================

#include "CJMODInterface.h"
#include <regex>
#include <sstream>
#include <iostream>

using namespace chtl::cjmod;

/**
 * 我的自定义扩展类
 * 继承ICJMODExtension接口
 */
class MyCustomExtension : public ICJMODExtension {
public:
    // ========================================
    // 必须实现的接口方法
    // ========================================
    
    std::string getExtensionName() const override {
        return "MyCustomExtension";
    }
    
    std::string getExtensionVersion() const override {
        return "1.0.0";
    }
    
    std::vector<std::string> getSupportedSyntax() const override {
        return {
            "toast",           // toast('消息', 'success')
            "slideShow",       // slideShow({{#container}}, images)
            "countdown",       // countdown({{#timer}}, 60, callback)
            "parallax",        // parallax({{#element}}, speed)
            "typewriter"       // typewriter({{#text}}, '要打字的内容', speed)
        };
    }
    
    bool supportsSyntax(const std::string& syntaxName) const override {
        auto supported = getSupportedSyntax();
        return std::find(supported.begin(), supported.end(), syntaxName) != supported.end();
    }
    
    // ========================================
    // 核心方法：解析CHTL JS语法
    // ========================================
    
    SyntaxMatch parseSyntax(const std::string& chtljs_code) const override {
        SyntaxMatch match;
        
        // 1. 尝试匹配toast语法
        if (parseToastSyntax(chtljs_code, match)) {
            return match;
        }
        
        // 2. 尝试匹配slideShow语法
        if (parseSlideShowSyntax(chtljs_code, match)) {
            return match;
        }
        
        // 3. 尝试匹配countdown语法
        if (parseCountdownSyntax(chtljs_code, match)) {
            return match;
        }
        
        // 4. 尝试匹配parallax语法
        if (parseParallaxSyntax(chtljs_code, match)) {
            return match;
        }
        
        // 5. 尝试匹配typewriter语法
        if (parseTypewriterSyntax(chtljs_code, match)) {
            return match;
        }
        
        // 没有匹配
        match.matched = false;
        return match;
    }
    
    // ========================================
    // 核心方法：生成JavaScript代码
    // ========================================
    
    std::string generateJavaScript(const SyntaxMatch& match, const CJMODContext& context) const override {
        if (match.syntaxName == "toast") {
            return generateToastJS(match, context);
        } else if (match.syntaxName == "slideShow") {
            return generateSlideShowJS(match, context);
        } else if (match.syntaxName == "countdown") {
            return generateCountdownJS(match, context);
        } else if (match.syntaxName == "parallax") {
            return generateParallaxJS(match, context);
        } else if (match.syntaxName == "typewriter") {
            return generateTypewriterJS(match, context);
        }
        
        return "";
    }
    
    bool validateParameters(const SyntaxMatch& match, std::vector<CJMODError>& errors) const override {
        if (match.syntaxName == "toast") {
            if (match.parameters.size() < 1) {
                errors.push_back({"toast语法至少需要1个参数", "语法验证", 0, 0, "error"});
                return false;
            }
        }
        // 其他验证...
        return true;
    }
    
    std::string getSyntaxDocumentation(const std::string& syntaxName) const override {
        if (syntaxName == "toast") {
            return "toast(message, type) - 显示吐司通知\n参数：message(字符串), type(可选，'success'|'error'|'warning'|'info')";
        } else if (syntaxName == "slideShow") {
            return "slideShow(container, images) - 创建图片轮播\n参数：container(选择器), images(图片数组)";
        }
        // 其他文档...
        return "暂无文档";
    }
    
    bool initialize(const CJMODContext& context) override {
        std::cout << "🚀 MyCustomExtension 初始化完成" << std::endl;
        return true;
    }
    
    void cleanup() override {
        std::cout << "🧹 MyCustomExtension 清理完成" << std::endl;
    }

private:
    // ========================================
    // 私有方法：具体的语法解析实现
    // ========================================
    
    bool parseToastSyntax(const std::string& code, SyntaxMatch& match) const {
        // 匹配: toast('消息', 'success')
        std::regex toastRegex(R"(toast\s*\(\s*['"](.*?)['"](?:,\s*['"](.*?)['"])?\s*\))");
        std::smatch matches;
        
        if (std::regex_search(code, matches, toastRegex)) {
            match.matched = true;
            match.syntaxName = "toast";
            match.parameters.push_back(matches[1].str()); // 消息
            if (matches.size() > 2 && !matches[2].str().empty()) {
                match.parameters.push_back(matches[2].str()); // 类型
            } else {
                match.parameters.push_back("info"); // 默认类型
            }
            return true;
        }
        
        return false;
    }
    
    bool parseSlideShowSyntax(const std::string& code, SyntaxMatch& match) const {
        // 匹配: slideShow({{#container}}, ['img1.jpg', 'img2.jpg'])
        std::regex slideRegex(R"(slideShow\s*\(\s*\{\{([^}]+)\}\},\s*\[([^\]]+)\]\s*\))");
        std::smatch matches;
        
        if (std::regex_search(code, matches, slideRegex)) {
            match.matched = true;
            match.syntaxName = "slideShow";
            match.parameters.push_back(matches[1].str()); // 选择器
            match.parameters.push_back(matches[2].str()); // 图片数组
            return true;
        }
        
        return false;
    }
    
    bool parseCountdownSyntax(const std::string& code, SyntaxMatch& match) const {
        // 匹配: countdown({{#timer}}, 60, callback)
        std::regex countdownRegex(R"(countdown\s*\(\s*\{\{([^}]+)\}\},\s*(\d+),\s*(.*?)\s*\))");
        std::smatch matches;
        
        if (std::regex_search(code, matches, countdownRegex)) {
            match.matched = true;
            match.syntaxName = "countdown";
            match.parameters.push_back(matches[1].str()); // 选择器
            match.parameters.push_back(matches[2].str()); // 秒数
            match.parameters.push_back(matches[3].str()); // 回调函数
            return true;
        }
        
        return false;
    }
    
    bool parseParallaxSyntax(const std::string& code, SyntaxMatch& match) const {
        // 匹配: parallax({{#element}}, 0.5)
        std::regex parallaxRegex(R"(parallax\s*\(\s*\{\{([^}]+)\}\},\s*([\d.]+)\s*\))");
        std::smatch matches;
        
        if (std::regex_search(code, matches, parallaxRegex)) {
            match.matched = true;
            match.syntaxName = "parallax";
            match.parameters.push_back(matches[1].str()); // 选择器
            match.parameters.push_back(matches[2].str()); // 速度
            return true;
        }
        
        return false;
    }
    
    bool parseTypewriterSyntax(const std::string& code, SyntaxMatch& match) const {
        // 匹配: typewriter({{#text}}, '要打字的内容', 100)
        std::regex typeRegex(R"(typewriter\s*\(\s*\{\{([^}]+)\}\},\s*['"](.*?)['"](?:,\s*(\d+))?\s*\))");
        std::smatch matches;
        
        if (std::regex_search(code, matches, typeRegex)) {
            match.matched = true;
            match.syntaxName = "typewriter";
            match.parameters.push_back(matches[1].str()); // 选择器
            match.parameters.push_back(matches[2].str()); // 文本内容
            if (matches.size() > 3 && !matches[3].str().empty()) {
                match.parameters.push_back(matches[3].str()); // 速度
            } else {
                match.parameters.push_back("100"); // 默认速度
            }
            return true;
        }
        
        return false;
    }
    
    // ========================================
    // 私有方法：JavaScript代码生成实现
    // ========================================
    
    std::string generateToastJS(const SyntaxMatch& match, const CJMODContext& context) const {
        std::string message = match.parameters[0];
        std::string type = match.parameters[1];
        
        std::stringstream js;
        js << "(() => {\n";
        js << "    // 创建吐司通知\n";
        js << "    const toast = document.createElement('div');\n";
        js << "    toast.className = 'cjmod-toast cjmod-toast-" << type << "';\n";
        js << "    toast.textContent = '" << CJMODHelper::escapeJavaScriptString(message) << "';\n";
        js << "    \n";
        js << "    // 设置样式\n";
        js << "    toast.style.cssText = `\n";
        js << "        position: fixed;\n";
        js << "        top: 20px;\n";
        js << "        right: 20px;\n";
        js << "        padding: 15px 20px;\n";
        js << "        border-radius: 8px;\n";
        js << "        color: white;\n";
        js << "        font-weight: bold;\n";
        js << "        z-index: 9999;\n";
        js << "        transform: translateX(100%);\n";
        js << "        transition: all 0.3s ease;\n";
        js << "        cursor: pointer;\n";
        
        // 根据类型设置颜色
        if (type == "success") {
            js << "        background: #4caf50;\n";
        } else if (type == "error") {
            js << "        background: #f44336;\n";
        } else if (type == "warning") {
            js << "        background: #ff9800;\n";
        } else {
            js << "        background: #2196f3;\n";
        }
        
        js << "    `;\n";
        js << "    \n";
        js << "    // 添加到页面\n";
        js << "    document.body.appendChild(toast);\n";
        js << "    \n";
        js << "    // 动画进入\n";
        js << "    setTimeout(() => {\n";
        js << "        toast.style.transform = 'translateX(0)';\n";
        js << "    }, 10);\n";
        js << "    \n";
        js << "    // 点击关闭\n";
        js << "    toast.addEventListener('click', () => {\n";
        js << "        toast.style.transform = 'translateX(100%)';\n";
        js << "        setTimeout(() => {\n";
        js << "            if (toast.parentNode) {\n";
        js << "                toast.parentNode.removeChild(toast);\n";
        js << "            }\n";
        js << "        }, 300);\n";
        js << "    });\n";
        js << "    \n";
        js << "    // 自动消失\n";
        js << "    setTimeout(() => {\n";
        js << "        if (toast.parentNode) {\n";
        js << "            toast.style.transform = 'translateX(100%)';\n";
        js << "            setTimeout(() => {\n";
        js << "                if (toast.parentNode) {\n";
        js << "                    toast.parentNode.removeChild(toast);\n";
        js << "                }\n";
        js << "            }, 300);\n";
        js << "        }\n";
        js << "    }, 3000);\n";
        js << "    \n";
        js << "    console.log('🍞 Toast显示:', '" << message << "');\n";
        js << "})()";
        
        return js.str();
    }
    
    std::string generateTypewriterJS(const SyntaxMatch& match, const CJMODContext& context) const {
        std::string selector = match.parameters[0];
        std::string text = match.parameters[1];
        std::string speed = match.parameters[2];
        
        std::string jsSelector = CJMODHelper::parseSelector(selector);
        
        std::stringstream js;
        js << "(() => {\n";
        js << "    const element = " << jsSelector << ";\n";
        js << "    if (!element) {\n";
        js << "        console.error('❌ 打字机效果：元素未找到');\n";
        js << "        return;\n";
        js << "    }\n";
        js << "    \n";
        js << "    const text = '" << CJMODHelper::escapeJavaScriptString(text) << "';\n";
        js << "    const speed = " << speed << ";\n";
        js << "    let index = 0;\n";
        js << "    \n";
        js << "    element.textContent = '';\n";
        js << "    \n";
        js << "    const typeInterval = setInterval(() => {\n";
        js << "        if (index < text.length) {\n";
        js << "            element.textContent += text[index];\n";
        js << "            index++;\n";
        js << "        } else {\n";
        js << "            clearInterval(typeInterval);\n";
        js << "            console.log('⌨️ 打字机效果完成');\n";
        js << "        }\n";
        js << "    }, speed);\n";
        js << "    \n";
        js << "    console.log('⌨️ 打字机效果启动:', text);\n";
        js << "})()";
        
        return js.str();
    }
    
    std::string generateCountdownJS(const SyntaxMatch& match, const CJMODContext& context) const {
        std::string selector = match.parameters[0];
        std::string seconds = match.parameters[1];
        std::string callback = match.parameters[2];
        
        std::string jsSelector = CJMODHelper::parseSelector(selector);
        
        std::stringstream js;
        js << "(() => {\n";
        js << "    const element = " << jsSelector << ";\n";
        js << "    if (!element) {\n";
        js << "        console.error('❌ 倒计时：元素未找到');\n";
        js << "        return;\n";
        js << "    }\n";
        js << "    \n";
        js << "    let timeLeft = " << seconds << ";\n";
        js << "    \n";
        js << "    const updateDisplay = () => {\n";
        js << "        const minutes = Math.floor(timeLeft / 60);\n";
        js << "        const secs = timeLeft % 60;\n";
        js << "        element.textContent = `${minutes.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;\n";
        js << "    };\n";
        js << "    \n";
        js << "    updateDisplay();\n";
        js << "    \n";
        js << "    const countdownInterval = setInterval(() => {\n";
        js << "        timeLeft--;\n";
        js << "        updateDisplay();\n";
        js << "        \n";
        js << "        if (timeLeft <= 0) {\n";
        js << "            clearInterval(countdownInterval);\n";
        js << "            element.textContent = '时间到！';\n";
        js << "            console.log('⏰ 倒计时结束');\n";
        js << "            \n";
        js << "            // 执行回调\n";
        js << "            try {\n";
        js << "                (" << callback << ")();\n";
        js << "            } catch (e) {\n";
        js << "                console.error('❌ 倒计时回调错误:', e);\n";
        js << "            }\n";
        js << "        }\n";
        js << "    }, 1000);\n";
        js << "    \n";
        js << "    console.log('⏰ 倒计时启动:', " << seconds << "秒);\n";
        js << "})()";
        
        return js.str();
    }
    
    std::string generateSlideShowJS(const SyntaxMatch& match, const CJMODContext& context) const {
        std::string selector = match.parameters[0];
        std::string images = match.parameters[1];
        
        std::string jsSelector = CJMODHelper::parseSelector(selector);
        
        std::stringstream js;
        js << "(() => {\n";
        js << "    const container = " << jsSelector << ";\n";
        js << "    if (!container) {\n";
        js << "        console.error('❌ 轮播：容器未找到');\n";
        js << "        return;\n";
        js << "    }\n";
        js << "    \n";
        js << "    const images = [" << images << "];\n";
        js << "    let currentIndex = 0;\n";
        js << "    \n";
        js << "    // 创建轮播结构\n";
        js << "    container.style.cssText = 'position: relative; overflow: hidden; border-radius: 10px;';\n";
        js << "    \n";
        js << "    const slideContainer = document.createElement('div');\n";
        js << "    slideContainer.style.cssText = 'display: flex; transition: transform 0.5s ease;';\n";
        js << "    \n";
        js << "    images.forEach(imgSrc => {\n";
        js << "        const img = document.createElement('img');\n";
        js << "        img.src = imgSrc;\n";
        js << "        img.style.cssText = 'width: 100%; height: 300px; object-fit: cover; flex-shrink: 0;';\n";
        js << "        slideContainer.appendChild(img);\n";
        js << "    });\n";
        js << "    \n";
        js << "    container.appendChild(slideContainer);\n";
        js << "    \n";
        js << "    // 创建导航按钮\n";
        js << "    const prevBtn = document.createElement('button');\n";
        js << "    prevBtn.textContent = '❮';\n";
        js << "    prevBtn.style.cssText = 'position: absolute; left: 10px; top: 50%; transform: translateY(-50%); background: rgba(0,0,0,0.5); color: white; border: none; padding: 10px; cursor: pointer; border-radius: 5px;';\n";
        js << "    \n";
        js << "    const nextBtn = document.createElement('button');\n";
        js << "    nextBtn.textContent = '❯';\n";
        js << "    nextBtn.style.cssText = 'position: absolute; right: 10px; top: 50%; transform: translateY(-50%); background: rgba(0,0,0,0.5); color: white; border: none; padding: 10px; cursor: pointer; border-radius: 5px;';\n";
        js << "    \n";
        js << "    container.appendChild(prevBtn);\n";
        js << "    container.appendChild(nextBtn);\n";
        js << "    \n";
        js << "    // 切换函数\n";
        js << "    const showSlide = (index) => {\n";
        js << "        currentIndex = (index + images.length) % images.length;\n";
        js << "        slideContainer.style.transform = `translateX(-${currentIndex * 100}%)`;\n";
        js << "    };\n";
        js << "    \n";
        js << "    // 按钮事件\n";
        js << "    prevBtn.addEventListener('click', () => showSlide(currentIndex - 1));\n";
        js << "    nextBtn.addEventListener('click', () => showSlide(currentIndex + 1));\n";
        js << "    \n";
        js << "    // 自动播放\n";
        js << "    setInterval(() => showSlide(currentIndex + 1), 3000);\n";
        js << "    \n";
        js << "    console.log('🖼️ 轮播图已创建，图片数量:', images.length);\n";
        js << "})()";
        
        return js.str();
    }
    
    std::string generateParallaxJS(const SyntaxMatch& match, const CJMODContext& context) const {
        std::string selector = match.parameters[0];
        std::string speed = match.parameters[1];
        
        std::string jsSelector = CJMODHelper::parseSelector(selector);
        
        std::stringstream js;
        js << "(() => {\n";
        js << "    const element = " << jsSelector << ";\n";
        js << "    if (!element) {\n";
        js << "        console.error('❌ 视差效果：元素未找到');\n";
        js << "        return;\n";
        js << "    }\n";
        js << "    \n";
        js << "    const speed = " << speed << ";\n";
        js << "    \n";
        js << "    window.addEventListener('scroll', () => {\n";
        js << "        const scrolled = window.pageYOffset;\n";
        js << "        const parallax = scrolled * speed;\n";
        js << "        element.style.transform = `translateY(${parallax}px)`;\n";
        js << "    });\n";
        js << "    \n";
        js << "    console.log('🌊 视差效果已启用，速度:', speed);\n";
        js << "})()";
        
        return js.str();
    }
};

// ========================================
// CJMOD注册 - 让CHTL编译器能找到你的扩展
// ========================================

// 使用注册宏
CJMOD_REGISTER_EXTENSION(MyCustomExtension)

// 或者手动实现（如果不想用宏）：
/*
extern "C" ICJMODExtension* createCJMODExtension() {
    return new MyCustomExtension();
}

extern "C" void destroyCJMODExtension(ICJMODExtension* extension) {
    delete extension;
}

extern "C" const char* getCJMODExtensionName() {
    return "MyCustomExtension";
}
*/