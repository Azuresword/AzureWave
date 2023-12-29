#include <winsock2.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

// 检查字符是否属于Base64编码字符集
static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

// 清理Base64字符串，移除非Base64字符
std::string cleanBase64String(const std::string& input) {
    std::string output;
    output.reserve(input.size());

    std::copy_if(input.begin(), input.end(), std::back_inserter(output), is_base64);

    return output;
}

// Base64解码函数
std::string base64_decode(std::string const& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4], char_array_3[3];
    std::string ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
    }

    return ret;
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    char buffer[1024] = {0};
    int clientSize = sizeof(client);

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        std::cerr << "Winsock initialization failed. Error code: " << WSAGetLastError() << std::endl;
        return 1;
    }

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation failed. Error code: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("192.168.50.218"); // Specific IP address
    server.sin_port = htons(8888);

    if (bind(serverSocket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Bind failed. Error code: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    listen(serverSocket, 3);
    std::cout << "Waiting for incoming connections..." << std::endl;

    while (true) {  // Main loop to accept new connections
        clientSocket = accept(serverSocket, (struct sockaddr *)&client, &clientSize);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accepting connection failed. Error code: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "Connection established!" << std::endl;
        std::stringstream ss;

        while (true) {  // Loop for handling multiple data transfers within the same connection
            memset(buffer, 0, 1024);
            int bytesReceived = recv(clientSocket, buffer, 1024, 0);
            if (bytesReceived == SOCKET_ERROR || bytesReceived == 0) {
                std::cerr << "Client disconnected" << std::endl;
                break;
            }

            ss << buffer;
            if (ss.str().find("END") != std::string::npos) {
                std::string encoded_str = ss.str();
                size_t end_pos = encoded_str.find("END");
                encoded_str = encoded_str.substr(0, end_pos);

                std::string clean_encoded_str = cleanBase64String(encoded_str);
                std::string decoded_data = base64_decode(clean_encoded_str);

                std::ofstream output_file("output.jpg", std::ios::binary);
                if (output_file.is_open()) {
                    output_file.write(decoded_data.c_str(), decoded_data.size());
                    output_file.close();
                    std::cout << "Decoded image written to output.jpg" << std::endl;

                    send(clientSocket, "START", 5, 0);
                    ss.clear();
                    ss.str("");
                } else {
                    std::cerr << "Failed to open output file." << std::endl;
                }
            }
        }

        closesocket(clientSocket);  // Close the client socket
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}