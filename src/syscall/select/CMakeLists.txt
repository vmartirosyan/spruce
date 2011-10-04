cmake_minimum_required(VERSION 2.4)

message("Configuring select syscall checker module.")

project (Spruce)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(select STATIC Select.cpp)

