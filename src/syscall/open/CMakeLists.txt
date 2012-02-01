cmake_minimum_required(VERSION 2.4)

message("Configuring dup syscall checker module.")

project (Spruce)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Open.cpp ${CMAKE_CURRENT_BINARY_DIR}/Open.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#ADD_LIBRARY(chmod STATIC Open.cpp)

