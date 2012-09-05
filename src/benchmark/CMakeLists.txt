CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

MESSAGE("Configuring project benchmark BINARYs")
PROJECT (Spruce)

ADD_SUBDIRECTORY(utils)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

ADD_SUBDIRECTORY(FindFile)

ADD_SUBDIRECTORY(MultipleFilesCreation)

ADD_SUBDIRECTORY(LargeFileCreation)

ADD_SUBDIRECTORY(MultipleFilesDeletion)

ADD_SUBDIRECTORY(MultipleFilesCompression)

ADD_SUBDIRECTORY(LargeFileCopyTest)

ADD_SUBDIRECTORY(SplitLargeFile)

ADD_SUBDIRECTORY(MultipleFilesCopy)

ADD_SUBDIRECTORY(MultipleFilesCatting)

ADD_SUBDIRECTORY(CattingLargeFile)

ADD_SUBDIRECTORY(MultipleFilesDecompression)

ADD_SUBDIRECTORY(SearchMapFile)

ADD_EXECUTABLE(benchmark	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/MultipleFilesCreation/Creator.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/MultipleFilesDeletion/Deleter.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/MultipleFilesCompression/Compressor.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/LargeFileCreation/LargeFile.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/FindFile/FindFile.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/LargeFileCopyTest/LargeFileCopyTest.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/MultipleFilesDecompression/Decompressor.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/MultipleFilesCopy/MultipleFilesCopy.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/MultipleFilesCatting/MultipleFilesCatting.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/CattingLargeFile/CattingLargeFile.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/SearchMapFile/SearchMapFile.cpp
							${CMAKE_CURRENT_SOURCE_DIR}/SplitLargeFile/SplitLargeFile.cpp)
TARGET_LINK_LIBRARIES(benchmark utils)
#TARGET_LINK_LIBRARIES(benchmark utils MultipleFilesCreation MultipleFilesDeletion 
#	MultipleFilesCompression LargeFileCreation FindFile LargeFileCopyTest MultipleFilesDecompression MultipleFilesCopy 
#	MultipleFilesCatting CattingLargeFile SearchMapFile SplitLargeFile)
	
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/main.cpp ${CMAKE_CURRENT_BINARY_DIR}/main.cpp)

install (	FILES "${CMAKE_BINARY_DIR}/bin/benchmark"
			DESTINATION bin
			PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ)
