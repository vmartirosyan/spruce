set(spruce_executable "${CMAKE_INSTALL_PREFIX}/bin/spruce")
set(report_generator "perl ${CMAKE_SOURCE_DIR}/report_generator/report.pl")

add_subdirectory (kmod)

add_subdirectory(oops-checker)
add_subdirectory(leak-checker)
