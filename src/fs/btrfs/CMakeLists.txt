MESSAGE("-- Configuring BTRFS module")
include(../../arch.cmake)

ADD_CUSTOM_COMMAND(
   OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/module.cpp 
   COMMAND ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR}/engine
   DEPENDS ${CMAKE_BINARY_DIR}/bin/engine ${CMAKE_SOURCE_DIR}/engine/module.xslt ${CMAKE_SOURCE_DIR}/engine/testset.xslt ${CMAKE_CURRENT_SOURCE_DIR}/module.xml ${CMAKE_CURRENT_SOURCE_DIR}/Ioctl.xml
   )

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

arch("btrfs")
