#include "ext4.h"
#include "ioctl_test.hpp"

string DeviceName = "";
string MountPoint = "";

int Usage(char ** argv);

int main(int argc, char ** argv)
{
	if ( geteuid() != 0 )
	{
		// The tests should be executed by the root user;
		cerr << "Only root can execute these tests..." << endl;
		return -1;
	}
	
	/*
	 * Usage: extfs <log_file> <device> <mountpoint>
	 * */
	 
	if ( argc < 4 )
		return Usage(argv);
		
	// Skip the log file for now
	DeviceName = argv[2];
	MountPoint = argv[3];
	
	system(("mkdir " + MountPoint).c_str());
	system(("mount " + DeviceName + " " + MountPoint).c_str());
	
	TestCollection tests;
	
	//tests.AddTest(new IoctlTest(Normal, SetFlagsGetFlags, ""));
	//tests.AddTest(new IoctlTest(Normal, ClearExtentsFlags, ""));
	//tests.AddTest(new IoctlTest(Normal, SetFlagsNotOwner, ""));
	//tests.AddTest(new IoctlTest(Normal, SetVersionGetVersion, ""));
	//tests.AddTest(new IoctlTest(Normal, WaitForReadonly, ""));
	tests.AddTest(new IoctlTest(Normal, GroupExtend, ""));
	//tests.AddTest(new IoctlTest(Normal, MoveExtent, ""));
	//tests.AddTest(new IoctlTest(Normal, GroupAdd, ""));
	//tests.AddTest(new IoctlTest(Normal, Migrate, ""));
	//tests.AddTest(new IoctlTest(Normal, AllocDABlocks, ""));
	//tests.AddTest(new IoctlTest(Normal, Fitrim, ""));
	//tests.AddTest(new IoctlTest(Normal, Unsupported, ""));
	
	TestResultCollection res = tests.Run();
	
	cout << res.ToXML() << endl;
}

int Usage(char ** argv)
{
	cerr << "Usage: " << argv[0] << " <log_file> <device> <mountpoint>" << endl;
	return -2;
}
