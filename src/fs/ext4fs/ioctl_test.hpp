#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

#include "test_base.hpp"
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ext4.h"

enum IoctlOperations
{
	SetFlagsGetFlags,
	ClearExtentsFlags,
	SetFlagsNotOwner,
	SetVersionGetVersion,
	WaitForReadonly,
	GroupExtend,
	MoveExtent
};

class IoctlTest : public Test
{
public:	
	IoctlTest(Mode m, int op, string a):
		Test(m, op, a)
	{
		_file = open("ioctl_file", O_CREAT | O_RDWR );
		_file_donor = open("ioctl_donor_file", O_CREAT | O_RDWR );
		
		/*_operations.push_back(pair<int, string>(SetFlagsGetFlags, "" ));
		_operations.push_back(pair<int, string>(ClearExtentsFlags, "" ));
		//_operations.push_back(pair<int, string>(SetFlagsNotOwner, "" ));
		_operations.push_back(pair<int, string>(SetVersionGetVersion, "" ));
		_operations.push_back(pair<int, string>(WaitForReadonly, "" ));
		_operations.push_back(pair<int, string>(GroupExtend, "" ));
		_operations.push_back(pair<int, string>(MoveExtent, "" ));*/
	}
		
	virtual ~IoctlTest()
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
	virtual Status Main();
private:
	int _file;
	int _file_donor;
	Status TestSetFlagsGetFlags();
	Status TestClearExtentsFlags();
	Status TestSetFlagsNotOwner();
	Status TestSetVersionGetVersion();
	Status TestWaitForReadonly();
	Status TestGroupExtend();
	Status TestMoveExtent();
};

#endif /* IOCTL_TEST_H */
