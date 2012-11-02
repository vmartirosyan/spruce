cmake_minimum_required(VERSION 2.6)
project (Spruce)
message("Configuring JFS specific sources.")

#include_directories(${CMAKE_CURRENT_BINARY_DIR})

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/jfs.cpp ${CMAKE_CURRENT_BINARY_DIR}/jfs.cpp)

add_executable( jfs ${CMAKE_CURRENT_SOURCE_DIR}/jfs.cpp jfs_ioctl_test.cpp jfs_ioctl_test.hpp ${CMAKE_SOURCE_DIR}/include/config.hpp)
target_link_libraries(jfs utils)

install (	FILES "${CMAKE_BINARY_DIR}/bin/jfs"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
