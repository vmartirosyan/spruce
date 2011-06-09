#include "Creator.hpp"
#include "Compressor.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;
	tests.AddTest(new CreatorTest(Normal, MultipleFilesCreation, ""));
	tests.AddTest(new CompressTest(Normal, MultipleFilesCompression, ""));
	
	TestResultCollection res = tests.Run();
		
	cout << res.ToXML() << endl;
}
