set(kmodule_name "kedr_fsim_bio")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/header.data.in"
    "${CMAKE_CURRENT_BINARY_DIR}/header.data")

add_fsim_payload(${kmodule_name}
    "bio_add_page"
    #"submit_bio"
)

