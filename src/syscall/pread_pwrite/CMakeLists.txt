cmake_minimum_required(VERSION 2.4)

message("Configuring pread_pwrite syscall checker module.")

project (Spruce)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#ADD_LIBRARY(pread_pwrite STATIC PreadPwrite.cpp)
