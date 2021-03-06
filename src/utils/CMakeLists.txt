message("-- Configuring utils sources.")

add_library(utils STATIC test.cpp process.cpp PartitionManager.cpp JFSCtl.cpp )

IF (NOT ${OS_32_BITS} AND ${HAVE_MULTILIB} )	
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
   
   ADD_CUSTOM_COMMAND(
   OUTPUT  PartitionManager_32.cpp
   COMMAND cp ${CMAKE_SOURCE_DIR}/src/utils/PartitionManager.cpp PartitionManager_32.cpp
   DEPENDS PartitionManager.cpp
   )
   
   ADD_CUSTOM_COMMAND(
   OUTPUT  JFSCtl_32.cpp
   COMMAND cp ${CMAKE_SOURCE_DIR}/src/utils/JFSCtl.cpp JFSCtl_32.cpp
   DEPENDS JFSCtl.cpp
   )
	set_source_files_properties(test_32.cpp process_32.cpp PartitionManager_32.cpp JFSCtl_32.cpp PROPERTIES COMPILE_FLAGS "-DCOMPAT")
	set_source_files_properties(test_32.cpp process_32.cpp PartitionManager_32.cpp JFSCtl_32.cpp PROPERTIES COMPILE_FLAGS "-m32")
	
	add_library(utils_32 STATIC test_32.cpp process_32.cpp PartitionManager_32.cpp JFSCtl_32.cpp)
	set_target_properties(utils_32 PROPERTIES LINK_FLAGS "-m32")
	
ENDIF (NOT ${OS_32_BITS} AND ${HAVE_MULTILIB})
