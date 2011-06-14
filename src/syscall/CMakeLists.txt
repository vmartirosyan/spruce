cmake_minimum_required(VERSION 2.4)

message("Configuring syscall checker module.")

project (Spruce)

add_executable(syscall main.cpp DupFileDescriptor.cpp) 
target_link_libraries(syscall utils)

install (	FILES "${CMAKE_BINARY_DIR}/bin/syscall"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
