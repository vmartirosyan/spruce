message("Configuring main module.")

if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

ADD_EXECUTABLE(spruce 	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_SOURCE_DIR}/include/kedr_integrator.hpp ${CMAKE_SOURCE_DIR}/include/leak_checker.hpp )

TARGET_LINK_LIBRARIES(spruce utils)
TARGET_LINK_LIBRARIES(spruce rt)

install (	FILES "${CMAKE_BINARY_DIR}/bin/spruce"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)




