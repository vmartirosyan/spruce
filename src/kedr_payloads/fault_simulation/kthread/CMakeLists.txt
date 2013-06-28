set(kmodule_name "kedr_fsim_kthread")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/header.data.in"
    "${CMAKE_CURRENT_BINARY_DIR}/header.data")

add_fsim_payload(${kmodule_name}
	"kthread_create_on_node" 
)

