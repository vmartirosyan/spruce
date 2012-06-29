cmake_minimum_required(VERSION 2.4)

message("Configuring fstat syscall checker module.")

project (Spruce)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Fstat.cpp ${CMAKE_CURRENT_BINARY_DIR}/Fstat.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

