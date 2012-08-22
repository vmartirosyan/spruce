cmake_minimum_required(VERSION 2.4)

project (Spruce)

message("Configuring project sources.")

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}/include")
link_directories("${CMAKE_BINARY_DIR}/src/utils")

#add_subdirectory (benchmark)
#add_subdirectory (common)
add_subdirectory (fs)
add_subdirectory (main)
add_subdirectory (syscall_new)
#add_subdirectory (syscall)
#add_subdirectory (utils)

