CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT (Spruce)
MESSAGE("Configuring project configuration files")

install (	FILES "${CMAKE_CURRENT_SOURCE_DIR}/xslt/processor.xslt"
			DESTINATION share/spruce/config/xslt
			PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
			
install (	FILES "${CMAKE_CURRENT_SOURCE_DIR}/xslt/module.xslt"
			DESTINATION share/spruce/config/xslt
			PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)

install (	FILES "${CMAKE_CURRENT_SOURCE_DIR}/PartitionManager.cfg"
			DESTINATION share/spruce/config
			PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
			

