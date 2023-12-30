#include "usrpthread.h"
#include <iostream>

int main() {

    USB_CAM USB_CAM;
    Encoder_and_transfer encoder(USB_CAM.getMutex());
    USB_CAM.start();


    encoder.start();
    if(encoder.isRunning()){std::cout << " encoder thread is running" << std::endl;}
    if(USB_CAM.isRunning()) {std::cout << "USB_CAM thread is running" << std::endl;

    }
    encoder.join();
    USB_CAM.join();
    return 0;
}
