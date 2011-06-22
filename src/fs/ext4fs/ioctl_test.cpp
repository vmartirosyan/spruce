//      ioctl_test.cpp
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

#include "ioctl_test.hpp"
#include <sys/capability.h>
#include <sys/mount.h>

int Ext4IoctlTest::_file = -1;	
int Ext4IoctlTest::_file_donor = -1;	

ProcessResult * Ext4IoctlTest::Execute(vector<string> args)
{
	ProcessResult * p_res = Ext4fsTest::Execute(args);
	
	TestResult * t_res = new Ext4IoctlTestResult(*p_res, _operation, _args);
	
	delete p_res;
	
	return t_res;
}

int Ext4IoctlTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{
		switch (_operation)
		{
			case Ext4IoctlSetFlagsGetFlags:
				return TestSetFlagsGetFlags();
			case Ext4IoctlClearExtentsFlags:
				return TestClearExtentsFlags();
			case Ext4IoctlSetFlagsNotOwner:
				return TestSetFlagsNotOwner();
			case Ext4IoctlSetVersionGetVersion:
				return TestSetVersionGetVersion();
			case Ext4IoctlWaitForReadonly:
				return TestWaitForReadonly();
			case Ext4IoctlGroupExtend:
				return TestGroupExtend();
			case Ext4IoctlMoveExtent:
				return TestMoveExtent();
			case Ext4IoctlGroupAdd:
				return TestGroupAdd();
			case Ext4IoctlMigrate:
				return TestMigrate();
			case Ext4IoctlAllocDABlocks:
				return TestAllocDABlocks();
			case Ext4IoctlFitrim:
				return TestFitrim();
			case Ext4IoctlUnsupported:
				return TestUnsupported();
			default:
				cerr << "Unsupported operation.";	
				return Unres;
		}
	}
	cerr << "Test was successful";	
	return Success;
}

Status Ext4IoctlTest::TestSetFlagsGetFlags()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	int set_flags = EXT4_EXTENTS_FL | 1; // We may NOT clear the extents flag...
	int get_flags = 0;
	
	// Backup the old values just in case
	int old_flags;
	if ( ioctl(_file, EXT4_IOC_GETFLAGS, &old_flags) == -1 )
	{
		cerr << "Error backing up old values. " << strerror(errno);
		return Unres;
	}
	
	// Set our testing flag values
	if ( ioctl(_file, EXT4_IOC_SETFLAGS, &set_flags) == -1 )
	{
		cerr << "Error setting new flag values. " << strerror(errno);
		return Unres;
	}
	
	// Get the flags back
	if ( ioctl(_file, EXT4_IOC_GETFLAGS, &get_flags) == -1 )
	{
		cerr << "Error getting flag values back. " << strerror(errno);
		return Unres;
	}
	
	// Restore the original flags
	if ( ioctl(_file, EXT4_IOC_SETFLAGS, &old_flags) == -1 )
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

Status Ext4IoctlTest::TestClearExtentsFlags()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	int result = 0;
	
	// Backup the old values just in case
	int old_flags;
	if ( ioctl(_file, EXT4_IOC_GETFLAGS, &old_flags) == -1 )
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

Status Ext4IoctlTest::TestSetFlagsNotOwner()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	int flags = 1;
	int result = 0;
	
	cap_user_header_t header = new __user_cap_header_struct;
	cap_user_data_t data = new __user_cap_data_struct;
	
	header->version = _LINUX_CAPABILITY_VERSION_3;
	header->pid = getpid();
	
	data->permitted |= CAP_CHOWN;
	data->inheritable |= CAP_CHOWN;
	data->effective |= CAP_CHOWN;
	
	if ( capset( header, data ) )
	{
		cerr << "Cannot set file capabilities: " <<  strerror(errno);
		return Unres;
	}
	
	if ( fchown(_file, 0, 0) )
	{
		cerr << "Cannot change the file owner: " <<  strerror(errno);
		return Unres;
	}
	
	// Try to set the non-permitted flag
	if ( ( result = ioctl(_file, EXT4_IOC_SETFLAGS, &flags ) ) != -EACCES )
	{
		cerr << "It was permitted to set the flag not being file owner!. " << strerror(errno);
		return Fail;
	}
	else 
	{
		cerr << "It was NOT permitted to set flag not being file owner!. " << strerror(errno);
		return Success;
	}
}

Status Ext4IoctlTest::TestSetVersionGetVersion()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	int set_version = 10; 
	int get_version = 0;
	
	// Backup the old version just in case
	int old_version;
	if ( ioctl(_file, EXT4_IOC_GETVERSION, &old_version) == -1 )
	{
		cerr << "Error backing up old version. " << strerror(errno);
		return Unres;
	}
	
	// Set our testing version value
	if ( ioctl(_file, EXT4_IOC_SETVERSION, &set_version) == -1 )
	{
		cerr << "Error setting new version values. " << strerror(errno);
		return Unres;
	}
	
	// Get the version back
	if ( ioctl(_file, EXT4_IOC_GETVERSION, &get_version) == -1 )
	{
		cerr << "Error getting version value back. " << strerror(errno);
		return Unres;
	}
	
	// Restore the original version
	if ( ioctl(_file, EXT4_IOC_SETVERSION, &old_version) == -1 )
	{
		cerr << "Error restoring old version value. " << strerror(errno);
		return Unres;
	}
	
	// Compare them
	if ( get_version != set_version )
	{
		cerr << "Set and Get version mismatch";
		return Fail;
	}
	else
	{
		cerr << "Set and Get versions match";
		return Success;
	}
}

Status Ext4IoctlTest::TestWaitForReadonly()
{

	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	if ( ioctl(_file, EXT4_IOC_WAIT_FOR_READONLY, NULL) == -1 )
	{
		cerr << "Error waiting for readonly. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Wait for readonly was successful. " << strerror(errno);
		return Success;
	}
}

extern string MountPoint;


Status Ext4IoctlTest::TestGroupExtend()
{	
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	close(_file);
	unlink("ioctl_file");
	_file = open(MountPoint.c_str(), O_RDONLY);
	
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	Status status;
	
	if ( ioctl(_file, EXT4_IOC_GROUP_EXTEND, &_PartitionSizeInBlocks) == -1 )
	{
		cerr << "Error during online resize. " << strerror(errno);
		status = Fail;
	}
	else
	{
		cerr << "Online resize was successful. " << strerror(errno);
		status = Success;
	}
	
out:
	close(_file);
	_file = open("ioctl_file", O_RDWR);
	return status;
}

Status Ext4IoctlTest::TestMoveExtent()
{

	if ( _file == -1 || _file_donor == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	unlink("ioctl_file_donor");
	
	if ( fallocate(_file_donor, 0, 0, 10*4096) ) // 10 blocks
	{
		cerr << "Cannot allocate space for donor file: " << strerror(errno);
		return Unres;
	}
	
	
	if ( fallocate(_file, 0, 0, 10*4096) ) // 10 blocks
	{
		cerr << "Cannot allocate space for original file: " << strerror(errno);
		return Unres;
	}
	
	unsigned int BlockCount = 1;
	
	struct move_extent me;
	memset(&me, 0, sizeof(me));
	//me.orig_fd = _file;
	me.donor_fd = _file_donor;
	me.orig_start = 0;
	me.donor_start = 0;
	me.len = 10;
	me.moved_len = 10;
	
	
	if ( ioctl(_file, EXT4_IOC_MOVE_EXT, &me) == -1 )
	{
		cerr << "Error moving extent. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Move extent was successful. " ;
		return Success;
	}
}

Status Ext4IoctlTest::TestGroupAdd()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	struct ext4_new_group_data input;
	memset(&input, 0, sizeof(input));
	
	if ( ioctl(_file, EXT4_IOC_GROUP_ADD, &input) == -1 )
	{
		cerr << "Error adding group. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Group was added successful. " << strerror(errno);
		return Success;
	}
	return Unknown;
}

Status Ext4IoctlTest::TestMigrate()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	close(_file);
	
	
	//umount( (_MountPoint).c_str());
	//cerr << ("mkfs.ext2 " + _DeviceName).c_str() << endl;
	//system(("mkfs.ext2 " + _DeviceName).c_str());
	//mount(_MountPoint.c_str(), _DeviceName.c_str(), "ext2", 0, 0);
	_file = open(_MountPoint.c_str(), O_RDONLY);
	
	if ( ioctl(_file, EXT4_IOC_MIGRATE, 0) == -1 )
	{
		cerr << "Error migrating. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Migration was successful. ";
		return Success;
	}
	return Unknown;
}

Status Ext4IoctlTest::TestAllocDABlocks()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	if ( ioctl(_file, EXT4_IOC_ALLOC_DA_BLKS, 0) == -1 )
	{
		cerr << "Error during delayed allocation of blocks. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "Blocks delayed allocation was successful. " << strerror(errno);
		return Success;
	}
	return Unknown;
}

Status Ext4IoctlTest::TestFitrim()
{
	cerr << "Fitrim???";
#ifdef FITRIM	
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	if ( ioctl(_file, FITRIM, 0) == -1 )
	{
		cerr << "Error FITRIM. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "FITRIM was successful. " << strerror(errno);
		return Success;
	}
#else
	return Unres;
#endif
}
Status Ext4IoctlTest::TestUnsupported()
{
	cerr << "Unsupported...";
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
	return Unknown;
}

string Ext4IoctlTestResult::OperationToString()
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
