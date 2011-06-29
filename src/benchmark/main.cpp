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
#include <config.hpp>

int main(int argc, char ** argv)
{
	Configuration<CreatorTest> conf1("/home/eduard/workspace/spruce/config/creator.conf");
	//Configuration<CreatorTest> conf2("/home/eduard/workspace/spruce/config/creator.conf");
	Configuration<FindFileTest> conf2("/home/eduard/workspace/spruce/config/findfile.conf");

	TestCollection tests = conf1.Read();
	TestCollection tests2 = conf2.Read();
	tests.Merge(tests2);
	
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
