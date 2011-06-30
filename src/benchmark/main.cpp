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
	Configuration<CreatorTest> conf1("/home/eduard/workspace/spruce/config/creator.conf");
	Configuration<FindFileTest> conf2("/home/eduard/workspace/spruce/config/findfile.conf");
	Configuration<CompressTest> conf3("/home/eduard/workspace/spruce/config/compression.conf");
	Configuration<DecompressTest> conf4("/home/eduard/workspace/spruce/config/decompression.conf");
	Configuration<LargeFileTest> conf5("/home/eduard/workspace/spruce/config/largefile.conf");
	Configuration<SearchMapFileTest> conf6("/home/eduard/workspace/spruce/config/searchmapfile.conf");
		
	TestCollection tests = conf1.Read();
	TestCollection tests2 = conf2.Read();
	TestCollection tests3 = conf3.Read();
	TestCollection tests4 = conf4.Read();
	TestCollection tests5 = conf5.Read();
	TestCollection tests6 = conf6.Read();
	tests.Merge(tests2);
	tests.Merge(tests3);
	tests.Merge(tests4);
	tests.Merge(tests5);
	tests.Merge(tests6);
	
	/*TestCollection tests;
	tests.AddTest(new CreatorTest(Normal, MultipleFilesCreation, ""));
	tests.AddTest(new FindFileTest(Normal, FindFile, ""));
	tests.AddTest(new CompressTest(Normal, MultipleFilesCompression, ""));
	tests.AddTest(new DecompressTest(Normal, MultipleFilesDecompression, ""));
	tests.AddTest(new LargeFileTest(Normal, LargeFileCreation, ""));
	tests.AddTest(new LargeFileCopyTest(Normal, LargeFileCopying, ""));
	
	tests.AddTest(new SplitLargeFile(Normal, SplitLargeFileOp, ""));
	
	tests.AddTest(new MultipleFilesCopy(Normal, MultipleFilesCopyOp, ""));
	tests.AddTest(new MultipleFilesCatting(Normal, MultipleFilesCattingOp, ""));
	tests.AddTest(new CattingLargeFile(Normal, CattingLargeFileOp, ""));*/
	//tests.AddTest(new DeleterTest(Normal, MultipleFilesDeletion, ""));
	
	TestResultCollection res = tests.Run();
		
	cout << res.ToXML() << endl;
}
