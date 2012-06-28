cmake_minimum_required(VERSION 2.4)

message("Configuring access syscall checker module.")

project (Spruce)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Access.cpp ${CMAKE_CURRENT_BINARY_DIR}/Access.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

