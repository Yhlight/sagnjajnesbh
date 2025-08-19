// ========================================
// 🚀 CJMOD编译输出 - C++扩展语法转换成的JavaScript代码
// 这展示了CJMOD的核心工作原理
// ========================================

let timerController = null;

// ========================================
// CJMOD扩展语法编译结果
// ========================================

// 🎨 animate扩展语法编译结果
document.getElementById('bounce-btn').addEventListener('click', function() {
    // animate({target: {{#animate-box}}, effect: 'bounce', duration: 800})
    // ↓ C++处理后生成的JavaScript代码：
    (() => {
        const element = document.getElementById('animate-box');
        if (!element) return;
        
        // 弹跳动画效果
        element.animate([
            { transform: 'translateY(0px)' },
            { transform: 'translateY(-20px)' },
            { transform: 'translateY(0px)' },
            { transform: 'translateY(-10px)' },
            { transform: 'translateY(0px)' }
        ], {
            duration: 800,
            easing: 'cubic-bezier(0.68, -0.55, 0.265, 1.55)'
        });
    })();
    
    document.getElementById('animate-output').textContent = '✅ 弹跳动画已触发';
});

document.getElementById('shake-btn').addEventListener('click', function() {
    // animate({target: {{#animate-box}}, effect: 'shake', duration: 600})
    // ↓ C++处理后生成的JavaScript代码：
    (() => {
        const element = document.getElementById('animate-box');
        if (!element) return;
        
        // 震动动画效果
        element.animate([
            { transform: 'translateX(0px)' },
            { transform: 'translateX(-10px)' },
            { transform: 'translateX(10px)' },
            { transform: 'translateX(-5px)' },
            { transform: 'translateX(5px)' },
            { transform: 'translateX(0px)' }
        ], {
            duration: 600,
            easing: 'ease-in-out'
        });
    })();
    
    document.getElementById('animate-output').textContent = '✅ 震动动画已触发';
});

// 💾 storage扩展语法编译结果
document.getElementById('save-btn').addEventListener('click', function() {
    const key = document.getElementById('storage-key').value;
    const value = document.getElementById('storage-value').value;
    
    if (key && value) {
        // storage.set(key, value)
        // ↓ C++处理后生成的JavaScript代码：
        const success = (() => {
            try {
                const data = {
                    value: value,
                    timestamp: Date.now(),
                    type: typeof(value)
                };
                localStorage.setItem(key, JSON.stringify(data));
                console.log('💾 存储成功:', key, data.value);
                return true;
            } catch (e) {
                console.error('❌ 存储失败:', e);
                return false;
            }
        })();
        
        document.getElementById('storage-output').textContent = success ? 
            '✅ 数据保存成功' : '❌ 保存失败';
    }
});

document.getElementById('load-btn').addEventListener('click', function() {
    const key = document.getElementById('storage-key').value;
    
    if (key) {
        // storage.get(key, '未找到数据')
        // ↓ C++处理后生成的JavaScript代码：
        const value = (() => {
            try {
                const stored = localStorage.getItem(key);
                if (!stored) return '未找到数据';
                
                const data = JSON.parse(stored);
                console.log('📁 读取成功:', key, data.value);
                
                // 类型还原
                if (data.type === 'number') return Number(data.value);
                if (data.type === 'boolean') return Boolean(data.value);
                return data.value;
            } catch (e) {
                console.error('❌ 读取失败:', e);
                return '未找到数据';
            }
        })();
        
        document.getElementById('storage-output').textContent = `📁 读取结果: ${value}`;
    }
});

// 🌐 http扩展语法编译结果
document.getElementById('get-btn').addEventListener('click', async function() {
    const url = document.getElementById('api-url').value;
    
    document.getElementById('http-output').textContent = '🌐 正在请求...';
    
    // http.get(url)
    // ↓ C++处理后生成的JavaScript代码：
    const result = await (async () => {
        try {
            console.log('🌐 发送GET请求:', url);
            
            const response = await fetch(url, {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json',
                    'User-Agent': 'CHTL-JS/1.0'
                }
            });
            
            if (!response.ok) {
                throw new Error(`HTTP ${response.status}: ${response.statusText}`);
            }
            
            const data = await response.json();
            console.log('✅ 请求成功:', data);
            return data;
        } catch (error) {
            console.error('❌ 请求失败:', error);
            return { error: error.message };
        }
    })();
    
    document.getElementById('http-output').textContent = `✅ 请求完成: ${JSON.stringify(result, null, 2)}`;
});

// ⏰ timer扩展语法编译结果
document.getElementById('timer-after-btn').addEventListener('click', function() {
    document.getElementById('timer-output').textContent = '⏰ 3秒后将执行...';
    
    // timer.after(3000, callback)
    // ↓ C++处理后生成的JavaScript代码：
    (() => {
        const timerId = setTimeout(() => {
            try {
                console.log('⏰ 定时器触发:', 3000, 'ms');
                (function() {
                    document.getElementById('timer-output').textContent = '✅ 定时任务执行完成！';
                    
                    // notification.show('定时器', '3秒定时任务完成！', 'success')
                    // ↓ C++处理后生成的JavaScript代码：
                    (() => {
                        // 创建通知容器（如果不存在）
                        let container = document.getElementById('chtl_notification_container');
                        if (!container) {
                            container = document.createElement('div');
                            container.id = 'chtl_notification_container';
                            container.style.position = 'fixed';
                            container.style.top = '20px';
                            container.style.right = '20px';
                            container.style.zIndex = '9999';
                            document.body.appendChild(container);
                        }
                        
                        // 创建通知元素
                        const notification = document.createElement('div');
                        const notificationId = 'notification_' + Date.now();
                        notification.id = notificationId;
                        
                        // 设置样式
                        const colors = {
                            success: { bg: '#4caf50', icon: '✅' },
                            error: { bg: '#f44336', icon: '❌' },
                            warning: { bg: '#ff9800', icon: '⚠️' },
                            info: { bg: '#2196f3', icon: 'ℹ️' }
                        };
                        
                        const color = colors['success'] || colors.info;
                        
                        notification.style.cssText = `
                            background: ${color.bg};
                            color: white;
                            padding: 15px 20px;
                            border-radius: 8px;
                            margin-bottom: 10px;
                            box-shadow: 0 4px 12px rgba(0,0,0,0.2);
                            transform: translateX(100%);
                            transition: all 0.3s ease;
                            cursor: pointer;
                            max-width: 350px;
                            word-wrap: break-word;
                        `;
                        
                        notification.innerHTML = `
                            <div style="display: flex; align-items: center; gap: 10px;">
                                <span style="font-size: 18px;">${color.icon}</span>
                                <div>
                                    <div style="font-weight: bold; margin-bottom: 4px;">定时器</div>
                                    <div style="font-size: 14px; opacity: 0.9;">3秒定时任务完成！</div>
                                </div>
                                <span style="margin-left: auto; opacity: 0.7; cursor: pointer;">×</span>
                            </div>
                        `;
                        
                        // 添加到容器
                        container.appendChild(notification);
                        
                        // 动画进入
                        setTimeout(() => {
                            notification.style.transform = 'translateX(0)';
                        }, 10);
                        
                        // 点击关闭
                        notification.addEventListener('click', () => {
                            notification.style.transform = 'translateX(100%)';
                            setTimeout(() => {
                                if (notification.parentNode) {
                                    notification.parentNode.removeChild(notification);
                                }
                            }, 300);
                        });
                        
                        // 自动消失
                        setTimeout(() => {
                            if (notification.parentNode) {
                                notification.style.transform = 'translateX(100%)';
                                setTimeout(() => {
                                    if (notification.parentNode) {
                                        notification.parentNode.removeChild(notification);
                                    }
                                }, 300);
                            }
                        }, 4000);
                        
                        console.log('🔔 通知已显示:', '定时器');
                        return notificationId;
                    })();
                })();
            } catch (error) {
                console.error('❌ 定时器回调错误:', error);
            }
        }, 3000);
        
        // 返回取消函数
        return () => {
            clearTimeout(timerId);
            console.log('⏹️ 定时器已取消');
        };
    })();
});

// 🧮 math扩展语法编译结果
document.getElementById('calculate-btn').addEventListener('click', function() {
    const expression = document.getElementById('math-expression').value;
    
    // math.calculate(expression, 4)
    // ↓ C++处理后生成的JavaScript代码：
    const result = (() => {
        try {
            const expression = document.getElementById('math-expression').value;
            console.log('🧮 计算表达式:', expression);
            
            // 安全的表达式验证
            const safeExpression = expression.replace(/[^0-9+\-*/.() ]/g, '');
            if (safeExpression !== expression) {
                throw new Error('表达式包含非法字符');
            }
            
            // 计算结果
            const result = Function('"use strict"; return (' + safeExpression + ')')();
            const roundedResult = Number(result.toFixed(4));
            
            console.log('✅ 计算结果:', expression, '=', roundedResult);
            return roundedResult;
        } catch (error) {
            console.error('❌ 计算错误:', error);
            return NaN;
        }
    })();
    
    document.getElementById('math-output').textContent = `🧮 计算结果: ${expression} = ${result}`;
    
    // notification.show编译结果（省略，与上面相同）
});

// 🎯 初始化函数
function initDemo() {
    console.log('🚀 CJMOD语法扩展演示初始化...');
    
    // notification.show编译结果
    (() => {
        // ... 通知显示代码（与上面相同）
    })();
    
    // storage.get编译结果
    const demoCount = (() => {
        try {
            const stored = localStorage.getItem('demo_count');
            if (!stored) return 0;
            
            const data = JSON.parse(stored);
            console.log('📁 读取成功:', 'demo_count', data.value);
            
            if (data.type === 'number') return Number(data.value);
            if (data.type === 'boolean') return Boolean(data.value);
            return data.value;
        } catch (e) {
            console.error('❌ 读取失败:', e);
            return 0;
        }
    })();
    
    if (demoCount > 0) {
        // 又一个notification.show编译结果...
    }
    
    // 设置示例值
    document.getElementById('storage-key').value = 'test_key';
    document.getElementById('storage-value').value = 'Hello CJMOD!';
    document.getElementById('email-input').value = 'test@example.com';
    document.getElementById('phone-input').value = '13812345678';
    
    console.log('✅ 演示初始化完成！');
    console.log('💡 所有按钮都使用了C++编写的CJMOD语法扩展');
}

// 启动演示
initDemo();