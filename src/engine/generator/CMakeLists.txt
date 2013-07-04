message("-- Configuring the test generator.")

#add_executable(generator generator.cpp ${CMAKE_SOURCE_DIR}/src/utils/process.cpp testpackage.xslt testset.xslt)

execute_process(COMMAND 
	bash -c "mkdir ${CMAKE_BINARY_DIR}/bin > /dev/null 2>&1"
	OUTPUT_VARIABLE output)

execute_process(COMMAND 	
	g++ ${CMAKE_CURRENT_SOURCE_DIR}/generator.cpp ${CMAKE_SOURCE_DIR}/src/utils/process.cpp 
	-o ${CMAKE_BINARY_DIR}/bin/generator
	-I ${CMAKE_SOURCE_DIR}/include -I ${CMAKE_BINARY_DIR}/include)
	
#execute_process(   
#   COMMAND ${CMAKE_BINARY_DIR}/bin/generator ${CMAKE_SOURCE_DIR}/src/tests ${CMAKE_BINARY_DIR}/src/tests ${CMAKE_SOURCE_DIR}/src/engine/generator
#   )
   
add_custom_target(Generator ALL 
				COMMENT "Building test generator"
				DEPENDS ${CMAKE_SOURCE_DIR}/src/engine/generator/generator.cpp ${CMAKE_SOURCE_DIR}/src/engine/generator/testpackage.xslt ${CMAKE_SOURCE_DIR}/src/engine/generator/testset.xslt)
				
add_executable( generator ${CMAKE_CURRENT_SOURCE_DIR}/generator.cpp ${CMAKE_SOURCE_DIR}/src/utils/process.cpp ${CMAKE_SOURCE_DIR}/src/engine/generator/testpackage.xslt ${CMAKE_SOURCE_DIR}/src/engine/generator/testset.xslt )
