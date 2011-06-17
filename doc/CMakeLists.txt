cmake_minimum_required(VERSION 2.4)

message("Configuring project documentation.")

project (Spruce)

execute_process(COMMAND echo "Generating documentation")

execute_process(COMMAND ${CMAKE_SOURCE_DIR}/doc/generate.sh ${CMAKE_BINARY_DIR} 2>/dev/null )

execute_process(COMMAND echo "Copying documentation")

execute_process(COMMAND bash -c "cp -r ${CMAKE_SOURCE_DIR}/doc/*.html ${CMAKE_BINARY_DIR}/doc/"
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc)
				

install (FILES ${CMAKE_BINARY_DIR}/doc/index.html DESTINATION share/doc/spruce)
install (FILES ${CMAKE_BINARY_DIR}/doc/main/index.html ${CMAKE_BINARY_DIR}/doc/main/ch01.html DESTINATION share/doc/spruce/main)
install (FILES ${CMAKE_BINARY_DIR}/doc/syscall/index.html ${CMAKE_BINARY_DIR}/doc/syscall/ch01.html DESTINATION share/doc/spruce/syscall)
install (FILES ${CMAKE_BINARY_DIR}/doc/common/index.html ${CMAKE_BINARY_DIR}/doc/common/ch01.html DESTINATION share/doc/spruce/common)
install (FILES ${CMAKE_BINARY_DIR}/doc/benchmark/index.html ${CMAKE_BINARY_DIR}/doc/benchmark/ch01.html DESTINATION share/doc/spruce/benchmark)

