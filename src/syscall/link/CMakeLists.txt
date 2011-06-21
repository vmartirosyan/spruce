cmake_minimum_required(VERSION 2.4)

message("Configuring dup syscall checker module.")

project (Spruce)

ADD_LIBRARY(link STATIC Link.cpp)

