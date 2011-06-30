CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

MESSAGE("Configuring project benchmark BINARYs")
PROJECT (Spruce)

ADD_SUBDIRECTORY(utils)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/include)

ADD_SUBDIRECTORY(FindFile)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/FindFile)

ADD_SUBDIRECTORY(MultipleFilesCreation)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/MultipleFilesCreation)

ADD_SUBDIRECTORY(LargeFileCreation)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/LargeFileCreation)

ADD_SUBDIRECTORY(MultipleFilesDeletion)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/MultipleFilesDeletion)

ADD_SUBDIRECTORY(MultipleFilesCompression)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/MultipleFilesCompression)

ADD_SUBDIRECTORY(LargeFileCopyTest)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/LargeFileCopyTest)

ADD_SUBDIRECTORY(SplitLargeFile)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/SplitLargeFile)

ADD_SUBDIRECTORY(MultipleFilesCopy)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/MultipleFilesCopy)

ADD_SUBDIRECTORY(MultipleFilesCatting)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/MultipleFilesCatting)

ADD_SUBDIRECTORY(CattingLargeFile)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/CattingLargeFile)

ADD_SUBDIRECTORY(MultipleFilesDecompression)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/MultipleFilesDecompression)

ADD_SUBDIRECTORY(SearchMapFile)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/src/benchmark/SearchMapFile)



ADD_EXECUTABLE(benchmark ${CMAKE_CURRENT_BINARY_DIR}/main.cpp)
TARGET_LINK_LIBRARIES(benchmark utils MultipleFilesCreation MultipleFilesDeletion 
	MultipleFilesCompression LargeFileCreation FindFile LargeFileCopyTest MultipleFilesDecompression MultipleFilesCopy 
	MultipleFilesCatting CattingLargeFile SearchMapFile SplitLargeFile)
	
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_BINARY_DIR}/main.cpp)

install (	FILES "${CMAKE_BINARY_DIR}/bin/benchmark"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
