#ifndef IOCTL_TEST_H
#define IOCTL_TEST_H

#include "test_base.hpp"
#include <fcntl.h>
#include <sys/ioctl.h>

enum Ioctl_Operations
{
	SetFlagsGetFlags,
	SetVersionGetVersion
};

class IoctlTest : public TestBase
{
public:	
	IoctlTest()
	{
		_file = open("ioctl_file", O_CREAT | O_RDONLY );
	}
		
	virtual ~IoctlTest()
	{
		if ( _file > 0 )
		{
			close( _file );
			unlink("ioctl_file");
		}
	}
	virtual TestResultCollection Run();
protected:	
	virtual Status RealRun(int operation, string args);
private:
	int _file;
	Status TestSetFlagsGetFlags();
	Status TestSetVersionGetVersion();
};

#endif /* IOCTL_TEST_H */
