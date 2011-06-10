#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

#include "test.hpp"
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
	MoveExtent,
	GroupAdd,
	Migrate,
	AllocDABlocks,
	Fitrim,
	Unsupported	
};

class IoctlTest : public Test
{
public:	
	IoctlTest(Mode m, int op, string a):
		Test(m, op, a)
	{
		//if ( _file == -1 )
			//_file = open("ioctl_file", O_CREAT | O_RDWR );
		//if ( _file_donor == -1 )
			//_file_donor = open("ioctl_donor_file", O_CREAT | O_RDWR );
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
	virtual Status Main(vector<string> args);
private:
	static int _file;
	static int _file_donor;
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
