message("-- Configuring main module.")


ADD_EXECUTABLE(spruce 	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_SOURCE_DIR}/include/KedrIntegrator.hpp ${CMAKE_SOURCE_DIR}/include/LeakChecker.hpp )

TARGET_LINK_LIBRARIES(spruce utils btrfs)
#TARGET_LINK_LIBRARIES(spruce rt)

install (	FILES "${CMAKE_BINARY_DIR}/bin/spruce"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)




