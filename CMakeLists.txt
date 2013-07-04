cmake_minimum_required(VERSION 2.6)
project (Spruce)

INCLUDE (CheckIncludeFiles)
INCLUDE (CheckTypeSize)

include_directories("${CMAKE_SOURCE_DIR}/src/tests/fs/ext4fs/Headers")
include_directories("${CMAKE_SOURCE_DIR}/src/tests/fs/jfs/Headers")
include_directories("${CMAKE_SOURCE_DIR}/src/tests/fs/xfs/Headers")
include_directories("${CMAKE_SOURCE_DIR}/src/tests/fs/btrfs/Headers")


set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

INCLUDE (common)

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

# Default compiler flags.
if(NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "-Wall -pedantic -Wno-variadic-macros -Wextra -Weffc++ -fcheck-new -ffor-scope -Wabi -Wstrict-null-sentinel")
endif(NOT CMAKE_CXX_FLAGS)

# NOTE: currently doesn't used anywhere than config file.
set(TEST_TIMEOUT 15 CACHE STRING "Timeout value used by tests")

# NOTE: currently doesn't used anywhere than config file.
set(FIFO_PATH "/tmp/spruce_fifo" CACHE PATH "<Description is required>")


# Check if KEDR framework is there
check_executable("kedr" KEDR_PATH)

# We need additional KEDR payloads. For build them we need
# corresponded KEDR infrustructure. So, we need KEDR install path.
if(NOT KEDR_INSTALL_PREFIX)
    # Assume KEDR_PATH is ${KEDR_INSTALL_PREFIX}/bin/kedr.
    # So, deduce install prefix from path to KEDR executable.
    string(REGEX REPLACE "/bin/kedr$" "" KEDR_INSTALL_ROOT ${KEDR_PATH})
    if("${KEDR_INSTALL_ROOT}" STREQUAL "${KEDR_PATH}")
	    message("Cannot deduce KEDR install prefix from path to KEDR executable (${KEDR_PATH}).")
	    message(FATAL_ERROR "Plesase, KEDR_INSTALL_PREFIX to KEDR install prefix.")
    endif("${KEDR_INSTALL_ROOT}" STREQUAL "${KEDR_PATH}")
    
    set(KEDR_INSTALL_PREFIX ${KEDR_INSTALL_ROOT} CACHE PATH "KEDR installation prefix")
    message("Use '${KEDR_INSTALL_PREFIX}' as KEDR_INSTALL_PREFIX.")
endif(NOT KEDR_INSTALL_PREFIX)


# Check if quotacheck is there
check_executable("quotacheck" QUOTACHECK_PATH)

check_include ("sys/types.h;attr/xattr.h" HAVE_XATTR_H "header file")


check_include ("sys/acl.h" HAVE_ACL_H "header file")

SET(CMAKE_REQUIRED_DEFINITIONS "-D _GNU_SOURCE")
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

# KEDR configuration file which lists our fault simulation payloads.
set (FAULT_SIMULATION_CUSTOM_PAYLOADS_FILE "fault_sim_custom.conf")
set (FAULT_SIMULATION_CUSTOM_PAYLOADS_DIR "${CMAKE_INSTALL_PREFIX}/etc/spruce")
set (FAULT_SIMULATION_CUSTOM_PAYLOADS "${FAULT_SIMULATION_CUSTOM_PAYLOADS_DIR}/${FAULT_SIMULATION_CUSTOM_PAYLOADS_FILE}")
 
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
#add_subdirectory (engine)
#add_subdirectory (bin)
add_subdirectory (include)
#add_subdirectory (lib)
add_subdirectory (src)
add_subdirectory (config)
add_subdirectory (doc)
add_subdirectory (codemarker)

# Uninstall the system
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
    
if(SPRUCE_ENABLE_TESTING)
	spruce_enable_testing()

	add_subdirectory (tests)
endif(SPRUCE_ENABLE_TESTING)
