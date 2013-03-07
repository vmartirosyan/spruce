message("-- Configuring project sources.")

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}/include")
link_directories("${CMAKE_BINARY_DIR}/src/utils")

INCLUDE (arch)

#add_subdirectory (benchmark)
#add_subdirectory (common)
add_subdirectory (engine/generator)

add_subdirectory (main)
add_subdirectory (tests)
add_subdirectory (engine/doer)

#add_subdirectory (utils)
#add_subdirectory (fault_sim)

