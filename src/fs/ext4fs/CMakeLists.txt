cmake_minimum_required(VERSION 2.4)

message("Configuring EXT4 specific sources.")

project (Spruce)

include_directories("${CMAKE_SOURCE_DIR}/include")

add_executable(ext4fs_plugin ext4fs.cpp test_base.cpp ioctl_test.cpp )

install (	FILES "${CMAKE_BINARY_DIR}/bin/ext4fs_plugin"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
