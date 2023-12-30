#include "usrpthread.h"
#include <iostream>

int main() {
    Encoder_and_transfer tcp_transfer_decoded_photo;
    tcp_transfer_decoded_photo.start();
    tcp_transfer_decoded_photo.join();

    std::cout << "线程已完成运行。" << std::endl;
    return 0;
}
