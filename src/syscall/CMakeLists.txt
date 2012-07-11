CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

MESSAGE("Configuring project syscall module")
PROJECT (Spruce)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)



ADD_EXECUTABLE(syscall 	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/chmod/Chmod.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/chown/Chown.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/close/Close.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/creat/Creat.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/open/Open.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/openat/Openat.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/linkat/Linkat.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/dup/DupFileDescriptor.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/fcntl/fcntlFD.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/fdatasync/FDataSyncTest.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/fsync/FSyncTest.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/getdents/getdents.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/ioctl/IoctlTest.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/link/Link.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/lseek/LseekTest.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/mknod/MknodTest.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/pread_pwrite/PreadPwrite.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/read_write/ReadWriteFile.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/readv_writev/ReadvWritev.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/rename/rename.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/select/Select.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/stat/StatTest.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/umask/Umask.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/unlink/Unlink.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/unlinkat/UnlinkAT.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/mmap/MmapMemory.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/msync/MsyncMemory.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/mlock/MlockMemory.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/mprotect/MprotectMemory.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/readdir/ReadDir.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/readlink/Readlink.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/xattr/Xattr.cpp
						# Karen
						#${CMAKE_CURRENT_SOURCE_DIR}/mount_umount/Mount_Umount.cpp		
						# Gio
						${CMAKE_CURRENT_SOURCE_DIR}/chdir/chdir.cpp	
						# Vahram
						${CMAKE_CURRENT_SOURCE_DIR}/utime/Utime.cpp	
						${CMAKE_CURRENT_SOURCE_DIR}/access/Access.cpp	
						${CMAKE_CURRENT_SOURCE_DIR}/fstat/Fstat.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/statfs/Statfs.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/fstatfs/Fstatfs.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/truncate/Truncate.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/ftruncate/Ftruncate.cpp
						# Suren
						${CMAKE_CURRENT_SOURCE_DIR}/bdflush/Bdflush.cpp
						# Gurgen
						${CMAKE_CURRENT_SOURCE_DIR}/fchmod/Fchmod.cpp	
						${CMAKE_CURRENT_SOURCE_DIR}/fchown/Fchown.cpp	
						${CMAKE_CURRENT_SOURCE_DIR}/sync/Sync.cpp	
						# Ruzanna

						# Ani
						)
						

TARGET_LINK_LIBRARIES(syscall utils)
TARGET_LINK_LIBRARIES(syscall rt)

install (	FILES "${CMAKE_BINARY_DIR}/bin/syscall"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
