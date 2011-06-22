cmake_minimum_required(VERSION 2.8)
include (ExternalProject)

project (Spruce)

# Let's install Spruce in /opt/spruce
SET( CMAKE_INSTALL_PREFIX "/opt/spruce/")

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
 
message(" -- Copying source files to build folder")
execute_process(COMMAND cp -R ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}) 

# Changing the source directory to point to the binary directory
SET(CMAKE_SOURCE_DIRECTORY, ${CMAKE_BINARY_DIRECTORY})

add_subdirectory (bin)
add_subdirectory (include)
add_subdirectory (lib)
add_subdirectory (src)
add_subdirectory (tests)
add_subdirectory (doc)

# Uninstall the system
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
