set(kmodule_name "kedr_fsim_mempool")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/header.data.in"
    "${CMAKE_CURRENT_BINARY_DIR}/header.data")

add_fsim_payload(${kmodule_name}
    "mempool_alloc"
    "mempool_create"
    "mempool_create_node"
)