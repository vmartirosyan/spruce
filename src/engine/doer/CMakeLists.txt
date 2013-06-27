message("-- Configuring the test doer.")

add_executable(doer doer.cpp )

target_link_libraries(doer utils)
   
add_custom_target(Doer ALL 
				COMMENT "Building test doer"
				DEPENDS doer.cpp)


target_link_libraries(doer utils)
target_link_libraries(doer common)
target_link_libraries(doer ext4)
target_link_libraries(doer jfs)
target_link_libraries(doer xfs)
target_link_libraries(doer btrfs)

add_dependencies(spruce Doer)

install (	FILES "${CMAKE_BINARY_DIR}/bin/doer"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)


IF (NOT ${OS_32_BITS} AND ${HAVE_MULTILIB})	

	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/doer_32.cpp
		COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/doer.cpp ${CMAKE_CURRENT_BINARY_DIR}/doer_32.cpp
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/doer.cpp)

	set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/doer_32.cpp PROPERTIES COMPILE_FLAGS "-DCOMPAT -m32")
	
	add_executable(doer_32 ${CMAKE_CURRENT_BINARY_DIR}/doer_32.cpp )
	
	target_link_libraries(doer_32 utils_32)
   
	add_custom_target(Doer_32 ALL 
					COMMENT "Building test doer"
					DEPENDS doer_32.cpp)

	target_link_libraries(doer_32 utils_32)
	target_link_libraries(doer_32 common_32)
	target_link_libraries(doer_32 ext4_32)
	target_link_libraries(doer_32 jfs_32)
	target_link_libraries(doer_32 xfs_32)
	target_link_libraries(doer_32 btrfs_32)

	add_dependencies(spruce Doer_32)
	
	set_target_properties(doer_32 PROPERTIES LINK_FLAGS "-m32")

	install (	FILES "${CMAKE_BINARY_DIR}/bin/doer_32"
				DESTINATION bin
				PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
ENDIF ()
