cmake_minimum_required(VERSION 2.8)
include (ExternalProject)

project (Spruce)

# By default install Spruce to /opt/spruce
IF (NOT CMAKE_INSTALL_PREFIX)
	SET( CMAKE_INSTALL_PREFIX "/opt/spruce/")
ENDIF (NOT CMAKE_INSTALL_PREFIX)

IF (NOT CMAKE_CXX_FLAGS)
	SET( CMAKE_CXX_FLAGS "-Wall -pedantic -Wno-variadic-macros")
ENDIF (NOT CMAKE_CXX_FLAGS)

IF (NOT TEST_TIMEOUT)
	SET ( TEST_TIMEOUT 2 )
ENDIF (NOT TEST_TIMEOUT)

IF (NOT FIFO_PATH)
	SET ( FIFO_PATH "/tmp/spruce_fifo" )
ENDIF (NOT FIFO_PATH)

SET (LIBRARY_OUTPUT_PATH
 ${PROJECT_BINARY_DIR}/lib
 CACHE PATH
 "Directory for all Libraries"
 )
 
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
 ${PROJECT_BINARY_DIR}/bin
 CACHE PATH
 "Directory for all Binaries"
 )
 
message("-- Copying source files to build folder")
execute_process(COMMAND mkdir ${CMAKE_BINARY_DIR}/include/ -p )
#execute_process(COMMAND bash -c "cp -r ${CMAKE_SOURCE_DIR}/include/*.hpp ${CMAKE_BINARY_DIR}/include/ ") 

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}/include")

# Changing the source directory to point to the binary directory
#SET(CMAKE_SOURCE_DIR, ${CMAKE_BINARY_DIR})

add_subdirectory (src/utils)
add_subdirectory (engine)
add_subdirectory (bin)
add_subdirectory (include)
add_subdirectory (lib)
add_subdirectory (src)
add_subdirectory (tests)
add_subdirectory (config)
add_subdirectory (doc)


# Uninstall the system
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    

