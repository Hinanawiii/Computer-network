#ifndef CHAT_UTILS_H
#define CHAT_UTILS_H

#include <string>
#include <netinet/in.h>

// 初始化Socket库的工具函数（用于Windows系统）
void initialize_socket_library();

// 创建并绑定一个服务器Socket
int create_server_socket(int port);

// 打印Socket错误信息
void print_socket_error(const std::string &message);

// 将IP地址转换为可读字符串
std::string get_client_ip(sockaddr_in &client_addr);

#endif // CHAT_UTILS_H