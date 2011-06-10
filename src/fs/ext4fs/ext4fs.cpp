#include "ext4.h"
#include "ioctl_test.hpp"
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>

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
	
	mkdir( MountPoint.c_str(), 0x0777 );
	mount ( DeviceName.c_str(), MountPoint.c_str(), "ext4", 0 ,0);
	
	TestCollection tests;
	
	tests.AddTest(new IoctlTest(Normal, SetFlagsGetFlags, ""));
	tests.AddTest(new IoctlTest(Normal, ClearExtentsFlags, ""));
	tests.AddTest(new IoctlTest(Normal, SetFlagsNotOwner, ""));
	tests.AddTest(new IoctlTest(Normal, SetVersionGetVersion, ""));
	//tests.AddTest(new IoctlTest(Normal, WaitForReadonly, ""));
	tests.AddTest(new IoctlTest(Normal, GroupExtend, "", DeviceName, MountPoint));
	tests.AddTest(new IoctlTest(Normal, MoveExtent, ""));
	tests.AddTest(new IoctlTest(Normal, GroupAdd, ""));
	tests.AddTest(new IoctlTest(Normal, Migrate, "", DeviceName, MountPoint));
	tests.AddTest(new IoctlTest(Normal, AllocDABlocks, ""));
	//tests.AddTest(new IoctlTest(Normal, Fitrim, ""));
	tests.AddTest(new IoctlTest(Normal, Unsupported, ""));
	
	TestResultCollection res = tests.Run();
	
	cout << res.ToXML() << endl;
}

int Usage(char ** argv)
{
	cerr << "Usage: " << argv[0] << " <log_file> <device> <mountpoint>" << endl;
	return -2;
}
