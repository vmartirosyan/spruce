#include "ext4.h"
#include "ioctl_test.hpp"

int main(int argc, char ** argv)
{
	TestBase * test = new IoctlTest();
	TestResultCollection res = test->Run();
	delete test;
	
	cout << res.ToXML() << endl;
}
