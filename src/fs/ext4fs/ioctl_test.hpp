//      ioctl_test.hpp
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

#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

#include "ext4fs_test.hpp"
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ext4.hpp"
#include <fstream>

using std::ifstream;

// Operations
 enum IoctlOperations
 {
 	Ext4IoctlSetFlagsGetFlags,
 	Ext4IoctlClearExtentsFlags,
 	Ext4IoctlSetFlagsNotOwner,
 	Ext4IoctlSetVersionGetVersion,
 	Ext4IoctlWaitForReadonly,
 	Ext4IoctlGroupExtend,
 	Ext4IoctlMoveExtent,
 	Ext4IoctlGroupAdd,
 	Ext4IoctlMigrate,
 	Ext4IoctlAllocDABlocks,
 	Ext4IoctlFitrim,
 	Ext4IoctlUnsupported	
 };

class Ext4IoctlTestResult : public Ext4fsTestResult
{
public:
	Ext4IoctlTestResult(ProcessResult pr, int op, string args):
		Ext4fsTestResult(pr, op, args) {}
protected:
	virtual string OperationToString();
};

class Ext4IoctlTest : public Ext4fsTest
{
public:	
	Ext4IoctlTest(Mode m, int op, string a):
		Ext4fsTest(m, op, a)
	{
		Init();
	}
	
	Ext4IoctlTest(Mode m, int op, string a, string dev, string mtpt):
		_DeviceName(dev),
		_MountPoint(mtpt),
		Ext4fsTest(m,op,a)
		{
			Init();
			// Get the partition size.
			string ParentDev = _DeviceName.substr(5, 3);
			ifstream inf( ("/sys/block/" + ParentDev + "/" + _DeviceName.substr(5, 4) + "/size").c_str());
			int SizeInBytes = 0;
			inf >> SizeInBytes;
			_PartitionSizeInBlocks = SizeInBytes / 8;
		}
		
	virtual ~Ext4IoctlTest()
	{
		if ( _file > 0 )
		{
			close( _file );			
		}
		unlink("ioctl_file");
		
		if ( _file_donor > 0 )
		{
			close( _file_donor );			
		}
		unlink("ioctl_file_donor");
	}	
protected:	
	virtual ProcessResult * Execute(vector<string> args);
	virtual int Main(vector<string> args);
private:
	static int _file;
	static int _file_donor;
	int _PartitionSizeInBlocks; // Each 4096 byte
	string _DeviceName;
	string _MountPoint;
	void Init()
	{
		if ( _file == -1 )
			_file = open("ioctl_file", O_CREAT | O_RDWR );
		if ( _file_donor == -1 )
			_file_donor = open("ioctl_donor_file", O_CREAT | O_RDWR );
	}
	Status TestSetFlagsGetFlags();
	Status TestClearExtentsFlags();
	Status TestSetFlagsNotOwner();
	Status TestSetVersionGetVersion();
	Status TestWaitForReadonly();
	Status TestGroupExtend();
	Status TestMoveExtent();
	Status TestGroupAdd();
	Status TestMigrate();
	Status TestAllocDABlocks();
	Status TestFitrim();
	Status TestUnsupported();
};


#endif /* IOCTL_TEST_H */

