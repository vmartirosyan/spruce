set(kmodule_name "kedr_fsim_mempool")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/header.data.in"
    "${CMAKE_CURRENT_BINARY_DIR}/header.data")

add_fsim_payload(${kmodule_name}
    # This function is used in jfs driver in fail-never scenario.
    # Disable payload for the function until it will support fail-never scenario.
    #"mempool_alloc"
    "mempool_create"
    "mempool_create_node"
)