cmake_minimum_required(VERSION 3.7)
project(FDWebSocket)

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_FILES
        libfdws/FDHttp.c
        libfdws/FDHttp.h
        libfdws/FDSocket.c
        libfdws/FDSocket.h
        libfdws/FDWebSocket.c
        libfdws/FDWebSocket.h
        main.c)

add_executable(FDWebSocket ${SOURCE_FILES})