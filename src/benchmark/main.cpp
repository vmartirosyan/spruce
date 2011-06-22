#include "Creator.hpp"
#include "Deleter.hpp"
#include "Compressor.hpp"
#include "LargeFile.hpp"
#include "FindFile.hpp"
#include "LargeFileCopyTest.hpp"
#include "Decompressor.hpp"
#include "MultipleFilesCopy.hpp"
#include "CattingLargeFile.hpp"
#include "SearchMapFile.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;
	tests.AddTest(new CreatorTest(Normal, MultipleFilesCreation, ""));
	tests.AddTest(new FindFileTest(Normal, FindFile, ""));
	tests.AddTest(new CompressTest(Normal, MultipleFilesCompression, ""));
	tests.AddTest(new DecompressTest(Normal, MultipleFilesDecompression, ""));
	tests.AddTest(new LargeFileTest(Normal, LargeFileCreation, ""));
	tests.AddTest(new LargeFileCopyTest(Normal, LargeFileCopying, ""));
	tests.AddTest(new MultipleFilesCopy(Normal, MultipleFilesCopyOp, ""));
	tests.AddTest(new CattingLargeFile(Normal, CattingLargeFileOp, ""));	
	tests.AddTest(new SearchMapFileTest(Normal, SearchMapFile, ""));
	tests.AddTest(new DeleterTest(Normal, MultipleFilesDeletion, ""));
	
	TestResultCollection res = tests.Run();
		
	cout << res.ToXML() << endl;
}
