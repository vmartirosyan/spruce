//      ioctl_test.cpp
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
#include <ioctl_test.hpp>
#include <sys/mount.h>
#include <pwd.h>
#include <File.hpp>

//int Ext4IoctlTest::_file = -1;	
//int Ext4IoctlTest::_file_donor = -1;	

Ext4IoctlTest::Ext4IoctlTest(Mode m, int op, string a, string dev, string mtpt):
	//_DeviceName(dev),
	//_MountPoint(mtpt),
	Ext4fsTest(m,op,a, "ioctl")
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

Ext4fsTestResult * Ext4IoctlTest::Execute(vector<string> args)
{
	TestResult * p_res = Ext4fsTest::Execute(args);
	
	Ext4fsTestResult * t_res = new Ext4IoctlTestResult(p_res);
	
	delete p_res;
	
	return t_res;
}

int Ext4IoctlTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{
		//cerr << "Executing operation" << Operation::ToString((Operations)_operation) << endl;
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
	try
	{
		File file("ioctl_file");
		int _file = file.GetFileDescriptor();
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
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}

Status Ext4IoctlTest::TestClearExtentsFlags()
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
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}

Status Ext4IoctlTest::TestSetFlagsNotOwner()
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
		
		int flags = EXT4_EXTENTS_FL;
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
		
		result = ioctl(_file, EXT4_IOC_SETFLAGS, &flags );
		
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

Status Ext4IoctlTest::TestSetVersionGetVersion()
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
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}

Status Ext4IoctlTest::TestWaitForReadonly()
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
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}



Status Ext4IoctlTest::TestGroupExtend()
{	
	try
	{
		File file(MountPoint, S_IRUSR | S_IWUSR, O_DIRECTORY);
		int _file = file.GetFileDescriptor();	
		if ( _file == -1 )
		{
			cerr << "The file descriptor is invalid: " << strerror(errno);
			return Unres;
		}
		
		struct stat st;
		if ( fstat (_file, &st) == -1 )
		{
			cerr << "Cannot stat file " << MountPoint;
			return Unres;
		}
		
		cerr << "Shallow test";
		int NewPartitionSizeInBlocks = 210000;
		ioctl(_file, EXT4_IOC_GROUP_EXTEND, &NewPartitionSizeInBlocks);
		
		return Success;
		
		//Status status;
		
		NewPartitionSizeInBlocks = 210000;// _PartitionSizeInBlocks + 10;
		if ( ioctl(_file, EXT4_IOC_GROUP_EXTEND, &NewPartitionSizeInBlocks) == -1 )
		{
			cerr << "Error during online resize. " << strerror(errno);
			return Fail;
		}
		else
		{
			cerr << "Online resize was successful. " << strerror(errno);
			return Success;
		}
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}

Status Ext4IoctlTest::TestMoveExtent()
{
	try
	{
		File file("ioctl_file");
		int _file = file.GetFileDescriptor();	
		File file_donor("ioctl_donor_file");
		int _file_donor = file_donor.GetFileDescriptor();	
	
		
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
		
		//unsigned int BlockCount = 1;
		
		struct move_extent me;
		memset(&me, 0, sizeof(me));
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
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
	
}

Status Ext4IoctlTest::TestGroupAdd()
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
		
		struct ext4_new_group_data input;
		//memset(&input, 0, sizeof(input));		
		
		cerr << "Shallow test" << endl;
		ioctl(_file, EXT4_IOC_GROUP_ADD, &input);
		
		return Success;
		
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
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}

Status Ext4IoctlTest::TestMigrate()
{
	if ( chdir ("/") == -1 )
	{
		cerr << "Cannot change directory. Error : " << strerror(errno);
		return Unres;
	}
	
	//if ( umount2( MountPoint.c_str(), MNT_FORCE ) == -1 )	
	if ( umount( MountPoint.c_str() ) == -1 )
	{
		cerr << "Cannot unmount " << MountPoint << ". Error: " << strerror(errno);		
		return Unres;
	}
	//cerr << ("mkfs.ext2 " + _DeviceName).c_str() << endl;
	
	UnixCommand mkfs("mkfs.ext3");
	vector<string> args;			
	//args.push_back("-O");
	//args.push_back("extents");
	args.push_back(DeviceName);
	
	ProcessResult * res;
	res = mkfs.Execute(args);
	if ( res->GetStatus() != Success )
	{
		cerr << "Cannot create ext3 filesystem on device " << DeviceName << endl;
		cerr << "Error: " << res->GetOutput() << endl;
		return Unres;
	}
	
	if ( mount( DeviceName.c_str(), MountPoint.c_str(), "ext3", 0, 0) == -1)
	{
		cerr << "Cannot mount ext3 FS " << DeviceName << " to " << MountPoint << " Error: " << strerror(errno);
		return Unres;
	}
	
	Status result = Unknown;
	
	try
	{
		File file(MountPoint, S_IRUSR | S_IWUSR, O_DIRECTORY | O_RDONLY);
		int _file = file.GetFileDescriptor();	
	
		if ( _file == -1 )
		{
			cerr << "Cannot open folder: " << MountPoint << ". Error : " << strerror(errno);
			result = Unres;
			goto finally;
		}
		
		
		if ( ioctl(_file, EXT4_IOC_MIGRATE, 0) == -1 )
		{
			cerr << "Error migrating. " << strerror(errno) << endl;
			result = Fail;
			goto finally;
		}
		else
		{
			cerr << "Migration was successful. ";
			result = Success;
			goto finally;
		}
	}
	catch (Exception e)
	{
		// Restore the system state!
		
		cerr << "Exception was thrown: " << e.GetMessage();		
		result = Unres;
	}
finally:
	if ( umount( MountPoint.c_str() ) == -1 )
	{
		cerr << "Cannot unmount " << MountPoint << ". Error: " << strerror(errno);		
		return Unres;
	}
	
	UnixCommand mkfs2("mkfs.ext4");
	vector<string> args2;		
	args2.push_back(DeviceName);
	
	ProcessResult * res2;
	res2 = mkfs2.Execute(args2);
	if ( res2->GetStatus() != Success )
	{
		cerr << "Cannot create ext4 filesystem on device " << DeviceName << endl;
		cerr << "Error: " << res2->GetOutput() << endl;
		return Unres;
	}
	
	//system(("mkfs.ext2 " + _DeviceName).c_str());
	if ( mount( MountPoint.c_str(), DeviceName.c_str(), "ext4", 0, 0) == -1)
	{
		cerr << "Cannot mount " << DeviceName << " back to " << MountPoint << endl;
		cerr << "Error: " << strerror(errno);
		result = Unres;
	}
		
	return result;
}

Status Ext4IoctlTest::TestAllocDABlocks()
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
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
}

Status Ext4IoctlTest::TestFitrim()
{	
#ifdef FITRIM
	try
	{
		File file("ioctl_file");
		int _file = file.GetFileDescriptor();
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
	}
	catch (Exception e)
	{
		cerr << "Exception was thrown: " << e.GetMessage();
		return Unres;
	}
#else
	cerr << "FITRIM is not supported";
	return Unsupported;
#endif
}
Status Ext4IoctlTest::TestUnsupported()
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
