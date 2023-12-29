cmake_minimum_required(VERSION 3.23)
project(upgradeOHOS)

set(CMAKE_CXX_STANDARD 14)

add_executable(upgradeOHOS main.cpp)
target_link_libraries(upgradeOHOS ws2_32)
