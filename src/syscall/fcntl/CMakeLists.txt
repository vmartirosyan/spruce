cmake_minimum_required(VERSION 2.4)

message("Configuring dup syscall checker module.")

project (Spruce)

ADD_LIBRARY(fcntl SHARED fcntlFile.cpp)

install (	FILES "${CMAKE_BINARY_DIR}/bin/syscall"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
