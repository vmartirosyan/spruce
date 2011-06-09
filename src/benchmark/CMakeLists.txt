CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

MESSAGE("Configuring project benchmark sources")
PROJECT (Spruce)

ADD_SUBDIRECTORY(utils)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/benchmark/include)

ADD_SUBDIRECTORY(MultipleFilesCreation)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/benchmark/MultipleFilesCreation)

ADD_SUBDIRECTORY(LargeFileCreation)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/benchmark/LargeFileCreation)

ADD_SUBDIRECTORY(MultipleFilesDeletion)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/benchmark/MultipleFilesDeletion)

ADD_SUBDIRECTORY(MultipleFilesCompression)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/benchmark/MultipleFilesCompression)

ADD_EXECUTABLE(Benchmark main.cpp)
TARGET_LINK_LIBRARIES(Benchmark utils MultipleFilesCreation MultipleFilesDeletion MultipleFilesCompression LargeFileCreation)

