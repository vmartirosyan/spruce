CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

MESSAGE("Configuring project syscall BINARYs")
PROJECT (Spruce)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/include)

ADD_SUBDIRECTORY(dup)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/dup)

ADD_SUBDIRECTORY(read_write)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/read_write)

ADD_SUBDIRECTORY(close)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/close)

ADD_SUBDIRECTORY(fcntl)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/fcntl)

ADD_SUBDIRECTORY(link)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/link)

ADD_SUBDIRECTORY(chmod)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/chmod)

ADD_SUBDIRECTORY(chown)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/chown)

ADD_SUBDIRECTORY(ioctl)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/ioctl)

ADD_SUBDIRECTORY(stat)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/stat)

ADD_SUBDIRECTORY(fsync)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/fsync)

ADD_SUBDIRECTORY(fdatasync)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/fdatasync)

ADD_SUBDIRECTORY(getdents)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/getdents)

ADD_SUBDIRECTORY(rename)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/rename)

ADD_SUBDIRECTORY(unlink)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/unlink)

ADD_SUBDIRECTORY(unlinkat)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/unlinkat)

ADD_SUBDIRECTORY(readv_writev)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/readv_writev)

ADD_SUBDIRECTORY(pread_pwrite)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/pread_pwrite)

ADD_SUBDIRECTORY(creat)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/creat)

ADD_SUBDIRECTORY(umask)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/umask)

ADD_SUBDIRECTORY(select)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/select)

ADD_SUBDIRECTORY(lseek)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/lseek)

ADD_SUBDIRECTORY(mknod)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/mknod)

ADD_SUBDIRECTORY(mmap)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/mmap)

ADD_SUBDIRECTORY(msync)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/msync)

ADD_SUBDIRECTORY(mlock)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/mlock)

ADD_SUBDIRECTORY(mprotect)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/mprotect)

ADD_SUBDIRECTORY(readdir)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/readdir)

ADD_SUBDIRECTORY(readlink)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/readlink)

ADD_SUBDIRECTORY(openat)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/openat)

ADD_SUBDIRECTORY(open)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/open)

ADD_SUBDIRECTORY(linkat)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/linkat)

ADD_SUBDIRECTORY(xattr)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/syscall/xattr)

# Karen
		
# Gio
		
# Vahram

# Suren

# Gurgen

# Ruzanna

# Ani

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_BINARY_DIR}/main.cpp)

ADD_EXECUTABLE(syscall 	${CMAKE_CURRENT_BINARY_DIR}/main.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/chmod/Chmod.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/chown/Chown.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/close/Close.cpp
						${CMAKE_CURRENT_SOURCE_DIR}/creat/Creat.cpp
						${CMAKE_CURRENT_BINARY_DIR}/open/Open.cpp
						${CMAKE_CURRENT_BINARY_DIR}/openat/Openat.cpp
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
		
						# Gio
							
						# Vahram

						# Suren

						# Gurgen

						# Ruzanna

						# Ani
						)
						

TARGET_LINK_LIBRARIES(syscall utils)
TARGET_LINK_LIBRARIES(syscall rt)
#TARGET_LINK_LIBRARIES(syscall dup)
#TARGET_LINK_LIBRARIES(syscall close)
#TARGET_LINK_LIBRARIES(syscall chmod)
#TARGET_LINK_LIBRARIES(syscall chown)
#TARGET_LINK_LIBRARIES(syscall link)
#TARGET_LINK_LIBRARIES(syscall read_write)
#TARGET_LINK_LIBRARIES(syscall fcntl)
#TARGET_LINK_LIBRARIES(syscall ioctl)
#TARGET_LINK_LIBRARIES(syscall stat)
#TARGET_LINK_LIBRARIES(syscall fsync)
#TARGET_LINK_LIBRARIES(syscall fdatasync)
#TARGET_LINK_LIBRARIES(syscall getdents)
#TARGET_LINK_LIBRARIES(syscall rename)
#TARGET_LINK_LIBRARIES(syscall unlink)
#TARGET_LINK_LIBRARIES(syscall unlinkat)
#TARGET_LINK_LIBRARIES(syscall readv_writev)
#TARGET_LINK_LIBRARIES(syscall pread_pwrite)
#TARGET_LINK_LIBRARIES(syscall creat)
#TARGET_LINK_LIBRARIES(syscall umask)
#TARGET_LINK_LIBRARIES(syscall select)
#TARGET_LINK_LIBRARIES(syscall lseek)
#TARGET_LINK_LIBRARIES(syscall mknod)

install (	FILES "${CMAKE_BINARY_DIR}/bin/syscall"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
