//      jfs_ioctl_test.hpp
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

#ifndef JFS_IOCTL_TEST_H
#define JFS_IOCTL_TEST_H

#include "jfs_test.hpp"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
//#include "ext4.hpp"
#include <fstream>

using std::ifstream;

extern string MountPoint;
extern string DeviceName;


// Operations
enum IoctlOperations
{
	JFSIoctlSetFlagsGetFlags,
	JFSIoctlClearExtentsFlags,
	JFSIoctlSetFlagsNotOwner,	
	JFSIoctlUnsupported	
};

class JFSIoctlTestResult : public JFSTestResult
{
public:
	JFSIoctlTestResult(TestResult* tr):
		JFSTestResult(tr, "ioctl") {}
protected:
	virtual string OperationToString();
};

class JFSIoctlTest : public JFSTest
{
public:	
	JFSIoctlTest(Mode m, int op, string a):
		JFSTest(m, op, a, "ioctl")
	{
		Init();
		//_MountPoint = envget("MountAt");
		//_DeviceName = envget("Partition");		
	}
	
	JFSIoctlTest(Mode m, int op, string a, string dev, string mtpt);

		
	virtual ~JFSIoctlTest()
	{
	/*	if ( _file > 0 )
		{
			close( _file );			
		}
		unlink("ioctl_file");
		
		if ( _file_donor > 0 )
		{
			close( _file_donor );			
		}
		unlink("ioctl_file_donor");*/
	}	
protected:	
	virtual JFSTestResult * Execute(vector<string> args);
	virtual int Main(vector<string> args);
private:
	//static int _file;
	//static int _file_donor;
	int _PartitionSizeInBlocks; // Each 4096 byte
	//string _DeviceName;
	//string _MountPoint;
	void Init()
	{
		/*if ( _file == -1 )
			_file = open("ioctl_file", O_CREAT | O_RDWR );
		if ( _file_donor == -1 )
			_file_donor = open("ioctl_donor_file", O_CREAT | O_RDWR );*/
	}
	Status TestSetFlagsGetFlags();
	Status TestClearExtentsFlags();
	Status TestSetFlagsNotOwner();	
	Status TestUnsupported();
};


#endif /* JFS_IOCTL_TEST_H */

