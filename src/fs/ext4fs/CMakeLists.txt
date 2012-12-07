message("-- Configuring EXT4FS specific sources.")


IF ( NOT ${HAVE_EXT2_FS_H} )
	Message("The ext2fs/ext2_fs.h header file is not found. Skiping Ext4 specific tests.")																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
	return()
ENDIF ( NOT ${HAVE_EXT2_FS_H} )

ADD_CUSTOM_COMMAND(
   OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/module.cpp
   COMMAND ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/engine
   DEPENDS ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_SOURCE_DIR}/engine/module.xslt ${CMAKE_SOURCE_DIR}/engine/testset.xslt ${CMAKE_CURRENT_SOURCE_DIR}/module.xml Ioctl.xml
   )

IF ( ${OS_32_BITS} OR NOT ${HAVE_MULTILIB})	
ADD_EXECUTABLE(ext4 ${CMAKE_CURRENT_BINARY_DIR}/module.cpp )

					
TARGET_LINK_LIBRARIES(ext4 utils)
#TARGET_LINK_LIBRARIES(ext4 rt)

install (	FILES "${CMAKE_BINARY_DIR}/bin/ext4"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
ELSE ( ${OS_32_BITS} OR NOT ${HAVE_MULTILIB})	
	ADD_EXECUTABLE(ext4_64 ${CMAKE_CURRENT_BINARY_DIR}/module.cpp)
				
	TARGET_LINK_LIBRARIES(ext4_64 utils)
#	TARGET_LINK_LIBRARIES(ext4_64 rt)

	install (	FILES "${CMAKE_BINARY_DIR}/bin/ext4_64"
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
	
	
	ADD_EXECUTABLE(ext4_32 module_32.cpp)
	TARGET_LINK_LIBRARIES(ext4_32 utils_32)
#	TARGET_LINK_LIBRARIES(ext4_32 rt)
	
	set_target_properties(ext4_32 PROPERTIES LINK_FLAGS "-m32") 
	
	install (	FILES "${CMAKE_BINARY_DIR}/bin/ext4_32"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
	
ENDIF (NOT ${OS_32_BITS} AND ${HAVE_MULTILIB})	
