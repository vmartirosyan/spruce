#include "Creator.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;	
	tests.AddTest(new CreatorTest(Normal, MultipleFilesCreation, ""));
	
	TestResultCollection res = tests.Run();
		
	cout << res.ToXML() << endl;
}
