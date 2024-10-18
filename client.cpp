// client.cpp
// 聊天程序客户端实现，支持连接服务器、发送和接收消息，并添加私聊功能

#include <iostream>
#include <string>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"

#define SERVER_IP "127.0.0.1"
#define PORT 12345

using namespace std;

// 接收消息的函数
void receive_messages(int client_socket) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            cout << "服务器断开连接" << endl;
            close(client_socket);
            break;
        }
        buffer[bytes_received] = '\0';
        ChatMessage msg = ChatMessage::decode(buffer);
        cout << msg.sender << ": " << msg.content << endl;
    }
}

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        cerr << "无法创建客户端套接字" << endl;
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        cerr << "无法连接到服务器" << endl;
        return -1;
    }

    cout << "已成功连接到服务器" << endl;

    // 输入用户名并注册到服务器
    string sender;
    cout << "请输入您的用户名: ";
    getline(cin, sender);

    // 发送用户名注册消息到服务器
    ChatMessage register_msg;
    register_msg.type = CONNECT;
    register_msg.sender = sender;
    register_msg.receiver = "server";
    register_msg.content = "注册用户名";
    send(client_socket, register_msg.encode().c_str(), register_msg.encode().length(), 0);

    // 启动接收消息的线程
    thread(receive_messages, client_socket).detach();

    // 发送消息
    while (true) {
        string content;
        getline(cin, content);
        if (content == "exit") {
            ChatMessage msg;
            msg.type = DISCONNECT;
            msg.sender = sender;
            msg.receiver = "server";
            msg.content = "Bye!";
            send(client_socket, msg.encode().c_str(), msg.encode().length(), 0);
            break;
        }

        ChatMessage msg;
        if (content.rfind("/msg ", 0) == 0) {
            // 私聊消息，格式为 /msg <接收者> <消息>（都带空格）
            size_t first_space = content.find(' ', 5);
            if (first_space != string::npos) {
                msg.type = PRIVATE;
                msg.sender = sender;
                msg.receiver = content.substr(5, first_space - 5);
                msg.content = content.substr(first_space + 1);
            } else {
                cout << "私聊消息格式错误，应为: /msg <接收者> <消息>" << endl;
                continue;
            }
        } else {
            // 群聊消息
            msg.type = BROADCAST;
            msg.sender = sender;
            msg.receiver = "ALL";
            msg.content = content;
        }
        send(client_socket, msg.encode().c_str(), msg.encode().length(), 0);
    }

    close(client_socket);
    return 0;
}