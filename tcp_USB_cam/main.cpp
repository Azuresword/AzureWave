#include "usrpthread.h"
#include <iostream>

int main() {
    Encoder_and_transfer tcp_transfer_decoded_photo;
    USB_CAM USB_CAM;
    USB_CAM.start();
    USB_CAM.join();
    std::cout << "USB_CAM thread is running" << std::endl;
    tcp_transfer_decoded_photo.start();
    tcp_transfer_decoded_photo.join();
    std::cout << " tcp_transfer_decoded_photo thread is running" << std::endl;

    return 0;
}
