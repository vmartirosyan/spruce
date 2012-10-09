CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

MESSAGE("Configuring Syscall module")
PROJECT (Spruce)

ADD_CUSTOM_COMMAND(
   OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/module.cpp
   COMMAND ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/engine
   DEPENDS ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_SOURCE_DIR}/engine/module.xslt ${CMAKE_SOURCE_DIR}/engine/testset.xslt ${CMAKE_CURRENT_SOURCE_DIR}/module.xml access.xml chdir.xml chmod.xml chown.xml chroot.xml close.xml creat.xml dup.xml fallocate.xml fchmod.xml fchown.xml fcntl.xml fdatasync.xml fstat.xml fstatfs.xml fsync.xml ftruncate.xml getcwd.xml ioctl.xml lchown.xml link.xml linkat.xml lseek.xml lseek64.xml mkdir_rmdir.xml mknod.xml mlock.xml mmap.xml mount_umount.xml mprotect.xml msync.xml open.xml openat.xml pread_pwrite.xml quotactl.xml read_write.xml readdir.xml readv_writev.xml rename.xml select.xml splice.xml stat.xml statfs.xml sync.xml truncate.xml umask.xml unlink.xml unlinkat.xml utime.xml xattr.xml name_to_handle.xml
   )
   
IF ( ${OS_32_BITS})	
ADD_EXECUTABLE(syscall ${CMAKE_CURRENT_BINARY_DIR}/module.cpp )

					
TARGET_LINK_LIBRARIES(syscall utils)
TARGET_LINK_LIBRARIES(syscall rt)

install (	FILES "${CMAKE_BINARY_DIR}/bin/syscall"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
ELSE ( ${OS_32_BITS})
	ADD_EXECUTABLE(syscall_64 ${CMAKE_CURRENT_BINARY_DIR}/module.cpp)
				
	TARGET_LINK_LIBRARIES(syscall_64 utils)
	TARGET_LINK_LIBRARIES(syscall_64 rt)

	install (	FILES "${CMAKE_BINARY_DIR}/bin/syscall_64"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)

ENDIF ( ${OS_32_BITS})	

IF (NOT ${OS_32_BITS})	
	ADD_CUSTOM_COMMAND(
   OUTPUT  module_32.cpp
   COMMAND cp module.cpp module_32.cpp
   DEPENDS module.cpp
   )
	set_source_files_properties(module_32.cpp PROPERTIES COMPILE_FLAGS "-m32")
	
	
	ADD_EXECUTABLE(syscall_32 module_32.cpp)
	TARGET_LINK_LIBRARIES(syscall_32 utils_32)
	TARGET_LINK_LIBRARIES(syscall_32 rt)
	
	set_target_properties(syscall_32 PROPERTIES LINK_FLAGS "-m32") 
	
	install (	FILES "${CMAKE_BINARY_DIR}/bin/syscall_32"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
	
ENDIF (NOT ${OS_32_BITS})
