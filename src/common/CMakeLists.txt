cmake_minimum_required(VERSION 2.4)

message("Configuring project common operations sources.")

project (Spruce)

ADD_EXECUTABLE(common main.cpp)
TARGET_LINK_LIBRARIES(common utils)
