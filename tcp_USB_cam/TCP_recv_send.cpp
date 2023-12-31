//
// Created by fsr on 23-12-31.
//

#include "TCP_recv_send.h"
#include "usrpthread.h"
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

void send_and_receive(const char* server_ip, int port, const std::string& message, int num_iterations) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // 创建套接字
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    // 转换地址
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address / Address not supported" << std::endl;
        return;
    }

    // 连接到服务器
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection Failed" << std::endl;
        return;
    }

    for (int i = 0; i < num_iterations; ++i) {
        // 发送数据
        send(sock, message.c_str(), message.length(), 0);
        std::cout << "Message sent: " << message << std::endl;

        // 清空buffer
        memset(buffer, 0, sizeof(buffer));

        // 接收服务器回复的数据
        valread = recv(sock, buffer, 1024, 0);
        std::cout << "Server reply: " << buffer << std::endl;

        // 可以在此处添加延迟，避免过于频繁的发送
        usleep(1000); // 睡眠1秒
    }

    // 关闭套接字
    close(sock);
}