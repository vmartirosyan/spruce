cmake_minimum_required(VERSION 2.4)

message("Configuring dup syscall checker module.")

project (Spruce)

ADD_LIBRARY(chmod STATIC ${CMAKE_CURRENT_BINARY_DIR}/Chmod.cpp)

