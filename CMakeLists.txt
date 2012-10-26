cmake_minimum_required(VERSION 2.8)
include (ExternalProject)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckTypeSize)

project (Spruce)


IF( UNIX )
    EXECUTE_PROCESS( 
      COMMAND uname -a
      OUTPUT_VARIABLE OS_ARCH 
      )	
	
	IF( OS_ARCH MATCHES ".*Linux.*" )
      IF( OS_ARCH MATCHES ".*x86_64.*" )
	SET( OS_32_BITS 0 )
      ELSEIF( OS_ARCH MATCHES ".*ia64.*" )
	SET( OS_32_BITS 0 )
      ELSEIF( OS_ARCH MATCHES ".*i686.*" )
	SET( OS_32_BITS 1 )
      ENDIF( OS_ARCH MATCHES ".*x86_64.*" )
    ENDIF( OS_ARCH MATCHES ".*Linux.*" )
    
    Message("Is 32 bit: ${OS_32_BITS}")
ENDIF ( UNIX )

SET( CMAKE_REQUIRED_DEFINITIONS "-D _GNU_SOURCE")
SET(CMAKE_EXTRA_INCLUDE_FILES "fcntl.h")
CHECK_TYPE_SIZE("struct file_handle" STRUCT_FILE_HANDLE)
if (NOT HAVE_STRUCT_FILE_HANDLE)
SET(HAVE_STRUCT_FILE_HANDLE 0)
else (NOT HAVE_STRUCT_FILE_HANDLE)
SET(HAVE_STRUCT_FILE_HANDLE 1)
endif (NOT HAVE_STRUCT_FILE_HANDLE)
SET(CMAKE_EXTRA_INCLUDE_FILES)

IF (NOT XSLTPROC_PATH)
	SET( XSLTPROC_PATH "/usr/bin/xsltproc")
ENDIF (NOT XSLTPROC_PATH)

IF (NOT EXISTS ${XSLTPROC_PATH})
	MESSAGE(FATAL_ERROR "xsltproc is not found at ${XSLTPROC_PATH}. Please install the corresponding package and/or provide the XSLTPROC_PATH variable.")
ENDIF (NOT EXISTS ${XSLTPROC_PATH})

# By default install Spruce to /opt/spruce
IF (NOT CMAKE_INSTALL_PREFIX)
	SET( CMAKE_INSTALL_PREFIX "/opt/spruce/")
ENDIF (NOT CMAKE_INSTALL_PREFIX)

IF (NOT CMAKE_CXX_FLAGS)
	SET( CMAKE_CXX_FLAGS "-Wall -pedantic -Wno-variadic-macros -Wextra -Weffc++ -fcheck-new -ffor-scope -Wabi -Wstrict-null-sentinel")
ENDIF (NOT CMAKE_CXX_FLAGS)

IF (NOT TEST_TIMEOUT)
	SET ( TEST_TIMEOUT 2 )
ENDIF (NOT TEST_TIMEOUT)

IF (NOT FIFO_PATH)
	SET ( FIFO_PATH "/tmp/spruce_fifo" )
ENDIF (NOT FIFO_PATH)

IF (NOT KEDR_ROOT_DIR)
	SET ( KEDR_ROOT_DIR "/usr/bin" )
ENDIF (NOT KEDR_ROOT_DIR)

IF (NOT QUOTACHECK_PATH)
	SET( QUOTACHECK_PATH "/usr/sbin/quotacheck")
ENDIF (NOT QUOTACHECK_PATH)

SET (QUOTA_ENABLED 0)
Message("-- Looking for quotacheck file")
IF (EXISTS ${QUOTACHECK_PATH})
	Message("-- Looking for quotacheck file - found")
	SET ( QUOTA_ENABLED 1 )	
ELSE (EXISTS ${QUOTACHECK_PATH})
	Message("-- Looking for quotacheck file - NOT found")
	Message("-- Please install the package quota(-tools) and/or provide the correct QUOTACHECK_PATH.")
ENDIF (EXISTS ${QUOTACHECK_PATH})

CHECK_INCLUDE_FILES ("sys/types.h;attr/xattr.h" HAVE_XATTR_H)
IF (NOT HAVE_XATTR_H)
	SET(HAVE_XATTR_H 0)
ENDIF (NOT HAVE_XATTR_H)

CHECK_INCLUDE_FILES ("xfs/xfs.h" HAVE_XFS_H)
IF (NOT HAVE_XFS_H)
	SET(HAVE_XFS_H 0)
ENDIF (NOT HAVE_XFS_H)

SET (LIBRARY_OUTPUT_PATH
 ${PROJECT_BINARY_DIR}/lib
 CACHE PATH
 "Directory for all Libraries"
 )
 
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY
 ${PROJECT_BINARY_DIR}/bin
 CACHE PATH
 "Directory for all Binaries"
 )
 
message("-- Copying source files to build folder")
execute_process(COMMAND mkdir ${CMAKE_BINARY_DIR}/include/ -p )
#execute_process(COMMAND bash -c "cp -r ${CMAKE_SOURCE_DIR}/include/*.hpp ${CMAKE_BINARY_DIR}/include/ ") 

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}/include")

# Changing the source directory to point to the binary directory
#SET(CMAKE_SOURCE_DIR, ${CMAKE_BINARY_DIR})

add_subdirectory (src/utils)
add_subdirectory (engine)
add_subdirectory (bin)
add_subdirectory (include)
add_subdirectory (lib)
add_subdirectory (src)
#add_subdirectory (tests)
add_subdirectory (config)
#add_subdirectory (doc)



# Uninstall the system
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    
