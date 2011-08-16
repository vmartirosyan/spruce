cmake_minimum_required(VERSION 2.4)

message("Configuring ioctl syscall checker module.")

project (Spruce)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(ioctl STATIC IoctlTest.cpp)

