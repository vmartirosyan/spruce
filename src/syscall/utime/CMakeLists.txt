cmake_minimum_required(VERSION 2.4)

message("Configuring utime syscall checker module.")

project (Spruce)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Utime.cpp ${CMAKE_CURRENT_BINARY_DIR}/Utime.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

INCLUDE (CheckIncludeFiles)

#ADD_LIBRARY(chmod STATIC Open.cpp)


