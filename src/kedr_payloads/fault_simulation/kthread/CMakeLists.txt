set(kmodule_name "kedr_fsim_kthread")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/header.data.in"
    "${CMAKE_CURRENT_BINARY_DIR}/header.data")

set(functions)

kmodule_is_function_exist(kthread_create_on_node EXISTS_kthread_create_on_node)

if(EXISTS_kthread_create_on_node)
	list(APPEND functions "kthread_create_on_node")
endif(EXISTS_kthread_create_on_node)

if(functions)
    add_fsim_payload(${kmodule_name} ${functions})
endif(functions)    
