cmake_minimum_required(VERSION 2.6)
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckTypeSize)

project (Spruce)

function (check_include name HAVE_H type)
	CHECK_INCLUDE_FILES ("${name}" ${HAVE_H})
		IF (NOT ${HAVE_H})
			Message(FATAL_ERROR "The '${name}' ${type} wasn't found. Please install it.")
			#SET(HAVE_XFS_H 0)
		ENDIF (NOT ${HAVE_H})
endfunction()

function (set_default var value)
		IF (NOT ${var})
		SET( ${var} "${value}")
		ENDIF (NOT ${var})
endfunction()

function (check_executable name path)
	Message("-- Looking for '${name}' executable")
	IF ( NOT EXISTS ${${path}} )
	Message("   The ${path} wasn't set, looking for it ...")
	find_program(${path} ${name})
	IF ( NOT EXISTS ${${path}} )
		Message(FATAL_ERROR "'${name}' cannot be executed. Set correct path of '${name}' executable.")
	ENDIF ( NOT EXISTS ${${path}} )
	ENDIF ( NOT EXISTS ${${path}} )
	execute_process(COMMAND ${${path}} -V
				RESULT_VARIABLE STAT
				OUTPUT_QUIET
				ERROR_QUIET)
	IF ( ${STAT} )
	Message(FATAL_ERROR "Cannot get '${name}' via '${path} -V'. Set ${path} to correct path of '${name}' executable.")
	ENDIF( ${STAT} )
	Message("-- Looking for '${name}' executable - found")
endfunction()

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
		check_include ("gnu/stubs-32.h" HAVE_MULTILIB "library")
	ELSE ( NOT ${OS_32_BITS} ) 
		SET (HAVE_MULTILIB 0)
	ENDIF ( NOT ${OS_32_BITS} )
    
ENDIF ( UNIX )

# Check if xsltproc is there
check_executable("xsltproc" XSLTPROC_PATH)


# By default install Spruce to /opt/spruce
set_default( CMAKE_INSTALL_PREFIX "/opt/spruce/")

set_default( CMAKE_CXX_FLAGS "-Wall -pedantic -Wno-variadic-macros -Wextra -Weffc++ -fcheck-new -ffor-scope -Wabi -Wstrict-null-sentinel")

set_default( TEST_TIMEOUT 15 )

set_default( FIFO_PATH "/tmp/spruce_fifo" )

# Check if KEDR framework is there
check_executable("kedr" KEDR_PATH)

# Check if quotacheck is there
check_executable("quotacheck" QUOTACHECK_PATH)

check_include ("sys/types.h;attr/xattr.h" HAVE_XATTR_H "header file")

check_include ("ext2fs/ext2_fs.h" HAVE_EXT2_FS_H "header file")

check_include ("sys/acl.h" HAVE_ACL_H "header file")

check_include ("xfs/xfs.h" HAVE_XFS_H "header file")

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
 
execute_process(COMMAND mkdir ${CMAKE_BINARY_DIR}/include/ -p )
execute_process(COMMAND bash -c "hg heads | grep changeset | tr -d '\n'"
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				RESULT_VARIABLE HG_STATUS
				OUTPUT_VARIABLE HG_REV)
IF ( ${HG_STATUS} )
	set(HG_REV, "No revision")
ENDIF ( ${HG_STATUS} )

include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}/include")


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
    
