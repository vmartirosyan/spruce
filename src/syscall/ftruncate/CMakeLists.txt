cmake_minimum_required(VERSION 2.4)

message("Configuring ftruncate syscall checker.")

project (Spruce)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Ftruncate.cpp ${CMAKE_CURRENT_BINARY_DIR}/Ftruncate.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

