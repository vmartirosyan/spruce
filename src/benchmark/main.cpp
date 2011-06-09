#include "Creator.hpp"
#include "LargeFile.hpp"
#include "Deleter.hpp"
#include "Compressor.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;
	tests.AddTest(new CreatorTest(Normal, MultipleFilesCreation, ""));
	tests.AddTest(new CreatorTest(Normal, LargeFileCreation, ""));
	tests.AddTest(new CompressTest(Normal, MultipleFilesCompression, ""));
	tests.AddTest(new DeleterTest(Normal, MultipleFilesDeletion, ""));
	
	TestResultCollection res = tests.Run();
		
	cout << res.ToXML() << endl;
}
