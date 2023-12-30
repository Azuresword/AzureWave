//
// Created by dylan on 2023/12/29.
//

#ifndef THREAD_TEST_THREAD_H
#define THREAD_TEST_THREAD_H

#include <thread>
#include <linux/videodev2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <malloc.h>
#include <math.h>
#include <errno.h>
#include <assert.h>
#include <mutex>
/**
 * @brief Encoder_and_transfer,传输函数类
 */

class Encoder_and_transfer {
public:
    Encoder_and_transfer(std::mutex& photo_mutex);
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

    std::mutex& getMutex(){
        return photo_mutex;
    }

private:
    std::mutex photo_mutex;
    void run();
    std::thread thread;
    bool running;
};



#endif //THREAD_TEST_THREAD_H