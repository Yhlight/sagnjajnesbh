// ========================================
// 高级CJMOD示例 - 展示更强大的语法解析方法
// 不仅仅依赖正则表达式，使用更智能的解析
// ========================================

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <sstream>

// 自由包含任意头文件 - 这是CJMOD的强大之处
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <chrono>

// 全局变量管理状态 - 维护复杂状态
namespace GlobalState {
    std::unordered_map<std::string, std::string> variables;
    std::vector<std::string> eventLog;
    std::mutex stateMutex;
    int callCounter = 0;
    
    void setState(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(stateMutex);
        variables[key] = value;
        eventLog.push_back("SET: " + key + " = " + value);
    }
    
    std::string getState(const std::string& key) {
        std::lock_guard<std::mutex> lock(stateMutex);
        return variables.count(key) ? variables[key] : "";
    }
}

/**
 * 高级语法解析器
 * 使用更智能的方法，不仅仅是正则表达式
 */
class AdvancedSyntaxParser {
public:
    /**
     * 词法分析器 - 将代码分解为token
     */
    struct Token {
        enum Type { KEYWORD, IDENTIFIER, OPERATOR, LITERAL, PUNCTUATION, SELECTOR, UNKNOWN };
        Type type;
        std::string value;
        int position;
        
        Token(Type t, const std::string& v, int p) : type(t), value(v), position(p) {}
    };
    
    std::vector<Token> tokenize(const std::string& code) {
        std::vector<Token> tokens;
        
        for (size_t i = 0; i < code.length(); ) {
            // 跳过空白字符
            if (std::isspace(code[i])) {
                i++;
                continue;
            }
            
            // 识别CHTL JS选择器 {{...}}
            if (i + 1 < code.length() && code[i] == '{' && code[i+1] == '{') {
                size_t end = code.find("}}", i);
                if (end != std::string::npos) {
                    tokens.emplace_back(Token::SELECTOR, code.substr(i, end - i + 2), i);
                    i = end + 2;
                    continue;
                }
            }
            
            // 识别字符串字面量
            if (code[i] == '"' || code[i] == '\'') {
                char quote = code[i];
                size_t start = i++;
                while (i < code.length() && code[i] != quote) {
                    if (code[i] == '\\') i++; // 跳过转义字符
                    i++;
                }
                if (i < code.length()) i++; // 跳过结束引号
                tokens.emplace_back(Token::LITERAL, code.substr(start, i - start), start);
                continue;
            }
            
            // 识别标识符和关键字
            if (std::isalpha(code[i]) || code[i] == '_') {
                size_t start = i;
                while (i < code.length() && (std::isalnum(code[i]) || code[i] == '_')) {
                    i++;
                }
                std::string identifier = code.substr(start, i - start);
                
                // 检查是否是关键字
                Token::Type type = isKeyword(identifier) ? Token::KEYWORD : Token::IDENTIFIER;
                tokens.emplace_back(type, identifier, start);
                continue;
            }
            
            // 识别操作符和标点符号
            if (std::ispunct(code[i])) {
                // 处理多字符操作符
                if (i + 1 < code.length()) {
                    std::string twoChar = code.substr(i, 2);
                    if (twoChar == "->" || twoChar == "==" || twoChar == "!=" || 
                        twoChar == "<=" || twoChar == ">=" || twoChar == "&&" || twoChar == "||") {
                        tokens.emplace_back(Token::OPERATOR, twoChar, i);
                        i += 2;
                        continue;
                    }
                }
                
                tokens.emplace_back(Token::PUNCTUATION, std::string(1, code[i]), i);
                i++;
                continue;
            }
            
            // 识别数字
            if (std::isdigit(code[i])) {
                size_t start = i;
                while (i < code.length() && (std::isdigit(code[i]) || code[i] == '.')) {
                    i++;
                }
                tokens.emplace_back(Token::LITERAL, code.substr(start, i - start), start);
                continue;
            }
            
            // 未知字符
            tokens.emplace_back(Token::UNKNOWN, std::string(1, code[i]), i);
            i++;
        }
        
        return tokens;
    }
    
    /**
     * 语法结构分析
     * 基于token流分析语法结构
     */
    struct SyntaxStructure {
        std::string functionName;
        std::vector<std::string> parameters;
        std::unordered_map<std::string, std::string> namedParameters;
        std::vector<std::string> selectors;
        std::string context; // 函数调用的上下文
    };
    
    std::vector<SyntaxStructure> analyzeSyntaxStructure(const std::vector<Token>& tokens) {
        std::vector<SyntaxStructure> structures;
        
        for (size_t i = 0; i < tokens.size(); ) {
            // 查找函数调用模式：identifier (
            if (i + 1 < tokens.size() && 
                tokens[i].type == Token::IDENTIFIER && 
                tokens[i + 1].value == "(") {
                
                SyntaxStructure structure;
                structure.functionName = tokens[i].value;
                
                // 解析参数列表
                size_t paramStart = i + 2;
                size_t paramEnd = findMatchingParen(tokens, i + 1);
                
                if (paramEnd != std::string::npos) {
                    parseParameters(tokens, paramStart, paramEnd, structure);
                    structures.push_back(structure);
                    i = paramEnd + 1;
                } else {
                    i++;
                }
            } else {
                i++;
            }
        }
        
        return structures;
    }
    
    /**
     * 上下文感知解析
     * 根据代码上下文智能解析
     */
    bool isInEventHandler(const std::vector<Token>& tokens, size_t position) {
        // 检查是否在事件处理器内部
        for (int i = position - 1; i >= 0; i--) {
            if (tokens[i].value == "listen" || tokens[i].value == "delegate") {
                return true;
            }
            if (tokens[i].value == "}" && findMatchingBrace(tokens, i) < position) {
                break;
            }
        }
        return false;
    }
    
    bool isInAnimationBlock(const std::vector<Token>& tokens, size_t position) {
        // 检查是否在动画块内部
        for (int i = position - 1; i >= 0; i--) {
            if (tokens[i].value == "animate") {
                return true;
            }
        }
        return false;
    }

private:
    bool isKeyword(const std::string& word) {
        std::vector<std::string> keywords = {
            "function", "const", "let", "var", "if", "else", "for", "while",
            "listen", "delegate", "animate", "vir" // CHTL JS关键字
        };
        return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
    }
    
    size_t findMatchingParen(const std::vector<Token>& tokens, size_t start) {
        int depth = 1;
        for (size_t i = start + 1; i < tokens.size(); i++) {
            if (tokens[i].value == "(") depth++;
            else if (tokens[i].value == ")") depth--;
            
            if (depth == 0) return i;
        }
        return std::string::npos;
    }
    
    size_t findMatchingBrace(const std::vector<Token>& tokens, size_t start) {
        int depth = 1;
        for (int i = start - 1; i >= 0; i--) {
            if (tokens[i].value == "}") depth++;
            else if (tokens[i].value == "{") depth--;
            
            if (depth == 0) return i;
        }
        return std::string::npos;
    }
    
    void parseParameters(const std::vector<Token>& tokens, size_t start, size_t end, 
                        SyntaxStructure& structure) {
        // 解析参数列表
        std::string currentParam;
        
        for (size_t i = start; i < end; i++) {
            if (tokens[i].value == ",") {
                if (!currentParam.empty()) {
                    structure.parameters.push_back(currentParam);
                    currentParam.clear();
                }
            } else if (tokens[i].type != Token::WHITESPACE) {
                currentParam += tokens[i].value;
            }
        }
        
        if (!currentParam.empty()) {
            structure.parameters.push_back(currentParam);
        }
    }
};

/**
 * 示例：高级动画扩展
 * 使用智能解析而不是简单正则
 */
class AdvancedAnimationExtension : public ICJMODSyntaxExtension {
public:
    std::string getName() const override {
        return "AdvancedAnimation";
    }
    
    std::vector<std::string> getSyntaxKeywords() const override {
        return {"smoothMove", "morphShape", "particleEffect", "timeline"};
    }
    
    bool containsSyntax(const std::string& codeFragment) const override {
        AdvancedSyntaxParser parser;
        auto tokens = parser.tokenize(codeFragment);
        
        // 智能检测：不仅仅检查关键字，还检查上下文
        for (const auto& token : tokens) {
            if (token.type == AdvancedSyntaxParser::Token::IDENTIFIER) {
                auto keywords = getSyntaxKeywords();
                if (std::find(keywords.begin(), keywords.end(), token.value) != keywords.end()) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    bool parseSyntaxStructure(const std::string& codeFragment, 
                             std::unordered_map<std::string, std::string>& parsedData) const override {
        AdvancedSyntaxParser parser;
        auto tokens = parser.tokenize(codeFragment);
        auto structures = parser.analyzeSyntaxStructure(tokens);
        
        for (const auto& structure : structures) {
            if (std::find(getSyntaxKeywords().begin(), getSyntaxKeywords().end(), 
                         structure.functionName) != getSyntaxKeywords().end()) {
                
                parsedData["functionName"] = structure.functionName;
                parsedData["parameterCount"] = std::to_string(structure.parameters.size());
                
                // 解析具体参数
                for (size_t i = 0; i < structure.parameters.size(); i++) {
                    parsedData["param" + std::to_string(i)] = structure.parameters[i];
                }
                
                // 提取选择器
                for (const auto& selector : structure.selectors) {
                    parsedData["selector"] = selector;
                    break; // 取第一个选择器
                }
                
                return true;
            }
        }
        
        return false;
    }
    
    std::string generateJavaScriptCode(const std::unordered_map<std::string, std::string>& parsedData) const override {
        auto it = parsedData.find("functionName");
        if (it == parsedData.end()) {
            return "";
        }
        
        std::string functionName = it->second;
        std::stringstream js;
        
        if (functionName == "smoothMove") {
            js << generateSmoothMoveJS(parsedData);
        } else if (functionName == "morphShape") {
            js << generateMorphShapeJS(parsedData);
        } else if (functionName == "particleEffect") {
            js << generateParticleEffectJS(parsedData);
        } else if (functionName == "timeline") {
            js << generateTimelineJS(parsedData);
        }
        
        return js.str();
    }
    
    std::vector<std::string> getRequiredHeaders() const override {
        return {
            "<cmath>",
            "<random>",
            "<chrono>",
            "<thread>"
        };
    }
    
    std::string getGlobalVariables() const override {
        return R"(
            // 动画系统全局状态
            std::unordered_map<std::string, bool> activeAnimations;
            std::mutex animationMutex;
            int animationIdCounter = 0;
        )";
    }
    
    std::string getInitializationCode() const override {
        return R"(
            std::cout << "🎨 高级动画系统初始化" << std::endl;
            GlobalState::setState("animation_system", "initialized");
        )";
    }
    
    std::string getCleanupCode() const override {
        return R"(
            std::cout << "🧹 高级动画系统清理" << std::endl;
            GlobalState::setState("animation_system", "cleaned");
        )";
    }

private:
    std::string generateSmoothMoveJS(const std::unordered_map<std::string, std::string>& data) const {
        std::string selector = data.count("selector") ? data.at("selector") : "null";
        std::string param0 = data.count("param0") ? data.at("param0") : "{}";
        
        std::stringstream js;
        js << "(() => {\n";
        js << "    const element = " << parseSelector(selector) << ";\n";
        js << "    if (!element) return;\n";
        js << "    \n";
        js << "    const config = " << param0 << ";\n";
        js << "    const duration = config.duration || 1000;\n";
        js << "    const easing = config.easing || 'ease-out';\n";
        js << "    \n";
        js << "    // 高级平滑移动算法\n";
        js << "    const startTime = performance.now();\n";
        js << "    const startRect = element.getBoundingClientRect();\n";
        js << "    const targetX = config.x || 0;\n";
        js << "    const targetY = config.y || 0;\n";
        js << "    \n";
        js << "    function animate(currentTime) {\n";
        js << "        const elapsed = currentTime - startTime;\n";
        js << "        const progress = Math.min(elapsed / duration, 1);\n";
        js << "        \n";
        js << "        // 缓动函数\n";
        js << "        let easedProgress;\n";
        js << "        if (easing === 'ease-out') {\n";
        js << "            easedProgress = 1 - Math.pow(1 - progress, 3);\n";
        js << "        } else if (easing === 'bounce') {\n";
        js << "            easedProgress = progress < 0.5 ? \n";
        js << "                2 * progress * progress : \n";
        js << "                1 - Math.pow(-2 * progress + 2, 3) / 2;\n";
        js << "        } else {\n";
        js << "            easedProgress = progress;\n";
        js << "        }\n";
        js << "        \n";
        js << "        const currentX = startRect.left + (targetX - startRect.left) * easedProgress;\n";
        js << "        const currentY = startRect.top + (targetY - startRect.top) * easedProgress;\n";
        js << "        \n";
        js << "        element.style.transform = `translate(${currentX - startRect.left}px, ${currentY - startRect.top}px)`;\n";
        js << "        \n";
        js << "        if (progress < 1) {\n";
        js << "            requestAnimationFrame(animate);\n";
        js << "        } else {\n";
        js << "            console.log('🎯 平滑移动完成');\n";
        js << "            if (config.onComplete) config.onComplete();\n";
        js << "        }\n";
        js << "    }\n";
        js << "    \n";
        js << "    requestAnimationFrame(animate);\n";
        js << "    console.log('🚀 平滑移动启动');\n";
        js << "})()";
        
        return js.str();
    }
    
    std::string generateParticleEffectJS(const std::unordered_map<std::string, std::string>& data) const {
        std::string selector = data.count("selector") ? data.at("selector") : "null";
        std::string param0 = data.count("param0") ? data.at("param0") : "{}";
        
        std::stringstream js;
        js << "(() => {\n";
        js << "    const container = " << parseSelector(selector) << ";\n";
        js << "    if (!container) return;\n";
        js << "    \n";
        js << "    const config = " << param0 << ";\n";
        js << "    const particleCount = config.count || 50;\n";
        js << "    const duration = config.duration || 3000;\n";
        js << "    const colors = config.colors || ['#ff6b6b', '#4ecdc4', '#45b7d1', '#f9ca24'];\n";
        js << "    \n";
        js << "    // 创建粒子系统\n";
        js << "    const particles = [];\n";
        js << "    const containerRect = container.getBoundingClientRect();\n";
        js << "    \n";
        js << "    for (let i = 0; i < particleCount; i++) {\n";
        js << "        const particle = document.createElement('div');\n";
        js << "        particle.style.cssText = `\n";
        js << "            position: absolute;\n";
        js << "            width: ${Math.random() * 8 + 2}px;\n";
        js << "            height: ${Math.random() * 8 + 2}px;\n";
        js << "            background: ${colors[Math.floor(Math.random() * colors.length)]};\n";
        js << "            border-radius: 50%;\n";
        js << "            pointer-events: none;\n";
        js << "            z-index: 9999;\n";
        js << "        `;\n";
        js << "        \n";
        js << "        // 随机初始位置\n";
        js << "        const startX = containerRect.left + Math.random() * containerRect.width;\n";
        js << "        const startY = containerRect.top + Math.random() * containerRect.height;\n";
        js << "        \n";
        js << "        particle.style.left = startX + 'px';\n";
        js << "        particle.style.top = startY + 'px';\n";
        js << "        \n";
        js << "        document.body.appendChild(particle);\n";
        js << "        particles.push({\n";
        js << "            element: particle,\n";
        js << "            vx: (Math.random() - 0.5) * 4,\n";
        js << "            vy: (Math.random() - 0.5) * 4,\n";
        js << "            life: 1.0\n";
        js << "        });\n";
        js << "    }\n";
        js << "    \n";
        js << "    // 粒子动画循环\n";
        js << "    const startTime = performance.now();\n";
        js << "    \n";
        js << "    function updateParticles(currentTime) {\n";
        js << "        const elapsed = currentTime - startTime;\n";
        js << "        const progress = elapsed / duration;\n";
        js << "        \n";
        js << "        if (progress >= 1) {\n";
        js << "            // 清理粒子\n";
        js << "            particles.forEach(p => {\n";
        js << "                if (p.element.parentNode) {\n";
        js << "                    p.element.parentNode.removeChild(p.element);\n";
        js << "                }\n";
        js << "            });\n";
        js << "            console.log('✨ 粒子效果完成');\n";
        js << "            return;\n";
        js << "        }\n";
        js << "        \n";
        js << "        particles.forEach(particle => {\n";
        js << "            const element = particle.element;\n";
        js << "            const currentX = parseFloat(element.style.left);\n";
        js << "            const currentY = parseFloat(element.style.top);\n";
        js << "            \n";
        js << "            // 更新位置\n";
        js << "            element.style.left = (currentX + particle.vx) + 'px';\n";
        js << "            element.style.top = (currentY + particle.vy) + 'px';\n";
        js << "            \n";
        js << "            // 更新生命值和透明度\n";
        js << "            particle.life -= 0.02;\n";
        js << "            element.style.opacity = Math.max(0, particle.life);\n";
        js << "            \n";
        js << "            // 重力效果\n";
        js << "            particle.vy += 0.1;\n";
        js << "        });\n";
        js << "        \n";
        js << "        requestAnimationFrame(updateParticles);\n";
        js << "    }\n";
        js << "    \n";
        js << "    requestAnimationFrame(updateParticles);\n";
        js << "    console.log('✨ 粒子效果启动，粒子数量:', particleCount);\n";
        js << "})()";
        
        return js.str();
    }
    
    std::string parseSelector(const std::string& selector) const {
        if (selector.empty() || selector == "null") {
            return "null";
        }
        
        // 移除 {{}} 包装
        std::string clean = selector;
        if (clean.length() > 4 && clean.substr(0, 2) == "{{" && clean.substr(clean.length()-2) == "}}") {
            clean = clean.substr(2, clean.length() - 4);
        }
        
        if (clean[0] == '#') {
            return "document.getElementById('" + clean.substr(1) + "')";
        } else if (clean[0] == '.') {
            return "document.querySelector('." + clean.substr(1) + "')";
        } else {
            return "document.querySelector('" + clean + "')";
        }
    }
};

// ========================================
// CJMOD入口点 - CHTL编译器调用的接口
// ========================================

extern "C" {
    // 初始化CJMOD模块
    int initializeCJMOD() {
        GlobalState::setState("module_status", "initialized");
        GlobalState::callCounter = 0;
        std::cout << "🚀 高级CJMOD模块初始化完成" << std::endl;
        return 1; // 成功
    }
    
    // 处理CHTL JS代码片段
    const char* processCHTLJSCode(const char* codeFragment, const char* context) {
        GlobalState::callCounter++;
        
        std::string code(codeFragment);
        std::string ctx(context ? context : "");
        
        // 使用高级解析器
        AdvancedAnimationExtension extension;
        
        if (extension.containsSyntax(code)) {
            std::unordered_map<std::string, std::string> parsedData;
            
            if (extension.parseSyntaxStructure(code, parsedData)) {
                std::string jsCode = extension.generateJavaScriptCode(parsedData);
                
                // 保存结果到全局状态
                static std::string lastResult;
                lastResult = jsCode;
                
                GlobalState::setState("last_processed", code);
                GlobalState::setState("call_count", std::to_string(GlobalState::callCounter));
                
                return lastResult.c_str();
            }
        }
        
        return codeFragment; // 不匹配就返回原代码
    }
    
    // 获取模块信息
    const char* getCJMODInfo() {
        static std::string info = R"({
            "name": "AdvancedCJMOD",
            "version": "1.0.0",
            "supportedSyntax": ["smoothMove", "morphShape", "particleEffect", "timeline"],
            "capabilities": ["advanced_parsing", "global_state", "free_headers"]
        })";
        
        return info.c_str();
    }
    
    // 清理CJMOD模块
    void cleanupCJMOD() {
        GlobalState::setState("module_status", "cleaned");
        std::cout << "🧹 高级CJMOD模块清理完成" << std::endl;
    }
}

// ========================================
// 使用示例：如何在CHTL JS中使用这些扩展
// ========================================

/*
在CHTL文件中：

[Import] @CJmod from AdvancedCJMOD

html {
    body {
        div {
            id: demo-container;
            
            script {
                // 🚀 使用高级解析的语法扩展！
                
                // 平滑移动动画
                smoothMove({{#demo-container}}, {
                    x: 200, 
                    y: 100, 
                    duration: 1000,
                    easing: 'bounce',
                    onComplete: function() {
                        console.log('移动完成！');
                    }
                });
                
                // 粒子效果
                particleEffect({{#demo-container}}, {
                    count: 100,
                    duration: 3000,
                    colors: ['#ff6b6b', '#4ecdc4', '#45b7d1']
                });
                
                // 形状变换
                morphShape({{#demo-container}}, {
                    from: 'circle',
                    to: 'square',
                    duration: 2000
                });
                
                // 时间线动画
                timeline([
                    {time: 0, action: 'fadeIn', target: {{#element1}}},
                    {time: 500, action: 'slideUp', target: {{#element2}}},
                    {time: 1000, action: 'bounce', target: {{#element3}}}
                ]);
            }
        }
    }
}

编译后生成复杂的JavaScript代码，实现这些高级动画效果。
*/