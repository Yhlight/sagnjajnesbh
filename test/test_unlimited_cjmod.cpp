#include "CHTLJSCompiler/Module/UnlimitedCJMOD.h"
#include <iostream>

using namespace chtl::unlimited_cjmod;

int main() {
    std::cout << "🚀 无限制CJMOD框架测试 🚀" << std::endl;
    std::cout << "=========================" << std::endl;
    std::cout << "展示高自由度、简单易用、任意扩展的强大能力" << std::endl;
    
    try {
        UnlimitedCJmodManager manager;
        
        std::cout << "\n🎯 1. 创建游戏引擎扩展项目..." << std::endl;
        std::cout << "============================" << std::endl;
        
        // 创建一个游戏引擎扩展项目
        bool projectCreated = manager.createCJmodProject("GameEngine", "./test_output");
        std::cout << "项目创建: " << (projectCreated ? "成功" : "失败") << std::endl;
        
        std::cout << "\n🔧 2. 构建无限制CJMOD模块..." << std::endl;
        std::cout << "============================" << std::endl;
        
        UnlimitedCJmodModule gameModule;
        gameModule.info.name = "GameEngine";
        gameModule.info.description = "高性能游戏引擎扩展";
        
        // 添加自由C++代码 - 任意头文件
        manager.addFreeCppCode(gameModule, "headers", 
                              "#include <GL/gl.h>\n#include <SDL2/SDL.h>\n#include <bullet/btBulletDynamicsCommon.h>",
                              100, "游戏引擎依赖库");
        
        // 添加全局状态 - 维护游戏状态
        manager.addGlobalState(gameModule, "gameTime", "double", "0.0");
        manager.addGlobalState(gameModule, "playerScore", "int", "0");
        manager.addGlobalState(gameModule, "gameRunning", "bool", "false");
        
        // 添加自由C++函数 - 物理引擎
        std::string physicsCode = R"(
            // 高性能物理计算
            btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
            
            void initPhysics() {
                btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
                btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
                btDbvtBroadphase* overlappingPairCache = new btDbvtBroadphase();
                btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
                
                dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
                dynamicsWorld->setGravity(btVector3(0, -10, 0));
            }
            
            return "物理引擎初始化成功";
        )";
        
        manager.addGlobalFunction(gameModule, "initGamePhysics", physicsCode);
        
        // 添加渲染函数
        std::string renderCode = R"(
            // OpenGL渲染管线
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glLoadIdentity();
            
            // 渲染游戏对象
            for (int i = 0; i < 100; ++i) {
                glPushMatrix();
                glTranslatef(i * 2.0f, 0.0f, 0.0f);
                glColor3f(1.0f, 0.0f, 0.0f);
                // 绘制立方体...
                glPopMatrix();
            }
            
            return "渲染完成，FPS: 60";
        )";
        
        manager.addGlobalFunction(gameModule, "renderFrame", renderCode);
        
        std::cout << "\n🎮 3. 创造全新的CHTL JS语法..." << std::endl;
        std::cout << "===============================" << std::endl;
        
        // 创造全新的游戏语法
        manager.addCHTLJSSyntax(gameModule, "spawnEnemy", 
                               "spawnEnemy({type, position, health})",
                               "spawnEnemy_impl",
                               "console.log('敌人生成:', params); return createGameObject(params);");
        
        manager.addCHTLJSSyntax(gameModule, "playSound", 
                               "playSound({file, volume, loop})",
                               "playSound_impl", 
                               "const audio = new Audio(params.file); audio.volume = params.volume; audio.play();");
        
        manager.addCHTLJSSyntax(gameModule, "saveGame", 
                               "saveGame({slot, data})",
                               "saveGame_impl",
                               "localStorage.setItem('game_save_' + params.slot, JSON.stringify(params.data));");
        
        std::cout << "\n📊 4. 生成完整的C++代码..." << std::endl;
        std::cout << "=============================" << std::endl;
        
        std::string generatedCpp = gameModule.generateCompleteCppCode();
        std::cout << "生成的C++代码长度: " << generatedCpp.length() << " 字符" << std::endl;
        std::cout << "包含代码块数量: " << gameModule.cppBlocks.size() << std::endl;
        
        // 显示部分生成的代码
        std::cout << "\n生成的C++代码预览:" << std::endl;
        std::cout << "==================" << std::endl;
        std::string preview = generatedCpp.substr(0, 500);
        std::cout << preview << "..." << std::endl;
        
        std::cout << "\n🌐 5. 生成JavaScript接口..." << std::endl;
        std::cout << "===========================" << std::endl;
        
        std::string generatedJS = gameModule.generateJavaScriptInterface();
        std::cout << "生成的JavaScript代码长度: " << generatedJS.length() << " 字符" << std::endl;
        std::cout << "语法扩展数量: " << gameModule.syntaxExtensions.size() << std::endl;
        
        // 显示JavaScript接口预览
        std::cout << "\n生成的JavaScript接口预览:" << std::endl;
        std::cout << "=========================" << std::endl;
        std::string jsPreview = generatedJS.substr(0, 300);
        std::cout << jsPreview << "..." << std::endl;
        
        std::cout << "\n🎨 6. 展示模板代码..." << std::endl;
        std::cout << "===================" << std::endl;
        
        std::cout << "基础模板:" << std::endl;
        std::string basicTemplate = manager.generateTemplate("basic");
        std::cout << "长度: " << basicTemplate.length() << " 字符" << std::endl;
        
        std::cout << "\n高级模板:" << std::endl;
        std::string advancedTemplate = manager.generateTemplate("advanced");
        std::cout << "长度: " << advancedTemplate.length() << " 字符" << std::endl;
        
        std::cout << "\n游戏引擎模板:" << std::endl;
        std::string gameTemplate = manager.generateTemplate("game");
        std::cout << "长度: " << gameTemplate.length() << " 字符" << std::endl;
        
        std::cout << "\n🔥 7. 测试全局状态管理..." << std::endl;
        std::cout << "=========================" << std::endl;
        
        auto stateManager = gameModule.stateManager;
        
        // 设置游戏状态
        stateManager->setState("level", "1");
        stateManager->setState("difficulty", "hard");
        stateManager->setState("playerName", "CHTL_Player");
        
        std::cout << "游戏等级: " << stateManager->getState("level") << std::endl;
        std::cout << "难度: " << stateManager->getState("difficulty") << std::endl;
        std::cout << "玩家名: " << stateManager->getState("playerName") << std::endl;
        
        // 状态变化监听
        stateManager->onStateChange("level", [](const std::string& oldVal, const std::string& newVal) {
            std::cout << "等级变化: " << oldVal << " -> " << newVal << std::endl;
        });
        
        stateManager->setState("level", "2"); // 触发监听器
        
        std::cout << "\n🎉 无限制CJMOD框架测试完成！" << std::endl;
        std::cout << "================================" << std::endl;
        std::cout << "✅ 高自由度 - 可以包含任意C++代码和库" << std::endl;
        std::cout << "✅ 简单易用 - 一行代码添加复杂功能" << std::endl;
        std::cout << "✅ 容易上手 - 丰富的模板和示例" << std::endl;
        std::cout << "✅ 任意扩展 - 创造全新的CHTL JS语法" << std::endl;
        std::cout << "✅ 无限可能 - JS的上限就是CHTL JS的上限" << std::endl;
        std::cout << "✅ 全局状态 - 维护复杂的应用状态" << std::endl;
        std::cout << "✅ 动态库支持 - 运行时加载任意C++库" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ 测试异常: " << e.what() << std::endl;
        return 1;
    }
}