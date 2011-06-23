cmake_minimum_required(VERSION 2.4)

message("Configuring project documentation.")

project (Spruce)

execute_process(COMMAND echo "Copying documentation")

#execute_process(COMMAND ${CMAKE_SOURCE_DIR}/doc/generate.sh ${CMAKE_SOURCE_DIR}/doc ${CMAKE_BINARY_DIR}/doc 2>/dev/null )

	
install (FILES ${CMAKE_BINARY_DIR}/doc/spruce.html DESTINATION share/doc/spruce)
install (FILES ${CMAKE_BINARY_DIR}/doc/main/index.html ${CMAKE_BINARY_DIR}/doc/main/ch01.html DESTINATION share/doc/spruce/main)
install (FILES ${CMAKE_BINARY_DIR}/doc/syscall/index.html ${CMAKE_BINARY_DIR}/doc/syscall/ch01.html DESTINATION share/doc/spruce/syscall)
install (FILES ${CMAKE_BINARY_DIR}/doc/common/index.html ${CMAKE_BINARY_DIR}/doc/common/ch01.html DESTINATION share/doc/spruce/common)
install (FILES ${CMAKE_BINARY_DIR}/doc/benchmark/index.html ${CMAKE_BINARY_DIR}/doc/benchmark/ch01.html DESTINATION share/doc/spruce/benchmark)

