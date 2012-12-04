cmake_minimum_required(VERSION 2.6)
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
    
    IF ( NOT ${OS_32_BITS} )		
		CHECK_INCLUDE_FILES ("gnu/stubs-32.h" HAVE_MULTILIB)
		IF (NOT HAVE_MULTILIB)
			Message(FATAL_ERROR "The 'multilib' library wasn't found. Please install it.")
			#SET(HAVE_XFS_H 0)
		ENDIF (NOT HAVE_MULTILIB)
	ELSE ( NOT ${OS_32_BITS} ) 
		SET (HAVE_MULTILIB 0)
	ENDIF ( NOT ${OS_32_BITS} )
    
ENDIF ( UNIX )

# Check if xsltproc is there
Message("-- Looking for 'xsltproc' executable.")
IF ( XSLTPROC_PATH AND NOT EXISTS ${XSLTPROC_PATH} )
	Message(FATAL_ERROR "${XSLTPROC_PATH} cannot be executed. Set XSLTPROC_PATH to correct path of 'xsltproc' executable.")


	execute_process( COMMAND ${XSLTPROC_PATH} -V 
					RESULT_VARIABLE XSLTPROC_STATUS
					OUTPUT_QUIET
					ERROR_QUIET)
					
	IF ( ${XSLTPROC_STATUS} )
		Message(FATAL_ERROR "Cannot get xsltproc version via '${XSLTPROC_PATH} -V'. Set XSLTPROC_PATH to correct path of 'xsltproc' executable.")
	ENDIF ( ${XSLTPROC_STATUS} )

ELSE ( XSLTPROC_PATH AND NOT EXISTS ${XSLTPROC_PATH} )

	find_program(XSLTPROC_PATH xsltproc 
		PATHS /usr/bin)

	IF ( NOT XSLTPROC_PATH )
		MESSAGE(FATAL_ERROR "'xsltproc' executable wasn't found. "
		"\nIf xsltproc is installed into a non-default directory, set XSLTPROC_PATH to the path of 'xsltproc' executable. ")
	ENDIF ( NOT XSLTPROC_PATH )

ENDIF ( XSLTPROC_PATH AND NOT EXISTS ${XSLTPROC_PATH} )
Message("-- Looking for 'xsltproc' executable - found.")

# By default install Spruce to /opt/spruce
IF (NOT CMAKE_INSTALL_PREFIX)
	SET( CMAKE_INSTALL_PREFIX "/opt/spruce/")
ENDIF (NOT CMAKE_INSTALL_PREFIX)

IF (NOT CMAKE_CXX_FLAGS)
	SET( CMAKE_CXX_FLAGS "-Wall -pedantic -Wno-variadic-macros -Wextra -Weffc++ -fcheck-new -ffor-scope -Wabi -Wstrict-null-sentinel")
ENDIF (NOT CMAKE_CXX_FLAGS)

IF (NOT TEST_TIMEOUT)
	SET ( TEST_TIMEOUT 15 )
ENDIF (NOT TEST_TIMEOUT)

IF (NOT FIFO_PATH)
	SET ( FIFO_PATH "/tmp/spruce_fifo" )
ENDIF (NOT FIFO_PATH)

# Check if KEDR framework is there
Message("-- Looking for 'kedr' executable.")
IF ( KEDR_PATH AND NOT EXISTS ${KEDR_PATH} )
	Message(FATAL_ERROR "${KEDR_PATH} cannot be executed. Set KEDR_PATH to correct path of 'kedr' executable.")

	execute_process( COMMAND ${KEDR_PATH} status 
				RESULT_VARIABLE KEDR_STATUS
				OUTPUT_QUIET
				ERROR_QUIET)
				
	IF ( ${KEDR_STATUS} )
		Message(FATAL_ERROR "Cannot get KEDR status via '${KEDR_PATH} status'. Set KEDR_PATH to correct path of 'kedr' executable.")
	ENDIF ( ${KEDR_STATUS} )
ELSE ( KEDR_PATH AND NOT EXISTS ${KEDR_PATH} )

	find_program(KEDR_PATH kedr
		PATHS /usr/local/bin)
	IF (NOT KEDR_PATH)
		Message(FATAL_ERROR "'kedr' executable wasn't found. "
		"\nIf KEDR is installed into a non-default directory, set KEDR_PATH to the path of 'kedr' executable. ")	
	ENDIF (NOT KEDR_PATH)
ENDIF ( KEDR_PATH AND NOT EXISTS ${KEDR_PATH} )
Message("-- Looking for 'kedr' executable - found.")

# Check if quotacheck is there
Message("-- Looking for 'quotacheck' executable.")
IF ( QUOTACHECK_PATH AND NOT EXISTS ${QUOTACHECK_PATH} )
	Message(FATAL_ERROR "${QUOTACHECK_PATH} cannot be executed. Set QUOTACHECK_PATH to correct path of 'quotacheck' executable.")

	execute_process( COMMAND ${QUOTACHECK_PATH} -V 
				RESULT_VARIABLE QUOTACHECK_STATUS
				OUTPUT_QUIET
				ERROR_QUIET)
				
	IF ( ${QUOTACHECK_STATUS} )
		Message(FATAL_ERROR "Cannot get quotacheck version via '${QUOTACHECK_PATH} -V'. Set QUOTACHECK_PATH to correct path of 'quotacheck' executable.")
	ENDIF ( ${QUOTACHECK_STATUS} )
	
ELSE ( QUOTACHECK_PATH AND NOT EXISTS ${QUOTACHECK_PATH} )

	find_program(QUOTACHECK_PATH quotacheck
		PATHS /usr/sbin /sbin)
	IF (NOT QUOTACHECK_PATH)
		Message(FATAL_ERROR "'quotacheck' executable wasn't found. "
		"\nIf quotacheck is installed into a non-default directory, set QUOTACHECK_PATH to the path of 'quotacheck' executable. ")	
	ENDIF (NOT QUOTACHECK_PATH)
ENDIF ( QUOTACHECK_PATH AND NOT EXISTS ${QUOTACHECK_PATH} )
Message("-- Looking for 'quotacheck' executable - found.")


CHECK_INCLUDE_FILES ("sys/types.h;attr/xattr.h" HAVE_XATTR_H)
IF (NOT HAVE_XATTR_H)
	Message(FATAL_ERROR "'attr/xattr.h' header file wasn't found. Please install the 'attr' package.")
	#SET(HAVE_XATTR_H 0)
ENDIF (NOT HAVE_XATTR_H)

CHECK_INCLUDE_FILES (ext2fs/ext2_fs.h HAVE_EXT2_FS_H)
IF (NOT HAVE_EXT2_FS_H)
	Message(FATAL_ERROR "'ext2fs/ext2_fs.h' header file wasn't found. Please install the 'e2fslibs' package.")
	#SET(HAVE_EXT2_FS_H 0)
ENDIF (NOT HAVE_EXT2_FS_H)


CHECK_INCLUDE_FILES ("xfs/xfs.h" HAVE_XFS_H)
IF (NOT HAVE_XFS_H)
	Message(FATAL_ERROR "'xfs/xfs.h' header file wasn't found. Please install the 'xfslibs' package.")
	#SET(HAVE_XFS_H 0)
ENDIF (NOT HAVE_XFS_H)

SET( CMAKE_REQUIRED_DEFINITIONS "-D _GNU_SOURCE")
SET(CMAKE_EXTRA_INCLUDE_FILES "fcntl.h")
CHECK_TYPE_SIZE("struct file_handle" STRUCT_FILE_HANDLE)
if (NOT HAVE_STRUCT_FILE_HANDLE)
SET(HAVE_STRUCT_FILE_HANDLE 0)
else (NOT HAVE_STRUCT_FILE_HANDLE)
SET(HAVE_STRUCT_FILE_HANDLE 1)
endif (NOT HAVE_STRUCT_FILE_HANDLE)
SET(CMAKE_EXTRA_INCLUDE_FILES)


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
#add_subdirectory (bin)
add_subdirectory (include)
#add_subdirectory (lib)
add_subdirectory (src)
#add_subdirectory (tests)
add_subdirectory (config)
add_subdirectory (doc)



# Uninstall the system
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    
