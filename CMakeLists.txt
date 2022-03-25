cmake_minimum_required(VERSION 3.21)
project(Ping C)

set(CMAKE_C_STANDARD 99)

add_executable(Ping Source/main.c Source/console.c Headers/console.h)
