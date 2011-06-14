CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

MESSAGE("Configuring project benchmark sources")
PROJECT (Spruce)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/syscall/include)

ADD_SUBDIRECTORY(dup)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/syscall/dup)

ADD_SUBDIRECTORY(read_write)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/syscall/read_write)

ADD_EXECUTABLE(syscall main.cpp)
TARGET_LINK_LIBRARIES(syscall utils dup read_write)

