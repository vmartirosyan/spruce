cmake_minimum_required(VERSION 2.6)

message("Configuring template engine.")

add_executable(engine engine.cpp ${CMAKE_SOURCE_DIR}/src/utils/process.cpp )
   
add_custom_target(Engine ALL 
				COMMENT "Generating engine"
				DEPENDS engine.cpp)

add_dependencies(utils Engine)
