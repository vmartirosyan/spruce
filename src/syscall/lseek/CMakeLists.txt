cmake_minimum_required(VERSION 2.4)

message("Configuring lseek syscall checker module.")

project (Spruce)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(lseek SHARED LseekTest.cpp)

install (	FILES "${CMAKE_BINARY_DIR}/bin/syscall"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
