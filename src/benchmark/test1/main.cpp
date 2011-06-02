#include "Creator.hpp"

int main(int argc, char ** argv)
{
	TestBase *test = new CreatorTest();
	TestResultCollection res = test->Run();
	delete test;
	
	cout << res.ToXML() << endl;
}
