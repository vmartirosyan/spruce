# Needed for all CMakeLists files, otherwise cmake will warn
cmake_minimum_required (VERSION 2.6)

project(kmodule_try_compile)
# Create rule for produce file from another file via copiing
macro(copy_file FILENAME SOURCE)
add_custom_command(OUTPUT "${FILENAME}"
					COMMAND cp -p "${SOURCE}" "${FILENAME}"
					DEPENDS "${SOURCE}")
endmacro(copy_file FILENAME SOURCE)

if(NOT DEFINED SRC_FILE)
	message(FATAL_ERROR "'SRC_FILE' should be defined, but it is not.")
endif(NOT DEFINED SRC_FILE)

if(NOT DEFINED KERNELDIR)
	message(FATAL_ERROR "'KERNELDIR' should be defined, but it is not.")
endif(NOT DEFINED KERNELDIR)


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Kbuild.in"
	"${CMAKE_CURRENT_BINARY_DIR}/Kbuild")
# 'make all' should produce try_compile.ko file
add_custom_target(kmodule_compile ALL DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/try_compile.ko")

# Rule to produce try_compile.ko from try_compile.c
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/try_compile.ko"
	COMMAND $(MAKE) -C ${KERNELDIR} M=${CMAKE_CURRENT_BINARY_DIR} modules
	DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/try_compile.c")

# Kbuild system cannot work with out-of-source build,
# so copy source file into build dir and rename it to try_compile.c.
copy_file("${CMAKE_CURRENT_BINARY_DIR}/try_compile.c" "${SRC_FILE}")
