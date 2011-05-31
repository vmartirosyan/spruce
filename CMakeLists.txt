cmake_minimum_required(VERSION 2.4)


project (Spruce)

add_subdirectory (bin)
add_subdirectory (doc)
add_subdirectory (include)
add_subdirectory (lib)
add_subdirectory (src)
add_subdirectory (tests)

# Uninstall the system
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
