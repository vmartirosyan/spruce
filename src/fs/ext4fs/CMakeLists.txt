cmake_minimum_required(VERSION 2.6)
project (Spruce)
message("Configuring EXT4 specific sources.")

#include (CheckIncludeFiles)

#message("Checking for libcapability.")
#check_include_files(sys/capability.h capability_exists)
#if ( NOT capability_exists EQUAL "1" )
	#message(FATAL_ERROR "The library libcapability is required for the Ext4FS checker subsystem.")
#endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ext4fs.cpp ${CMAKE_CURRENT_BINARY_DIR}/ext4fs.cpp)

add_executable( ext4fs ${CMAKE_CURRENT_BINARY_DIR}/ext4fs.cpp ioctl_test.cpp ioctl_test.hpp ${CMAKE_BINARY_DIR}/include/config.hpp)
target_link_libraries(ext4fs utils)

install (	FILES "${CMAKE_BINARY_DIR}/bin/ext4fs"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
