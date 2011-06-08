#include "ext4.h"
#include "ioctl_test.hpp"

int main(int argc, char ** argv)
{
	TestCollection tests;
	
	tests.AddTest(new IoctlTest(Normal, SetFlagsGetFlags, ""));
	tests.AddTest(new IoctlTest(Normal, ClearExtentsFlags, ""));
	tests.AddTest(new IoctlTest(Normal, SetFlagsNotOwner, ""));
	tests.AddTest(new IoctlTest(Normal, SetVersionGetVersion, ""));
	tests.AddTest(new IoctlTest(Normal, WaitForReadonly, ""));
	tests.AddTest(new IoctlTest(Normal, GroupExtend, ""));
	tests.AddTest(new IoctlTest(Normal, MoveExtent, ""));
	tests.AddTest(new IoctlTest(Normal, GroupAdd, ""));
	tests.AddTest(new IoctlTest(Normal, Migrate, ""));
	tests.AddTest(new IoctlTest(Normal, AllocDABlocks, ""));
	tests.AddTest(new IoctlTest(Normal, Fitrim, ""));
	tests.AddTest(new IoctlTest(Normal, Unsupported, ""));
	
	TestResultCollection res = tests.Run();
	
	cout << res.ToXML() << endl;
}
