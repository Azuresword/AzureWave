cmake_minimum_required(VERSION 3.24)
project(tcp_USB_cam)

set(CMAKE_CXX_STANDARD 17)

add_executable(tcp_USB_cam main.cpp usrpthread.cpp usrpthread.h Base64_encoder.cpp Base64_encoder.h camera.cpp camera.h)
find_package(Threads)

target_link_libraries(tcp_USB_cam ${CMAKE_THREAD_LIBS_INIT})