//      DupFileDescriptor.cpp
//      
//		Copyright (C) 2011, Institute for System Programming
//                          of the Russian Academy of Sciences (ISPRAS)
//      Authors:
//      	Narek Saribekyan <narek.saribekyan@gmail.com>
//      	Ruzanna Karakozova <r.karakozova@gmail.com>

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

#include "DupFileDescriptor.hpp"
#include "File.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/kdev_t.h>

int DupFileDescriptorTest::Main(vector<string> args)
{	
	if (_mode == Normal)
	{		
		switch (_operation)
		{		
			case Dup:
				return DupTest(args);
			case Dup2:
				return Dup2Test(args);
			case Dup3:			
				return Dup3Test(args);
			case DupErrEBADF:
				return DupErrEBADFTest(args);
			case Dup2ErrEBADF:
				return Dup2ErrEBADFTest(args);
			case Dup3ErrEBADF:
				return Dup3ErrEBADFTest(args);
			case Dup3ErrEINVAL:
				return Dup3ErrEINVALTest(args);
			default:
				return Unknown; 
		}
	}
	else
	{		
	}
	return 0;
} 


int DupFileDescriptorTest::DupTest(vector<string> args)
{	
	int status = Success;
	
	try 
	{		
		File file("newfile");
		int fd = file.GetFileDescriptor();		
		int newFd = dup(fd);
				
		if (newFd == -1)
		{				
			cerr << "System call dup failed: " << strerror(errno);
			return Fail; 
		}	
		
		struct stat oldFileStat, newFileStat;
		if(fstat(fd, &oldFileStat) == -1 || fstat(newFd, &newFileStat) == -1)
		{
			cerr << "System call fstat failed: " << strerror(errno);
			status = Unres;
		}
		else if(!statsEqual(oldFileStat, newFileStat))
		{
			cerr << "System call dup failed: first and second arguments fd's stats are not equal";
			status = Fail;		
		}
		
		int newFlag = fcntl(fd, F_GETFD);
		if(newFlag < 0)
		{
			cerr << "System call fcntl failed: " << strerror(errno);
			status = Unres;
		}	
		else if(!cloexecFlagOff(newFlag))
		{
			cerr << "System call dup failed: O_CLOEXEC flag isn't off" << strerror(errno);
			status = Fail;
		}
		
		int statusFlag = fcntl(fd, F_GETFL);
		int newStatusFlag = fcntl(newFd, F_GETFL);
		if(statusFlag < 0 || newStatusFlag < 0)
		{
			cerr << "System call fcntl failed: " << strerror(errno);
			status = Unres;		
		}
		else if(!statusFlagsEqual(statusFlag, newStatusFlag))
		{
			cerr << "System call dup failed: first and second arguments fd's status flags are not equal" << strerror(errno);
			status = Fail;
		}
		
		int fdOffset = lseek(fd, 0, SEEK_CUR);
		int newFdOffset = lseek(newFd, 0, SEEK_CUR);
		if(fdOffset == -1 || newFdOffset == -1)
		{
			cerr << "System call lseek failed: " << strerror(errno);
			status = Unres;
		} 
		else if(fdOffset != newFdOffset)
		{
			cerr << "System call dup failed: first and second arguments fd's offsets are not equal" << strerror(errno);
			status = Fail;
		}
		
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		status = Unres;
	}
	
	return status;
}

int DupFileDescriptorTest::Dup2Test(vector<string> args)
{	
	int status = Success;
	
	try 
	{		
		File file1("newfile1");
		
		int fd = file1.GetFileDescriptor();		
		int newFd = 7;
		
		if(dup2(fd, newFd) == -1)
		{
			cerr << "System call dup2 failed: " << strerror(errno);
			return Fail; 
		}	
		
		struct stat oldFileStat, newFileStat;
		if(fstat(fd, &oldFileStat) == -1 || fstat(newFd, &newFileStat) == -1)
		{
			cerr << "System call fstat failed: " << strerror(errno);
			status = Unres;
		}
		else if(!statsEqual(oldFileStat, newFileStat))
		{
			cerr << "System call dup2 failed: first and second arguments fd's stats are not equal" << strerror(errno);
			status = Fail;		
		}
		
		int newFlag = fcntl(fd, F_GETFD);
		if(newFlag < 0)
		{
			cerr << "System call fcntl failed: " << strerror(errno);
			status = Unres;
		}
		else if(!cloexecFlagOff(newFlag))
		{
			cerr << "System call dup2 failed: O_CLOEXEC flag isn't off" << strerror(errno);
			status = Fail;
		}
		
		int statusFlag = fcntl(fd, F_GETFL);
		int newStatusFlag = fcntl(newFd, F_GETFL);
		if(statusFlag < 0 || newStatusFlag < 0)
		{ 
			cerr << "System call fcntl failed: first and second arguments fd's status flags are not equal" << strerror(errno);
			status = Unres;		
		}
		else if(!statusFlagsEqual(statusFlag, newStatusFlag))
		{
			cerr << "System call dup2 failed: " << strerror(errno);
			status = Fail;
		}	

		int fdOffset = lseek(fd, 0, SEEK_CUR);
		int newFdOffset = lseek(newFd, 0, SEEK_CUR);
		if(fdOffset == -1 || newFdOffset == -1)
		{
			cerr << "System call lseek failed: " << strerror(errno);
			status = Unres;
		}
		else if(fdOffset != newFdOffset)
		{
			cerr << "System call dup2 failed: first and second arguments fd's offsets are not equal" << strerror(errno);
			status = Fail;
		}
				
		newFd = fd;
		int res = dup2(fd, newFd);
		if(res == -1)	
		{
			cerr << "System call dup2 failed: " << strerror(errno);
			status = Fail; 
		}
		else if(res != newFd)
		{
			cerr << "System call dup2 failed: second argument fd was not returned";
			status = Fail;
		}
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return status;
}

int DupFileDescriptorTest::Dup3Test(vector<string> args)
{
	int status = Success;
	
	try 
	{		
		File file1("newfile1");
		
		int fd = file1.GetFileDescriptor();		
		int newFd = 7;
		
		if(dup3(fd, newFd, O_CLOEXEC) == -1)
		{
			cerr << "System call dup3 failed: " << strerror(errno);
			return Fail; 
		}	
		
		struct stat oldFileStat, newFileStat;
		if(fstat(fd, &oldFileStat) == -1 || fstat(newFd, &newFileStat) == -1)
		{
			cerr << "System call fstat failed: " << strerror(errno);
			status = Unres;
		}
		else if(!statsEqual(oldFileStat, newFileStat))
		{
			cerr << "System call dup3 failed: first and second arguments fd's stats are not equal" << strerror(errno);
			status = Fail;		
		}
		
		int newFlag = fcntl(fd, F_GETFD);
		if(newFlag < 0)
		{
			cerr << "System call fcntl failed: " << strerror(errno);
			status = Unres;
		}
		else if(!cloexecFlagOff(newFlag))
		{
			cerr << "System call dup3 failed: O_CLOEXEC flag isn't off" << strerror(errno);
			status = Fail;
		}
		
		int statusFlag = fcntl(fd, F_GETFL);
		int newStatusFlag = fcntl(newFd, F_GETFL);
		if(statusFlag < 0 || newStatusFlag < 0)
		{
			cerr << "System call fcntl failed: " << strerror(errno);
			status = Unres;		
		}
		else if(!statusFlagsEqual(statusFlag, newStatusFlag))
		{
			cerr << "System call dup failed: first and second arguments fd's status flags are not equal" << strerror(errno);
			status = Fail;
		}	
		
		int fdOffset = lseek(fd, 0, SEEK_CUR);
		int newFdOffset = lseek(newFd, 0, SEEK_CUR);
		if(fdOffset == -1 || newFdOffset == -1)
		{
			cerr << "System call lseek failed: " << strerror(errno);
			status = Unres;
		}
		else if(fdOffset != newFdOffset)
		{
			cerr << "System call dup3 failed: first and second arguments fd's offsets are not equal" << strerror(errno);
			status = Fail;
		}	
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		status = Unres;
	}
		
	return status;
}

int DupFileDescriptorTest::DupErrEBADFTest(vector<string> args)
{
	int fd = -1;
	int newFd = dup(fd);
	
	if(newFd != -1 || errno != EBADF)
	{
		cerr << "EBADF error expected";
		return Fail;
	}
	
	return Success;
}

int DupFileDescriptorTest::Dup2ErrEBADFTest(vector<string> args)
{
	int status = Success;
	int fd = -1;
	int newFd = -1;
	
	if(dup2(fd, newFd) != -1 || errno != EBADF) 
	{
		cerr << "EBADF error expected: first argument fd is not an open fd";
		status = Fail;
	}
	
	try
	{
		File file("newfile");
		fd = file.GetFileDescriptor();
		newFd = -1;
		
		if(dup2(fd, newFd) != -1 || errno != EBADF)
		{
			cerr << "EBADF error expected: second argument fd is out of the allowed range for fds";
			status = Fail;
		}
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		status = Unres;
	}
	
	return status;
}

int DupFileDescriptorTest::Dup3ErrEBADFTest(vector<string> args)
{
	int status = Success;
	int fd = -1;
	int newFd = -2;
	
	if(dup3(fd, newFd, 0) != -1 || errno != EBADF)
	{
		cerr << "EBADF error expected: first argument fd is not an open fd. Error code: " << strerror(errno);
		status = Fail;
	}
	
	try
	{
		File file("newfile");
		fd = file.GetFileDescriptor();
		newFd = -1;
		
		if(dup3(fd, newFd, 0) != -1 || errno != EBADF)
		{
			cerr << "EBADF error expected: second argument fd is out of the allowed range for fds";
			status = Fail;
		}
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		status = Unres;
	}
	
	return status;
}

int DupFileDescriptorTest::Dup3ErrEINVALTest(vector<string> args)
{
	int status = Success;
	try 
	{
		File file("newFile");
		int fd = file.GetFileDescriptor();
		int newFd = fd;
		if(dup3(fd, newFd, 0) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: first and second arguments fds are equal";
			status = Fail;
		}
		
		newFd = -1;
		if(dup3(fd, newFd, 1) != -1 || errno != EINVAL)
		{
			cerr << "EINVAL error expected: flags contain invalid value";
			status = Fail;			
		}		
	}
	catch (Exception ex) 
	{
		cerr << ex.GetMessage();
		status = Unres;
	}
	
	return status;
}

bool DupFileDescriptorTest::statsEqual(struct stat stat1, struct stat stat2)
{	
	//dev_t st_dev
	if(MAJOR(stat1.st_dev) != MAJOR(stat2.st_dev) || MINOR(stat1.st_dev) != MINOR(stat2.st_dev))
	{
		return false;
	}
	
	//ino_t st_ino
	if(stat1.st_ino != stat2.st_ino)
	{
		return false;
	}
	
	//mode_t st_mode
	if(stat1.st_mode != stat2.st_mode)
	{
		return false;
	}
	
	//nlink_t st_nlink
	if(stat1.st_nlink != stat2.st_nlink)
	{
		return false;
	}
	
	//uid_t st_uid
	if(stat1.st_uid != stat2.st_uid)
	{
		return false;
	}
	
	//gid_t st_gid
	if(stat1.st_gid != stat2.st_gid)
	{
		return false;
	}	

	//devt_t st_rdev
	if(MAJOR(stat1.st_rdev) != MAJOR(stat2.st_rdev) || MINOR(stat1.st_rdev) != MINOR(stat2.st_rdev))
	{
		return false;
	}
	
	//off_t st_size
	if(stat1.st_size != stat2.st_size)
	{
		return false;
	}	
	
	//blksize_t st_blksize
	if(stat1.st_blksize != stat2.st_blksize)
	{
		return false;
	}
	
	//blkcnt_t st_blocks
	if(stat1.st_blocks != stat2.st_blocks)
	{
		return false;
	}
	
	//time_t st_atime
	if(stat1.st_atime != stat2.st_atime)
	{
		return false;
	}
	
	//time_t st_mtime
	if(stat1.st_mtime != stat2.st_mtime)
	{
		return false;
	}
	
	//time_t st_ctime
	if(stat1.st_ctime != stat2.st_ctime)
	{
		return false;
	}

	return true;
}

bool DupFileDescriptorTest::cloexecFlagOff(int flag)
{
	if((flag & FD_CLOEXEC) == 1)
	{
		return false;
	}	
	
	return true;
}

bool DupFileDescriptorTest::statusFlagsEqual(int flag1, int flag2)
{
	if(flag1 != flag2)
	{
		return false;
	}	
	
	return true;
}
