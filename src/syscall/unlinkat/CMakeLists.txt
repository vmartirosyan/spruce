cmake_minimum_required(VERSION 2.4)

message("Configuring unlinkat syscall checker module.")

project (Spruce)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(unlinkat STATIC UnlinkAT.cpp)
