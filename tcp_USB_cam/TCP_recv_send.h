//
// Created by fsr on 23-12-31.
//

#ifndef TCP_USB_CAM_TCP_RECV_SEND_H
#define TCP_USB_CAM_TCP_RECV_SEND_H
#include<iostream>
void send_and_receive(const char* server_ip, int port, const std::string& message, int num_iterations);
#endif //TCP_USB_CAM_TCP_RECV_SEND_H
