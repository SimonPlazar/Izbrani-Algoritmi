cmake_minimum_required(VERSION 3.26)
project(IA_N2)

set(CMAKE_CXX_STANDARD 23)

add_executable(IA_N2 main.cpp
        bigint.h
        bigint.cpp
        Miller-Rabin.cpp
        Miller-Rabin.h
)
