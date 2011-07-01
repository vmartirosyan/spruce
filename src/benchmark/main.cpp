#include "Creator.hpp"
#include "Deleter.hpp"
#include "Compressor.hpp"
#include "LargeFile.hpp"
#include "FindFile.hpp"
#include "LargeFileCopyTest.hpp"
#include "Decompressor.hpp"
#include "MultipleFilesCopy.hpp"
#include "CattingLargeFile.hpp"
#include "MultipleFilesCatting.hpp"
#include "SplitLargeFile.hpp"
#include "SearchMapFile.hpp"
#include <config.hpp>

int main(int argc, char ** argv)
{
	Configuration<CreatorTest> conf1("${CMAKE_INSTALL_PREFIX}config/creator.conf");
	Configuration<FindFileTest> conf2("${CMAKE_INSTALL_PREFIX}config/findfile.conf");
	Configuration<CompressTest> conf3("${CMAKE_INSTALL_PREFIX}config/compression.conf");
	Configuration<DecompressTest> conf4("${CMAKE_INSTALL_PREFIX}config/decompression.conf");
	Configuration<LargeFileTest> conf5("${CMAKE_INSTALL_PREFIX}config/largefile.conf");
	Configuration<SearchMapFileTest> conf6("${CMAKE_INSTALL_PREFIX}config/searchmapfile.conf");
	Configuration<CattingLargeFile> conf7("${CMAKE_INSTALL_PREFIX}config/cattinglargefile.conf");
	Configuration<LargeFileCopyTest> conf8("${CMAKE_INSTALL_PREFIX}config/largefilecopy.conf");
	Configuration<MultipleFilesCatting> conf9("${CMAKE_INSTALL_PREFIX}config/multiplefilescatting.conf");
	Configuration<MultipleFilesCopy> conf10("${CMAKE_INSTALL_PREFIX}config/multiplefilescopy.conf");
	Configuration<SplitLargeFile> conf11("${CMAKE_INSTALL_PREFIX}config/splitlargefile.conf");
	Configuration<DeleterTest> conf12("${CMAKE_INSTALL_PREFIX}config/deleter.conf");
		
	TestCollection tests = conf1.Read();
	TestCollection tests2 = conf2.Read();
	TestCollection tests3 = conf3.Read();
	TestCollection tests4 = conf4.Read();
	TestCollection tests5 = conf5.Read();	
	TestCollection tests6 = conf6.Read();
	TestCollection tests7 = conf7.Read();
	TestCollection tests8 = conf8.Read();
	TestCollection tests9 = conf9.Read();
	TestCollection tests10 = conf10.Read();
	TestCollection tests11 = conf11.Read();
	TestCollection tests12 = conf12.Read();
	tests.Merge(tests2);
	tests.Merge(tests3);
	tests.Merge(tests4);
	tests.Merge(tests5);
	tests.Merge(tests6);
	tests.Merge(tests7);
	tests.Merge(tests8);
	tests.Merge(tests9);
	tests.Merge(tests10);
	tests.Merge(tests11);
	tests.Merge(tests12);
	
	TestResultCollection res = tests.Run();
		
	cout << res.ToXML() << endl;
}
