// ========================================
// 实时聊天系统 - CJMOD示例
// 展示：高自由度 + 简单易用 + 任意扩展
// ========================================

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <thread>
#include <chrono>
#include <random>
#include <regex>

// 自由包含任意库 - 网络、加密、JSON等
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <nlohmann/json.hpp>
#include <openssl/sha.h>

// ========================================
// 全局状态管理 - 维护聊天系统状态
// ========================================

struct ChatUser {
    std::string id;
    std::string nickname;
    std::string avatar;
    bool isOnline;
    long lastActivity;
    
    ChatUser() : isOnline(false), lastActivity(0) {}
};

struct ChatMessage {
    std::string id;
    std::string userId;
    std::string content;
    std::string type;  // text, image, emoji, system
    long timestamp;
    bool isEncrypted;
    
    ChatMessage() : timestamp(0), isEncrypted(false) {}
};

struct ChatRoom {
    std::string id;
    std::string name;
    std::vector<std::string> userIds;
    std::vector<ChatMessage> messages;
    bool isPrivate;
    std::string password;
    
    ChatRoom() : isPrivate(false) {}
};

// 全局聊天状态
std::unordered_map<std::string, ChatUser> connectedUsers;
std::unordered_map<std::string, ChatRoom> chatRooms;
std::queue<ChatMessage> messageQueue;
std::mutex chatMutex;
bool serverRunning = false;
int totalMessages = 0;
int onlineUserCount = 0;

// ========================================
// 强大的聊天功能实现
// ========================================

// 用户连接管理
std::string connectUser(const std::string& params) {
    std::lock_guard<std::mutex> lock(chatMutex);
    
    // 解析用户信息（简化版JSON解析）
    std::string userId = "user_" + std::to_string(connectedUsers.size() + 1);
    std::string nickname = "访客" + std::to_string(connectedUsers.size() + 1);
    
    ChatUser user;
    user.id = userId;
    user.nickname = nickname;
    user.avatar = "default_avatar.png";
    user.isOnline = true;
    user.lastActivity = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    connectedUsers[userId] = user;
    onlineUserCount++;
    
    // 广播用户上线消息
    ChatMessage systemMsg;
    systemMsg.id = "msg_" + std::to_string(totalMessages++);
    systemMsg.userId = "system";
    systemMsg.content = nickname + " 加入了聊天";
    systemMsg.type = "system";
    systemMsg.timestamp = user.lastActivity;
    
    messageQueue.push(systemMsg);
    
    return "{\"userId\":\"" + userId + "\",\"nickname\":\"" + nickname + "\",\"status\":\"connected\"}";
}

// 发送消息 - 支持加密、表情、图片
std::string sendMessage(const std::string& params) {
    std::lock_guard<std::mutex> lock(chatMutex);
    
    // 简化的参数解析
    std::string userId = "user_1";  // 实际应从params解析
    std::string content = params;
    std::string roomId = "general";
    
    if (connectedUsers.find(userId) == connectedUsers.end()) {
        return "{\"error\":\"用户未连接\"}";
    }
    
    // 创建消息
    ChatMessage msg;
    msg.id = "msg_" + std::to_string(totalMessages++);
    msg.userId = userId;
    msg.content = content;
    msg.type = "text";
    msg.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // 智能消息处理
    if (content.find("http") != std::string::npos) {
        msg.type = "link";
    } else if (content.find(":") != std::string::npos && content.find(":") < 10) {
        msg.type = "emoji";
    }
    
    // 添加到房间
    if (chatRooms.find(roomId) == chatRooms.end()) {
        ChatRoom room;
        room.id = roomId;
        room.name = "通用聊天室";
        chatRooms[roomId] = room;
    }
    
    chatRooms[roomId].messages.push_back(msg);
    messageQueue.push(msg);
    
    // 更新用户活动时间
    connectedUsers[userId].lastActivity = msg.timestamp;
    
    return "{\"messageId\":\"" + msg.id + "\",\"status\":\"sent\",\"timestamp\":" + std::to_string(msg.timestamp) + "}";
}

// 获取聊天历史
std::string getChatHistory(const std::string& params) {
    std::lock_guard<std::mutex> lock(chatMutex);
    
    std::string roomId = params.empty() ? "general" : params;
    
    if (chatRooms.find(roomId) == chatRooms.end()) {
        return "{\"messages\":[]}";
    }
    
    std::string history = "{\"messages\":[";
    const auto& messages = chatRooms[roomId].messages;
    
    // 获取最近50条消息
    int startIdx = std::max(0, (int)messages.size() - 50);
    for (int i = startIdx; i < messages.size(); ++i) {
        const auto& msg = messages[i];
        
        history += "{";
        history += "\"id\":\"" + msg.id + "\",";
        history += "\"userId\":\"" + msg.userId + "\",";
        history += "\"content\":\"" + msg.content + "\",";
        history += "\"type\":\"" + msg.type + "\",";
        history += "\"timestamp\":" + std::to_string(msg.timestamp);
        history += "}";
        
        if (i < messages.size() - 1) history += ",";
    }
    
    history += "]}";
    return history;
}

// 在线用户列表
std::string getOnlineUsers(const std::string& params) {
    std::lock_guard<std::mutex> lock(chatMutex);
    
    std::string userList = "{\"users\":[";
    bool first = true;
    
    for (const auto& pair : connectedUsers) {
        const auto& user = pair.second;
        if (user.isOnline) {
            if (!first) userList += ",";
            
            userList += "{";
            userList += "\"id\":\"" + user.id + "\",";
            userList += "\"nickname\":\"" + user.nickname + "\",";
            userList += "\"avatar\":\"" + user.avatar + "\",";
            userList += "\"lastActivity\":" + std::to_string(user.lastActivity);
            userList += "}";
            
            first = false;
        }
    }
    
    userList += "],\"count\":" + std::to_string(onlineUserCount) + "}";
    return userList;
}

// 创建私人聊天室
std::string createPrivateRoom(const std::string& params) {
    std::lock_guard<std::mutex> lock(chatMutex);
    
    std::string roomId = "private_" + std::to_string(chatRooms.size() + 1);
    
    ChatRoom room;
    room.id = roomId;
    room.name = "私人聊天室";
    room.isPrivate = true;
    room.password = "1234"; // 简化的密码
    
    chatRooms[roomId] = room;
    
    return "{\"roomId\":\"" + roomId + "\",\"status\":\"created\"}";
}

// 智能消息过滤 - 反垃圾、敏感词检测
std::string filterMessage(const std::string& params) {
    std::string content = params;
    
    // 敏感词列表
    std::vector<std::string> bannedWords = {"垃圾", "广告", "spam"};
    
    for (const auto& word : bannedWords) {
        size_t pos = content.find(word);
        while (pos != std::string::npos) {
            content.replace(pos, word.length(), "***");
            pos = content.find(word, pos + 3);
        }
    }
    
    // 链接检测和安全验证
    std::regex urlRegex(R"(https?://[^\s]+)");
    content = std::regex_replace(content, urlRegex, "[链接已过滤]");
    
    return "{\"filtered\":\"" + content + "\",\"original\":\"" + params + "\"}";
}

// 消息加密功能
std::string encryptMessage(const std::string& params) {
    // 简化的加密实现（实际应使用更安全的加密）
    std::string encrypted = "";
    for (char c : params) {
        encrypted += std::to_string((int)c + 3) + ",";
    }
    
    return "{\"encrypted\":\"" + encrypted + "\",\"algorithm\":\"simple_shift\"}";
}

// 消息解密功能
std::string decryptMessage(const std::string& params) {
    // 对应的解密实现
    std::string decrypted = "";
    std::stringstream ss(params);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        if (!token.empty()) {
            int charCode = std::stoi(token) - 3;
            decrypted += (char)charCode;
        }
    }
    
    return "{\"decrypted\":\"" + decrypted + "\"}";
}

// 聊天机器人 - AI回复
std::string chatBot(const std::string& params) {
    std::vector<std::string> responses = {
        "你好！有什么可以帮助你的吗？",
        "这是一个很有趣的话题！",
        "让我想想...",
        "你说得对！",
        "还有其他问题吗？"
    };
    
    // 简单的关键词回复
    std::string input = params;
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    
    std::string response;
    if (input.find("hello") != std::string::npos || input.find("你好") != std::string::npos) {
        response = "你好！欢迎使用CJMOD聊天系统！";
    } else if (input.find("time") != std::string::npos || input.find("时间") != std::string::npos) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        response = "当前时间：" + std::string(std::ctime(&time_t));
    } else {
        // 随机回复
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, responses.size() - 1);
        response = responses[dis(gen)];
    }
    
    return "{\"botReply\":\"" + response + "\",\"confidence\":0.85}";
}

// 获取聊天统计信息
std::string getChatStats(const std::string& params) {
    std::lock_guard<std::mutex> lock(chatMutex);
    
    int totalRooms = chatRooms.size();
    int totalUsers = connectedUsers.size();
    
    std::string stats = "{";
    stats += "\"totalMessages\":" + std::to_string(totalMessages) + ",";
    stats += "\"onlineUsers\":" + std::to_string(onlineUserCount) + ",";
    stats += "\"totalUsers\":" + std::to_string(totalUsers) + ",";
    stats += "\"chatRooms\":" + std::to_string(totalRooms) + ",";
    stats += "\"serverUptime\":" + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count());
    stats += "}";
    
    return stats;
}