# Current directory; used for access internal directories in it.
set(kedr_payloads_dir ${CMAKE_CURRENT_SOURCE_DIR})
# Current binary directory. Used for try_compile sources with functions calls.
set(kedr_payloads_binary_dir ${CMAKE_CURRENT_BINARY_DIR})

# kmodule_try_compile(RESULT_VAR bindir srcfile)

# Similar to try_module in simplified form, but compile srcfile as
# kernel module, instead of user space program.
function(kmodule_try_compile RESULT_VAR bindir srcfile)
	to_abs_path(src_abs_path "${srcfile}")
    try_compile(result_tmp "${bindir}"
            "${kedr_payloads_dir}/kmodule_files"
            "kmodule_try_compile"
            CMAKE_FLAGS "-DSRC_FILE:path=${src_abs_path}"
                "-DKERNELDIR=/lib/modules/${CMAKE_SYSTEM_VERSION}/build"
            # debug
            #OUTPUT_VARIABLE output_var
    )
    # debug
    #if(NOT result_tmp)
        #message("Kernel module compilation output: ${output_var}")
    #endif(NOT result_tmp)
    
	set("${RESULT_VAR}" "${result_tmp}" PARENT_SCOPE)
endfunction(kmodule_try_compile RESULT_VAR bindir srcfile)


# kmodule_is_function_exist(function_name RESULT_VAR)
# Verify, whether given function exist in the kernel space on the current system.
# RESULT_VAR is TRUE, if function_name exist in the kernel space, FALSE otherwise.
# RESULT_VAR is cached.
#
# NOTE: Source file "${function_name}.c" should exist under ./check_functions
# directory and contain given function call.

function(kmodule_is_function_exist function_name RESULT_VAR)
    set(kmodule_is_function_exist_message "Looking for ${function_name} in the kernel")

    if(DEFINED ${RESULT_VAR})
        set(kmodule_is_function_exist_message "${kmodule_is_function_exist_message} [cached]")
    else(DEFINED ${RESULT_VAR})
        kmodule_try_compile(kmodule_function_is_exist
            "${kedr_payloads_binary_dir}/check_functions/${function_name}"
            "${kedr_payloads_dir}/check_functions/${function_name}.c"
        )
        if (kmodule_function_is_exist)
            set(${RESULT_VAR} "TRUE" CACHE INTERNAL "Does ${function_name} exist in the kernel?")
        else(kmodule_function_is_exist)
            set(${RESULT_VAR} "FALSE" CACHE INTERNAL "Does ${function_name} exist in the kernel?")
        endif(kmodule_function_is_exist)
    endif(DEFINED ${RESULT_VAR})
    if (${RESULT_VAR})
        message(STATUS "${kmodule_is_function_exist_message} - found")
    else(${RESULT_VAR})
        message(STATUS "${kmodule_is_function_exist_message} - not found")
    endif(${RESULT_VAR})
endfunction(kmodule_is_function_exist function_name RESULT_VAR)


add_subdirectory(fault_simulation)