cmake_minimum_required(VERSION 2.4)

message("Configuring dup syscall checker module.")

project (Spruce)

ADD_LIBRARY(link STATIC ${CMAKE_CURRENT_BINARY_DIR}/Link.cpp)

