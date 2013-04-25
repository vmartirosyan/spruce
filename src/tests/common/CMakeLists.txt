add_test_package(common)

target_link_libraries(common acl)

IF (NOT ${OS_32_BITS} AND ${HAVE_MULTILIB})	
	target_link_libraries(common_32 acl)
ENDIF()
