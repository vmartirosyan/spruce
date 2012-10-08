cmake_minimum_required(VERSION 2.6)

message("Configuring utils sources.")

#add_library(utils STATIC test.cpp process.cpp ${CMAKE_BINARY_DIR}/src/utils/operations.cpp )
add_library(utils STATIC test.cpp process.cpp  )

IF (NOT ${OS_32_BITS})	
	ADD_CUSTOM_COMMAND(
   OUTPUT  test_32.cpp
   COMMAND cp ${CMAKE_SOURCE_DIR}/src/utils/test.cpp test_32.cpp
   DEPENDS test.cpp
   )
   
   ADD_CUSTOM_COMMAND(
   OUTPUT  process_32.cpp
   COMMAND cp ${CMAKE_SOURCE_DIR}/src/utils/process.cpp process_32.cpp
   DEPENDS process.cpp
   )
	set_source_files_properties(test_32.cpp process_32.cpp PROPERTIES COMPILE_FLAGS "-m32")
	
	add_library(utils_32 STATIC test_32.cpp process_32.cpp  )
	set_target_properties(utils_32 PROPERTIES LINK_FLAGS "-m32") 
	
ENDIF (NOT ${OS_32_BITS})

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
