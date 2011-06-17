cmake_minimum_required(VERSION 2.4)
message("Configuring EXT4 specific sources.")

include (CheckIncludeFiles)

project (Spruce)

message("Checking for libcapability.")
check_include_files(sys/capability.h capability_exists)
if ( NOT capability_exists EQUAL "1" )
	message(FATAL_ERROR "The library libcapability is required for the Ext4FS checker subsystem.")
endif()

add_executable(ext4fs_plugin ext4fs.cpp ioctl_test.cpp ext4fs_test.hpp )
target_link_libraries(ext4fs_plugin utils)

install (	FILES "${CMAKE_BINARY_DIR}/bin/ext4fs_plugin"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
