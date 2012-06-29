//      jfs.cpp
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

#include <jfs_ioctl_test.hpp>

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
	
	// Skip the log file for now
	if ( getenv("Partition") )
		DeviceName = getenv("Partition");
	
	if ( getenv("MountAt") )
		MountPoint = getenv("MountAt");
	
	
	Configuration<JFSIoctlTest> conf("${CMAKE_INSTALL_PREFIX}/share/spruce/config/jfs.conf");
	//Configuration<Ext4IoctlTest> conf("/home/vmartirosyan/workspace/spruce/config/ext4fs.conf");
	
	TestCollection tests = conf.Read();	
	
	TestResultCollection res = tests.Run();
	
	cout << "<Module Name=\"jfs\">" << res.ToXML() << "</Module>" << endl;
}
