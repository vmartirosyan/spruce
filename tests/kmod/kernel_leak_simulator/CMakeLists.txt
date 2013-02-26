set(module_name "leak_simulator")

add_custom_target(${module_name}
	DEPENDS ${module_name}.ko)

set(payload_data_file "${CMAKE_CURRENT_SOURCE_DIR}/payload.data")
set(payload_c_filename "payload.c")
set(params_c_filename "params.c")
set(functions_support_filename "functions_support.c")

set(kedr_symvers_dir "${KEDR_INSTALL_PREFIX}/lib/modules/${CMAKE_SYSTEM_VERSION}/symvers")

set(symvers_files
	"${kedr_symvers_dir}/kedr.symvers"
	"${kedr_symvers_dir}/kedr_leak_check.symvers")

set(kedr_gen_tool "${KEDR_INSTALL_PREFIX}/lib/kedr/kedr_gen")

set(kedr_gen_templates_dir "${KEDR_INSTALL_PREFIX}/share/kedr/templates")

add_custom_command(OUTPUT ${module_name}.ko
	COMMAND cat ${symvers_files} > "${CMAKE_CURRENT_BINARY_DIR}/Module.symvers"
	COMMAND make -C /lib/modules/${CMAKE_SYSTEM_VERSION}/build M=${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${payload_c_filename}"
			"${CMAKE_CURRENT_BINARY_DIR}/${functions_support_filename}"
			"${CMAKE_CURRENT_BINARY_DIR}/${params_c_filename}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Kbuild.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Kbuild"
	@ONLY@)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${payload_c_filename}"
	COMMAND ${kedr_gen_tool} "${kedr_gen_templates_dir}/payload_leak_check.c" "${payload_data_file}" > "${CMAKE_CURRENT_BINARY_DIR}/${payload_c_filename}"
	DEPENDS "${payload_data_file}")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${functions_support_filename}"
	COMMAND ${kedr_gen_tool} "${kedr_gen_templates_dir}/functions_support.c" "${payload_data_file}" > "${CMAKE_CURRENT_BINARY_DIR}/${functions_support_filename}"
	DEPENDS "${payload_data_file}")

rule_copy_source("${params_c_filename}")

test_add_target(${module_name})
