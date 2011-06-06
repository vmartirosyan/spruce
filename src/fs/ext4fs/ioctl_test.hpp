#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

#include "test_base.hpp"
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ext4.h"

enum Ioctl_Operations
{
	SetFlagsGetFlags,
	ClearExtentsFlags,
	SetFlagsNotOwner,
	SetVersionGetVersion	
};

class IoctlTest : public TestBase
{
public:	
	IoctlTest()
	{
		_file = open("ioctl_file", O_CREAT | O_RDONLY );
		
		_operations.push_back(pair<int, string>(SetFlagsGetFlags, "" ));
		_operations.push_back(pair<int, string>(ClearExtentsFlags, "" ));
		_operations.push_back(pair<int, string>(SetFlagsNotOwner, "" ));
		_operations.push_back(pair<int, string>(SetVersionGetVersion, "" ));
	}
		
	virtual ~IoctlTest()
	{
		if ( _file > 0 )
		{
			close( _file );
			unlink("ioctl_file");
		}
	}	
protected:	
	virtual Status RealRun(Mode mode, int operation, string args);
private:
	int _file;
	Status TestSetFlagsGetFlags();
	Status TestClearExtentsFlags();
	Status TestSetFlagsNotOwner();
	Status TestSetVersionGetVersion();
};

#endif /* IOCTL_TEST_H */
