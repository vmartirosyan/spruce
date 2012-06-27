//      ext4fs.cpp
//      
//      Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Author:
//			Vahram Martirosyan <vmartirosyan@gmail.com>
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

#include <ext4.hpp>
#include <ioctl_test.hpp>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <config.hpp>

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
	 
	//if ( argc < 4 )
	//	return Usage(argv);
	
	// Skip the log file for now
	if ( getenv("Partition") )
		DeviceName = getenv("Partition");
	
	if ( getenv("MountAt") )
		MountPoint = getenv("MountAt");
	
	
	//mkdir( MountPoint.c_str(), 0x0777 );
	//mount ( DeviceName.c_str(), MountPoint.c_str(), "ext4", 0 ,0);
	
	cout << "Executing ext4 fs tests for " << MountPoint << " (" << DeviceName << ")" << endl;
	
	Configuration<Ext4IoctlTest> conf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/ext4fs.conf");
	//Configuration<Ext4IoctlTest> conf("/home/vmartirosyan/workspace/spruce/config/ext4fs.conf");
	
	TestCollection tests = conf.Read();	
	
	//Configuration<Ext4IoctlTest> conf1("/home/vmartirosyan/workspace/spruce/config/ext4fs.conf");
	
	//TestCollection tests2 = conf1.Read();
	
	//tests.Merge(tests2);
	
	//Configuration<Ext4IoctlTest> conf2("/home/vmartirosyan/workspace/spruce/config/ext4fs.conf");
	
	//tests2 = conf2.Read();
	
	//tests.Merge(tests2);
	
	TestResultCollection res = tests.Run();
	
	cout << res.ToXML() << endl;
	
	/*TestResultCollection res2 = tests2.Run();
	
	cout << res2.ToXML() << endl;*/
}

int Usage(char ** argv)
{
	cerr << "Usage: " << argv[0] << " <log_file> <device> <mountpoint>" << endl;
	return -2;
}
