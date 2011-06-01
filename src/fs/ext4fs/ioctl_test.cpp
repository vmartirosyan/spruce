#include "ioctl_test.hpp"

TestResultCollection IoctlTest::Run()
{
	_operations.push_back(pair<int, string>(SetVersionGetVersion, "" ));
	_operations.push_back(pair<int, string>(SetFlagsGetFlags, "" ));
	
	return TestBase::Run();
}

Status IoctlTest::RealRun(int operation, string args)
{
	switch (operation)
	{
		case SetFlagsGetFlags:
			return TestSetFlagsGetFlags();
		case SetVersionGetVersion:
			return TestSetVersionGetVersion();
		default:
			cerr << "Unsupported operation.";	
			return Unres;
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
