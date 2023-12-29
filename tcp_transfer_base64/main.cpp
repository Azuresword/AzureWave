#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i)
    {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];

        while((i++ < 3))
            ret += '=';
    }

    return ret;
}

int main() {
    int sock;
    struct sockaddr_in server;
    bool transferCompleted = false;

    while (!transferCompleted) {
        // 创建套接字
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            std::cerr << "Could not create socket" << std::endl;
            return 1;
        }

        server.sin_addr.s_addr = inet_addr("192.168.50.218");
        server.sin_family = AF_INET;
        server.sin_port = htons(8888);

        // 连接到服务器
        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
            std::cerr << "Connect failed. Error" << std::endl;
            close(sock);
            return 1;
        }

        std::cout << "Connected to server" << std::endl;

        // 读取和发送图像文件
        std::ifstream file("./image0.jpg", std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file." << std::endl;
            close(sock);
            return 1;
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

    return 0;
}
