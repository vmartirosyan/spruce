CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

MESSAGE("Configuring project benchmark sources")
PROJECT (Spruce)

ADD_SUBDIRECTORY(utils)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/benchmark/include)

ADD_SUBDIRECTORY(MultipleFileCreation)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/benchmark/MultipleFileCreation)

ADD_SUBDIRECTORY(MultipleFileCompression)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/benchmark/MultipleFileCompression)

ADD_EXECUTABLE(Benchmark main.cpp)
TARGET_LINK_LIBRARIES(Benchmark utils MultipleFileCreation.a MultipleFileCompression.a)
