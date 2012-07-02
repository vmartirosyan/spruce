cmake_minimum_required(VERSION 2.4)

message("Configuring creat syscall checker module.")

project (Spruce)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Openat.cpp ${CMAKE_CURRENT_BINARY_DIR}/Openat.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#ADD_LIBRARY(creat STATIC Openat.cpp)

