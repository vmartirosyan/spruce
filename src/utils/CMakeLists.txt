cmake_minimum_required(VERSION 2.4)

message("Configuring utils sources.")

add_library(utils STATIC test_base.cpp process.cpp)
