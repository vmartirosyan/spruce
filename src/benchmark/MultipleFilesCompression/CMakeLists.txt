CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

MESSAGE("Configuring MultipleFilesCompression benchmark test")
PROJECT (Spruce)

SET(TEST_NAME MultipleFilesCompression)

INSTALL(
		FILES ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.sh
		DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
		PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
)

