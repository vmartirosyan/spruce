//      jfs_ioctl_test.cpp
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

#include <UnixCommand.hpp>
#include "jfs_ioctl_test.hpp"
#include <pwd.h>
#include <File.hpp>

//int Ext4IoctlTest::_file = -1;	
//int Ext4IoctlTest::_file_donor = -1;	

JFSIoctlTest::JFSIoctlTest(Mode m, int op, string a, string dev, string mtpt):
	//_DeviceName(dev),
	//_MountPoint(mtpt),
	JFSTest(m,op,a, "ioctl")
{
	Init();
	// Get the partition size.
	/*string ParentDev = DeviceName.substr(5, 3);
	cerr << "/sys/block/" + ParentDev + "/" + DeviceName.substr(5, 4) + "/size" << endl;
	ifstream inf( ("/sys/block/" + ParentDev + "/" + DeviceName.substr(5, 4) + "/size").c_str());
	int SizeInBytes = 0;
	inf >> SizeInBytes;
	_PartitionSizeInBlocks = SizeInBytes / 8;*/
}

JFSTestResult * JFSIoctlTest::Execute(vector<string> args)
{
	TestResult * p_res = JFSTest::Execute(args);
	
	JFSTestResult * t_res = new JFSIoctlTestResult(p_res);
	
	delete p_res;
	
	return t_res;
}

int JFSIoctlTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{
		//cerr << "Executing operation" << Operation::ToString((Operations)_operation) << endl;
		switch (_operation)
		{
			case JFSIoctlSetFlagsGetFlags:
				return TestSetFlagsGetFlags();
			case JFSIoctlClearExtentsFlags:
				return TestClearExtentsFlags();
			case JFSIoctlSetFlagsNotOwner:
				return TestSetFlagsNotOwner();
			case JFSIoctlUnsupported:
				return TestUnsupported();
			default:
				cerr << "Unsupported operation.";	
				return Unres;
		}
	}
	cerr << "Test was successful";	
	return Success;
}

Status JFSIoctlTest::TestSetFlagsGetFlags()
{
	try
	{
		File file("ioctl_file");
		int _file = file.GetFileDescriptor();
		if ( _file == -1 )
		{
			cerr << "The file descriptor is invalid: " << strerror(errno);
			return Unres;
		}
		
		int set_flags = 1; // We may NOT clear the extents flag...
		int get_flags = 0;
		
		// Backup the old values just in case
		int old_flags;
		if ( ioctl(_file, FS_IOC_GETFLAGS, &old_flags) == -1 )
		{
			cerr << "Error backing up old values. " << strerror(errno);
			return Unres;
		}
		
		// Set our testing flag values
		if ( ioctl(_file, FS_IOC_SETFLAGS, &set_flags) == -1 )
		{
			cerr << "Error setting new flag values. " << strerror(errno);
			return Unres;
		}
		
		// Get the flags back
		if ( ioctl(_file, FS_IOC_GETFLAGS, &get_flags) == -1 )
		{
			cerr << "Error getting flag values back. " << strerror(errno);
			return Unres;
		}
		
		// Restore the original flags
		if ( ioctl(_file, FS_IOC_SETFLAGS, &old_flags) == -1 )
		{
			cerr << "Error restoring old flag values. " << strerror(errno);
			return Unres;
		}
		
		// Compare them
		if ( get_flags != set_flags )
		{
			cerr << "Set and Get flags mismatch";
			return Fail;
		}
		else
		{
			cerr << "Set and Get flags match";
			return Success;
		}
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}

Status JFSIoctlTest::TestClearExtentsFlags()
{
	return Unsupported;
	/*try
	{
		File file("ioctl_file");
		int _file = file.GetFileDescriptor();
		if ( _file == -1 )
		{
			cerr << "The file descriptor is invalid: " << strerror(errno);
			return Unres;
		}
		
		int result = 0;
		
		// Backup the old values just in case
		int old_flags;
		if ( ioctl(_file, FS_IOC_GETFLAGS, &old_flags) == -1 )
		{
			cerr << "Error backing up old values. " << strerror(errno);
			return Unres;
		}
		
		int non_permitted_flags = old_flags & ~EXT4_EXTENTS_FL; // We may NOT clear the extents flag... but we shall try!
		
		// Try to set the non-permitted flag
		if ( ( result = ioctl(_file, EXT4_IOC_SETFLAGS, &non_permitted_flags ) ) == 0  )
		{
			cerr << "It was permitted to set non-permitted flag!. " << strerror(errno);
			return Fail;
		}
		else 
		{
			cerr << "It was NOT permitted to set non-permitted flag!. " << strerror(errno);
			return Success;
		}
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}*/
}

Status JFSIoctlTest::TestSetFlagsNotOwner()
{
	try
	{
		File file("ioctl_file");
		int _file = file.GetFileDescriptor();
		if ( _file == -1 )
		{
			cerr << "The file descriptor is invalid: " << strerror(errno);
			return Unres;
		}
		
		int flags = 1;
		int result = 0;
		
		struct passwd* Nobody = getpwnam("nobody");
		if ( Nobody == NULL )
		{
			cerr << "Cannot obtain information about the 'nobody' user. " << strerror(errno);
			return Unres;
		}
		
		if (  seteuid( Nobody->pw_uid ) == -1 )
		{
			cerr << "Cannot change the user ID: " <<  strerror(errno);
			return Unres;
		}
		
		// Try to set the non-permitted flag
		
		result = ioctl(_file, FS_IOC_SETFLAGS, &flags );
		
		// Restore root user
		seteuid(0);
		
		if ( result == 0 )
		{
			cerr << "It was permitted to set the flag not being file owner!. ";
			return Fail;
		}
		else if ( result == -1 && errno != EACCES )
		{
			cerr << "Error while setting flags. " << strerror(errno);
			return Fail;
		}
		else 
		{
			cerr << "It was NOT permitted to set flag not being file owner!. " << strerror(errno);
			return Success;
		}
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}

Status JFSIoctlTest::TestUnsupported()
{
	try
	{
		File file("ioctl_file");
		int _file = file.GetFileDescriptor();
		if ( _file == -1 )
		{
			cerr << "The file descriptor is invalid: " << strerror(errno);
			return Unres;
		}
		
		if ( ( ioctl(_file, -100, 0) == -1 ) && ( errno == ENOTTY ) )
		{
			cerr << "Unsupported command was processed successfully. " << strerror(errno);
			return Success;
		}
		else
		{
			cerr << "Error on unsupported command. " << strerror(errno);
			return Fail;
		}
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}

string JFSIoctlTestResult::OperationToString()
{
	return Operation::ToString(static_cast<Operations>(_operation));
/*	switch (_operation)
		{
			case SetFlagsGetFlags:
				return "SetFlagsGetFlags";
			case ClearExtentsFlags:
				return "ClearExtentsFlags";
			case SetFlagsNotOwner:
				return "SetFlagsNotOwner";
			case SetVersionGetVersion:
				return "SetVersionGetVersion";
			case WaitForReadonly:
				return "WaitForReadonly";
			case GroupExtend:
				return "OnlineResize";
			case MoveExtent:
				return "MoveExtent";
			case GroupAdd:
				return "GroupAdd";
			case Migrate:
				return "Migrate";
			case AllocDABlocks:
				return "AllocDABlocks";
			case Fitrim:
				return "Fitrim";
			case Unsupported:
			default:
				return "Unsupported";			
		}*/
}
