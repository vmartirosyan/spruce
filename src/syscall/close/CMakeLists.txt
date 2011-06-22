cmake_minimum_required(VERSION 2.4)

message("Configuring dup syscall checker module.")

project (Spruce)

ADD_LIBRARY(close STATIC Close.cpp)

