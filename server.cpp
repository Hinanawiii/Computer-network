// server.cpp
// 聊天程序服务器端实现，支持多人连接和群聊功能，并添加私聊功能

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "protocol.h"

#define PORT 12345
#define MAX_CLIENTS 100

std::map<int, std::string> clients; // 客户端套接字到用户名的映射
std::mutex clients_mutex;

// 处理客户端消息的函数
void handle_client(int client_socket) {
    char buffer[1024];
    while (true) {
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            // 客户端断开连接
            clients_mutex.lock();
            std::cout << clients[client_socket] << " 已断开连接" << std::endl;
            clients.erase(client_socket);
            clients_mutex.unlock();
            close(client_socket);
            break;
        }

        // 解码消息
        buffer[bytes_received] = '\0';
        ChatMessage msg = ChatMessage::decode(buffer);

        // 处理不同类型的消息
        clients_mutex.lock();
        if (msg.type == CONNECT) {
            // 注册用户名
            clients[client_socket] = msg.sender;
            std::cout << "新客户端注册: " << msg.sender << std::endl;
        } else if (msg.type == BROADCAST) {
            // 群聊消息，广播给所有客户端
            for (const auto &client : clients) {
                if (client.first != client_socket) {
                    send(client.first, buffer, bytes_received, 0);
                }
            }
        } else if (msg.type == PRIVATE) {
            // 私聊消息，发送给特定接收者
            bool recipient_found = false;
            for (const auto &client : clients) {
                if (client.second == msg.receiver) {
                    send(client.first, buffer, bytes_received, 0);
                    recipient_found = true;
                    break;
                }
            }
            if (!recipient_found) {
                // 如果找不到接收者，向发送者返回错误消息
                ChatMessage error_msg;
                error_msg.type = SERVER_NOTICE;
                error_msg.sender = "server";
                error_msg.receiver = msg.sender;
                error_msg.content = "接收者不存在: " + msg.receiver;
                std::string encoded_msg = error_msg.encode();
                send(client_socket, encoded_msg.c_str(), encoded_msg.length(), 0);
            }
        }
        clients_mutex.unlock();
    }
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        std::cerr << "无法创建服务器套接字" << std::endl;
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "绑定失败" << std::endl;
        return -1;
    }

    if (listen(server_socket, MAX_CLIENTS) == -1) {
        std::cerr << "监听失败" << std::endl;
        return -1;
    }

    std::cout << "服务器正在监听端口 " << PORT << " ..." << std::endl;

    while (true) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            std::cerr << "无法接受客户端连接" << std::endl;
            continue;
        }

        // 先将客户端套接字加入到映射中，稍后由 CONNECT 消息注册用户名
        clients_mutex.lock();
        clients[client_socket] = "未知用户";
        clients_mutex.unlock();

        std::cout << "新客户端连接: " << inet_ntoa(client_addr.sin_addr) << std::endl;

        // 创建新线程处理客户端
        std::thread(handle_client, client_socket).detach();
    }

    close(server_socket);
    return 0;
}
