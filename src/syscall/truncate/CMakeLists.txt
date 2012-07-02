cmake_minimum_required(VERSION 2.4)

message("Configuring truncate syscall checker module.")

project (Spruce)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/Truncate.cpp ${CMAKE_CURRENT_BINARY_DIR}/Truncate.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

