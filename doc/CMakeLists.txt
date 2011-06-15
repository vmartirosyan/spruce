cmake_minimum_required(VERSION 2.4)

message("Configuring project documentation.")

project (Spruce)

install (FILES ${CMAKE_SOURCE_DIR}/doc/index.html DESTINATION share/doc/spruce)
