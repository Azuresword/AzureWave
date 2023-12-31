//
// Created by fsr on 23-12-29.
//

#include "usrpthread.h"
#include "Base64_encoder.h"
#include "camera.h"
#include "TCP_recv_send.h"
#include <iostream>
/**
 * @brief Encoder_and_transfer,传输函数类
 */

Encoder_and_transfer::Encoder_and_transfer(std::mutex& photo_Mutex) : running(false) {}

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
    //12600端口的数据
    int sock;
    struct sockaddr_in server;
    bool transferCompleted = false;
    //12700端口的数据
    const char* server_ip = "192.168.50.34";
    std::string message = "ok";
    int num_iterations = 1; // 这里假设循环5次
    while (!transferCompleted) {

        // 创建套接字
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            std::cerr << "Could not create socket" << std::endl;
            //return 1;
        }

        server.sin_addr.s_addr = inet_addr("192.168.50.34");
        server.sin_family = AF_INET;
        server.sin_port = htons(12600);

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
        send_and_receive(server_ip, 12700, message, num_iterations);
    }

}

/**
 * 摄像头功能类  USB_CAM
 */


USB_CAM::USB_CAM():running(false) {}

USB_CAM::~USB_CAM() {
    if (thread.joinable()) {
        thread.join();
    }
}

void USB_CAM::start() {
    running = true;
    thread = std::thread(&USB_CAM::run, this);
}

void USB_CAM::join() {
    if (thread.joinable()) {
        thread.join();
    }
}

bool USB_CAM::isRunning() const {
    return running;
}

void USB_CAM::run() {
    FILE *fp = NULL;
    int index = 0;
    int ret = 0;
    int i = 0;
    char buf[16] = {0};
    struct _v4l2_video video;
    fd_set fds;
    struct timeval tv;
    while (true) {
        video.fd = open_video_device("/dev/video0");
        if (video.fd < 0) {

        }

        ret = query_video_device_cap(&video);
        if (ret < 0) {
            goto __exit;
        }

        ret = set_video_device_par(&video);
        if (ret < 0) {
            goto __exit;
        }

        ret = set_video_device_mmap(&video);
        if (ret < 0) {
            goto __exit;
        }

        ret = set_video_device_stream_on(&video);
        if (ret < 0) {
            goto __exit;
        }

        for (i = 0; i < 5; i++) {/* 采集5张(帧)图片 */
            FD_ZERO(&fds);
            FD_SET(video.fd, &fds);

            tv.tv_sec = 5;    /* wait time */
            tv.tv_usec = 0;
            ret = select(video.fd + 1, &fds, NULL, NULL, &tv);

            if (ret < 0) {
                perror("select error");
                goto __exit;
            } else if (ret == 0) {
                printf("select timeout\n");
                goto __exit;
            }
            ret = read_video_device_stream_frame(&video, &index);
            if (ret < 0) {
                goto __exit;
            }
            sprintf(buf, "./image%d.jpg", i);
            fp = fopen(buf, "wb");    /* 保存为图片文件 */
            if (fp == NULL) {
                perror("open image file failed\n");
                goto __exit;
            }
            printf("save %s \n", buf);
            fwrite(video.mmap_buf[index].addr, video.mmap_buf[index].size, 1, fp);
            fclose(fp);
            set_video_device_stream_queue(&video, index);
            usleep(1000);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        __exit:
        set_video_device_stream_off(&video);
        close_video_device(&video);
    }
}

