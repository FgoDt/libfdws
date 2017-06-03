cmake_minimum_required(VERSION 3.5)
project(FDWebSocket)

set(CMAKE_CXX_STANDARD 11)

set(FDWS_DIR ./libfdws)

include_directories(${FDWS_DIR})

add_subdirectory(${FDWS_DIR})

set(SOURCE_FILES
        main.c)

add_executable(FDWebSocket ${SOURCE_FILES})

target_link_libraries(FDWebSocket fdws)
