cmake_minimum_required(VERSION 2.6)

message("Configuring read_write syscall checker module.")

project (Spruce)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(read_write STATIC ReadWriteFile.cpp)
