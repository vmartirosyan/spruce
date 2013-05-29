add_custom_target(kernel_bug_module
	DEPENDS kernel_bug_module.ko)

add_custom_command(OUTPUT kernel_bug_module.ko
	COMMAND make -C /lib/modules/${CMAKE_SYSTEM_VERSION}/build M=${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/module.c"
			"${CMAKE_CURRENT_BINARY_DIR}/Kbuild")

rule_copy_source("module.c")
rule_copy_source("Kbuild")

test_add_target(kernel_bug_module)
