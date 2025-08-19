// ========================================
// 完整的创新CJMOD示例
// 完美实现您的syntaxAnalys + scanner设计
// ========================================

#include "InnovativeSyntaxSystem.h"
#include <iostream>
#include <string>

using namespace chtl::innovative_cjmod;

/**
 * 示例：智能图表语法
 * CHTL JS: smartChart({{#container}}, **, 'line')
 * 展示您的完整设计威力
 */
void demonstrateInnovativeDesign() {
    std::cout << "🚀 演示您的创新CJMOD设计" << std::endl;
    std::cout << "=========================" << std::endl;
    
    // 🎯 第一步：使用您的syntaxAnalys
    const auto keyword = InnovativeSyntaxAnalyzer::syntaxAnalys("selector ** string");
    
    std::cout << "✅ 语法模式解析完成: 'selector ** string'" << std::endl;
    
    // 🔧 第二步：设置参数处理器 - 您的设计！
    keyword.arg[0] = [](const KeywordValue& val) -> std::string {
        std::cout << "📍 处理选择器参数: " << val.value << std::endl;
        
        // 智能选择器处理
        std::string selector = val.value;
        if (selector.find("{{") != std::string::npos) {
            selector = selector.substr(2, selector.length() - 4);
            
            if (selector[0] == '#') {
                return "document.getElementById('" + selector.substr(1) + "')";
            } else if (selector[0] == '.') {
                return "document.querySelector('." + selector.substr(1) + "')";
            }
        }
        
        return "document.querySelector('" + selector + "')";
    };
    
    keyword.arg[1] = []() -> std::string {
        std::cout << "⭐ 处理通配符参数: **" << std::endl;
        
        // ** 通配符 - 返回复杂的图表配置
        return R"({
            // 智能图表配置
            chartConfig: {
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        position: 'top',
                        labels: {
                            usePointStyle: true,
                            padding: 20
                        }
                    },
                    tooltip: {
                        mode: 'index',
                        intersect: false,
                        backgroundColor: 'rgba(0,0,0,0.8)',
                        titleColor: '#fff',
                        bodyColor: '#fff',
                        borderColor: '#667eea',
                        borderWidth: 1
                    }
                },
                scales: {
                    x: {
                        display: true,
                        title: {
                            display: true,
                            text: 'X轴'
                        },
                        grid: {
                            color: 'rgba(0,0,0,0.1)'
                        }
                    },
                    y: {
                        display: true,
                        title: {
                            display: true,
                            text: 'Y轴'
                        },
                        grid: {
                            color: 'rgba(0,0,0,0.1)'
                        }
                    }
                },
                animation: {
                    duration: 2000,
                    easing: 'easeInOutQuart'
                }
            },
            
            // 数据生成器
            generateData: function(type) {
                const labels = ['一月', '二月', '三月', '四月', '五月', '六月'];
                const colors = ['#ff6b6b', '#4ecdc4', '#45b7d1', '#f9ca24', '#a55eea', '#fd79a8'];
                
                if (type === 'line') {
                    return {
                        labels: labels,
                        datasets: [{
                            label: '销售数据',
                            data: [65, 59, 80, 81, 56, 55],
                            borderColor: '#667eea',
                            backgroundColor: 'rgba(102, 126, 234, 0.1)',
                            borderWidth: 3,
                            fill: true,
                            tension: 0.4
                        }]
                    };
                } else if (type === 'bar') {
                    return {
                        labels: labels,
                        datasets: [{
                            label: '用户增长',
                            data: [12, 19, 3, 5, 2, 3],
                            backgroundColor: colors.map(c => c + '80'),
                            borderColor: colors,
                            borderWidth: 2
                        }]
                    };
                } else if (type === 'pie') {
                    return {
                        labels: ['Chrome', 'Firefox', 'Safari', 'Edge', 'Others'],
                        datasets: [{
                            data: [45, 25, 15, 10, 5],
                            backgroundColor: colors,
                            borderColor: '#fff',
                            borderWidth: 2
                        }]
                    };
                }
                
                return { labels: [], datasets: [] };
            },
            
            // 图表创建器
            createChart: function(container, type) {
                // 创建canvas元素
                const canvas = document.createElement('canvas');
                canvas.width = 800;
                canvas.height = 400;
                canvas.style.cssText = 'max-width: 100%; height: auto;';
                
                container.innerHTML = '';
                container.appendChild(canvas);
                
                // 模拟Chart.js（实际应该加载真实的Chart.js库）
                const ctx = canvas.getContext('2d');
                const data = this.generateData(type);
                
                // 简化的图表绘制
                this.drawChart(ctx, data, type);
                
                return canvas;
            },
            
            // 简化的图表绘制
            drawChart: function(ctx, data, type) {
                ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height);
                
                // 绘制标题
                ctx.fillStyle = '#333';
                ctx.font = 'bold 24px Arial';
                ctx.textAlign = 'center';
                ctx.fillText('智能图表 - ' + type.toUpperCase(), ctx.canvas.width / 2, 40);
                
                // 绘制数据（简化版本）
                if (type === 'line' && data.datasets[0]) {
                    this.drawLineChart(ctx, data.datasets[0].data);
                } else if (type === 'bar' && data.datasets[0]) {
                    this.drawBarChart(ctx, data.datasets[0].data);
                } else if (type === 'pie' && data.datasets[0]) {
                    this.drawPieChart(ctx, data.datasets[0].data);
                }
            },
            
            drawLineChart: function(ctx, data) {
                const padding = 60;
                const width = ctx.canvas.width - padding * 2;
                const height = ctx.canvas.height - padding * 2;
                const maxValue = Math.max(...data);
                
                ctx.strokeStyle = '#667eea';
                ctx.lineWidth = 3;
                ctx.beginPath();
                
                for (let i = 0; i < data.length; i++) {
                    const x = padding + (width / (data.length - 1)) * i;
                    const y = padding + height - (data[i] / maxValue) * height;
                    
                    if (i === 0) {
                        ctx.moveTo(x, y);
                    } else {
                        ctx.lineTo(x, y);
                    }
                }
                
                ctx.stroke();
            },
            
            drawBarChart: function(ctx, data) {
                const padding = 60;
                const width = ctx.canvas.width - padding * 2;
                const height = ctx.canvas.height - padding * 2;
                const maxValue = Math.max(...data);
                const barWidth = width / data.length * 0.8;
                
                for (let i = 0; i < data.length; i++) {
                    const x = padding + (width / data.length) * i + (width / data.length - barWidth) / 2;
                    const barHeight = (data[i] / maxValue) * height;
                    const y = padding + height - barHeight;
                    
                    ctx.fillStyle = '#' + Math.floor(Math.random()*16777215).toString(16);
                    ctx.fillRect(x, y, barWidth, barHeight);
                }
            },
            
            drawPieChart: function(ctx, data) {
                const centerX = ctx.canvas.width / 2;
                const centerY = ctx.canvas.height / 2;
                const radius = Math.min(centerX, centerY) - 60;
                const total = data.reduce((sum, val) => sum + val, 0);
                
                let currentAngle = 0;
                
                for (let i = 0; i < data.length; i++) {
                    const sliceAngle = (data[i] / total) * 2 * Math.PI;
                    
                    ctx.fillStyle = '#' + Math.floor(Math.random()*16777215).toString(16);
                    ctx.beginPath();
                    ctx.moveTo(centerX, centerY);
                    ctx.arc(centerX, centerY, radius, currentAngle, currentAngle + sliceAngle);
                    ctx.closePath();
                    ctx.fill();
                    
                    currentAngle += sliceAngle;
                }
            }
        })";
    };
    
    keyword.arg[2] = [](const std::string& chartType) -> std::string {
        std::cout << "📊 处理图表类型参数: " << chartType << std::endl;
        
        // 验证图表类型
        std::vector<std::string> validTypes = {"line", "bar", "pie", "doughnut", "radar"};
        
        if (std::find(validTypes.begin(), validTypes.end(), chartType) != validTypes.end()) {
            return "'" + chartType + "'";
        } else {
            return "'line'"; // 默认类型
        }
    };
    
    // 🔍 第三步：设置扫描器接口 - 您的创新扩展！
    CJMODScanner scanner;
    
    scanner.scanKeyword(keyword.arg[1], [&]() {
        std::cout << "🔍 扫描器回调执行！" << std::endl;
        
        // 获取前一个关键字
        KeywordValue prevKeyword = scanner.peekKeyword(-1);
        std::cout << "👈 前一个关键字: " << prevKeyword.value << " (类型: " << prevKeyword.type << ")" << std::endl;
        
        // 获取后一个关键字
        KeywordValue nextKeyword = scanner.peekKeyword(1);
        std::cout << "👉 后一个关键字: " << nextKeyword.value << " (类型: " << nextKeyword.type << ")" << std::endl;
        
        // 🚀 在扫描器回调中调用其他参数处理器！
        if (keyword.argProcessors.size() > 0) {
            std::string result0 = keyword.argProcessors[0](prevKeyword);
            std::cout << "🔧 arg[0]处理结果: " << result0 << std::endl;
        }
        
        if (keyword.argProcessors.size() > 2) {
            std::string result2 = keyword.argProcessors[2](nextKeyword);
            std::cout << "🔧 arg[2]处理结果: " << result2 << std::endl;
        }
        
        // 根据上下文动态调整处理逻辑
        if (prevKeyword.type == "selector" && nextKeyword.type == "string") {
            std::cout << "🎯 检测到图表语法模式，启用高级处理" << std::endl;
            GlobalState::setState("chart_mode", "advanced");
        }
    });
    
    // 🎯 第四步：生成最终代码
    std::string finalCode = InnovativeCodeGenerator::generateCode(keyword);
    
    std::cout << "\n📝 生成的JavaScript代码：" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << finalCode << std::endl;
    
    std::cout << "\n🎉 您的设计演示完成！" << std::endl;
    std::cout << "✅ syntaxAnalys + lambda + scanner的完美结合！" << std::endl;
}

/**
 * 更复杂的示例：游戏AI语法
 * CHTL JS: gameAI({{#player}}, **, {{#enemies}})
 */
void demonstrateGameAISyntax() {
    std::cout << "\n🎮 游戏AI语法演示" << std::endl;
    std::cout << "=================" << std::endl;
    
    const auto keyword = InnovativeSyntaxAnalyzer::syntaxAnalys("selector ** selector");
    
    keyword.arg[0] = [](const KeywordValue& val) -> std::string {
        std::cout << "🎯 处理玩家选择器: " << val.value << std::endl;
        return "document.querySelector('" + val.value + "')";
    };
    
    keyword.arg[1] = []() -> std::string {
        std::cout << "🧠 处理AI配置通配符: **" << std::endl;
        
        return R"({
            // 复杂的游戏AI系统
            aiConfig: {
                difficulty: 'normal',
                aggressiveness: 0.7,
                adaptability: 0.8,
                learningRate: 0.1
            },
            
            behaviors: {
                chase: function(player, enemy) {
                    const dx = player.x - enemy.x;
                    const dy = player.y - enemy.y;
                    const distance = Math.sqrt(dx*dx + dy*dy);
                    
                    if (distance > 0) {
                        enemy.vx = (dx / distance) * enemy.speed * this.aiConfig.aggressiveness;
                        enemy.vy = (dy / distance) * enemy.speed * this.aiConfig.aggressiveness;
                    }
                },
                
                patrol: function(enemy) {
                    if (!enemy.patrolTarget) {
                        enemy.patrolTarget = {
                            x: enemy.x + (Math.random() - 0.5) * 200,
                            y: enemy.y + (Math.random() - 0.5) * 200
                        };
                    }
                    
                    const dx = enemy.patrolTarget.x - enemy.x;
                    const dy = enemy.patrolTarget.y - enemy.y;
                    const distance = Math.sqrt(dx*dx + dy*dy);
                    
                    if (distance < 10) {
                        enemy.patrolTarget = null; // 到达目标，选择新目标
                    } else {
                        enemy.vx = (dx / distance) * enemy.speed * 0.5;
                        enemy.vy = (dy / distance) * enemy.speed * 0.5;
                    }
                },
                
                avoid: function(player, enemy) {
                    const dx = player.x - enemy.x;
                    const dy = player.y - enemy.y;
                    const distance = Math.sqrt(dx*dx + dy*dy);
                    
                    if (distance < 100) {
                        enemy.vx = -(dx / distance) * enemy.speed;
                        enemy.vy = -(dy / distance) * enemy.speed;
                    }
                }
            },
            
            // AI决策系统
            makeDecision: function(player, enemy) {
                const distance = this.getDistance(player, enemy);
                const playerHealth = player.health || 100;
                const enemyHealth = enemy.health || 100;
                
                // 基于距离和血量的智能决策
                if (distance < 50 && enemyHealth > playerHealth) {
                    return 'chase';
                } else if (distance < 100 && enemyHealth < playerHealth * 0.3) {
                    return 'avoid';
                } else {
                    return 'patrol';
                }
            },
            
            getDistance: function(obj1, obj2) {
                const dx = obj1.x - obj2.x;
                const dy = obj1.y - obj2.y;
                return Math.sqrt(dx*dx + dy*dy);
            },
            
            // 学习系统
            learn: function(action, result) {
                if (!this.memory) this.memory = {};
                if (!this.memory[action]) this.memory[action] = { success: 0, failure: 0 };
                
                if (result === 'success') {
                    this.memory[action].success++;
                } else {
                    this.memory[action].failure++;
                }
                
                // 调整AI配置
                const successRate = this.memory[action].success / 
                    (this.memory[action].success + this.memory[action].failure);
                
                if (successRate > 0.8) {
                    this.aiConfig.aggressiveness = Math.min(1.0, this.aiConfig.aggressiveness + 0.1);
                } else if (successRate < 0.3) {
                    this.aiConfig.aggressiveness = Math.max(0.1, this.aiConfig.aggressiveness - 0.1);
                }
            }
        })";
    };
    
    keyword.arg[2] = [](const KeywordValue& val) -> std::string {
        std::cout << "👹 处理敌人选择器: " << val.value << std::endl;
        return "document.querySelectorAll('" + val.value + "')";
    };
    
    // 🔍 设置扫描器接口
    CJMODScanner scanner;
    
    scanner.scanKeyword(keyword.arg[1], [&]() {
        std::cout << "🔍 游戏AI扫描器回调执行！" << std::endl;
        
        KeywordValue player = scanner.peekKeyword(-1);
        KeywordValue enemies = scanner.peekKeyword(1);
        
        std::cout << "🎮 检测到游戏AI语法:" << std::endl;
        std::cout << "   玩家: " << player.value << std::endl;
        std::cout << "   敌人: " << enemies.value << std::endl;
        
        // 🚀 在扫描时执行智能处理
        if (player.type == "selector" && enemies.type == "selector") {
            std::cout << "🧠 启用高级游戏AI处理模式" << std::endl;
            
            // 可以在这里调用其他参数处理器
            keyword.argProcessors[0](player);  // 处理玩家
            keyword.argProcessors[2](enemies); // 处理敌人
        }
    });
    
    // 生成最终代码
    std::string gameAICode = InnovativeCodeGenerator::generateOptimizedCode(keyword);
    
    std::cout << "\n🎮 游戏AI语法生成的代码：" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << gameAICode.substr(0, 500) << "..." << std::endl;
}

/**
 * 主演示函数
 */
int main() {
    std::cout << "🚀 创新CJMOD完整设计演示" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "基于您的syntaxAnalys + lambda + scanner设计" << std::endl;
    std::cout << std::endl;
    
    // 演示智能图表语法
    demonstrateInnovativeDesign();
    
    // 演示游戏AI语法
    demonstrateGameAISyntax();
    
    std::cout << "\n🎊 您的设计总结：" << std::endl;
    std::cout << "===============" << std::endl;
    std::cout << "✅ syntaxAnalys('模式') - 直观的模式定义" << std::endl;
    std::cout << "✅ keyword.arg[n] = lambda - 灵活的参数处理" << std::endl;
    std::cout << "✅ ** 通配符 - 无限复杂度支持" << std::endl;
    std::cout << "✅ scanner.scanKeyword() - 智能扫描回调" << std::endl;
    std::cout << "✅ peekKeyword(±n) - 上下文感知" << std::endl;
    std::cout << "✅ generateCode() - 优雅的代码生成" << std::endl;
    std::cout << "\n🌟 这个设计让CJMOD开发变得既强大又简单！" << std::endl;
    
    return 0;
}

// ========================================
// CJMOD导出函数
// ========================================

extern "C" const char* processInnovativeSyntax(const char* chtljs_code) {
    std::string code(chtljs_code);
    
    // 🎯 使用您的创新设计处理语法
    if (code.find("smartChart") != std::string::npos) {
        const auto keyword = InnovativeSyntaxAnalyzer::syntaxAnalys("selector ** string");
        
        CJMODScanner scanner;
        InnovativeSyntaxAnalyzer::applySyntaxToCode(keyword, code, scanner);
        
        // 设置处理器
        keyword.arg[0] = [](const KeywordValue& val) {
            return "document.querySelector('" + val.value + "')";
        };
        keyword.arg[1] = []() {
            return "{ /* 复杂图表配置 */ }";
        };
        keyword.arg[2] = [](const std::string& type) {
            return "'" + type + "'";
        };
        
        static std::string result = InnovativeCodeGenerator::generateCode(keyword);
        return result.c_str();
    }
    
    if (code.find("gameAI") != std::string::npos) {
        const auto keyword = InnovativeSyntaxAnalyzer::syntaxAnalys("selector ** selector");
        
        CJMODScanner scanner;
        InnovativeSyntaxAnalyzer::applySyntaxToCode(keyword, code, scanner);
        
        static std::string result = InnovativeCodeGenerator::generateCode(keyword);
        return result.c_str();
    }
    
    return chtljs_code;
}