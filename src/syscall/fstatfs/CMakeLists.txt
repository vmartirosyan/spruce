cmake_minimum_required(VERSION 2.4)

message("Configuring fstatfs syscall checker module.")

project (Spruce)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Fstatfs.cpp ${CMAKE_CURRENT_BINARY_DIR}/Fstatfs.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

