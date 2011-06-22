cmake_minimum_required(VERSION 2.4)

message("Configuring tests.")

add_subdirectory (benchmark)
add_subdirectory (common)
add_subdirectory (fs)
add_subdirectory (main)
add_subdirectory (syscall)

project (Spruce)


