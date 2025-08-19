#include "AllJSEventsSupport.h"
#include <algorithm>

namespace chtl {
namespace chtljs {

AllJSEventsSupport::AllJSEventsSupport() {
    initializeAllJSEvents();
    initializeEventCategories();
    initializeElementCompatibility();
}

AllJSEventsSupport::~AllJSEventsSupport() = default;

// === 完整事件支持 ===

std::vector<std::string> AllJSEventsSupport::getAllSupportedEvents() {
    std::vector<std::string> allEvents;
    
    for (const auto& [eventName, eventInfo] : eventInfoMap_) {
        allEvents.push_back(eventName);
    }
    
    std::sort(allEvents.begin(), allEvents.end());
    return allEvents;
}

std::vector<std::string> AllJSEventsSupport::getEventsByCategory(const std::string& category) {
    std::vector<std::string> categoryEvents;
    
    for (const auto& [eventName, eventInfo] : eventInfoMap_) {
        if (eventInfo.category == category) {
            categoryEvents.push_back(eventName);
        }
    }
    
    return categoryEvents;
}

std::vector<AllJSEventsSupport::EventCategory> AllJSEventsSupport::getAllEventCategories() {
    std::vector<EventCategory> categories;
    
    for (const auto& [name, category] : eventCategories_) {
        categories.push_back(category);
    }
    
    return categories;
}

bool AllJSEventsSupport::isValidEventName(const std::string& eventName) {
    return eventInfoMap_.find(eventName) != eventInfoMap_.end();
}

AllJSEventsSupport::EventInfo AllJSEventsSupport::getEventInfo(const std::string& eventName) {
    auto it = eventInfoMap_.find(eventName);
    if (it != eventInfoMap_.end()) {
        return it->second;
    }
    
    // 返回默认的事件信息
    EventInfo defaultInfo;
    defaultInfo.name = eventName;
    defaultInfo.category = "custom";
    defaultInfo.description = "自定义事件";
    return defaultInfo;
}

// === 事件兼容性检查 ===

bool AllJSEventsSupport::isEventCompatibleWithElement(const std::string& eventName, const std::string& elementType) {
    auto it = elementEventCompatibility_.find(elementType);
    if (it != elementEventCompatibility_.end()) {
        return it->second.find(eventName) != it->second.end();
    }
    
    // 默认情况下，大多数事件都与大多数元素兼容
    return true;
}

std::vector<std::string> AllJSEventsSupport::getSupportedEventsForElement(const std::string& elementType) {
    std::vector<std::string> supportedEvents;
    
    auto it = elementEventCompatibility_.find(elementType);
    if (it != elementEventCompatibility_.end()) {
        for (const auto& eventName : it->second) {
            supportedEvents.push_back(eventName);
        }
    }
    
    return supportedEvents;
}

bool AllJSEventsSupport::isEventBubbling(const std::string& eventName) {
    auto it = eventInfoMap_.find(eventName);
    return it != eventInfoMap_.end() ? it->second.isBubbling : true;
}

// === 初始化方法 ===

void AllJSEventsSupport::initializeAllJSEvents() {
    // 初始化所有JavaScript事件
    
    // === 鼠标事件 ===
    eventInfoMap_["click"] = {"click", "mouse", "鼠标点击", true, true, {}};
    eventInfoMap_["dblclick"] = {"dblclick", "mouse", "鼠标双击", true, true, {}};
    eventInfoMap_["mousedown"] = {"mousedown", "mouse", "鼠标按下", true, true, {}};
    eventInfoMap_["mouseup"] = {"mouseup", "mouse", "鼠标释放", true, true, {}};
    eventInfoMap_["mousemove"] = {"mousemove", "mouse", "鼠标移动", true, true, {}};
    eventInfoMap_["mouseenter"] = {"mouseenter", "mouse", "鼠标进入", false, false, {}}; // 不冒泡
    eventInfoMap_["mouseleave"] = {"mouseleave", "mouse", "鼠标离开", false, false, {}}; // 不冒泡
    eventInfoMap_["mouseover"] = {"mouseover", "mouse", "鼠标悬停", true, true, {}};
    eventInfoMap_["mouseout"] = {"mouseout", "mouse", "鼠标移出", true, true, {}};
    eventInfoMap_["contextmenu"] = {"contextmenu", "mouse", "右键菜单", true, true, {}};
    eventInfoMap_["wheel"] = {"wheel", "mouse", "鼠标滚轮", true, true, {}};
    
    // === 键盘事件 ===
    eventInfoMap_["keydown"] = {"keydown", "keyboard", "按键按下", true, true, {}};
    eventInfoMap_["keyup"] = {"keyup", "keyboard", "按键释放", true, true, {}};
    eventInfoMap_["keypress"] = {"keypress", "keyboard", "按键按压", true, true, {}}; // 已废弃但仍支持
    
    // === 表单事件 ===
    eventInfoMap_["submit"] = {"submit", "form", "表单提交", true, true, {"form"}};
    eventInfoMap_["reset"] = {"reset", "form", "表单重置", true, true, {"form"}};
    eventInfoMap_["input"] = {"input", "form", "输入变化", true, false, {"input", "textarea", "select"}};
    eventInfoMap_["change"] = {"change", "form", "值改变", true, true, {"input", "textarea", "select"}};
    eventInfoMap_["focus"] = {"focus", "form", "获得焦点", false, false, {}}; // 不冒泡
    eventInfoMap_["blur"] = {"blur", "form", "失去焦点", false, false, {}}; // 不冒泡
    eventInfoMap_["focusin"] = {"focusin", "form", "焦点进入", true, false, {}};
    eventInfoMap_["focusout"] = {"focusout", "form", "焦点移出", true, false, {}};
    eventInfoMap_["select"] = {"select", "form", "文本选择", true, false, {"input", "textarea"}};
    eventInfoMap_["invalid"] = {"invalid", "form", "验证失败", false, true, {"input", "textarea", "select"}};
    
    // === 窗口和文档事件 ===
    eventInfoMap_["load"] = {"load", "window", "加载完成", false, false, {"window", "document", "img", "script"}};
    eventInfoMap_["unload"] = {"unload", "window", "页面卸载", false, false, {"window"}};
    eventInfoMap_["beforeunload"] = {"beforeunload", "window", "卸载前", false, true, {"window"}};
    eventInfoMap_["resize"] = {"resize", "window", "窗口调整", false, false, {"window"}};
    eventInfoMap_["scroll"] = {"scroll", "window", "滚动", false, false, {"window", "document", "element"}};
    eventInfoMap_["hashchange"] = {"hashchange", "window", "哈希变化", false, false, {"window"}};
    eventInfoMap_["popstate"] = {"popstate", "window", "历史状态", false, false, {"window"}};
    eventInfoMap_["storage"] = {"storage", "window", "存储变化", false, false, {"window"}};
    eventInfoMap_["online"] = {"online", "window", "网络连接", false, false, {"window"}};
    eventInfoMap_["offline"] = {"offline", "window", "网络断开", false, false, {"window"}};
    eventInfoMap_["visibilitychange"] = {"visibilitychange", "document", "可见性变化", false, false, {"document"}};
    
    // === 拖拽事件 ===
    eventInfoMap_["drag"] = {"drag", "drag", "拖拽中", true, true, {}};
    eventInfoMap_["dragstart"] = {"dragstart", "drag", "开始拖拽", true, true, {}};
    eventInfoMap_["dragend"] = {"dragend", "drag", "结束拖拽", true, true, {}};
    eventInfoMap_["dragenter"] = {"dragenter", "drag", "拖拽进入", true, true, {}};
    eventInfoMap_["dragover"] = {"dragover", "drag", "拖拽悬停", true, true, {}};
    eventInfoMap_["dragleave"] = {"dragleave", "drag", "拖拽离开", true, true, {}};
    eventInfoMap_["drop"] = {"drop", "drag", "拖拽释放", true, true, {}};
    
    // === 触摸事件 ===
    eventInfoMap_["touchstart"] = {"touchstart", "touch", "触摸开始", true, true, {}};
    eventInfoMap_["touchend"] = {"touchend", "touch", "触摸结束", true, true, {}};
    eventInfoMap_["touchmove"] = {"touchmove", "touch", "触摸移动", true, true, {}};
    eventInfoMap_["touchcancel"] = {"touchcancel", "touch", "触摸取消", true, false, {}};
    
    // === 媒体事件 ===
    eventInfoMap_["play"] = {"play", "media", "开始播放", false, false, {"audio", "video"}};
    eventInfoMap_["pause"] = {"pause", "media", "暂停播放", false, false, {"audio", "video"}};
    eventInfoMap_["ended"] = {"ended", "media", "播放结束", false, false, {"audio", "video"}};
    eventInfoMap_["loadstart"] = {"loadstart", "media", "开始加载", false, false, {"audio", "video", "img"}};
    eventInfoMap_["loadeddata"] = {"loadeddata", "media", "数据加载", false, false, {"audio", "video"}};
    eventInfoMap_["loadedmetadata"] = {"loadedmetadata", "media", "元数据加载", false, false, {"audio", "video"}};
    eventInfoMap_["canplay"] = {"canplay", "media", "可以播放", false, false, {"audio", "video"}};
    eventInfoMap_["canplaythrough"] = {"canplaythrough", "media", "可流畅播放", false, false, {"audio", "video"}};
    eventInfoMap_["durationchange"] = {"durationchange", "media", "时长变化", false, false, {"audio", "video"}};
    eventInfoMap_["timeupdate"] = {"timeupdate", "media", "时间更新", false, false, {"audio", "video"}};
    eventInfoMap_["progress"] = {"progress", "media", "加载进度", false, false, {"audio", "video"}};
    eventInfoMap_["volumechange"] = {"volumechange", "media", "音量变化", false, false, {"audio", "video"}};
    eventInfoMap_["ratechange"] = {"ratechange", "media", "播放速率变化", false, false, {"audio", "video"}};
    eventInfoMap_["seeking"] = {"seeking", "media", "定位中", false, false, {"audio", "video"}};
    eventInfoMap_["seeked"] = {"seeked", "media", "定位完成", false, false, {"audio", "video"}};
    eventInfoMap_["stalled"] = {"stalled", "media", "加载停滞", false, false, {"audio", "video"}};
    eventInfoMap_["suspend"] = {"suspend", "media", "加载暂停", false, false, {"audio", "video"}};
    eventInfoMap_["waiting"] = {"waiting", "media", "等待数据", false, false, {"audio", "video"}};
    eventInfoMap_["emptied"] = {"emptied", "media", "媒体清空", false, false, {"audio", "video"}};
    
    // === 网络和加载事件 ===
    eventInfoMap_["error"] = {"error", "network", "错误", true, false, {}};
    eventInfoMap_["abort"] = {"abort", "network", "中止", false, false, {}};
    eventInfoMap_["timeout"] = {"timeout", "network", "超时", false, false, {}};
    
    // === 动画和过渡事件 ===
    eventInfoMap_["animationstart"] = {"animationstart", "animation", "动画开始", true, false, {}};
    eventInfoMap_["animationend"] = {"animationend", "animation", "动画结束", true, false, {}};
    eventInfoMap_["animationiteration"] = {"animationiteration", "animation", "动画迭代", true, false, {}};
    eventInfoMap_["animationcancel"] = {"animationcancel", "animation", "动画取消", true, false, {}};
    eventInfoMap_["transitionstart"] = {"transitionstart", "transition", "过渡开始", true, false, {}};
    eventInfoMap_["transitionend"] = {"transitionend", "transition", "过渡结束", true, false, {}};
    eventInfoMap_["transitionrun"] = {"transitionrun", "transition", "过渡运行", true, false, {}};
    eventInfoMap_["transitioncancel"] = {"transitioncancel", "transition", "过渡取消", true, false, {}};
    
    // === 指针事件 (现代Web API) ===
    eventInfoMap_["pointerdown"] = {"pointerdown", "pointer", "指针按下", true, true, {}};
    eventInfoMap_["pointerup"] = {"pointerup", "pointer", "指针释放", true, true, {}};
    eventInfoMap_["pointermove"] = {"pointermove", "pointer", "指针移动", true, true, {}};
    eventInfoMap_["pointerenter"] = {"pointerenter", "pointer", "指针进入", false, false, {}};
    eventInfoMap_["pointerleave"] = {"pointerleave", "pointer", "指针离开", false, false, {}};
    eventInfoMap_["pointerover"] = {"pointerover", "pointer", "指针悬停", true, true, {}};
    eventInfoMap_["pointerout"] = {"pointerout", "pointer", "指针移出", true, true, {}};
    eventInfoMap_["pointercancel"] = {"pointercancel", "pointer", "指针取消", true, true, {}};
    eventInfoMap_["gotpointercapture"] = {"gotpointercapture", "pointer", "获得指针捕获", false, false, {}};
    eventInfoMap_["lostpointercapture"] = {"lostpointercapture", "pointer", "失去指针捕获", false, false, {}};
    
    // === 滚轮和手势事件 ===
    eventInfoMap_["gesturestart"] = {"gesturestart", "gesture", "手势开始", false, false, {}};
    eventInfoMap_["gesturechange"] = {"gesturechange", "gesture", "手势变化", false, false, {}};
    eventInfoMap_["gestureend"] = {"gestureend", "gesture", "手势结束", false, false, {}};
    
    // === 设备方向事件 ===
    eventInfoMap_["deviceorientation"] = {"deviceorientation", "device", "设备方向", false, false, {"window"}};
    eventInfoMap_["devicemotion"] = {"devicemotion", "device", "设备运动", false, false, {"window"}};
    
    // === 页面生命周期事件 ===
    eventInfoMap_["DOMContentLoaded"] = {"DOMContentLoaded", "lifecycle", "DOM加载完成", false, false, {"document"}};
    eventInfoMap_["readystatechange"] = {"readystatechange", "lifecycle", "就绪状态变化", false, false, {"document"}};
    
    // === 剪贴板事件 ===
    eventInfoMap_["copy"] = {"copy", "clipboard", "复制", true, true, {}};
    eventInfoMap_["cut"] = {"cut", "clipboard", "剪切", true, true, {}};
    eventInfoMap_["paste"] = {"paste", "clipboard", "粘贴", true, true, {}};
    
    // === 选择事件 ===
    eventInfoMap_["selectstart"] = {"selectstart", "selection", "选择开始", true, true, {}};
    eventInfoMap_["selectionchange"] = {"selectionchange", "selection", "选择变化", false, false, {"document"}};
    
    // === 全屏事件 ===
    eventInfoMap_["fullscreenchange"] = {"fullscreenchange", "fullscreen", "全屏状态变化", false, false, {"document"}};
    eventInfoMap_["fullscreenerror"] = {"fullscreenerror", "fullscreen", "全屏错误", false, false, {"document"}};
    
    // === 网络状态事件 ===
    eventInfoMap_["online"] = {"online", "network", "网络连接", false, false, {"window"}};
    eventInfoMap_["offline"] = {"offline", "network", "网络断开", false, false, {"window"}};
    
    // === 打印事件 ===
    eventInfoMap_["beforeprint"] = {"beforeprint", "print", "打印前", false, false, {"window"}};
    eventInfoMap_["afterprint"] = {"afterprint", "print", "打印后", false, false, {"window"}};
    
    // === 存储事件 ===
    eventInfoMap_["storage"] = {"storage", "storage", "存储变化", false, false, {"window"}};
    
    // === 消息事件 ===
    eventInfoMap_["message"] = {"message", "message", "消息", false, false, {"window"}};
    eventInfoMap_["messageerror"] = {"messageerror", "message", "消息错误", false, false, {"window"}};
    
    // === 性能事件 ===
    eventInfoMap_["resourcetimingbufferfull"] = {"resourcetimingbufferfull", "performance", "性能缓冲区满", false, false, {"performance"}};
    
    // === Web Components事件 ===
    eventInfoMap_["slotchange"] = {"slotchange", "webcomponents", "插槽变化", true, false, {}};
    
    // === 其他重要事件 ===
    eventInfoMap_["toggle"] = {"toggle", "ui", "切换状态", false, false, {"details"}};
    eventInfoMap_["cancel"] = {"cancel", "ui", "取消", false, true, {"dialog"}};
    eventInfoMap_["close"] = {"close", "ui", "关闭", false, false, {"dialog"}};
    eventInfoMap_["auxclick"] = {"auxclick", "mouse", "辅助点击", true, true, {}}; // 中键、右键点击
    
    // === 现代Web API事件 ===
    eventInfoMap_["beforeinput"] = {"beforeinput", "input", "输入前", true, true, {}};
    eventInfoMap_["compositionstart"] = {"compositionstart", "composition", "输入法开始", true, true, {}};
    eventInfoMap_["compositionupdate"] = {"compositionupdate", "composition", "输入法更新", true, true, {}};
    eventInfoMap_["compositionend"] = {"compositionend", "composition", "输入法结束", true, true, {}};
    
    // === 安全事件 ===
    eventInfoMap_["securitypolicyviolation"] = {"securitypolicyviolation", "security", "安全策略违规", false, false, {"document"}};
}

void AllJSEventsSupport::initializeEventCategories() {
    // 初始化事件分类
    
    eventCategories_["mouse"] = {"鼠标事件", {
        "click", "dblclick", "mousedown", "mouseup", "mousemove", 
        "mouseenter", "mouseleave", "mouseover", "mouseout", "contextmenu", "wheel", "auxclick"
    }, "所有鼠标相关的交互事件"};
    
    eventCategories_["keyboard"] = {"键盘事件", {
        "keydown", "keyup", "keypress"
    }, "所有键盘输入事件"};
    
    eventCategories_["form"] = {"表单事件", {
        "submit", "reset", "input", "change", "focus", "blur", 
        "focusin", "focusout", "select", "invalid"
    }, "表单元素相关事件"};
    
    eventCategories_["window"] = {"窗口事件", {
        "load", "unload", "beforeunload", "resize", "scroll", 
        "hashchange", "popstate", "storage", "online", "offline"
    }, "浏览器窗口相关事件"};
    
    eventCategories_["touch"] = {"触摸事件", {
        "touchstart", "touchend", "touchmove", "touchcancel"
    }, "移动设备触摸事件"};
    
    eventCategories_["drag"] = {"拖拽事件", {
        "drag", "dragstart", "dragend", "dragenter", "dragover", "dragleave", "drop"
    }, "拖拽操作相关事件"};
    
    eventCategories_["media"] = {"媒体事件", {
        "play", "pause", "ended", "loadstart", "loadeddata", "loadedmetadata",
        "canplay", "canplaythrough", "durationchange", "timeupdate", "progress",
        "volumechange", "ratechange", "seeking", "seeked", "stalled", "suspend", "waiting", "emptied"
    }, "音频视频媒体事件"};
    
    eventCategories_["animation"] = {"动画事件", {
        "animationstart", "animationend", "animationiteration", "animationcancel"
    }, "CSS动画相关事件"};
    
    eventCategories_["transition"] = {"过渡事件", {
        "transitionstart", "transitionend", "transitionrun", "transitioncancel"
    }, "CSS过渡相关事件"};
    
    eventCategories_["pointer"] = {"指针事件", {
        "pointerdown", "pointerup", "pointermove", "pointerenter", "pointerleave",
        "pointerover", "pointerout", "pointercancel", "gotpointercapture", "lostpointercapture"
    }, "现代指针API事件"};
}

void AllJSEventsSupport::initializeElementCompatibility() {
    // 初始化元素与事件的兼容性
    
    // 表单元素特有事件
    elementEventCompatibility_["input"] = {
        "input", "change", "focus", "blur", "select", "invalid"
    };
    
    elementEventCompatibility_["textarea"] = {
        "input", "change", "focus", "blur", "select", "invalid"
    };
    
    elementEventCompatibility_["select"] = {
        "input", "change", "focus", "blur", "invalid"
    };
    
    elementEventCompatibility_["form"] = {
        "submit", "reset"
    };
    
    // 媒体元素特有事件
    elementEventCompatibility_["audio"] = {
        "play", "pause", "ended", "loadstart", "loadeddata", "loadedmetadata",
        "canplay", "canplaythrough", "durationchange", "timeupdate", "progress",
        "volumechange", "ratechange", "seeking", "seeked", "stalled", "suspend", "waiting"
    };
    
    elementEventCompatibility_["video"] = elementEventCompatibility_["audio"]; // 与audio相同
    
    // 图片元素
    elementEventCompatibility_["img"] = {
        "load", "error", "abort"
    };
    
    // 详情元素
    elementEventCompatibility_["details"] = {
        "toggle"
    };
    
    // 对话框元素
    elementEventCompatibility_["dialog"] = {
        "cancel", "close"
    };
}

// === 事件代码生成优化 ===

std::string AllJSEventsSupport::generateOptimizedEventBinding(const std::string& eventName, const std::string& handler, const std::string& target) {
    // 根据事件特性生成最优的JavaScript代码
    
    EventInfo eventInfo = getEventInfo(eventName);
    
    std::string code = "// 优化的" + eventInfo.description + "事件绑定\n";
    
    if (!eventInfo.isBubbling) {
        code += "// 注意：此事件不支持冒泡\n";
    }
    
    if (!eventInfo.isCancelable) {
        code += "// 注意：此事件不可取消\n";
    }
    
    code += target + ".addEventListener('" + eventName + "', " + handler;
    
    // 根据事件特性添加选项
    if (!eventInfo.isBubbling || !eventInfo.isCancelable) {
        code += ", { ";
        if (!eventInfo.isBubbling) {
            code += "capture: true";
        }
        if (!eventInfo.isCancelable && !eventInfo.isBubbling) {
            code += ", ";
        }
        if (!eventInfo.isCancelable) {
            code += "passive: true";
        }
        code += " }";
    }
    
    code += ");";
    
    return code;
}

std::string AllJSEventsSupport::generateEventDelegateCode(const std::string& eventName, const std::string& parentSelector, 
                                                          const std::string& childSelector, const std::string& handler) {
    // 生成事件委托代码，考虑事件冒泡特性
    
    EventInfo eventInfo = getEventInfo(eventName);
    
    std::string code = "// " + eventInfo.description + "事件委托\n";
    
    if (!eventInfo.isBubbling) {
        code += "// 警告：此事件不支持冒泡，委托可能无效\n";
        code += "console.warn('事件 " + eventName + " 不支持冒泡，委托可能无效');\n";
    }
    
    code += parentSelector + ".addEventListener('" + eventName + "', function(event) {\n";
    code += "    if (event.target.matches && event.target.matches('" + childSelector + "')) {\n";
    code += "        (" + handler + ").call(event.target, event);\n";
    code += "    }\n";
    code += "}, " + (eventInfo.isBubbling ? "false" : "true") + ");"; // 不冒泡的事件使用捕获
    
    return code;
}

std::string AllJSEventsSupport::generateCompatibilityCheck(const std::string& eventName, const std::string& elementType) {
    // 生成事件兼容性检查代码
    
    if (!isEventCompatibleWithElement(eventName, elementType)) {
        return "// 警告：事件 " + eventName + " 可能与元素 " + elementType + " 不兼容\n";
    }
    
    return "";
}

} // namespace chtljs
} // namespace chtl