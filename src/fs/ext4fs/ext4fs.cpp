//      ext4fs.cpp
//      
//      Copyright 2011 Vahram Martirosyan <vmartirosyan@gmail.com>
//      
//      This program is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.

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
