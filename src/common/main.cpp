#include "UnixCommand.hpp"
#include <test.hpp>

int main(int argc, char ** argv)
{
	TestCollection tests;		
	TestResultCollection res = tests.Run();	
	cout << res.ToXML() << endl;	
}
