# Generate file using kedr_gen. Auxiliary function.
# Template is expected to be located in kedr templates directory
function(kedr_generate output_file input_file template_name)
    set(kedr_templates_dir "${KEDR_INSTALL_PREFIX}/share/kedr/templates")
    set(kedr_gen "${KEDR_INSTALL_PREFIX}/bin/kedr_gen")

    add_custom_command(OUTPUT "${output_file}"
        COMMAND ${kedr_gen} "${kedr_templates_dir}/${template_name}"
            "${input_file}" > "${output_file}"
# Delete output file if generator returns error.
            || (rm ${output_file} && /bin/false)
        DEPENDS "${input_file}"
    )
endfunction(kedr_generate output_file input_file template_name)

# Directory where current CMakeListst.txt is located.
set(fault_sim_payloads_dir ${CMAKE_CURRENT_SOURCE_DIR})

# Configuration file which lists our payloads
set(custom_config_file "${CMAKE_CURRENT_BINARY_DIR}/fsim_custom.conf")

file(WRITE ${custom_config_file} "# Payloads created for Spruce\n")

# Create fault simulation payload for given functions.
#
# 'name' - name of payload(module) created
# 'functions' - list of the functions names.
# 
# For each function with name <function_name> file <function_name>.data
# should exist and describe function processing.
#
# Also, file header.data should exists and contain global information
# about payload(payload name, includes...).

function(add_fsim_payload name functions)
    set(KBUILD_DIR "/lib/modules/${CMAKE_SYSTEM_VERSION}/build")
    set(KEDR_SYMVERS_DIR "${KEDR_INSTALL_PREFIX}/lib/modules/${CMAKE_SYSTEM_VERSION}/symvers")
    # Names of datafile chunks
    set(data_chunks_names)
    foreach(chunk_name "header" ${functions} ${ARGN})
        list(APPEND data_chunks_names "${chunk_name}.data")
    endforeach(chunk_name ${functions} ${ARGN})
    # Datafile chunks themselves
    to_abs_path(data_files ${data_chunks_names})
    
    # Combine all chunks of datafile.
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/payload.data"
        COMMAND cat ${data_files} > "${CMAKE_CURRENT_BINARY_DIR}/payload.data"
        DEPENDS ${data_files}
    )
    
    # Create payload source from datafile
    kedr_generate("${CMAKE_CURRENT_BINARY_DIR}/payload.c"
        "${CMAKE_CURRENT_BINARY_DIR}/payload_with_caller_address.data"
        "payload_fsim.c"
    )
    kedr_generate("${CMAKE_CURRENT_BINARY_DIR}/payload_with_caller_address.data"
        "${CMAKE_CURRENT_BINARY_DIR}/payload.data"
        "payload_fsim_add_caller_address.data"
    )
    
    # Create function_support.c from datafile
    kedr_generate("${CMAKE_CURRENT_BINARY_DIR}/functions_support.c"
        "${CMAKE_CURRENT_BINARY_DIR}/payload.data"
        "functions_support.c"
    )
    
    # Create payload module from source
    configure_file("${fault_sim_payloads_dir}/Kbuild.in"
        "${CMAKE_CURRENT_BINARY_DIR}/Kbuild"
        @ONLY@)
    add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${name}.ko"
        COMMAND cat "${KEDR_SYMVERS_DIR}/kedr.symvers" "${KEDR_SYMVERS_DIR}/kedr_fault_simulation.symvers"
            > "${CMAKE_CURRENT_BINARY_DIR}/Module.symvers"
        COMMAND $(MAKE) -C ${KBUILD_DIR} M=${CMAKE_CURRENT_BINARY_DIR} modules
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/payload.c"
                "${CMAKE_CURRENT_BINARY_DIR}/functions_support.c"
    )
    
    # Tell CMake that given files should be cleaned on 'make clean'.
    set(module_clean_files
        # List common files for cleaning
        "${CMAKE_CURRENT_BINARY_DIR}/built-in.o"
		"${CMAKE_CURRENT_BINARY_DIR}/.built-in.o.cmd"
		"${CMAKE_CURRENT_BINARY_DIR}/Module.markers"
        # List module name-depending files for cleaning
        "${CMAKE_CURRENT_BINARY_DIR}/${name}.o"
		"${CMAKE_CURRENT_BINARY_DIR}/${name}.mod.c"
		"${CMAKE_CURRENT_BINARY_DIR}/${name}.mod.o"
        "${CMAKE_CURRENT_BINARY_DIR}/.${name}.ko.cmd"
		"${CMAKE_CURRENT_BINARY_DIR}/.${name}.mod.o.cmd"
		"${CMAKE_CURRENT_BINARY_DIR}/.${name}.o.cmd"
        # List source name-depending files for cleaning
        "${CMAKE_CURRENT_BINARY_DIR}/payload.o"
        "${CMAKE_CURRENT_BINARY_DIR}/.payload.o.cmd"
        "${CMAKE_CURRENT_BINARY_DIR}/.payload.o.d"
        "${CMAKE_CURRENT_BINARY_DIR}/functions_support.o"
        "${CMAKE_CURRENT_BINARY_DIR}/.functions_support.o.cmd"
        "${CMAKE_CURRENT_BINARY_DIR}/.functions_support.o.d"
        )
	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${module_clean_files}")

    # Target
    add_custom_target(${name} ALL
        DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${name}.ko")
    
    # Install payload
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}.ko"
        DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/modules/${CMAKE_SYSTEM_VERSION}/misc")
    
    #Update configuration file with payloads list
    file(APPEND "${custom_config_file}" "payload ${CMAKE_INSTALL_PREFIX}/lib/modules/${CMAKE_SYSTEM_VERSION}/misc/${name}.ko\n")
endfunction(add_fsim_payload name functions)

#add_subdirectory(bio)
#add_subdirectory(kthread)
add_subdirectory(mempool)
add_subdirectory(vfs)

# Install configuration file itself
install(FILES "${custom_config_file}"
        DESTINATION "${FAULT_SIMULATION_CUSTOM_PAYLOADS_DIR}"
        RENAME "${FAULT_SIMULATION_CUSTOM_PAYLOADS_FILE}")
