// protocol.h
// 定义聊天协议，包括消息类型和编码/解码方法

#ifndef CHAT_PROTOCOL_H
#define CHAT_PROTOCOL_H

#include <string>
#include <sstream>

using namespace std;

// 消息类型定义
enum MessageType {
    CONNECT = 0,        // 连接消息
    DISCONNECT = 1,     // 断开连接消息
    BROADCAST = 2,      // 群聊消息
    PRIVATE = 3,        // 私聊消息，但是实现起来有bug
    SERVER_NOTICE = 4   // 服务器通知
};

// 聊天消息的结构体定义
struct ChatMessage {
    MessageType type;         // 消息类型
    string sender;       // 发送者名称或ID
    string receiver;     // 接收者名称或ID
    string content;      // 消息内容

    // 将消息编码为字符串格式
    string encode() const {
        return to_string(type) + " | " + sender + " | " + receiver + " | " + content;
    }

    // 从字符串格式解码为消息
    static ChatMessage decode(const string &messageStr) {
        ChatMessage msg;
        stringstream ss(messageStr);
        string segment;

        // 解析消息类型
        getline(ss, segment, '|');
        msg.type = static_cast<MessageType>(stoi(segment));

        // 解析发送者名称
        getline(ss, msg.sender, '|');
        msg.sender = trim(msg.sender);

        // 解析接收者名称
        getline(ss, msg.receiver, '|');
        msg.receiver = trim(msg.receiver);

        // 解析消息内容
        getline(ss, msg.content, '|');
        msg.content = trim(msg.content);

        return msg;
    }

private:
    // 去除字符串前后的空格
    static string trim(const string &str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return (first == string::npos || last == string::npos) ? "" : str.substr(first, (last - first + 1));
    }
};

#endif // CHAT_PROTOCOL_H
