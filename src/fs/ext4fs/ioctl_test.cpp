#include "ioctl_test.hpp"
#include <sys/capability.h>
#include <sys/mount.h>

int IoctlTest::_file = -1;	
int IoctlTest::_file_donor = -1;	


int IoctlTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{
		switch (_operation)
		{
			case SetFlagsGetFlags:
				return TestSetFlagsGetFlags();
			case ClearExtentsFlags:
				return TestClearExtentsFlags();
			case SetFlagsNotOwner:
				return TestSetFlagsNotOwner();
			case SetVersionGetVersion:
				return TestSetVersionGetVersion();
			case WaitForReadonly:
				return TestWaitForReadonly();
			case GroupExtend:
				return TestGroupExtend();
			case MoveExtent:
				return TestMoveExtent();
			case GroupAdd:
				return TestGroupAdd();
			case Migrate:
				return TestMigrate();
			case AllocDABlocks:
				return TestAllocDABlocks();
			case Fitrim:
				return TestFitrim();
			case Unsupported:
				return TestUnsupported();
			default:
				cerr << "Unsupported operation.";	
				return Unres;
		}
	}
	cerr << "Test was successful";	
	return Success;
}

Status IoctlTest::TestSetFlagsGetFlags()
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

Status IoctlTest::TestClearExtentsFlags()
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

Status IoctlTest::TestSetFlagsNotOwner()
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

Status IoctlTest::TestSetVersionGetVersion()
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

Status IoctlTest::TestWaitForReadonly()
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


Status IoctlTest::TestGroupExtend()
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

Status IoctlTest::TestMoveExtent()
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

Status IoctlTest::TestGroupAdd()
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

Status IoctlTest::TestMigrate()
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

Status IoctlTest::TestAllocDABlocks()
{
	if ( _file == -1 )
	{
		cerr << "The file descriptor is invalid: " << strerror(errno);
		return Unres;
	}
	
	if ( ioctl(_file, EXT4_IOC_ALLOC_DA_BLKS, 0) == -1 )
	{
		cerr << "Error allocating DA blocks. " << strerror(errno);
		return Fail;
	}
	else
	{
		cerr << "DA blocks allocation was successful. " << strerror(errno);
		return Success;
	}
	return Unknown;
}

Status IoctlTest::TestFitrim()
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
Status IoctlTest::TestUnsupported()
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
