configure_file("${CMAKE_CURRENT_SOURCE_DIR}/test.sh.in"
	"${CMAKE_CURRENT_BINARY_DIR}/test.sh"
	@ONLY)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/test.conf.in"
	"${CMAKE_CURRENT_BINARY_DIR}/test.conf"
	@ONLY)

add_test_script("oops_checker.kernel_bug.crash" "test.sh")
