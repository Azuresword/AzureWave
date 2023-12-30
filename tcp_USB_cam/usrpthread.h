//
// Created by dylan on 2023/12/29.
//

#ifndef THREAD_TEST_THREAD_H
#define THREAD_TEST_THREAD_H

#include <thread>


/**
 * @brief Encoder_and_transfer,传输函数类
 */

class Encoder_and_transfer {
public:
    Encoder_and_transfer();
    ~Encoder_and_transfer();

    void start();
    void join();
    bool isRunning() const;

private:
    void run();
    std::thread thread;
    bool running;
};


/**
 * 摄像头功能类  USB_CAM
 */
class USB_CAM{
public:
    USB_CAM();
    ~USB_CAM();

    void start();
    void join();
    bool isRunning() const;

private:
    void run();
    std::thread thread;
    bool running;
};



#endif //THREAD_TEST_THREAD_H