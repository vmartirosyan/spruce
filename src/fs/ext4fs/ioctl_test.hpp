#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

#include "test.hpp"
#include <fcntl.h>
#include <sys/ioctl.h>
#include "ext4.h"
#include <fstream>

using std::ifstream;

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
		Init();
	}
	
	IoctlTest(Mode m, int op, string a, string dev, string mtpt):
		_DeviceName(dev),
		_MountPoint(mtpt),
		Test(m,op,a)
		{
			Init();
			// Get the partition size.
			string ParentDev = _DeviceName.substr(5, 3);
			ifstream inf( ("/sys/block/" + ParentDev + "/" + _DeviceName.substr(5, 4) + "/size").c_str());
			int SizeInBytes = 0;
			inf >> SizeInBytes;
			_PartitionSizeInBlocks = SizeInBytes / 8;
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
