cmake_minimum_required(VERSION 2.4)

message("Configuring project includes.")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/platform_config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/platform_config.hpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
include_directories(${CMAKE_CURRENT_BINARY_DIR} )

project (Spruce)


