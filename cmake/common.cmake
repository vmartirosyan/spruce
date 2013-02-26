#Create rule for obtain one file by copying another one
function(rule_copy_file target_file source_file)
    add_custom_command(OUTPUT ${target_file}
                    COMMAND cp -p ${source_file} ${target_file}
                    DEPENDS ${source_file}
                    )
endfunction(rule_copy_file target_file source_file)

#Create rule for obtain file in binary tree by copiing it from source tree
function(rule_copy_source rel_source_file)
    rule_copy_file(${CMAKE_CURRENT_BINARY_DIR}/${rel_source_file} ${CMAKE_CURRENT_SOURCE_DIR}/${rel_source_file})
endfunction(rule_copy_source rel_source_file)


# to_abs_path(output_var path [...])
#
# Convert relative path of file to absolute path:
# use path in source tree, if file already exist there.
# otherwise use path in binary tree.
# If initial path already absolute, return it.
function(to_abs_path output_var)
    set(result)
    foreach(path ${ARGN})
        string(REGEX MATCH "^/" _is_abs_path ${path})
        if(_is_abs_path)
            list(APPEND result ${path})
        else(_is_abs_path)
            file(GLOB to_abs_path_file 
                "${CMAKE_CURRENT_SOURCE_DIR}/${path}"
            )
            if(NOT to_abs_path_file)
                set (to_abs_path_file "${CMAKE_CURRENT_BINARY_DIR}/${path}")
            endif(NOT to_abs_path_file)
            list(APPEND result ${to_abs_path_file})
        endif(_is_abs_path)
    endforeach(path ${ARGN})
    set("${output_var}" ${result} PARENT_SCOPE)
endfunction(to_abs_path output_var path)

# Initialize testing subsystem
macro (spruce_enable_testing)
	if(SPRUCE_ENABLE_TESTING)
		# For test Spruce itself we need pre-defined device(partition).
		# Because path to that device is system-depended, we require
		# that user will set TEST_DEVICE variable pointed to that device.
		#
		# NB: Currently only block device is supported (as it doesn't
		# require to set mount options in Spruce configuration file).
		if(NOT DEFINED TEST_DEVICE)
			message(FATAL_ERROR "TEST_DEVICE variable should be defined as path to device which will be used in Spruce testing.")
		endif(NOT DEFINED TEST_DEVICE)
		
		execute_process(COMMAND "test" "-b" "${TEST_DEVICE}"
			RESULT_VARIABLE TEST_DEVICE_IS_BLOCK
			OUTPUT_QUIET
			ERROR_QUIET)
		
		if(NOT TEST_DEVICE_IS_BLOCK STREQUAL "0")
			message(FATAL_ERROR """${TEST_DEVICE}"" is not point to block device.")
		endif(NOT TEST_DEVICE_IS_BLOCK STREQUAL "0")

		# For test how Spruce works with leaks, we need to simulate that
		# leaks in the driver. For that purpose, we use additional
		# KEDR LeakCheck payload. For build this payload we need
		# corresponded KEDR infrustructure. So, we need KEDR install path.
		if(NOT KEDR_INSTALL_PREFIX)
			# Assume KEDR_PATH is ${KEDR_INSTALL_PREFIX}/bin/kedr.
			# So, deduce install prefix from path to KEDR executable.
			string(REGEX REPLACE "/bin/kedr$" "" KEDR_INSTALL_PREFIX ${KEDR_PATH})
			if("${KEDR_INSTALL_PREFIX}" STREQUAL "${KEDR_PATH}")
				message("Cannot deduce KEDR install prefix from path to KEDR executable (${KEDR_PATH}).")
				message(FATAL_ERROR "Plesase, KEDR_INSTALL_PREFIX to KEDR install prefix.")
			endif("${KEDR_INSTALL_PREFIX}" STREQUAL "${KEDR_PATH}")
			#TODO: Check KEDR_INSTALL_PREFIX path.
			
			message("Use '${KEDR_INSTALL_PREFIX}' as KEDR_INSTALL_PREFIX.")
		endif(NOT KEDR_INSTALL_PREFIX)
		
		enable_testing ()
		add_custom_target (check 
			COMMAND ${CMAKE_CTEST_COMMAND} -E "crash"
		)
		add_custom_target (build_tests)
		add_dependencies (check build_tests)
	endif(SPRUCE_ENABLE_TESTING)
endmacro (spruce_enable_testing)
# Function for mark target as "for testing only"
function (test_add_target target_name)
	if(SPRUCE_ENABLE_TESTING)
		set_target_properties (${target_name}
			PROPERTIES EXCLUDE_FROM_ALL true
		)
		add_dependencies (build_tests ${target_name})
	endif(SPRUCE_ENABLE_TESTING)
endfunction (test_add_target target_name)

# Function for add test 
function (add_test_script test_name script_file)
	if(SPRUCE_ENABLE_TESTING)
	    to_abs_path (TEST_SCRIPT_FILE ${script_file})
	        
	    add_test (${test_name}
	        /bin/bash ${TEST_SCRIPT_FILE} ${ARGN}
	    )
	endif(SPRUCE_ENABLE_TESTING)
endfunction (add_test_script)

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
