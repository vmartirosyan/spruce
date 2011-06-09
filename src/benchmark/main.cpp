#include "Creator.hpp"
#include "Deleter.hpp"
#include "Compressor.hpp"
#include "LargeFile.hpp"
#include "FindFile.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;
	tests.AddTest(new CreatorTest(Normal, MultipleFilesCreation, ""));
	tests.AddTest(new FindFileTest(Normal, FindFile, ""));
	tests.AddTest(new CompressTest(Normal, MultipleFilesCompression, ""));
	tests.AddTest(new DeleterTest(Normal, MultipleFilesDeletion, ""));
	tests.AddTest(new LargeFileTest(Normal, LargeFileCreation, ""));
	
	TestResultCollection res = tests.Run();
		
	cout << res.ToXML() << endl;
}
