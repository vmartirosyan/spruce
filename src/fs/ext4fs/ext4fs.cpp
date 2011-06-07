#include "ext4.h"
#include "ioctl_test.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;
	
	tests.AddTest(new IoctlTest(Normal, SetFlagsGetFlags, ""));
	
	TestResultCollection res = tests.Run();
	
	
	cout << res.ToXML() << endl;
}
