cmake_minimum_required(VERSION 2.6)
project (Spruce)
message("Configuring EXT4 specific sources.")

#include (CheckIncludeFiles)

#message("Checking for libcapability.")
#check_include_files(sys/capability.h capability_exists)
#if ( NOT capability_exists EQUAL "1" )
	#message(FATAL_ERROR "The library libcapability is required for the Ext4FS checker subsystem.")
#endif()

#include_directories(${CMAKE_CURRENT_BINARY_DIR})

#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/ext4fs.cpp ${CMAKE_CURRENT_BINARY_DIR}/ext4fs.cpp)

#add_executable( ext4 ${CMAKE_CURRENT_SOURCE_DIR}/ext4fs.cpp ioctl_test.cpp ioctl_test.hpp ${CMAKE_SOURCE_DIR}/include/config.hpp)
#target_link_libraries(ext4 utils)

#install (	FILES "${CMAKE_BINARY_DIR}/bin/ext4"
#			DESTINATION bin
#			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

ADD_CUSTOM_COMMAND(
   OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/module.cpp
   COMMAND ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/engine
   DEPENDS ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_SOURCE_DIR}/engine/module.xslt ${CMAKE_SOURCE_DIR}/engine/testset.xslt ${CMAKE_CURRENT_SOURCE_DIR}/module.xml Ioctl.xml Mount.xml
   )

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_EXECUTABLE(ext4 ${CMAKE_CURRENT_BINARY_DIR}/module.cpp )
target_link_libraries(ext4 utils)
						
install (	FILES "${CMAKE_BINARY_DIR}/bin/ext4"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

