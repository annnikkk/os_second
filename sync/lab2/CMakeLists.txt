cmake_minimum_required(VERSION 3.22)
project(lab2 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab2 queue-threads.c
        queue.c
        queue.h)
