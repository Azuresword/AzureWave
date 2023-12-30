//
// Created by fsr on 23-12-29.
//

#ifndef TCP_USB_CAM_BASE64_ENCODER_H
#define TCP_USB_CAM_BASE64_ENCODER_H
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
#endif //TCP_USB_CAM_BASE64_ENCODER_H
