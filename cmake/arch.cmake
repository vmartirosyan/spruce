function (arch module)
	IF ( ${OS_32_BITS} OR NOT ${HAVE_MULTILIB})	
	ADD_EXECUTABLE(${module} ${CMAKE_CURRENT_BINARY_DIR}/module.cpp )

	TARGET_LINK_LIBRARIES(${module} utils)
	if (${module} STREQUAL "syscall" OR ${module} STREQUAL "fault_sim")
		TARGET_LINK_LIBRARIES(${module} acl)
	endif()
	
	
	install (	FILES "${CMAKE_BINARY_DIR}/bin/${module}"
				DESTINATION bin
				PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
	ELSE ( ${OS_32_BITS} OR NOT ${HAVE_MULTILIB})
		ADD_EXECUTABLE(${module}_64 ${CMAKE_CURRENT_BINARY_DIR}/module.cpp)
		TARGET_LINK_LIBRARIES(${module}_64 utils)
		if (${module} STREQUAL "syscall")
			TARGET_LINK_LIBRARIES(${module}_64 acl)
		endif()

		install (	FILES "${CMAKE_BINARY_DIR}/bin/${module}_64"
				DESTINATION bin
				PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

	ENDIF ( ${OS_32_BITS} OR NOT ${HAVE_MULTILIB})	

	IF (NOT ${OS_32_BITS} AND ${HAVE_MULTILIB})	
		ADD_CUSTOM_COMMAND(
	   OUTPUT  module_32.cpp
	   COMMAND cp module.cpp module_32.cpp
	   DEPENDS module.cpp
	   )
		set_source_files_properties(module_32.cpp PROPERTIES COMPILE_FLAGS "-m32")
		
		
		ADD_EXECUTABLE(${module}_32 module_32.cpp)
		TARGET_LINK_LIBRARIES(${module}_32 utils_32)
		if (${module} STREQUAL "syscall")
			TARGET_LINK_LIBRARIES(${module}_32 acl)
		endif()	
		
		set_target_properties(${module}_32 PROPERTIES LINK_FLAGS "-m32") 
		
		install (	FILES "${CMAKE_BINARY_DIR}/bin/${module}_32"
				DESTINATION bin
				PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
		
	ENDIF (NOT ${OS_32_BITS} AND ${HAVE_MULTILIB})

endfunction()
