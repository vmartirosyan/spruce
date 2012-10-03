cmake_minimum_required(VERSION 2.6)

message("Configuring utils sources.")

#add_library(utils STATIC test.cpp process.cpp ${CMAKE_BINARY_DIR}/src/utils/operations.cpp )
add_library(utils STATIC test.cpp process.cpp  )

#add_executable(operations_generator operations_generator.cpp process.cpp )

#ADD_CUSTOM_COMMAND(
#   OUTPUT  ${CMAKE_BINARY_DIR}/include/operations.hpp ${CMAKE_BINARY_DIR}/src/utils/operations.cpp
#   COMMAND ${CMAKE_BINARY_DIR}/bin/operations_generator ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src
#   DEPENDS ${CMAKE_BINARY_DIR}/bin/operations_generator
#   )
   
#add_custom_target(Headers ALL 
#				COMMENT "Generating headers"
#				DEPENDS ${CMAKE_BINARY_DIR}/include/operations.hpp)


#add_dependencies(utils Headers)
