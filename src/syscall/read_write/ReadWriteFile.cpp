//      ReadWriteFile.cpp
//      
//      Copyright 2011 Eduard Bagrov <ebagrov@gmail.com>
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

#include <sys/timerfd.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "ReadWriteFile.hpp"
#include "File.hpp"

int ReadWriteFileTest::Main(vector<string>)
{
	if ( _mode == Normal )
	{	
		switch (_operation)
		{
			case ReadBadFileDescriptor1:
				return ReadBadFileDescriptorTest1();
			case ReadBadFileDescriptor2:
				return ReadBadFileDescriptorTest2();
			case ReadEinvalError:
				return ReadEinvalErrorTest();
			case ReadIsdirError:
				return ReadIsdirErrorTest();
			case proba:
				return probaTest();
			default:
				cerr << "Unsupported operation.";
				return Unres;		
		}
	}
	cerr << "Test was successful";
	return Success;
}

// attempt to read from a file, which was opened in write mode
Status ReadWriteFileTest::ReadBadFileDescriptorTest1()
{
	try
	{
		File file("testfile.txt", S_IWUSR);
		
		char buf[1024];
		size_t count = 10;

		size_t fd = open("testfile.txt", O_WRONLY);

		ssize_t status = read(fd, buf, count);
		if (errno != EBADF)
		{
			cerr << "Expected to get bad file descriptor";
			return Fail;
		}
	}
	catch (Exception ex)
	{
		cerr << ex.GetMessage();
		return Unres;
	}
	
	return Success;
}

// attempt to read from -1 file descriptor
Status ReadWriteFileTest::ReadBadFileDescriptorTest2()
{
	char buf[1024];
	size_t count = 10;

	ssize_t status = read(-1, buf, count);
	if (errno != EBADF)
	{
		cerr << "Expected to get bad file descriptor";
		return Fail;
	}
	
	return Success;
}

// attempt to read 0 bytes from file descriptor created by timerfd_create()
Status ReadWriteFileTest::ReadEinvalErrorTest()
{	
	int fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd == -1)
    {
		cerr << strerror(errno);
		return Unres;
	}
               
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
       
    struct itimerspec new_value;
    new_value.it_value.tv_sec = now.tv_sec;
    new_value.it_value.tv_nsec = now.tv_nsec;
    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
    
    char buf[1024];
	size_t count = 0;
    ssize_t status = read(fd, buf, 0);
    
    if (errno != EINVAL)
    {
		cerr << "Expecting to get EINVAL error";
		return Fail;
	}
	
	return Success;
}

Status ReadWriteFileTest::ReadIsdirErrorTest()
{
	int dir = mkdir("directory", 777);
	if (dir == -1)
	{
		cerr << strerror(errno);
		return Unres;
	}
	
	size_t fd = open("directory", O_DIRECTORY);
	if (fd == -1)
	{
		cerr << strerror(errno);
		rmdir("directory");
		return Unres;
	}
	
	char buf[1024];
	size_t count = 10;

	ssize_t status = read(fd, buf, count);
	if (errno != EISDIR)
	{
		cerr << "Expecting to get EISDIR error";
		rmdir("directory");
		return Fail;
	}
		
	int rmstatus = rmdir("directory");
	if (rmstatus == -1)
	{
		return Unknown;
	}
	
	return Success;
}

Status ReadWriteFileTest::probaTest()
{
	return Success;
}
