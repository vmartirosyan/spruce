set(spruce_executable "${CMAKE_INSTALL_PREFIX}/bin/spruce")
set(report_generator "perl ${CMAKE_SOURCE_DIR}/report_generator/report.pl")
# On 64-bit OS Spruce test modules has sufficies, _64 or _32.
# Use '_64' (native) in that case.
if(NOT ${OS_32_BITS} AND ${HAVE_MULTILIB})
    set(MODULE_BIT_SUFFIX "_64")
else(NOT ${OS_32_BITS} AND ${HAVE_MULTILIB})
    set(MODULE_BIT_SUFFIX "")
endif(NOT ${OS_32_BITS} AND ${HAVE_MULTILIB})

add_subdirectory (kmod)

add_subdirectory(oops-checker)
add_subdirectory(leak-checker)
