//
// Created by fsr on 23-12-29.
//

#include "usrpthread.h"
#include "Base64_encoder.h"
#include <iostream>
/**
 * @brief Encoder_and_transfer,传输函数类
 */

Encoder_and_transfer::Encoder_and_transfer() : running(false) {}

Encoder_and_transfer::~Encoder_and_transfer() {
    if (thread.joinable()) {
        thread.join();
    }
}

void Encoder_and_transfer::start() {
    running = true;
    thread = std::thread(&Encoder_and_transfer::run, this);
}

void Encoder_and_transfer::join() {
    if (thread.joinable()) {
        thread.join();
    }
}

bool Encoder_and_transfer::isRunning() const {
    return running;
}

void Encoder_and_transfer::run() {
    int sock;
    struct sockaddr_in server;
    bool transferCompleted = false;

    while (!transferCompleted) {
        // 创建套接字
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            std::cerr << "Could not create socket" << std::endl;
            //return 1;
        }

        server.sin_addr.s_addr = inet_addr("192.168.50.218");
        server.sin_family = AF_INET;
        server.sin_port = htons(8888);

        // 连接到服务器
        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            std::cerr << "Connect failed. Error" << std::endl;
            close(sock);
            //return 1;
        }

        std::cout << "Connected to server" << std::endl;

        // 读取和发送图像文件
        std::ifstream file("./image0.jpg", std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file." << std::endl;
            close(sock);
            //return 1;
        }

        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(file), {});
        std::string encoded = base64_encode(&buffer[0], buffer.size());

        send(sock, encoded.c_str(), encoded.length(), 0);
        send(sock, "END", 3, 0);

        // 接收来自服务器的响应
        char response[1024] = {0};
        std::stringstream ss;
        int bytesReceived = recv(sock, response, 1024, 0);
        if (bytesReceived <= 0) {
            std::cerr << "Receive failed or connection closed" << std::endl;
            close(sock);
            break;
        }

        ss << response;
        if (ss.str().find("START") == std::string::npos) {
            transferCompleted = true;
        }

        close(sock); // 关闭当前套接字
    }

}

