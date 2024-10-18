#include "utils.h"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>


using namespace std;

void initialize_socket_library() {
    // 在Linux系统中不需要做任何事情，Windows系统中要使用WSAStartup,但是windows库又不一样，太麻烦了,所以我删掉了
}

int create_server_socket(int port) {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        print_socket_error("无法创建服务器套接字");
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        print_socket_error("绑定失败");
        close(server_socket);
        return -1;
    }

    if (listen(server_socket, 100) == -1) {
        print_socket_error("监听失败");
        close(server_socket);
        return -1;
    }

    return server_socket;
}

void print_socket_error(const string &message) {
    perror(message.c_str());
}

string get_client_ip(sockaddr_in &client_addr) {
    return string(inet_ntoa(client_addr.sin_addr));
}
