#include "MultipleFilesCreation/Creator.hpp"
#include "MultipleFilesDeletion/Deleter.hpp"
#include "MultipleFilesCompression/Compressor.hpp"
#include "LargeFileCreation/LargeFile.hpp"
#include "FindFile/FindFile.hpp"
#include "LargeFileCopyTest/LargeFileCopyTest.hpp"
#include "MultipleFilesDecompression/Decompressor.hpp"
#include "MultipleFilesCopy/MultipleFilesCopy.hpp"
#include "CattingLargeFile/CattingLargeFile.hpp"
#include "MultipleFilesCatting/MultipleFilesCatting.hpp"
#include "SplitLargeFile/SplitLargeFile.hpp"
#include "SearchMapFile/SearchMapFile.hpp"
#include <config.hpp>

int main(int argc, char ** argv)
{
	Configuration<CreatorTest> conf1(INSTALL_PREFIX"/share/spruce/config/creator.conf");
	Configuration<FindFileTest> conf2(INSTALL_PREFIX"/share/spruce/config/findfile.conf");
	Configuration<CompressTest> conf3(INSTALL_PREFIX"/share/spruce/config/compression.conf");
	Configuration<DecompressTest> conf4(INSTALL_PREFIX"/share/spruce/config/decompression.conf");
	Configuration<LargeFileTest> conf5(INSTALL_PREFIX"/share/spruce/config/largefile.conf");
	Configuration<SearchMapFileTest> conf6(INSTALL_PREFIX"/share/spruce/config/searchmapfile.conf");
	Configuration<CattingLargeFile> conf7(INSTALL_PREFIX"/share/spruce/config/cattinglargefile.conf");
	Configuration<LargeFileCopyTest> conf8(INSTALL_PREFIX"/share/spruce/config/largefilecopy.conf");
	Configuration<MultipleFilesCatting> conf9(INSTALL_PREFIX"/share/spruce/config/multiplefilescatting.conf");
	Configuration<MultipleFilesCopy> conf10(INSTALL_PREFIX"/share/spruce/config/multiplefilescopy.conf");
	Configuration<SplitLargeFile> conf11(INSTALL_PREFIX"/share/spruce/config/splitlargefile.conf");
	Configuration<DeleterTest> conf12(INSTALL_PREFIX"/share/spruce/config/deleter.conf");
		
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
		
	cout << "<Benchmark>\n" << res.ToXML() << "</Benchmark>" << endl;
}
