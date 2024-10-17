// client.cpp
// 聊天程序客户端实现，支持连接服务器、发送和接收消息

#include <iostream>
#include <string>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"

#define SERVER_IP "127.0.0.1"
#define PORT 12345

// 接收消息的函数
void receive_messages(int client_socket) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            std::cout << "服务器断开连接" << std::endl;
            close(client_socket);
            break;
        }
        buffer[bytes_received] = '\0';
        ChatMessage msg = ChatMessage::decode(buffer);
        std::cout << msg.sender << ": " << msg.content << std::endl;
    }
}

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        std::cerr << "无法创建客户端套接字" << std::endl;
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(client_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "无法连接到服务器" << std::endl;
        return -1;
    }

    std::cout << "已成功连接到服务器" << std::endl;

    // 启动接收消息的线程
    std::thread(receive_messages, client_socket).detach();

    // 发送消息
    std::string sender;
    std::cout << "请输入您的用户名: ";
    std::getline(std::cin, sender);

    while (true) {
        std::string content;
        std::getline(std::cin, content);
        if (content == "/exit") {
            ChatMessage msg;
            msg.type = DISCONNECT;
            msg.sender = sender;
            msg.receiver = "server";
            msg.content = "Bye!";
            send(client_socket, msg.encode().c_str(), msg.encode().length(), 0);
            break;
        }

        ChatMessage msg;
        msg.type = BROADCAST;
        msg.sender = sender;
        msg.receiver = "ALL";
        msg.content = content;
        send(client_socket, msg.encode().c_str(), msg.encode().length(), 0);
    }

    close(client_socket);
    return 0;
}