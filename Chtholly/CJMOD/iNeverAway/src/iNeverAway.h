#ifndef CHTHOLLY_CJMOD_INEVERAWAY_H
#define CHTHOLLY_CJMOD_INEVERAWAY_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <any>

namespace Chtholly {
    namespace CJMOD {
        
        // 前向声明
        class VirtualObject;
        class EventManager;
        
        // 函数类型枚举
        enum class FunctionType {
            VOID_NO_PARAMS,     // Void: function()
            VOID_WITH_STATE,    // Void<State>: function()
            HANDLE_EVENT,       // Handle: function(event)
            HANDLE_WITH_STATE,  // Handle<State>: function(event, data)
            PROCESS_DATA,       // Process<Type>: function(data) -> result
            CUSTOM_KEY          // 自定义键
        };
        
        // 事件处理函数包装器
        struct EventHandler {
            FunctionType type;
            std::string state;
            std::function<void()> voidHandler;
            std::function<void(const std::any&)> eventHandler;
            std::function<std::any(const std::any&)> processHandler;
            std::function<std::any(const std::vector<std::any>&)> customHandler;
            
            EventHandler(FunctionType t) : type(t) {}
        };
        
        // iNeverAway配置结构
        struct INeverAwayConfig {
            std::map<std::string, std::function<void()>> voidHandlers;
            std::map<std::string, std::function<void(const std::any&)>> eventHandlers;
            std::map<std::string, std::function<std::any(const std::any&)>> processHandlers;
            std::map<std::string, std::function<std::any(const std::vector<std::any>&)>> customHandlers;
            
            // 设置选项
            bool enableAutoMemoryManagement = true;
            bool enableChainedCalls = true;
            bool enableTypeSafetyChecks = true;
            bool enableDebugLogging = false;
            
            INeverAwayConfig() = default;
        };
        
        // 虚拟对象类 - 不存在的对象，转换为函数引用
        class VirtualObject {
        private:
            std::string objectName;
            std::shared_ptr<EventManager> eventManager;
            std::map<std::string, std::shared_ptr<EventHandler>> handlers;
            bool initialized;
            
        public:
            VirtualObject(const std::string& name);
            ~VirtualObject();
            
            // 注册处理器
            void registerVoidHandler(const std::string& key, std::function<void()> handler, const std::string& state = "");
            void registerEventHandler(const std::string& key, std::function<void(const std::any&)> handler, const std::string& state = "");
            void registerProcessHandler(const std::string& key, std::function<std::any(const std::any&)> handler, const std::string& state = "");
            void registerCustomHandler(const std::string& key, std::function<std::any(const std::vector<std::any>&)> handler);
            
            // 调用方法（->操作符模拟）
            void callVoid(const std::string& key, const std::string& state = "");
            void callHandle(const std::string& key, const std::any& data, const std::string& state = "");
            std::any callProcess(const std::string& key, const std::any& data, const std::string& state = "");
            std::any callCustom(const std::string& key, const std::vector<std::any>& args);
            
            // 工具方法
            bool hasHandler(const std::string& key, const std::string& state = "") const;
            std::vector<std::string> getRegisteredKeys() const;
            void clearHandlers();
            
            // 链式调用支持
            VirtualObject& chain();
            
            // 获取对象名称
            const std::string& getName() const { return objectName; }
        };
        
        // 事件管理器 - 管理虚拟对象的生命周期
        class EventManager {
        private:
            static std::shared_ptr<EventManager> instance;
            std::map<std::string, std::shared_ptr<VirtualObject>> virtualObjects;
            bool enableGlobalLogging;
            
            EventManager() : enableGlobalLogging(false) {}
            
        public:
            static std::shared_ptr<EventManager> getInstance();
            
            // 虚拟对象管理
            std::shared_ptr<VirtualObject> createVirtualObject(const std::string& name);
            std::shared_ptr<VirtualObject> getVirtualObject(const std::string& name);
            void destroyVirtualObject(const std::string& name);
            
            // 全局事件分发
            void dispatchGlobalEvent(const std::string& eventType, const std::any& data);
            
            // 日志和调试
            void enableLogging(bool enable) { enableGlobalLogging = enable; }
            void logMessage(const std::string& message);
            
            // 清理所有对象
            void cleanup();
            
            // 获取统计信息
            size_t getObjectCount() const { return virtualObjects.size(); }
            std::vector<std::string> getObjectNames() const;
        };
        
        // iNeverAway主类 - 工厂和管理器
        class INeverAway {
        private:
            static std::shared_ptr<EventManager> globalManager;
            
        public:
            // 创建虚拟对象的主要接口
            static std::shared_ptr<VirtualObject> create(const std::string& name, const INeverAwayConfig& config);
            
            // 便捷创建方法
            template<typename T>
            static std::shared_ptr<VirtualObject> createWithHandlers(const std::string& name, const T& handlerMap);
            
            // 全局管理
            static void cleanup();
            static void enableGlobalLogging(bool enable);
            
            // 工具方法
            static std::vector<std::string> getActiveObjects();
            static bool objectExists(const std::string& name);
            static void destroyObject(const std::string& name);
        };
        
        // 模板特化 - 支持不同的处理器映射类型
        template<>
        std::shared_ptr<VirtualObject> INeverAway::createWithHandlers<std::map<std::string, std::function<void()>>>(
            const std::string& name, 
            const std::map<std::string, std::function<void()>>& handlerMap
        );
        
    } // namespace CJMOD
} // namespace Chtholly

// C风格的导出函数（用于CHTL JS集成）
extern "C" {
    // 创建虚拟对象
    void* chtl_iNeverAway_create(const char* name);
    
    // 注册处理器
    void chtl_iNeverAway_register_void(void* obj, const char* key, void(*handler)());
    void chtl_iNeverAway_register_event(void* obj, const char* key, void(*handler)(void*));
    
    // 调用方法
    void chtl_iNeverAway_call_void(void* obj, const char* key, const char* state);
    void chtl_iNeverAway_call_handle(void* obj, const char* key, void* data);
    
    // 对象管理
    void chtl_iNeverAway_destroy(void* obj);
    void chtl_iNeverAway_cleanup();
    
    // 获取信息
    int chtl_iNeverAway_object_count();
    const char* chtl_iNeverAway_get_objects();
}

#endif // CHTHOLLY_CJMOD_INEVERAWAY_H