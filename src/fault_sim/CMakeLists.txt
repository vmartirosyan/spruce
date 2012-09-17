CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

MESSAGE("Configuring Fault Simulation module")
PROJECT (Spruce)

ADD_CUSTOM_COMMAND(
   OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/module.cpp
   COMMAND ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/engine
   DEPENDS ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_SOURCE_DIR}/engine/module.xslt ${CMAKE_CURRENT_SOURCE_DIR}/module.xml 
   ${CMAKE_BINARY_DIR}/src/syscall_new/module.cpp ${CMAKE_BINARY_DIR}/src/fs/ext4fs/module.cpp ${CMAKE_BINARY_DIR}/src/fs/xfs/module.cpp ${CMAKE_BINARY_DIR}/src/fs/jfs/module.cpp ${CMAKE_BINARY_DIR}/src/fs/btrfs/module.cpp
   )

include_directories("${CMAKE_BINARY_DIR}/src/syscall_new")
Include_Directories("${CMAKE_BINARY_DIR}/src/fs/ext4fs")
Include_Directories("${CMAKE_BINARY_DIR}/src/fs/xfs")
Include_Directories("${CMAKE_BINARY_DIR}/src/fs/jfs")
Include_Directories("${CMAKE_BINARY_DIR}/src/fs/btrfs")
include_directories("${CMAKE_SOURCE_DIR}/src/syscall_new")
Include_Directories("${CMAKE_SOURCE_DIR}/src/fs/ext4fs")
Include_Directories("${CMAKE_SOURCE_DIR}/src/fs/xfs")
Include_Directories("${CMAKE_SOURCE_DIR}/src/fs/jfs")
Include_Directories("${CMAKE_SOURCE_DIR}/src/fs/btrfs")

ADD_EXECUTABLE(fault_sim ${CMAKE_CURRENT_BINARY_DIR}/module.cpp )

TARGET_LINK_LIBRARIES(fault_sim utils)
TARGET_LINK_LIBRARIES(fault_sim rt)

install (	FILES "${CMAKE_BINARY_DIR}/bin/fault_sim"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
