cmake_minimum_required(VERSION 3.21)
project(Ping C)

set(CMAKE_C_STANDARD 99)
link_libraries(ws2_32 wsock32)

add_executable(Ping Source/main.c Headers/settings.h Source/logs.c Headers/logs.h Source/utils.c Headers/utils.h Source/network.c Headers/network.h Source/Playground.c)
